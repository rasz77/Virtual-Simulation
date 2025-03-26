#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>


#define MAX_CREATURES 100
#define MAX_ROOM_CREATURES 10


typedef struct Creature {
    // type of creature
    // 0 for player Character (PC); 1 for Non PC (NPC); 2 for Animal
    int type;
    //The pointer to the room the creature is located in
    struct Room* location;
} Creature;

typedef struct Room {
    //state  of the room
    // 0 for clean; 1 for half dirty; 2 for dirty
    int state;
    //  neighbors to the room on 4 directions; N,S,E,W
    struct Room* neighbors[4];
    // total number of creatures in the room
    int creatureCount;
    //creatures that are in the room
    struct Creature** creatures;
} Room;

typedef struct {
    //total number of rooms in the simluated world
    int roomCount;
    // rooms in the world
    struct Room* rooms;
    // creature count in the world
    int creatureCount;
    //total number of creatures
    Creature* creatures;
    //total respect of pc
    int respect;
} World;

void initializeWorld(World* world) {

    //  Store  room count in the world struct
    printf("Enter the number of rooms you will have in this world: ");
    scanf("%d", &world->roomCount);

    // allocate memory to store the rooms
    world->rooms = malloc(sizeof(Room) * world->roomCount);

    //start asking info for the rooms;
    // State: 0 clean, 1 half dirty, 2 dirty
    // 4 neighbours in each direction
    for (int i = 0; i < world->roomCount; ++i) {
        // For each room get  the state and neighbours
        Room* room = &world->rooms[i];
        //  We need 4 neighbours
        int neighbors[4];
        printf("For room %d\n", i);
        printf("Enter its state: ");
        scanf("%d", &room->state);
        printf("Enter number of neighbours:\n");
        printf(" North:");
        scanf("%d", &neighbors[0]);
        printf(" South:");
        scanf("%d", &neighbors[1]);
        printf(" East:");
        scanf("%d", &neighbors[2]);
        printf(" West:");
        scanf("%d", &neighbors[3]);

        // Each neigbors are stored in the rooms-> neighbours value and the worlds-> room->neighbours too
        // if neighbour is -1 then store null to neighbours in the room else store  the neighbour number
        // eg b = (a!=3) ? 1:2 means that if a is not equal to 3;  b = 1 else b = 3
        for (int j = 0; j < 4; j++) {
            room->neighbors[j] = (neighbors[j] != -1) ? &world->rooms[neighbors[j]] : NULL;
        }

        // We have not created creatures till now so initialize the room with zero creatures
        room->creatureCount = 0;
    }

    while (1) {

        //Get the total creatures in the world.
        printf("Enter the total creatures in this world (up to %d): ", MAX_CREATURES);
        scanf("%d", &world->creatureCount);

        // Check if max creature limit is broken else ask again
        if (world->creatureCount <= MAX_CREATURES)
            break;
        else
            printf("Invalid input. Maximum creatures allowed is %d.\n", MAX_CREATURES);
    }

    // Allocate memory to store the creatures
    world->creatures = malloc(sizeof(Creature) * world->creatureCount);

    bool pcCreated = false; // Flag to track if the PC has been created

    // Iterate for each creature
    for (int i = 0; i < world->creatureCount; ++i) {
        //Allocate memory for the new creature
        world->rooms[i].creatureCount = 0;
        world->rooms[i].creatures = NULL; // Initialize to NULL

        Creature* newCreature = malloc(sizeof(Creature));
        if (newCreature == NULL) {
            printf("Memory allocation failed for creature.\n");
            return;
        }
        //As the type of creature and the room location if the creature
        printf("For creature %d:", i);
        int type, locationIndex;
        while (1) {
            printf("Type (0 for PC, 1 for animal, 2 for NPC): ");
            scanf("%d", &type);

            // we set a initial pcCreated flag . We enable this to true when a PC is created.
            // If any more PC is being created we show a message and ask user to input again
            if (type == 0 && pcCreated == true) {
                printf("There can only be one PC in the world. Choose another type.\n");
            } else {
                if (type == 0) pcCreated = true; // Mark that the PC has been created
                break;
            }
        };
        while (1) {
            // Ask the room where creature is located . If there is no space in  the room ask user to place crature in new room again
            printf("Room index where it's located (0 to %d): ", world->roomCount - 1);
            scanf("%d", &locationIndex);

            Room* room = &world->rooms[locationIndex];
            if (room->creatureCount < MAX_ROOM_CREATURES) {
                break;  // Room has space for the creature
            } else {
                printf("Room %d is full. Choose another room.\n", locationIndex);
            }
        }

        // We now have a Creature.
        // Populate the world-> creatures type and location with our new creature
        world->creatures[i].type = type;
        world->creatures[i].location = &world->rooms[locationIndex];

        // We also have our room and creatures in it.
        Room* room = world->creatures[i].location;
        // Increase  the creature count for the room
        room->creatureCount++;
        // As creature increases  re allocate  the room size for according to the creature count
        //realloc is recently faiiling in my device. It is working in other machines. Maybe some kind of memory leak
        Creature** temp = realloc(room->creatures, sizeof(Creature*) * room->creatureCount);
        if (temp == NULL) {
            printf("Failed to allocate memory for creatures in room %d.\n", locationIndex);
            // Handle error, e.g., by freeing already allocated memory
            return;
        }
        room->creatures = temp;
        room->creatures[room->creatureCount - 1] = &world->creatures[i];
    }
    // Initialize the respect value with 40
    world->respect = 40;

}

