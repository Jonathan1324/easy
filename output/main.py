import os
import random
import time

def show_fireworks():
    colors = [
        "\033[32m",  # Green
        "\033[33m",  # Yellow
        "\033[31m",  # Red
        "\033[34m",  # Blue
        "\033[35m",  # Magenta
        "\033[36m"   # Cyan
    ]

    for _ in range(20):
        os.system('cls' if os.name == 'nt' else 'clear')

        x = random.randint(15, 65)  # Random x-coordinate
        y = random.randint(5, 15)   # Random y-coordinate

        # Select a single color for the current explosion
        color = random.choice(colors)

        # Draw the firework explosion
        offsets = [
            (-1, 0), (1, 0), (0, -1), (0, 1),
            (-1, -1), (1, 1), (-1, 1), (1, -1),
            (0, -2), (0, 2), (-2, 0), (2, 0)
        ]

        for dx, dy in offsets:
            nx, ny = x + dx, y + dy
            print(f"\033[{ny};{nx}H{color}*\033[0m", end="")

        print("", flush=True)
        time.sleep(0.3)

os.system('cls' if os.name == 'nt' else 'clear')
show_fireworks()
os.system('cls' if os.name == 'nt' else 'clear')
message = "\033[1;33mHappy New Year 2025!\033[0m"

width = 80
x = (width - len(message)) // 2
y = 12

print(f"\033[{y};{x}H{message}", flush=True)
time.sleep(3)


print ( "your wish is: " + input ( "Enter your wish: " ) )
