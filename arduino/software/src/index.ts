import * as fs from 'fs';
import mkdirp from 'mkdirp';
import unitsJSON from '../data/units.json' assert {type: 'json'};

/* Global variable */
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
const compileFile = (fileData: string, number: number): string => {
  let compileData: string = fileData;

  compileData = compileData.replace(/UNIT_NUMBER/g, String(unitsJSON.units[number].number));
  compileData = compileData.replace(/UNIT_MAC/g, unitsJSON.units[number].mac);

  return compileData;
}

/* Read file */

// main.ino
fs.readFile('./src/main.ino', 'utf-8', (err, fileData) => {
  if (err) throw err;

  outputFile('main', fileData);
  console.log(fileData);

  console.log('Completed main.ino!');
});

// sub.ino
fs.readFile('./src/sub.ino', 'utf-8', (err, fileData) => {
  let compileData: string = '';

  if (err) throw err;

  // loop the number of units
  for (let i = 0; i < unitsLength; i++) {
    compileData = compileFile(fileData, i);
    outputFile(`sub_${i + 1}`, compileData);
    console.log(compileData);
  }

  console.log('Completed sub.ino!');
});