void cleanRoom(Room* room) {
    if (room->state == 0) return; // Room is already clean
    room->state = (room->state == 2) ? 1 : 0; // Dirty to half-dirty, half-dirty to clean
}

void dirtyRoom(Room* room) {
    if (room->state == 2) return; // Room is already dirty
    room->state = (room->state == 0) ? 1 : 2; // Clean to half-dirty, half-dirty to dirty
}


void describeRoom(const Room* room, World*  world) {
    // Print out the details of the room with state
    const char* stateDescription[] = {"clean", "half-dirty", "dirty"};
    printf("Room %d, %s, neighbors ", (int)(room - world->rooms), stateDescription[room->state]);

    // Print out the neighbours of the room
    for (int i = 0; i < 4; i++) {
        if (room->neighbors[i] != NULL) {
            printf("%d to the %s ", (int)(room->neighbors[i] - world->rooms),
                   (i == 0) ? "North" : (i == 1) ? "South" : (i == 2) ? "East" : "West");
        }
    }

    // Print out the creatures in the room: if any of PC, NPC and Animals are there
    printf(", contains:\n");
    printf("\n%d\n", room->creatures[0]->type);
    for (int i = 0; i < room->creatureCount; i++) {
        Creature* creature = room->creatures[i];
        printf("%s %d\n",
               creature->type == 0 ? "PC" : creature->type == 1 ? "Animal" : "NPC",
               (int)(creature - world->creatures));
    }
}

// Look command: prints out the description of the room the PC is in.
void lookCommand(World* world) {
    // Look inside the room and describe/print out the room contents
    Room* pcRoom = NULL;
    for (int i = 0; i < world->creatureCount; i++) {
        // Type == 0 indicated the PC, we are searching for the room the pc is in and printing out the room description
        if (world->creatures[i].type == 0) {
            pcRoom = world->creatures[i].location;
            break;
        }
    }

    if (pcRoom != NULL) {
        describeRoom(pcRoom, world);
    } else {
        printf("Player character's location not found.\n");
    }
}

void updateRespect(World* world, int change) {
    // Update the respect values according to the value in t he change variable.
    world->respect += change;
}

