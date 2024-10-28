const readline = require('readline');
const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

function input(prompt) {
    return new Promise((resolve) => {
        rl.question(prompt, (userInput) => {
            resolve(userInput);
        });
    });
}

async function main() {
let str1 = "Hello, World!!!";
// Comment Test with '//';
let int1 = 1;
// Comment Test with '#';
let trueBool = true;
let falseBool = false;
let tBool = true;
let fBool = false;
console.log ( str1 );
console.log ( int1 );
int1 = 2;
console.log ( int1 );
console.log ( "Hello" );
console.log ( 25 );
console.log ( falseBool );
console.log ( tBool );
console.log ( false );
let test6 = 2;
console.log ( test6 );
const cTest = "Hi";
test6 = 1 + 2;
let test2 = 5 - 2;
let test3 = 6 / 2;
let test4 = test6 * test2;
let final = test4 + test3 - test6 * test2;
console.log ( "final: " + (final).toString() );
let int2 =  ( 1 + 4 )  * 2;
int2 = int2 + 2;
console.log ( int2 );
let test =  ( 1 + 1 )  * 2;
console.log ( test * 2 );
/* 
var val1 = input("What is your first number? ")var val2 = input("What is your second number? ")var result = int(val1) + int(val2)print(val1 + " + " + val2 + " = " + str(result));
*/;
test = "1 = " + (1).toString();
console.log ( test + " -- " + (true).toString() );
let str10 = "1 + 1 = ";
let OnePlusOne = 1 + 1;
console.log ( str10 + (OnePlusOne).toString() );
let inputTest = await input ( "What is your height? " );
console.log ( "Your height is " + inputTest );
let i = 4;
let s = "H";
let b = true;
console.log ( i );
console.log ( (b).toString() + s );
console.log ( s );
console.log ( b );
console.log ( 3 );
console.log ( true );

}

main().then(() => {
    rl.close();
});
