var fs = require("fs");
var readline = require("readline");
var argvs = process.argv.slice(2);
var read = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
});
read.question("Quantidade de vetores: ", function (answer) {
  console.time("Execução do programa");
  // Erases the file current data:
  var vectorsQuant = parseInt(answer);
  try {
    if (isNaN(vectorsQuant)) {
      read.close();
      throw new Error("Value passed is not numeric");
    }
    fs.writeFileSync(argvs[0], "");
    generateQuickSortFile(vectorsQuant);
    console.timeEnd("Execução do programa");
    read.close();
  } catch (error) {
    console.error(error);
  }
});
// Function to generate n numbers from 1 to 100:
function generateRandomVector(n) {
  var vector = [];
  var randomNum = 55;
  for (var i = 0; i < n; i++) {
    randomNum = Math.floor(Math.random() * 2000000) - 999999;
    vector.push(randomNum);
  }
  return vector;
}
function generateQuickSortFile(vectorsQuant) {
  fs.appendFileSync(argvs[0], "".concat(vectorsQuant.toString(), "\n"));
  // Iterator to write data in the file of n(vectorsQuant) vectors:
  for (let i = 0; i < vectorsQuant; i++) {
    // Chooses randomly the size of the vector from 3 to 15:
    var randomVectorSize = Math.round(Math.random() * 1800) + 200;
    var vector = generateRandomVector(randomVectorSize);
    // Writes the quantity of elements in one line:
    fs.appendFileSync(argvs[0], "".concat(randomVectorSize, "\n"));
    // The elements itself:
    for (var i_1 = 0; i_1 < randomVectorSize - 1; i_1++) {
      fs.appendFileSync(argvs[0], "".concat(vector[i_1], " "));
    }
    fs.appendFileSync(argvs[0], "".concat(vector[randomVectorSize - 1], "\n"));
  }
}