bool tryMoveCreature(World* world, Creature* creature, Room* currentRoom) {
    // loop 4 times for each adjacent room on 4 sides of the current room and see where it can go
    for (int i = 0; i < 4; i++) {
        Room* adjacentRoom = currentRoom->neighbors[i];
        if (adjacentRoom != NULL && adjacentRoom->creatureCount < MAX_ROOM_CREATURES) {
            // Check suitability for animal/NPC depending on the cleanliness of the room
            //animal want room that are clean or half dirty ie 0 or 1
            //npc want room that are half dirty or dirty ie 1 or 2
            bool suitableForAnimal = creature->type == 1 && adjacentRoom->state != 2;
            bool suitableForNPC = creature->type == 2 && adjacentRoom->state != 0;
            if (suitableForAnimal || suitableForNPC || creature->type == 0) {
                // Remove creature from the current room
                int j;
                for (j = 0; j < currentRoom->creatureCount; j++) {
                    if (currentRoom->creatures[j] == creature) {
                        break;
                    }
                }
                for (; j < currentRoom->creatureCount - 1; j++) {
                    currentRoom->creatures[j] = currentRoom->creatures[j + 1];
                }
                currentRoom->creatureCount--;

                // Add creature to the new room that is suitable for the creature
                adjacentRoom->creatures = realloc(adjacentRoom->creatures, sizeof(Creature*) * (adjacentRoom->creatureCount + 1));
                //Update the creature of the new room
                adjacentRoom->creatures[adjacentRoom->creatureCount] = creature;
                //Update the creature count of the new room
                adjacentRoom->creatureCount++;
                // make the creature location the new room
                creature->location = adjacentRoom;

                return true;
            }
        }
    }

    // Creature cannot find a suitable room, leaves the house
    int creatureIndex = creature - world->creatures;
    printf("Creature %d leaves the house through the roof.\n", creatureIndex);

    // Remove creature from the current room
    int j;
    // What we are doing here:
    // Suppose we have 1,2,3,4,5 creature
    // we find the creature that is leaving suppose 3 and break t he loop
    // 1,2,3
    for (j = 0; j < currentRoom->creatureCount; j++) {
        if (currentRoom->creatures[j] == creature) {
            break;
        }
    }
    // then continue the loop
    // the next animal's position is now shifted to  thes position
    // 3rd position is not given to 4th position and 4th to 5th's and so on
    //the list looks like 1,2,4,5; since 3rd position is given to 4th. Thus removing  the  3rd  element
    for (; j < currentRoom->creatureCount - 1; j++) {
        currentRoom->creatures[j] = currentRoom->creatures[j + 1];
    }
    //decrease the room's creature count
    currentRoom->creatureCount--;

    // Trigger additional reactions in the room
    //NPC grunble when an animal leaves the house
    // Animals growl
    // Respect decreases by 3
    for (int j = 0; j < currentRoom->creatureCount; j++) {
        Creature* otherCreature = currentRoom->creatures[j];
        if (otherCreature->type == 1) {  // Animal
            printf("Animal %d growls.\n", (int)(otherCreature - world->creatures));
            updateRespect(world, -3);
        } else if (otherCreature->type == 2) {  // NPC
            printf("NPC %d grumbles.\n", (int)(otherCreature - world->creatures));
            updateRespect(world, -3);
        }
    }

    return false;
}

void updateRoomStateAndReact(World* world, Room* room, int newState) {
    room->state = newState;
    int i = 0;
    while (i < room->creatureCount) {
        Creature* creature = room->creatures[i];
        // see which creatures are happy or not depending on the cleanliness of the room

        //animal want room that are clean or half dirty ie 0 or 1
        //npc want room that are half dirty or dirty ie 1 or 2
        // more in the pdf
        bool isHappy = ((creature->type == 1 && (newState == 0 || newState == 1)) ||
                        (creature->type == 2 && (newState == 1 || newState == 2)) ||
                        creature->type == 0);

        // change respect accordingly
        // if positive increase respect and if negative decrease respect
        if (isHappy) {
            updateRespect(world, creature->type == 0 ? 0 : 1); // Positive reaction
            i++;
        } else {
            updateRespect(world, -1); // Negative reaction
            // Move the creature accordingly if it doesnot like the state of the room
            if (!tryMoveCreature(world, creature, room)) {
                // If creature leaves the house, do not increment i. Break the loop here using continue.
                continue;
            }
            i++;
        }
    }
}


void dirtyCommand(World* world) {
    // Find PC's current room
    Room* pcRoom = NULL;
    for (int i = 0; i < world->creatureCount; i++) {
        // Type == 0 indicated the PC, we are searching for the room the pc is in and printing out the room description
        if (world->creatures[i].type == 0) {  // PC
            pcRoom = world->creatures[i].location;
            break;
        }
    }

    if (pcRoom == NULL) {
        printf("PC's location not found.\n");
        return;
    }

    // Check if the room is already dirty
    if (pcRoom->state == 2) {
        printf("The room is already dirty.\n");
        return;
    }

    // Update room state and handle creature reactions.
    // pc=>state + 1 => changing state from 0 to 1 ie clean to half dirty or from 1 to 2 ie. half dirty to dirty
    updateRoomStateAndReact(world, pcRoom, pcRoom->state + 1); // Dirty the room
}

