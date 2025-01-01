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

const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));
async function showFireworks() {
    const colors = [
        "\x1b[32m",  // Green
        "\x1b[33m",  // Yellow
        "\x1b[31m",  // Red
        "\x1b[34m",  // Blue
        "\x1b[35m",  // Magenta
        "\x1b[36m"   // Cyan
    ];

    for (let i = 0; i < 20; i++) {
       // Clear the console
        console.clear();

        const x = Math.floor(Math.random() * (65 - 15 + 1)) + 15;  // Random x-coordinate
        const y = Math.floor(Math.random() * (15 - 5 + 1)) + 5;    // Random y-coordinate

        // Select a single color for the current explosion
        const color = colors[Math.floor(Math.random() * colors.length)];

        // Draw the firework explosion
        const offsets = [
            [-1, 0], [1, 0], [0, -1], [0, 1],
            [-1, -1], [1, 1], [-1, 1], [1, -1],
            [0, -2], [0, 2], [-2, 0], [2, 0]
        ];

        offsets.forEach(offset => {
            const nx = x + offset[0]
            const ny = y + offset[1];
            // Move the cursor to the position and print a firework symbol
            process.stdout.write(`\x1b[${ny};${nx}H${color}*\x1b[0m`);
        });

        // Wait for 300 milliseconds
        await sleep(300);
    }
}
async function main() {
console.clear();
await showFireworks();
console.clear();

const message = "\x1b[1;33mHappy New Year 2025!\x1b[0m";
const width = 80;
const x = Math.floor((width - message.length) / 2);
const y = 12;

// Move the cursor and display the message
process.stdout.write(`\x1b[${y};${x}H${message}`);
await sleep(3000);
;
console.log ( "your wish is: " + await input ( "Enter your wish: " ) );

}

main().then(() => {
    rl.close();
});
