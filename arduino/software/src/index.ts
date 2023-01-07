import * as dotenv from 'dotenv';
import * as fs from 'fs';
import mkdirp from 'mkdirp';
import unitsJSON from '../data/units.json' assert {type: 'json'};

/* Global variable */
dotenv.config();
const unitsLength: number = Object.keys(unitsJSON.units).length;

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
    compileData = compileData.replace(/SUB10_MAC/g, String(unitsJSON.units[9].mac));
  } else if (type === 'sub') { // sub.ino

    // routes
    let i: number;
    let routeMac: string = '';
    let espNowSendRoute101: string = '';
    let espNowSendRoute102: string = '';
    let unitNumber: number = 0;

    // route101
    for (i = 0; i < unitsJSON.routes.route101[number].next.length; i++) {
      unitNumber = unitsJSON.routes.route101[number].next[i];
      routeMac += `uint8_t sub${unitNumber}Mac[] = ${unitsJSON.units[unitNumber - 1].mac};\n`;
      espNowSendRoute101 += `esp_now_send(sub${unitNumber}Mac, (uint8_t *) &myData, sizeof(myData)); // ESP-NOWでデータを送信\n    `;
    }

    // route102
    for (i = 0; i < unitsJSON.routes.route102[number].next.length; i++) {
      unitNumber = unitsJSON.routes.route102[number].next[i];
      routeMac += `uint8_t sub${unitNumber}Mac[] = ${unitsJSON.units[unitNumber - 1].mac};\n`;
      espNowSendRoute102 += `esp_now_send(sub${unitNumber}Mac, (uint8_t *) &myData, sizeof(myData)); // ESP-NOWでデータを送信\n    `;
    }
  
    compileData = compileData.replace(/ROUTE_MAC/g, String(routeMac));
    compileData = compileData.replace(/ROUTE_101_ESP_NOW_SEND/g, String(espNowSendRoute101));
    compileData = compileData.replace(/ROUTE_102_ESP_NOW_SEND/g, String(espNowSendRoute102));
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