void cleanCommand(World* world) {
    // Find PC's current room
    Room* pcRoom = NULL;
    for (int i = 0; i < world->creatureCount; i++) {
        // Type == 0 indicated the PC, we are searching for the room the pc is in and printing out the room description
        if (world->creatures[i].type == 0) {  // PC
            pcRoom = world->creatures[i].location;
            break;
        }
    }

    if (pcRoom == NULL) {
        printf("PC's location not found.\n");
        return;
    }

    // Check if the room is already clean
    if (pcRoom->state == 0) {
        printf("The room is already clean.\n");
        return;
    }

    // Update room state and handle creature reactions
    // pc=>state + 1 => changing state from 2 to 1 ie dirty to half dirty or from 1 to 0 ie. half dirty to clean
    updateRoomStateAndReact(world, pcRoom, pcRoom->state - 1); // Clean the room
}

void moveCreatureCommand(World* world, int creatureIndex, int direction) {
    if (creatureIndex < 0 || creatureIndex >= world->creatureCount) {
        printf("Invalid creature index.\n");
        return;
    }

    // Get the creature and the craeture room
    Creature* creature = &world->creatures[creatureIndex];
    Room* currentRoom = creature->location;

    if (direction < 0 || direction > 3) {
        printf("Invalid direction.\n");
        return;
    }

    // get the room with the required direction
    Room* newRoom = currentRoom->neighbors[direction];
    if (newRoom == NULL) {
        printf("There is no room in that direction.\n");
        return;
    }

    if (newRoom->creatureCount >= MAX_ROOM_CREATURES) {
        printf("The room in that direction is full. Creature can't move there.\n");
        return;
    }

    // Check for room suitability for animals/NPCs
    if ((creature->type == 1 && newRoom->state == 2) || // Animal can't move to dirty room
        (creature->type == 2 && newRoom->state == 0)) { // NPC can't move to clean room
        printf("The creature cannot move to an unsuitable room.\n");
        return;
    }

    // Move the creature
    // Remove creature from the current room
    int j;
    for (j = 0; j < currentRoom->creatureCount; j++) {
        if (currentRoom->creatures[j] == creature) {
            break;
        }
    }
    for (; j < currentRoom->creatureCount - 1; j++) {
        currentRoom->creatures[j] = currentRoom->creatures[j + 1];
    }
    currentRoom->creatureCount--;

    // Add creature to the new room
    newRoom->creatures = realloc(newRoom->creatures, sizeof(Creature*) * (newRoom->creatureCount + 1));
    newRoom->creatures[newRoom->creatureCount] = creature;
    newRoom->creatureCount++;
    creature->location = newRoom;

    printf("Creature %d moves to the %s.\n", creatureIndex, direction == 0 ? "north" : direction == 1 ? "south" : direction == 2 ? "east" : "west");
}

void creatureCommand(World* world, int creatureIndex, const char* action) {
    if (creatureIndex < 0 || creatureIndex >= world->creatureCount) {
        printf("Invalid creature index.\n");
        return;
    }

    Creature* creature = &world->creatures[creatureIndex];
    Room* creatureRoom = creature->location;

    // Find the PC's room
    Room* pcRoom = NULL;
    for (int i = 0; i < world->creatureCount; i++) {
        // Type == 0 indicated the PC, we are searching for the room the pc is in and printing out the room description
        if (world->creatures[i].type == 0) {
            pcRoom = world->creatures[i].location;
            break;
        }
    }

    // Check if the creature and the PC are in the same room
    if (pcRoom != creatureRoom) {
        printf("Creature %d is not in the same room as you.\n", creatureIndex);
        return;
    }

    if (strcmp(action, "clean") == 0) {
        // make creature clean the room it is in
        cleanRoom(creatureRoom);
        // update the  state of the room
        updateRoomStateAndReact(world, creatureRoom, creatureRoom->state);
        printf("Creature %d cleaned the room.\n", creatureIndex);
    } else if (strcmp(action, "dirty") == 0) {
        dirtyRoom(creatureRoom);
        updateRoomStateAndReact(world, creatureRoom, creatureRoom->state);
        printf("Creature %d dirtied the room.\n", creatureIndex);
    } else if(strcmp(action, "look")==0) {
        if (creatureRoom == NULL) {
            printf("Creature %d's location is not found.\n", creatureIndex);
            return;
        }
        describeRoom(creatureRoom, world);
    } else if(strcmp(action, "north")==0) {
        moveCreatureCommand(world, creatureIndex, 0);
    } else if(strcmp(action, "south")==0) {
        moveCreatureCommand(world, creatureIndex, 1);
    } else if(strcmp(action, "east")==0) {
        moveCreatureCommand(world, creatureIndex, 2);
    } else if(strcmp(action, "west")==0) {
        moveCreatureCommand(world, creatureIndex, 3);
    }else {
        printf("Invalid action.\n");
    }
}

