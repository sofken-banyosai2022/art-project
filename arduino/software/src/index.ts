import * as dotenv from 'dotenv';
import * as fs from 'fs';
import mkdirp from 'mkdirp';
import unitsJSON from '../data/units.json' assert {type: 'json'};

/* Global variable */
dotenv.config();
const unitsLength: number = Object.keys(unitsJSON.units).length;

interface Route {
  number: number;
  next: number[];
}

/* Output file */
const outputFile = (fileName: string, fileData: string): void => {

  // make directory
  mkdirp(`./dist/${fileName}`).then(() => {

    // make file
    fs.writeFile(`./dist/${fileName}/${fileName}.ino`, fileData, (err) => {
      if (err) throw err;
    });
  });
}

/* compile route */
const compileRoute = (routeName: string, route: Route[], compileData: string, number: number): string => {
  let unitNumber: number;
  let espNowSendRoute: string = '';

  for (let i = 0; i < route[number].next.length; i++) {
    unitNumber = route[number].next[i];

    if (number + 2 == unitNumber) { // nextMacと同じ場合
      espNowSendRoute += `esp_now_send(nextMac, (uint8_t *) &myData, sizeof(myData));\n    `;
    } else { // nextMacと違う場合
      compileData = compileData.replace(/NEXT_MAC/g, `NEXT_MAC;\nuint8_t sub${unitNumber}Mac[] = ${unitsJSON.units[unitNumber - 1].mac}`);
      compileData = compileData.replace(/デバイスを登録/g, `デバイスを登録\n  esp_now_add_peer(sub${unitNumber}Mac, ESP_NOW_ROLE_COMBO, 1, NULL, 0);`);
      espNowSendRoute += `esp_now_send(sub${unitNumber}Mac, (uint8_t *) &myData, sizeof(myData));\n    `;
    }
  }

  compileData = compileData.replace(new RegExp('ROUTE_' + routeName + '_ESP_NOW_SEND', 'g'), espNowSendRoute);

  return compileData;
}

/* compile file */
const compileFile = (type: string, fileData: string, number: number): string => {
  let compileData: string = fileData;

  if (type === 'main1') { // main1.ino
    compileData = compileData.replace(/MAIN2_MAC/g, String(unitsJSON.main_units[1].mac));
    compileData = compileData.replace(/WiFi_SSID/g, String(process.env.WiFi_SSID));
    compileData = compileData.replace(/WiFi_PASSWORD/g, String(process.env.WiFi_PASSWORD));
    compileData = compileData.replace(/IP_ADDRESS_IP/g, String(process.env.IP_ADDRESS_IP));
    compileData = compileData.replace(/IP_ADDRESS_GATEWAY/g, String(process.env.IP_ADDRESS_GATEWAY));
    compileData = compileData.replace(/IP_ADDRESS_SUBNET/g, String(process.env.IP_ADDRESS_SUBNET));
    compileData = compileData.replace(/OSC_PORT/g, String(process.env.OSC_PORT));
  } else if (type === 'main2')  { // main2.ino
    compileData = compileData.replace(/SUB1_MAC/g, String(unitsJSON.units[0].mac));
  } else if (type === 'sub') { // sub.ino
    compileData = compileRoute('101', unitsJSON.routes.route101, compileData, number); // compile route
    compileData = compileRoute('102', unitsJSON.routes.route102, compileData, number); // compile route
    compileData = compileData.replace(/UNIT_NUMBER/g, String(unitsJSON.units[number].number));
    compileData = compileData.replace(/NEXT_MAC/g, String(unitsJSON.units[number + 1].mac));
  }

  return compileData;
}

/* Read file */

// main.ino
fs.readFile('./src/main1.ino', 'utf-8', (err, fileData) => {
  let compileData: string = '';

  if (err) throw err;

  compileData = compileFile('main1', fileData, 0);
  outputFile('main1', compileData);

  console.log('Completed main1.ino!');
});

// main2.ino
fs.readFile('./src/main2.ino', 'utf-8', (err, fileData) => {
  let compileData: string = '';

  if (err) throw err;

  compileData = compileFile('main2', fileData, 0);
  outputFile('main2', compileData);

  console.log('Completed main2.ino!');
});

// sub.ino
fs.readFile('./src/sub.ino', 'utf-8', (err, fileData) => {
  let compileData: string = '';

  if (err) throw err;

  // loop the number of units
  for (let i = 0; i < unitsLength - 1; i++) {
    compileData = compileFile('sub', fileData, i);
    outputFile(`sub_${i + 1}`, compileData);
  }

  console.log('Completed sub.ino!');
});
