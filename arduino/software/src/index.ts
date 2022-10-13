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
    compileData = compileData.replace(/MAIN2_MAC/g, String(unitsJSON.main_units[0].mac));
    compileData = compileData.replace(/WiFi_SSID/g, String(process.env.WiFi_SSID));
    compileData = compileData.replace(/WiFi_PASSWORD/g, String(process.env.WiFi_PASSWORD));
    compileData = compileData.replace(/IP_ADDRESS_IP/g, String(process.env.IP_ADDRESS_IP));
    compileData = compileData.replace(/IP_ADDRESS_GATEWAY/g, String(process.env.IP_ADDRESS_GATEWAY));
    compileData = compileData.replace(/IP_ADDRESS_SUBNET/g, String(process.env.IP_ADDRESS_SUBNET));
    compileData = compileData.replace(/OSC_HOST/g, String(process.env.OSC_HOST));
    compileData = compileData.replace(/OSC_PORT/g, String(process.env.OSC_PORT));
  } else if (type === 'sub')  { // sub.ino
    compileData = compileData.replace(/UNIT_NUMBER/g, String(unitsJSON.units[number].number));
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
  for (let i = 0; i < unitsLength; i++) {
    compileData = compileFile('sub', fileData, i);
    outputFile(`sub_${i + 1}`, compileData);
  }

  console.log('Completed sub.ino!');
});