void movePC(World* world, int direction) {
    Creature* pc = NULL;
    for (int i = 0; i < world->creatureCount; i++) {
        // Type == 0 indicates the PC, we are searching for the room the pc is in and printing out the room description
        if (world->creatures[i].type == 0) {  // PC
            pc = &world->creatures[i];
            break;
        }
    }

    if (pc == NULL || pc->location == NULL) {
        printf("PC's location not found.\n");
        return;
    }

    // From the direction passed on to the function get the required room
    Room* oldRoom = pc->location;
    Room* newRoom = oldRoom->neighbors[direction];
    if (newRoom == NULL) {
        printf("There is no room in that direction.\n");
        return;
    }

    // If room is full then dont move the room and return here
    if (newRoom->creatureCount >= MAX_CREATURES) {
        printf("The room in that direction is full. You can't move there.\n");
        return;
    }

    // Remove PC from the old room
    for (int i = 0; i < oldRoom->creatureCount; i++) {
        if (oldRoom->creatures[i] == pc) {
            for (int j = i; j < oldRoom->creatureCount - 1; j++) {
                oldRoom->creatures[j] = oldRoom->creatures[j + 1];
            }
            oldRoom->creatureCount--;
            break;
        }
    }

    // Add PC to the new room
    newRoom->creatures = realloc(newRoom->creatures, sizeof(Creature*) * (newRoom->creatureCount + 1));
    newRoom->creatures[newRoom->creatureCount] = pc;
    newRoom->creatureCount++;

    // Update PC's location
    pc->location = newRoom;

    printf("You move to Room %d.\n", (int)(newRoom - world->rooms));
}

void processCommand(World* world, const char* command) {
    //Get the command and rpocess  it

    //Look command: look in the room where the main PC is located
    if (strcmp(command, "look") == 0) {
        lookCommand(world);
    } else if (strcmp(command, "clean") == 0) {
        //Clean command: Clean the room where   the PC is located
        cleanCommand(world);
    } else if (strcmp(command, "dirty") == 0) {
        //Dirty command: dirty the rooom where pc is located
        dirtyCommand(world);
    }else if (strcmp(command, "north") == 0) {
        // North: move the PC to north
        movePC(world, 0);
    } else if (strcmp(command, "south") == 0) {
        // South: move the PC to south
        movePC(world, 1);
    } else if (strcmp(command, "east") == 0) {
        // East: move the PC to east
        movePC(world, 2);
    } else if (strcmp(command, "west") == 0) {
        // West: move the PC to west
        movePC(world, 3);}
    else if (command[1] == ':') {
        // Creature commandL
        // 1:look, 1:clean, 1:dirty => commands creature besides PC (may be NPC or animal )
        int creatureIndex;
        char action[6]; // "clean" or "dirty" or "look"
        if (sscanf(command, "%d:%s", &creatureIndex, action) == 2) {
            creatureCommand(world, creatureIndex, action);
        } else {
            printf("Invalid creature command format.\n");
        }
    }
    else {
        printf("Unknown command.\n");
    }
}

bool checkGameOver(World* world) {
    // Print the respect  value
    printf("PC respect is: %d\n", world->respect);
    // End game id  respect is less than zero or more  than 80 with appropriate messages
    if (world->respect <= 0) {
        printf("Shame on you! You lose.\n");
        return true;
    } else if (world->respect > 80) {
        printf("Congratulations, you are praised!\n");
        return true;
    }
    return false;
}

void mainLoop(World* world) {
    //Start the simulation
    char command[100];
    while (1) {
        // Get the command
        printf("Enter a command: ");
        scanf("%s", command);

        // if "exit" string is entered, end t he game
        if (strcmp(command, "exit") == 0) {
            break;
        }
        // Process the command if any other
        processCommand(world, command);

        // See after each process command if game is over or not  based on respect value
        if (checkGameOver(world)) {
            break;
        }

    }
}

void cleanupWorld(World* world) {

    //  free the creatures in each room
    for (int i = 0; i < world->roomCount; ++i) {
        free(world->rooms[i].creatures);  // Free creatures in each room
    }
    free(world->rooms);  // Free rooms in the world
    free(world->creatures);  // Free creatures in the world
}

int main (){

    World world;
    // Initialize the rooms and creature of the world
    initializeWorld(&world);

    //Start taking input from the pc and run the simulation
    mainLoop(&world);

    // free up any pointers and memory allocations we did for the simulation
    cleanupWorld(&world);

    return 0;
}
