#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

void clearScreen() {
    // ANSI escape code to clear the screen
    cout << "\033[2J\033[H";
}

void drawFirework(int x, int y, const string& color) {
    vector<pair<int, int>> offsets = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1},
        {-1, -1}, {1, 1}, {-1, 1}, {1, -1},
        {0, -2}, {0, 2}, {-2, 0}, {2, 0}
    };

    for (const auto& offset : offsets) {
        int nx = x + offset.first;
        int ny = y + offset.second;
        cout << "\033[" << ny << ";" << nx << "H" << color << "*\033[0m";
    }
}

void showFireworks() {
    srand(static_cast<unsigned>(time(0)));
    vector<string> colors = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m"};

    for (int frame = 0; frame < 20; ++frame) {
        clearScreen();

        int x = rand() % 50 + 15; // Random x-coordinate
        int y = rand() % 10 + 5;  // Random y-coordinate

        // Select a single color for the current explosion
        string color = colors[rand() % colors.size()];

        // Draw the firework explosion
        drawFirework(x, y, color);

        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(300));
    }
}

void showNewYearMessage() {
    clearScreen();
    string message = "\033[1;33mHappy New Year 2025!\033[0m";

    int width = 80;
    int messageX = (width - message.size()) / 2;
    int messageY = 12;

    cout << "\033[" << messageY << ";" << messageX << "H" << message;
    cout.flush();
    this_thread::sleep_for(chrono::seconds(3));
}

void happy2025() {
    clearScreen();
    //cout << "\033[?25l"; // Hide the cursor

    showFireworks();
    showNewYearMessage();

    //cout << "\033[?25h"; // Show the cursor again
}
