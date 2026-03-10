System Programming(CSC 322) - Class project

Creature Room Simulation (C) - This project is a command-line simulation written in C that models a house with multiple rooms and different types of creatures interacting with the environment. The player controls a PC (Player Character) and can perform actions that affect room cleanliness and creature behavior.

Features:
- Dynamic world with multiple rooms connected in North, South, East, and West** directions.
- Three creature types: PC (Player Character), Animals, NPCs(Non-player characters)
- Rooms have cleanliness states: Clean, Half-dirty, Dirty
- Creatures react to room conditions and may move or leave the house if unhappy.
- Respect the system that increases or decreases based on creature reactions.
- Interactive command system to: Look at the current room, Clean or dirty rooms, Move between rooms, Command other creatures

Key Concepts Used
- C structs and pointers
- Dynamic memory allocation** (`malloc`, `realloc`, `free`)
- Arrays of pointers
- Command parsing
- Basic simulation/game loop design
