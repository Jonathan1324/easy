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
let inputTest = await input ( "What is the height of Jonathan? " );
console.log ( "Jonathan's height is " + inputTest.toString() );

}

main().then(() => {
    rl.close();
});
