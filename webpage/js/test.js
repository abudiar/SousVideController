

function convertToTemp(scale, temp) {
    if (scale == "F") {
        return Math.round(((temp * 9) / 5 + 32) * 100) / 100;
    } else {
        return Math.round((((temp - 32) * 5) / 9) * 100) / 100;
    }
}
var ftemp = "140.1";

console.log(convertToTemp("C", ftemp))

console.log(convertToTemp("F", convertToTemp("C", ftemp)))