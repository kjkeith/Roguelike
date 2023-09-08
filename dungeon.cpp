#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <string.h>
#include <endian.h>
#include <stdint.h>
#include <unistd.h>
#include <ncurses.h>
#include <queue>
#include "dungeon.h"
#include "priority.h"
#include "MonsterParser.h"

bool fogOfWar = true;
char messageString[DUNGEON_WIDTH];
std::string message;

int main(int argc, char* argv[]) {
    int save, load;
    int i;
    Dungeon dungeon;
    char saveStr[10];
    char loadStr[10];
    char monStr[10];
    int seed = time(NULL);
    srand(seed);

    // PARSE MONSTERS AND ITEMS
    std::string home = std::getenv("HOME");
    std::string saveLocation = "/.rlg327/object_desc.txt";
    std::string fileDest = home + saveLocation;

    ItemParser itemParser;
    dungeon.itemTemplates = itemParser.parseFile(fileDest);

    saveLocation = "/.rlg327/monster_desc.txt";
    fileDest = home + saveLocation;

    MonsterParser monsterParser;
    dungeon.monsterTemplates = monsterParser.parseFile(fileDest);

    strcpy(saveStr, "--save");
    strcpy(loadStr, "--load");
    strcpy(monStr, "--nummon");

    // checking to see if there are arguments
    if(argc != 1) {
        for(i = 1; i < argc; i++) {
            if(!strcmp(argv[i], saveStr)) {
                save = 1;
            }
            if(!strcmp(argv[i], loadStr)) {
                load = 1;
            }
            if(!strcmp(argv[i], monStr)) {
                dungeon.num_monsters = atoi(argv[i + 1]);
                i++;
            } else {
                dungeon.num_monsters = 1 + (rand() % 8);
            }
        }
    } else {
        dungeon.num_monsters = 1 + (rand() % 8);
    }

    if(load == 1) {
        loadDungeon(&dungeon);
    } else {
        generateDungeon(&dungeon, NULL);
    }

    if(save) {
        saveDungeon(&dungeon);
    }

    // initialize player's fog of war map
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(int j = 0; j < DUNGEON_WIDTH; j++) {
            dungeon.player->playerMap[i][j].character = ' ';
        }
    }

    // Here we'll generate the distance maps
    int tunnelersMap[DUNGEON_HEIGHT][DUNGEON_WIDTH];
    int nontunnelersMap[DUNGEON_HEIGHT][DUNGEON_WIDTH];

    buildDistanceMap(&dungeon, 0, nontunnelersMap);
    buildDistanceMap(&dungeon, 1, tunnelersMap);

    CharacterQueue *turnQueue = initializeCharacterQueue();
    // Adding player to turn queue
    addCharacterQueue(turnQueue, initializeCharacterQueueNode(1000/dungeon.player->speed, dungeon.player));

    // Adding monsters to turn queue
    for(i = 0; i < dungeon.num_monsters; i++) {
        addCharacterQueue(turnQueue, initializeCharacterQueueNode(1000/dungeon.newMonsters.at(i)->speed, dungeon.newMonsters.at(i)));
    }

    updateFogOfWarMap(&dungeon);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    clear();
    use_default_colors();
    start_color();
    init_pair(COLOR_RED, COLOR_RED, -1);
    init_pair(COLOR_GREEN, COLOR_GREEN, -1);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, -1);
    init_pair(COLOR_BLUE, COLOR_BLUE, -1);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, -1);
    init_pair(COLOR_CYAN, COLOR_CYAN, -1);
    init_pair(COLOR_WHITE, COLOR_WHITE, -1);

    strcpy(messageString, "Welcome to the dungeon!");

    // With everything placed, print the map for the first time
    printDungeon(&dungeon, messageString);

    // With everything built, begin the loop for the characters.
    while(1) {
        CharacterQueueNode characterNode = popCharacterQueue(turnQueue);
        int priority = characterNode.priority;
        Character *character = characterNode.character;


        if(character->isLiving) {
            // Add back to queue based on their previous speed. Make new node in queue to accommodate new priority
            if(character->isPlayer) {
                int speed = 0;
                for(auto &item : dungeon.player->equipedItems) {
                    if(item != nullptr) {
                         speed += item->getSpeedBonus();
                    }
                } 
                addCharacterQueue(turnQueue, initializeCharacterQueueNode(priority + (1000/(character->speed + speed)), character));
            } else {
                addCharacterQueue(turnQueue, initializeCharacterQueueNode(priority + (1000/character->speed), character));
            }
        }
        // check if character is the player
        if(character->isPlayer) {
            updateFogOfWarMap(&dungeon);
            /* Also need to print certain statuses out. Do here or in printDungeon()?*/
            printDungeon(&dungeon, messageString);

            int goUpDownStairs = characterPlayerAction(&dungeon, messageString);

            if(goUpDownStairs == TRUE) {
                // character has gone down or up stairs, creating new dungeon
                // reinitialize player's fog of war map
                for(i = 0; i < DUNGEON_HEIGHT; i++) {
                    for(int j = 0; j < DUNGEON_WIDTH; j++) {
                        dungeon.player->playerMap[i][j].character = ' ';
                    }
                }

                delete dungeon.dungeonRooms;

                generateDungeon(&dungeon, dungeon.player);
                deallocateCharacterQueue(turnQueue);
                turnQueue = initializeCharacterQueue();
                
                int speed;
                for(auto &item : dungeon.player->equipedItems) {
                    if(item != nullptr) {
                        speed += item->getSpeedBonus();
                    }
                }  

                // Adding player to turn queue
                addCharacterQueue(turnQueue, initializeCharacterQueueNode(1000/(dungeon.player->speed + speed),
                                                                          dungeon.player));

                // Adding monsters to turn queue
                for(i = 0; i < dungeon.num_monsters; i++) {
                    addCharacterQueue(turnQueue, initializeCharacterQueueNode(1000/dungeon.newMonsters.at(i)->speed,
                                                                              dungeon.newMonsters.at(i)));
                }
                strcpy(messageString, "You've made it to the next floor safely! Good luck. ");
            }

            printDungeon(&dungeon, messageString);
            usleep(250000);
            // // check if character has killed all monsters
            // int areThereMonsters = 0;
            // for(i = 0; i < dungeon.num_monsters; i++) {
            //     if(dungeon.newMonsters.at(i)->isLiving) {
            //         areThereMonsters = 1;
            //         // no longer need to check remaining monsters
            //         break;
            //     }
            // }
            // if(!areThereMonsters) {
            //     strcpy(messageString, "You've ... won? Good job for doing the impossible. ");
            //     printDungeon(&dungeon, messageString);
            //     stopProgram();
            //     // // break out of while loop
            //     // break;
            // }
            // since character has moved, create new distance maps
            buildDistanceMap(&dungeon, 0, nontunnelersMap);
            buildDistanceMap(&dungeon, 1, tunnelersMap);
        } else { // else its a monster
            if(character->isLiving) {
                moveCharacter(character, &dungeon, tunnelersMap, nontunnelersMap);
            }
            // check is monster has killed player during movement
            if(!dungeon.player->isLiving) {
                strcpy(messageString, "You have died! Better luck next time. ");
                printDungeon(&dungeon, messageString);
                stopProgram();
                // break;
            }
        }
    }
    return 0;
}

void printDungeon(Dungeon *dungeon, char messageString[DUNGEON_WIDTH]) {
    int i, j;
    for(i = 0; i < DUNGEON_WIDTH; i++) {
        mvprintw(0, i, " ");
    }
    mvprintw(0, 0, messageString);
    Map printMap[DUNGEON_HEIGHT][DUNGEON_WIDTH];

    // Print the dungeon layout
    if(fogOfWar == false) {
        for(i = 0; i < DUNGEON_HEIGHT; i++) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                printMap[i][j].character = dungeon->map[i][j].character;
                mvaddch(i+1, j, printMap[i][j].character);
            }
        }

        for(auto item : dungeon->items) {
             printMap[item->getY()][item->getX()].character = item->getCharRep();
             attron(COLOR_PAIR(item->getColor()));
             mvaddch(item->getY() + 1, item->getX(), item->getCharRep());
             attroff(COLOR_PAIR(item->getColor()));
        }

        for(i = 0; i < dungeon->num_monsters; i++) {
            if(dungeon->newMonsters.at(i)->isLiving) {
                printMap[dungeon->newMonsters.at(i)->y][dungeon->newMonsters.at(i)->x].character = dungeon->newMonsters.at(i)->charRep;
                attron(COLOR_PAIR(dungeon->newMonsters.at(i)->getColors().at(0)));
                mvaddch(dungeon->newMonsters.at(i)->y + 1, dungeon->newMonsters.at(i)->x, dungeon->newMonsters.at(i)->charRep);
                attroff(COLOR_PAIR(dungeon->newMonsters.at(i)->getColors().at(0)));
            }
        }

        if(dungeon->player->isLiving) {
            printMap[dungeon->player->y][dungeon->player->x].character = dungeon->player->charRep;
            mvaddch(dungeon->player->y + 1, dungeon->player->x, dungeon->player->charRep);
        }
    } else {
        int radius = 2;
        Item *light = dungeon->player->equipedItems.at(9);
        if(light != nullptr) {
            radius += light->getSpecAttr();
        }

        for(i = 0; i < DUNGEON_HEIGHT; i++) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                printMap[i][j].character = dungeon->player->playerMap[i][j].character;
                mvaddch(i+1, j, printMap[i][j].character);
            }
        }

        for(auto item : dungeon->items) {
            int xDist = abs(dungeon->player->x - item->getX());
            int yDist = abs(dungeon->player->y - item->getY());
            if(xDist < radius + 1 && yDist < radius + 1) {
                printMap[item->getY()][item->getX()].character = item->getCharRep();
                attron(COLOR_PAIR(item->getColor()));
                mvaddch(item->getY() + 1, item->getX(), item->getCharRep());
                attroff(COLOR_PAIR(item->getColor()));
            }
        }

        for(i = 0; i < dungeon->num_monsters; i++) {
            int xDist = abs(dungeon->player->x - dungeon->newMonsters.at(i)->x);
            int yDist = abs(dungeon->player->y - dungeon->newMonsters.at(i)->y);
            if(dungeon->newMonsters.at(i)->isLiving && xDist < radius + 1 && yDist < radius + 1) {
                printMap[dungeon->newMonsters.at(i)->y][dungeon->newMonsters.at(i)->x].character = dungeon->newMonsters.at(i)->charRep;
                attron(COLOR_PAIR(dungeon->newMonsters.at(i)->getColors().at(0)));
                mvaddch(dungeon->newMonsters.at(i)->y + 1, dungeon->newMonsters.at(i)->x, dungeon->newMonsters.at(i)->charRep);
                attroff(COLOR_PAIR(dungeon->newMonsters.at(i)->getColors().at(0)));
            }
        }

        if(dungeon->player->isLiving) {
            printMap[dungeon->player->y][dungeon->player->x].character = dungeon->player->charRep;
            mvaddch(dungeon->player->y + 1, dungeon->player->x, dungeon->player->charRep);
        }
    }

    refresh();
}

void generateDungeon(Dungeon *dungeon, Character *player) {
    int i, j;

    // dungeon->num_rooms = MIN_ROOM_COUNT + (rand() % (MAX_ROOM_COUNT - MIN_ROOM_COUNT + 1));
    // // printf("Number of rooms: %d\n", dungeon->num_rooms);
    // dungeon->dungeonRooms = (Room*)malloc(sizeof(Room) * dungeon->num_rooms);

    // Generate base dungeon (i.e. no rooms or corridors ...)
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            // Determine if this cell is a side edge ...
            if(i == 0 || i == DUNGEON_HEIGHT - 1) {
                dungeon->map[i][j].character = '-';
                dungeon->map[i][j].hardness = 255;

            }
                // Determine if this cell is a top or bottom edge ...
            else if (j == 0 || j == DUNGEON_WIDTH - 1) {
                dungeon->map[i][j].character = '|';
                dungeon->map[i][j].hardness = 255;
            }
                // If this is not a dungeon edge, make it simple rock for now ...
            else
            {
                dungeon->map[i][j].character = ' ';
                dungeon->map[i][j].hardness = 1 + (rand() % (253));
            }
        }
    }

    //Now that we have the outer walls and put rock everywhere, place the rooms!
    dungeon->num_rooms = MIN_ROOM_COUNT + (rand() % (MAX_ROOM_COUNT - MIN_ROOM_COUNT + 1));
    // printf("Number of rooms: %d\n", dungeon->num_rooms);
    
    dungeon->dungeonRooms = (Room*)malloc(sizeof(Room) * dungeon->num_rooms);
    generateRooms(dungeon);

    // Time to place the paths ...
    generatePaths(dungeon);

    //place the stairs ...
    // > downstairs
    // < upstairs
    i = rand() % (dungeon->num_rooms - 1);
    dungeon->downStairs.x = dungeon->dungeonRooms[i].x + (rand() % dungeon->dungeonRooms[i].width);
    dungeon->downStairs.y = dungeon->dungeonRooms[i].y + (rand() % dungeon->dungeonRooms[i].height);
    dungeon->map[dungeon->downStairs.y][dungeon->downStairs.x].character = '>';

    i = rand() % (dungeon->num_rooms - 1);
    dungeon->upStairs.x = dungeon->dungeonRooms[i].x + (rand() % dungeon->dungeonRooms[i].width);
    dungeon->upStairs.y = dungeon->dungeonRooms[i].y + (rand() % dungeon->dungeonRooms[i].height);
    dungeon->map[dungeon->upStairs.y][dungeon->upStairs.x].character = '<';

    if (player == NULL) {
        dungeon->player = (Character*)malloc(sizeof(Character));
        dungeon->player->speed = 10;
        dungeon->player->hp = 1000;
        dungeon->player->damage = Dice(0, 1, 4);
        dungeon->player->isLiving = true;
        dungeon->player->isPlayer = true;
        dungeon->player->charRep = '@';

        for(i = 0; i < 10; i++) {
            dungeon->player->inventory.push_back(nullptr);
        }
        for(i = 0; i< 12; i++) {
            dungeon->player->equipedItems.push_back(nullptr);
        }
    } else {
        dungeon->player = player;
    }
    // Finally, place the player
    i = rand() % (dungeon->num_rooms - 1);
    dungeon->player->x = dungeon->dungeonRooms[i].x + (rand() % dungeon->dungeonRooms[i].width);
    dungeon->player->y = dungeon->dungeonRooms[i].y + (rand() % dungeon->dungeonRooms[i].height);
    
    createMonsters(dungeon);

    generateItems(dungeon);
}

void generateRooms(Dungeon *dungeon) {
    int i, j, tries, currentRoomNum = 0;
    // Generate rooms within the dungeon ...
    // Begin by initializing all rooms ...
    for(i = 0; i < dungeon->num_rooms; i++) {
        dungeon->dungeonRooms[i].x = 0;
        dungeon->dungeonRooms[i].y = 0;
        dungeon->dungeonRooms[i].width = 0;
        dungeon->dungeonRooms[i].height = 0;
    }

    // Determine initial point for the room to start ...
    for(i = 0; i < dungeon->num_rooms; i++) {
        tries = 0;
        // place the room first, then check if room can be placed, or if new room should be tested.
        do {
            if(tries > 20000) {
                dungeon->num_rooms--;
                i--;
                break;
            }
            int width = MIN_ROOM_WIDTH + (rand() % (MAX_ROOM_WIDTH - MIN_ROOM_WIDTH + 1));
            int height = MIN_ROOM_HEIGHT + (rand() % (MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT + 1));
            // determine the width and height of the new room
            dungeon->dungeonRooms[i].width = width;
            dungeon->dungeonRooms[i].height = height;

            // determine the starting cell of the room
            // The below calculations sets the starting cell position between the edges of the dungeon ...
            dungeon->dungeonRooms[i].x = 1 + rand() % (DUNGEON_WIDTH - width - 1);
            dungeon->dungeonRooms[i].y = 1 + rand() % (DUNGEON_HEIGHT - height - 1);
            currentRoomNum = i;
            tries++;
            // determine if rooms overlap ...
        } while (doRoomsOverlap(dungeon, dungeon->dungeonRooms[i], currentRoomNum));

        if(!(tries > 20000)) {
            // run along the height of the room
            for (j = dungeon->dungeonRooms[i].y; j < dungeon->dungeonRooms[i].y + dungeon->dungeonRooms[i].height; j++) {
                // run along the width of the room
                for(int k = dungeon->dungeonRooms[i].x; k < dungeon->dungeonRooms[i].x + dungeon->dungeonRooms[i].width; k++) {
                    if(dungeon->map[j][k].character == ' ') {
                        dungeon->map[j][k].character = '.';
                        dungeon->map[j][k].hardness = 0;
                    }
                }
            }
        }
    }
}

void generatePaths(Dungeon *dungeon) {
    int i;

    for(i = 0; i < dungeon->num_rooms; i++) {
        // Create the starting point within the current room
        // this is done in the centroid of the room
        int startX, startY, endX, endY, cursorX, cursorY, xDir, yDir;
        startX = dungeon->dungeonRooms[i].x + (dungeon->dungeonRooms[i].width/2);
        startY = dungeon->dungeonRooms[i].y + (dungeon->dungeonRooms[i].height/2);

        // What room will we connect to? Connect to the next room, unless its the last.
        // Then connect back to the first room ...
        if(i < dungeon->num_rooms - 1) {
            endX = dungeon->dungeonRooms[i+1].x + (dungeon->dungeonRooms[i+1].width/2);
            endY = dungeon->dungeonRooms[i+1].y + (dungeon->dungeonRooms[i+1].height/2);
        }
        else {
            endX = dungeon->dungeonRooms[0].x + (dungeon->dungeonRooms[0].width/2);
            endY = dungeon->dungeonRooms[0].y + (dungeon->dungeonRooms[0].height/2);
        }

        // decide what direction you will move the cursor in
        if(startX != endX) {
            xDir = (endX - startX)/abs(endX - startX);
        } else {
            xDir = 0;
        }
        if (endY != startY){
            yDir = (endY - startY)/abs(endY - startY);
        } else {
            yDir = 0;
        }
        cursorX = startX;
        cursorY = startY;
        // start moving the cursor from the start to the end
        // we'll be doing this one direction at a time
        for(cursorX = startX; cursorX != endX; cursorX += xDir) {
            if(dungeon->map[cursorY][cursorX].character == ' ') {
                dungeon->map[cursorY][cursorX].character = '#';
                dungeon->map[cursorY][cursorX].hardness = 0;
            }
        }
        for(cursorY = startY; cursorY != endY; cursorY += yDir) {
            if(dungeon->map[cursorY][cursorX].character == ' ') {
                dungeon->map[cursorY][cursorX].character = '#';
                dungeon->map[cursorY][cursorX].hardness = 0;
            }
        }
        // From here, the path should be placed.
    }
}

void createMonsters(Dungeon *dungeon) {
    int i = 0, j = 0, room = 0, tempX = 0, tempY = 0;

    if(!dungeon->newMonsters.empty()) {
        for(auto &monster : dungeon->newMonsters) {
            delete monster;
        }
        dungeon->newMonsters.clear();
    }

    while(dungeon->newMonsters.size() < dungeon->num_monsters) {
        int generatedRarity = rand() % 99;
        std::random_shuffle(dungeon->monsterTemplates.begin(), dungeon->monsterTemplates.end());
        for(ParsedMonster &monstTemp : dungeon->monsterTemplates) {
            if(monstTemp.getRarity() > generatedRarity && !(monstTemp.getIsUnique() && monstTemp.getIsUsed())) {
                dungeon->newMonsters.push_back(monstTemp.generateMonster());
                break;
            }
        }
    }

    for(i = 0; i < dungeon->newMonsters.size(); i++) {
        while(true) {
            // place the monster / generate a random location
            room = rand() % (dungeon->num_rooms - 1);
            tempX = dungeon->dungeonRooms[room].x + (rand() % dungeon->dungeonRooms[room].width);
            tempY = dungeon->dungeonRooms[room].y + (rand() % dungeon->dungeonRooms[room].height);

            // check to see if anything else is already there
            // first the player ....
            if(dungeon->player->x != tempX && dungeon->player->y != tempY) {
                // and then the other monsters
                for(j = 0; j < dungeon->newMonsters.size(); j++) {
                    if(dungeon->newMonsters.at(j)->x == tempX && dungeon->newMonsters.at(j)->y == tempY) {
                        continue;
                    }
                }
                dungeon->newMonsters.at(i)->x = tempX;
                dungeon->newMonsters.at(i)->y = tempY;
                break;
            }
        }
    }
}

void generateItems(Dungeon *dungeon) {
    int i = 0, j = 0, room = 0, tempX = 0, tempY = 0;

    if(!dungeon->items.empty()) {
        for(auto &item : dungeon->items) {
            delete item;
        }
        dungeon->items.clear();
    }

    while(dungeon->items.size() < NUM_GENERATED_ITEMS) {
        int generatedRarity = rand() % 99;
        std::random_shuffle(dungeon->itemTemplates.begin(), dungeon->itemTemplates.end());
        for(ParsedItem &itemTemp : dungeon->itemTemplates) {
            if(itemTemp.getRarity() > generatedRarity && !(itemTemp.getIsArtifact() && itemTemp.getIsUsed())) {
                dungeon->items.push_back(itemTemp.generateItem());
                break;
            }
        }
    }

    for(i = 0; i < dungeon->items.size(); i++) {
        while(true) {
            // place the monster / generate a random location
            room = rand() % (dungeon->num_rooms - 1);
            tempX = dungeon->dungeonRooms[room].x + (rand() % dungeon->dungeonRooms[room].width);
            tempY = dungeon->dungeonRooms[room].y + (rand() % dungeon->dungeonRooms[room].height);

            // check to see if anything else is already there
            // first the player .... and stairs....
            if(dungeon->player->x != tempX && dungeon->player->y != tempY && dungeon->upStairs.x != tempX && dungeon->upStairs.y != tempY
                && dungeon->downStairs.x != tempX && dungeon->downStairs.y != tempY) {
                // then monsters
                for(j = 0; j < dungeon->items.size(); j++) {
                    if(dungeon->items.at(j)->getX() == tempX && dungeon->items.at(j)->getY() == tempY) {
                        continue;
                    }
                }
                // place it on the free room floor space
                dungeon->items.at(i)->setPoint(tempX, tempY);
                break;
            }
        }
    }
}

// Using rectangular intersection to determine if rooms overlap
int doRoomsOverlap(Dungeon *dungeon, Room room, int currentRoomNum) {
    int l1x, l1y, r1x, r1y, l2x, l2y, r2x, r2y;
    // determining if room can be placed by generating its 2 key points: top left, bottom right
    l1x = room.x;
    l1y = room.y;
    r1x = room.x + room.width;
    r1y = room.y + room.height;
    for(int i = 0; i < currentRoomNum; i++) {
        // generating already placed rooms' key points
        // subtracting and adding one to create a guaranteed space between rooms
        l2x = dungeon->dungeonRooms[i].x - 1;
        l2y = dungeon->dungeonRooms[i].y - 1;
        r2x = dungeon->dungeonRooms[i].x + dungeon->dungeonRooms[i].width + 1;
        r2y = dungeon->dungeonRooms[i].y + dungeon->dungeonRooms[i].height + 1;
        // if any room does overlap, return false
        if(l1x > r2x || l2x > r1x) {
            // If these are met, the rooms do not overlap
        } else {
            return 1;
        }

        if(l1y < r2y || l2y < r1y) {
            // if these are met, the rooms do not overlap
        } else {
            return 1;
        }
    }
    // if all rooms do not overlap, place new room
    return 0;
}

// SAVE/LOAD function

FILE *getSaveFile(char* mode) {
    char *home = getenv("HOME");
    char *saveLocation = "/.rlg327/dungeon";
    char *fileDest = (char*)malloc(strlen(home) + strlen(saveLocation) + 1);

    strcpy(fileDest, home);
    strcat(fileDest, saveLocation);

    return fopen(fileDest, mode);
}

void saveDungeon(Dungeon *dungeon) {
    char fileMarker[] = "RLG327-S2019";
    uint32_t fileVersion = htobe32(0);
    // total offset (1708 + roomCount*4 + upwardStairs*2 + downwardStairs*2)
    uint32_t fileSize = htobe32(1712 + (dungeon->num_rooms * 4));
    uint8_t playerXPos = dungeon->player->x;
    uint8_t playerYPos = dungeon->player->y;
    int i, j;

    FILE *file = getSaveFile("wb");

    fwrite(fileMarker, sizeof(char), 12, file);
    fwrite(&fileVersion, sizeof(uint32_t), 1, file);
    fwrite(&fileSize, sizeof(uint32_t), 1, file);
    fwrite(&playerXPos, sizeof(uint8_t), 1, file);
    fwrite(&playerYPos, sizeof(uint8_t), 1, file);

    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            uint8_t cellHardness;
            cellHardness = dungeon->map[i][j].hardness;
            fwrite(&cellHardness, sizeof(uint8_t), 1, file);
        }
    }

    uint16_t numRooms = htobe16(dungeon->num_rooms);
    fwrite(&numRooms, sizeof(uint16_t), 1, file);

    // Enter in room information for each room.
    for(i = 0; i < dungeon->num_rooms; i++) {
        uint8_t roomX = dungeon->dungeonRooms[i].x;
        uint8_t roomY = dungeon->dungeonRooms[i].y;
        uint8_t roomWidth = dungeon->dungeonRooms[i].width;
        uint8_t roomHeight = dungeon->dungeonRooms[i].height;

        fwrite(&roomX, sizeof(uint8_t), 1, file);
        fwrite(&roomY, sizeof(uint8_t), 1, file);
        fwrite(&roomWidth, sizeof(uint8_t), 1, file);
        fwrite(&roomHeight, sizeof(uint8_t), 1, file);
    }
    // write the number of upward staircases, followed by the position of each staircase
    uint16_t numUpStairs = htobe16(1);
    uint8_t upStairsX = dungeon->upStairs.x;
    uint8_t upStairsY = dungeon->upStairs.y;

    fwrite(&numUpStairs, sizeof(uint16_t), 1, file);
    fwrite(&upStairsX, sizeof(uint8_t), 1, file);
    fwrite(&upStairsY, sizeof(uint8_t), 1, file);

    // write the number of downward staircases, followed by the position of these stair cases.
    uint16_t numDownStairs = htobe16(1);
    uint8_t downStairsX = dungeon->downStairs.x;
    uint8_t downStairsY = dungeon->downStairs.y;

    fwrite(&numDownStairs, sizeof(uint16_t), 1, file);
    fwrite(&downStairsX, sizeof(uint8_t), 1, file);
    fwrite(&downStairsY, sizeof(uint8_t), 1, file);

    fclose(file);
}

void loadDungeon(Dungeon *dungeon) {
    FILE *file = getSaveFile("rb");

    char fileMarker[12];
    fread(&fileMarker, sizeof(char), 12, file);

    uint32_t fileVersion;
    fread(&fileVersion, sizeof(uint32_t), 1, file);
    fileVersion = be32toh(fileVersion);

    uint32_t fileSize;
    fread(&fileSize, sizeof(uint32_t), 1, file);
    fileSize = be32toh(fileSize);

    uint8_t playerXpos, playerYpos;
    // allocating memory for the player character
    dungeon->player = (Character*)malloc(sizeof(Character));

    fread(&playerXpos, sizeof(uint8_t), 1, file);
    fread(&playerYpos, sizeof(uint8_t), 1, file);

    dungeon->player->x = playerXpos;
    dungeon->player->y = playerYpos;

    int i, j;
    uint8_t cellHardness;

    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            fread(&cellHardness, sizeof(uint8_t), 1, file);
            dungeon->map[i][j].hardness = cellHardness;
        }
    }

    // getting the number of rooms from file
    uint16_t numRooms;
    fread(&numRooms, sizeof(uint16_t), 1, file);
    dungeon->num_rooms = be16toh(numRooms);

    // allocating memory for these rooms
    dungeon->dungeonRooms = (Room*)malloc(sizeof(Room) * dungeon->num_rooms);
    // getting the data for each rooms
    for(i = 0; i < dungeon->num_rooms; i++) {
        uint8_t roomX, roomY, roomWidth, roomHeight;

        fread(&roomX, sizeof(uint8_t), 1, file);
        fread(&roomY, sizeof(uint8_t), 1, file);
        fread(&roomWidth, sizeof(uint8_t), 1, file);
        fread(&roomHeight, sizeof(uint8_t), 1, file);

        dungeon->dungeonRooms[i].x = roomX;
        dungeon->dungeonRooms[i].y = roomY;
        dungeon->dungeonRooms[i].width = roomWidth;
        dungeon->dungeonRooms[i].height = roomHeight;
    }

    // Now that we have rooms, and hardness throughout the dungeon.
    // Start building the map (this includes dungeon roof, walls, and hallways)
    // hallways are currently anything where hardness == 0
    // changing this as rooms are added.

    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            if(i == 0 || i == DUNGEON_HEIGHT - 1) {
                dungeon->map[i][j].character = '-';
            } else if (j == 0 || j == DUNGEON_WIDTH - 1) {
                dungeon->map[i][j].character = '|';
            } else if(dungeon->map[i][j].hardness == 0) {
                dungeon->map[i][j].character = '#';
            } else {
                dungeon->map[i][j].character = ' ';
            }
        }
    }

    // Placing the rooms
    int count;
    for(count = 0; count < dungeon->num_rooms; count++) {
        for(i = dungeon->dungeonRooms[count].y; i < dungeon->dungeonRooms[count].y + dungeon->dungeonRooms[count].height; i++) {
            for(j = dungeon->dungeonRooms[count].x; j < dungeon->dungeonRooms[count].x + dungeon->dungeonRooms[count].width; j++) {
                dungeon->map[i][j].character = '.';
            }
        }
    }

    // get the number of stairs going up
    uint16_t numUpStairs;
    fread(&numUpStairs, sizeof(uint16_t), 1, file);
    numUpStairs = be16toh(numUpStairs);

    // get the locations of these stairs and place them
    uint8_t stairsX, stairsY;
    for(count = 0; count < numUpStairs; count++) {
        fread(&stairsX, sizeof(uint8_t), 1, file);
        fread(&stairsY, sizeof(uint8_t), 1, file);
        dungeon->upStairs.x = stairsX;
        dungeon->upStairs.y = stairsY;
        dungeon->map[stairsY][stairsX].character = '<';
    }

    // get the number of stairs going up
    uint16_t numDownStairs;
    fread(&numDownStairs, sizeof(uint16_t), 1, file);
    numDownStairs = be16toh(numDownStairs);

    // get the locations of these stairs and place them
    for(count = 0; count < numDownStairs; count++) {
        fread(&stairsX, sizeof(uint8_t), 1, file);
        fread(&stairsY, sizeof(uint8_t), 1, file);
        dungeon->downStairs.x = stairsX;
        dungeon->downStairs.y = stairsY;
        dungeon->map[stairsY][stairsX].character = '>';
    }

    dungeon->map[dungeon->player->y][dungeon->player->x].character = '@';

    fclose(file);

}

// This function is meant to represent djikstra's algorithm
void buildDistanceMap(Dungeon *dungeon, int tunneler, int distanceMap[DUNGEON_HEIGHT][DUNGEON_WIDTH]) {
    int i, j;

    for (i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            // setting the value of this to int32_max since we cannot hit infinity realistically
            distanceMap[i][j] = INT32_MAX;
        }
    }

    // setting the value of the player's location to 0
    distanceMap[dungeon->player->y][dungeon->player->x] = 0;

    // initialize queue and add players location
    Queue *queue = initializeQueue();
    addQueue(queue, initializeQueueNode(dungeon->player->x, dungeon->player->y, 0));

    while(queue->size != 0) {
        QueueNode p = popQueue(queue);
        int uX, uY, vX, vY;

        uX = p.x;
        uY = p.y;

        // go through all of the cells surrounding the current node
        for(i = -1; i < 2; i++) {
            for(j = -1; j < 2; j++) {
                if(!(i == 0 && j == 0)) {
                    vX = uX + i;
                    vY = uY + j;

                    char dungeonCell = dungeon->map[vY][vX].character;
                    // if one cannot move through cell, continue to the next value.
                    if (dungeonCell == '|' || dungeonCell == '-' || (tunneler == 0 && dungeonCell == ' ') || vX >= DUNGEON_WIDTH || vY >= DUNGEON_HEIGHT) {
                        continue;
                    }
                    // Applying distance from player's cell (u) with modifiers in placed based on hardness of the particular tile
                    int distance = distanceMap[uY][uX] + (dungeon->map[uY][uX].hardness / 85) + 1;

                    //Determining if we can move to the next node to determine distance there ...
                    if(distanceMap[vY][vX] > distance) {
                        distanceMap[vY][vX] = distance;
                        // add the cell to the queue
                        addQueue(queue, initializeQueueNode(vX, vY, distance));
                    }
                }
            }
        }
    }


    Map printMap[DUNGEON_HEIGHT][DUNGEON_WIDTH];
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            if(i == 0 || i == DUNGEON_HEIGHT -1) {
                printMap[i][j].character = '-';
            } else if(j == 0 || j == DUNGEON_WIDTH - 1) {
                printMap[i][j].character = '|';
            } else {
                if(tunneler == 0 && dungeon->map[i][j].character == ' ') {
                    printMap[i][j].character = ' ';
                } else {
                    printMap[i][j].character = '0' + (distanceMap[i][j] % 10);
                    if(distanceMap[i][j] == INT32_MAX) {
                        printMap[i][j].character = 'X';
                    }
                }
            }

            if(distanceMap[i][j] == 0) {
                printMap[i][j].character = '@';
            }
        }
    }

//    // Print the dungeon layout
//    for (i = 0; i < DUNGEON_HEIGHT; i++) {
//        for (j = 0; j < DUNGEON_WIDTH; j++) {
//            printf("%c", printMap[i][j].character);
//        }
//        printf("\n");
//    }

}

// Character Movement Functions
void moveCharacter(Character *character, Dungeon *dungeon,
                   int tunnelerMap[DUNGEON_HEIGHT][DUNGEON_WIDTH], int nonTunnelersMap[DUNGEON_HEIGHT][DUNGEON_WIDTH]) {
    int i = 0, j = 0, tempX = 0, tempY = 0;
    int spacePriority = INT32_MAX;

    // if monster is erratic, 50% of the time, it doesnt care where the player is
    if(character->attributes & Monster::ERRATIC) {
        int randomMovement = rand() % 2;
        if(randomMovement) {
            while(TRUE) {
                int randX = 0, randY = 0;
                while(randX == 0 && randY == 0) {
                    randX = (rand() % 3) - 1;
                    randY = (rand() % 3) - 1;
                }
                tempX = character->x + randX;
                tempY = character->y + randY;

                // check if it can move here
                // if yes, move there
                if(isMovementPossible(dungeon, tempX, tempY, (character->attributes & Monster::TUNNEL))) {  
                    // check if it killed a monster
                    for(i = 0; i < dungeon->num_monsters; i++){
                        // ignore the monster if it is the character
                        if(dungeon->newMonsters.at(i) != character) {
                            // check if a monster was here
                            if (dungeon->newMonsters.at(i)->x == tempX && dungeon->newMonsters.at(i)->y == tempY) {
                                attackCharacter(dungeon, character, dungeon->newMonsters.at(i));
                                if(!dungeon->newMonsters.at(i)->isLiving) {
                                    character->x = tempX;
                                    character->y = tempY;
                                    return;
                                } else {
                                    return;
                                }
                            }
                        }
                    }
                    // Also check to see if it killed the player
                    if(dungeon->player->x == tempX && dungeon->player->y == tempY) {
                        attackCharacter(dungeon, character, dungeon->player);
                        if(!dungeon->player->isLiving) {
                            character->x = tempX;
                            character->y = tempY;
                            return;
                        } else {
                            return;
                        }
                    }

                    character->x = tempX;
                    character->y = tempY;

                    // change rock into corridor
                    if(dungeon->map[character->y][character->x].character == ' ') {
                        dungeon->map[character->y][character->x].character = '#';
                    }
                    // character has moved ...
                    return;
                } // else carry on with loop
            }
        }
    }

    // if a character is telepathic, it always knows where player is ...
    int bestX = 0, bestY = 0;
    int tempCharPosX = 0, tempCharPosY = 0;
    if(character->attributes & Monster::TELE) {
        // these characters can still be unintelligent, however ...
        if(character->attributes & Monster::SMART) {
            for(i = -1; i < 2; i++) {
                for(j = -1; j < 2; j++) {
                    if(!(i == 0 && j == 0)) {
                        tempX = character->x + i;
                        tempY = character->y + j;
                        // check if character can tunnel and choose respective map
                        if(character->attributes & Monster::TUNNEL) {
                            // check if priority is better
                            if(spacePriority > tunnelerMap[tempY][tempX]) {
                                spacePriority = tunnelerMap[tempY][tempX];
                                bestX = tempX;
                                bestY = tempY;
                            }
                        } else {
                            if(spacePriority > nonTunnelersMap[tempY][tempX]) {
                                spacePriority = nonTunnelersMap[tempY][tempX];
                                bestX = tempX;
                                bestY = tempY;
                            }
                        }
                    }
                }
            }
        } else {  // character is unintelligent
            bestX = character->x;
            bestY = character->y;
        }
    } else { // character is not telepathic
        bestX = character->x;
        bestY = character->y;
    }

    // check if it killed a monster
    for(i = 0; i < dungeon->num_monsters; i++){
        // ignore the monster if it is the character
        if(dungeon->newMonsters.at(i) != character) {
            // check if a monster was here
            if (dungeon->newMonsters.at(i)->x == bestX && dungeon->newMonsters.at(i)->y == bestY) {
                attackCharacter(dungeon, character, dungeon->newMonsters.at(i));
                if(!dungeon->newMonsters.at(i)->isLiving) {
                    character->x = bestX;
                    character->y = bestY;
                    return;
                } else {
                    return;
                }
            }
        }
    }
    // Also check to see if it killed the player
    if(dungeon->player->x == bestX && dungeon->player->y == bestY) {
        attackCharacter(dungeon, character, dungeon->player);
        if(!dungeon->player->isLiving) {
            character->x = bestX;
            character->y = bestY;
            return;
        } else {
            return;
        }
    }

    character->x = bestX;
    character->y = bestY;
    
    // change rock into corridor
    if(dungeon->map[character->y][character->x].character == ' ') {
        dungeon->map[character->y][character->x].character = '#';
    }
    // character has moved ...
}

int isMovementPossible(Dungeon *dungeon, int x, int y, int isTunneler) {
    char c = dungeon->map[y][x].character;
    if(isTunneler && (c == ' ' || c == '.' || c == '#' || c == '>' || c == '<')) {
        return TRUE;
    } else if (!isTunneler && (c == '.' || c == '#' || c == '>' || c == '<')) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void movePlayerCharacter(Dungeon *dungeon, int x, int y) {
    // check if player killed a monster
    for(int i = 0; i < dungeon->num_monsters; i++){
       if(dungeon->newMonsters.at(i)->x == x && dungeon->newMonsters.at(i)->y == y && dungeon->newMonsters.at(i)->isLiving) {
            attackCharacter(dungeon, dungeon->player, dungeon->newMonsters.at(i));
            if(!dungeon->newMonsters.at(i)->isLiving) {
                dungeon->player->x = x;
                dungeon->player->y = y;
                return;
            } else {
                return;
            }
        }
    }

    dungeon->player->x = x;
    dungeon->player->y = y;
}

int characterPlayerAction(Dungeon *dungeon, char messageString[DUNGEON_WIDTH]) {
    int getChValue, tempX, tempY;

    // return or break to leave while loop after movement or quit
    // however, stay in the loop for menu options
    while(TRUE) {
        getChValue = getch();

        if(getChValue == 'q') {
            // exit program
            printf("Exiting the program! Press any button to continue.\n");
            stopProgram();
            return 0;
        } else if(getChValue == 'm') {
            // open monster list
            displayMonsterList(dungeon);
            // do not return because player can still move
        } else if(getChValue == '9' || getChValue == 'u' || getChValue == KEY_PPAGE) {
            // move character to the upper right
            tempX = dungeon->player->x + 1;
            tempY = dungeon->player->y - 1;
            if(isMovementPossible(dungeon, tempX, tempY, 0)) {
                movePlayerCharacter(dungeon, tempX, tempY);
                return 0;
            } else {
                strcpy(messageString, "This is an invalid move!");
                printDungeon(dungeon, messageString);
                //do a print message for the player to know the move is invalid
            }
        } else if(getChValue == '8' || getChValue == 'k' || getChValue == KEY_UP) {
            // move character upward
            tempX = dungeon->player->x;
            tempY = dungeon->player->y - 1;
            if(isMovementPossible(dungeon, tempX, tempY, 0)) {
                movePlayerCharacter(dungeon, tempX, tempY);
                return 0;
            } else {
                //do a print message for the player to know the move is invalid
                strcpy(messageString, "This is an invalid move!");
                printDungeon(dungeon, messageString);
            }
        } else if(getChValue == '7' || getChValue == 'y' || getChValue == KEY_HOME) {
            // move to the upper left
            tempX = dungeon->player->x - 1;
            tempY = dungeon->player->y - 1;
            if(isMovementPossible(dungeon, tempX, tempY, 0)) {
                movePlayerCharacter(dungeon, tempX, tempY);
                return 0;
            } else {
                //do a print message for the player to know the move is invalid
                strcpy(messageString, "This is an invalid move!");
                printDungeon(dungeon, messageString);
            }
        } else if(getChValue == '6' || getChValue == 'l' || getChValue == KEY_RIGHT) {
            // move to the right
            tempX = dungeon->player->x + 1;
            tempY = dungeon->player->y;
            if(isMovementPossible(dungeon, tempX, tempY, 0)) {
                movePlayerCharacter(dungeon, tempX, tempY);
                return 0;
            } else {
                //do a print message for the player to know the move is invalid
                strcpy(messageString, "This is an invalid move!");
                printDungeon(dungeon, messageString);
            }
        } else if(getChValue == '5' || getChValue == '.' || getChValue == ' ') {
            // stand still 
            //strcpy(messageString, "Standing still is a bold strategy.");
            return 0;
        } else if(getChValue == '4' || getChValue == 'h' || getChValue == KEY_LEFT) {
            // move to the left
            tempX = dungeon->player->x - 1;
            tempY = dungeon->player->y;
            if(isMovementPossible(dungeon, tempX, tempY, 0)) {
                movePlayerCharacter(dungeon, tempX, tempY);
                return 0;
            } else {
                //do a print message for the player to know the move is invalid
                strcpy(messageString, "This is an invalid move!");
                printDungeon(dungeon, messageString);
            }
        } else if(getChValue == '3' || getChValue == 'n' || getChValue == KEY_NPAGE) {
            // move to the lower right
            tempX = dungeon->player->x + 1;
            tempY = dungeon->player->y + 1;
            if(isMovementPossible(dungeon, tempX, tempY, 0)) {
                movePlayerCharacter(dungeon, tempX, tempY);
                return 0;
            } else {
                //do a print message for the player to know the move is invalid
                strcpy(messageString, "This is an invalid move!");
                printDungeon(dungeon, messageString);
            }
        } else if(getChValue == '2' || getChValue == 'j' || getChValue == KEY_DOWN) {
            // move downward
            tempX = dungeon->player->x;
            tempY = dungeon->player->y + 1;
            if(isMovementPossible(dungeon, tempX, tempY, 0)) {
                movePlayerCharacter(dungeon, tempX, tempY);
                return 0;
            } else {
                //do a print message for the player to know the move is invalid
                strcpy(messageString, "This is an invalid move!");
                printDungeon(dungeon, messageString);
            }
        } else if(getChValue == '1' || getChValue == 'b' || getChValue == KEY_END) {
            // move to the lower left
            tempX = dungeon->player->x - 1;
            tempY = dungeon->player->y + 1;
            if(isMovementPossible(dungeon, tempX, tempY, 0)) {
                movePlayerCharacter(dungeon, tempX, tempY);
                strcpy(messageString, "Are you sure you want to go that way?");
                return 0;
            } else {
                //do a print message for the player to know the move is invalid
                strcpy(messageString, "This is an invalid move!");
                printDungeon(dungeon, messageString);
            }
        } else if(getChValue == '>' && dungeon->downStairs.x == dungeon->player->x && dungeon->downStairs.y == dungeon->player->y) {
            // player is going downstairs
            return 1;
        } else if(getChValue == '<' && dungeon->upStairs.x == dungeon->player->x && dungeon->upStairs.y == dungeon->player->y) {
            // player is going upstairs
            return 1;
        } else if(getChValue == 'f') {
            fogOfWar = !fogOfWar;
            printDungeon(dungeon, messageString);
        } else if(getChValue == 'g') {
            teleportCheat(dungeon);
            updateFogOfWarMap(dungeon);
            printDungeon(dungeon, messageString);
        } else if(getChValue == 'i') {
            displayCharacterInventory(dungeon);
        } else if(getChValue == 'e') {
            displayCharacterEquipment(dungeon);
        } else if(getChValue == 'w') {
            wearItem(dungeon);
            return 0;
        } else if(getChValue == 't') {
            // take off an item
        } else if(getChValue == 'd') {
            // drop an item to inventory
        } else if(getChValue == 'x') {
            // expunge from game
        } else if(getChValue == 'I') {
            displayItemInfo(dungeon);
            return 0;
        } else if(getChValue == 'L') {
            targetAndDisplayMonsterInfo(dungeon);
        } else if(getChValue == ',') {
            pickupItem(dungeon);
            return 0;
        } else if(getChValue == 'r') {
            rangedAttack(dungeon);
            printDungeon(dungeon, messageString);
            return 0;
        }
    }
}

void rangedAttack(Dungeon *dungeon) {
    int tempX, tempY;
    int currX = dungeon->player->x;
    int currY = dungeon->player->y;
    strcpy(messageString, "(q) to quit rangedAttack, (r) to commit rangedAttack, (p) to cast PoisonBall");

    while(true) {
        printDungeon(dungeon, messageString);
        mvaddch(currY + 1, currX, '*');
        refresh();

        int getChValue = getch();
        if(getChValue == '9' || getChValue == 'u' || getChValue == KEY_PPAGE) {
            // move character to the upper right
            tempX = currX + 1;
            tempY = currY - 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '8' || getChValue == 'k' || getChValue == KEY_UP) {
            // move character upward
            tempX = currX;
            tempY = currY - 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '7' || getChValue == 'y' || getChValue == KEY_HOME) {
            // move to the upper left
            tempX = currX - 1;
            tempY = currY - 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '6' || getChValue == 'l' || getChValue == KEY_RIGHT) {
            // move to the right
            tempX = currX + 1;
            tempY = currY;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '4' || getChValue == 'h' || getChValue == KEY_LEFT) {
            // move to the left
            tempX = currX - 1;
            tempY = currY;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '3' || getChValue == 'n' || getChValue == KEY_NPAGE) {
            // move to the lower right
            tempX = currX + 1;
            tempY = currY + 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '2' || getChValue == 'j' || getChValue == KEY_DOWN) {
            // move downward
            tempX = currX;
            tempY = currY + 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '1' || getChValue == 'b' || getChValue == KEY_END) {
            // move to the lower left
            tempX = currX - 1;
            tempY = currY + 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == 'r') {
            if(dungeon->player->equipedItems.at(2) != nullptr) {
                bool found = false;
                for(auto monster : dungeon->newMonsters) {
                    if(monster->x == currX && monster->y == currY) {
                        int damage = 0;
                        for(auto &item : dungeon->player->equipedItems) {
                            if(item != nullptr && !(item->getType() & ParsedItem::WEAPON) && !(item->getType() & ParsedItem::OFFHAND)) {
                                damage += item->getDamageBonus().rollDice();
                            }
                        }
                        monster->hp -= damage;
                        message = monster->getName() + " attacked by player for " + std::to_string(damage) + ". HP is " + std::to_string(monster->hp);
                        char cstr[message.size() + 1];
                        strcpy(cstr, message.c_str());
                        strcpy(messageString, cstr);
                        if(monster->hp <= 0) {
                            monster->isLiving = false;
                            if(monster->attributes & Monster::BOSS) {
                                strcpy(messageString, "You've ... won? Good job for doing the impossible. ");
                                printDungeon(dungeon, messageString);
                                stopProgram();
                            }
                        }
                        found = true;
                    }
                }
                if(!found) {
                    strcpy(messageString, "No monster! Select a monster to attack!");
                    continue;
                }
                break;
            } else {
                strcpy(messageString, "You do not have a ranged weapon equipped!");
                printDungeon(dungeon, messageString);
            }
        } else if(getChValue == 'p') {
            bool found = false;
            for(auto monster : dungeon->newMonsters) {
                if(monster->x == currX && monster->y == currY) {
                    found = true;
                    Dice poisonDamage = Dice(0, 2, 8); //0+2d8
                    int damage = poisonDamage.rollDice();
                    int i, j;
                    for(i = -1; i < 2; i++) {
                        for(j = -1; j < 2; j++) {
                            for(auto &monster : dungeon->newMonsters) {
                                if(monster->x == (currX + i) && monster->y ==(currY + j)) {
                                    monster->hp -= damage;
                                    message = monster->getName() + " was poison-balled by player for " + std::to_string(damage) + ". HP is " + std::to_string(monster->hp);
                                    char cstr[message.size() + 1];
                                    strcpy(cstr, message.c_str());
                                    strcpy(messageString, cstr);
                                    if(monster->hp <= 0) {
                                        monster->isLiving = false;
                                        if(monster->attributes & Monster::BOSS) {
                                            strcpy(messageString, "You've ... won? Good job for doing the impossible. ");
                                            printDungeon(dungeon, messageString);
                                            stopProgram();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if(!found) {
                strcpy(messageString, "No monster! Select a monster to attack!");
                continue;
            }
            break;
        } else if(getChValue == 27 || getChValue == 'q') {
            printDungeon(dungeon, "Exited ranged attack ...");
            break;
        }
    }
}

void attackCharacter(Dungeon *dungeon, Character *attacker, Character *defender) {
    // if attacker is player, he hurts all monsters
    if(attacker->isPlayer) {
        Monster *monster = (Monster*)defender;
        int damage = attacker->damage.rollDice();
        for(auto item : attacker->equipedItems) {
            if(item != nullptr) {
                damage += item->getDamageBonus().rollDice();
            }
        }
        monster->hp -= damage;
        message = monster->getName() + " attacked by player for " + std::to_string(damage) + ". HP is " + std::to_string(monster->hp);
        char cstr[message.size() + 1];
	    strcpy(cstr, message.c_str());
        strcpy(messageString, cstr);
        if(monster->hp <= 0) {
            monster->isLiving = false;
            if(monster->attributes & Monster::BOSS) {
                strcpy(messageString, "You've ... won? Good job for doing the impossible. ");
                printDungeon(dungeon, messageString);
                stopProgram();
            }
        }
        // else, it is a monsters. Monsters do not hurt other monsters.
    } else {
        Monster *monsterAttacker = (Monster*)attacker;
        // if target is player, deal damage
        if(defender->isPlayer) {
            int damage = monsterAttacker->damage.rollDice();
            defender->hp -= damage;
            message = "Player attacked by" + monsterAttacker->getName() + " for " + std::to_string(damage) + ". HP is " + std::to_string(defender->hp);
            char cstr[message.size() + 1];
	        strcpy(cstr, message.c_str());
            strcpy(messageString, cstr);
            defender->hp -+ monsterAttacker->damage.rollDice();
            if(defender->hp <= 0) {
                defender->isLiving = false;
            }
        // else, target is a monster. repositon defender to adjacent 
        } else {
            int tempX, tempY;
            tempX = defender->x;
            tempY = defender->y;
            defender->x = monsterAttacker->x;
            defender->y = monsterAttacker->y;
            monsterAttacker->x = tempX;
            monsterAttacker->y = tempY;
        }
    }
}

void stopProgram() {
    // requiring input to ensure player sees death, loss, and win messages. 
    getch();

    echo();
    clear();
    endwin();
    exit(0);
}

void displayMonsterList(Dungeon *dungeon) {
    char monsterInfo[dungeon->num_monsters][60];
    int i, j, xDistance, yDistance;
    char xinfo[15], yinfo[15];

    for(i = 0; i < dungeon->num_monsters; i++) {
        if(dungeon->newMonsters.at(i)->isLiving) {
            xDistance = dungeon->player->x - dungeon->newMonsters.at(i)->x;
            yDistance = dungeon->player->y - dungeon->newMonsters.at(i)->y;

            if(yDistance >= 0) {
                snprintf(yinfo, 20, "%d North", yDistance);
            } else {
                yDistance *= -1;
                snprintf(yinfo, 20, "%d South", yDistance);
            }

            if(xDistance >= 0) {
                snprintf(xinfo, 20, "%d West", xDistance);
            } else {
                xDistance *= -1;
                snprintf(xinfo, 20, "%d East", xDistance);
            }
            snprintf(monsterInfo[i], 60, "A %c: %s by %s", dungeon->newMonsters.at(i)->charRep, yinfo, xinfo);
        } else {
            snprintf(monsterInfo[i], 60, "A %c is dead!         ", dungeon->newMonsters.at(i)->charRep);
        }
    }

    int monstersInList = 5;
    int listOffset = 0;

    while(TRUE) {
        clear();
        mvprintw(0, 20, "   Monster List (Total Monsters: %d)   ", dungeon->num_monsters);
        for(i = 1, j = listOffset; i < monstersInList; i++, j++) {
            if(i < dungeon->num_monsters + 1) {
                mvprintw(i, 20, "    %s   ", monsterInfo[j]);
            }
        }

        mvprintw(i + 1, 20, "Press ESC to exit the list!");
        mvprintw(i + 2, 20, "Press UP or DOWN to navigate the list!");

        int getChValue = getch();

        if(getChValue == 27 || getChValue == 'q') {
            printDungeon(dungeon, "Exited monster list ...");
            break;
        } else if(getChValue == KEY_UP && listOffset != 0) {
            listOffset--;
        } else if(getChValue == KEY_DOWN && (dungeon->num_monsters - listOffset + 1) > monstersInList) {
            listOffset++;
        }
    }
}

void updateFogOfWarMap(Dungeon *dungeon) {
    int i, j;
    int playerX = dungeon->player->x;
    int playerY = dungeon->player->y;
    int radius = 2;

    Item *item = dungeon->player->equipedItems.at(9);
    if(item != nullptr) {
        radius += item->getSpecAttr();
    }
    for(i = 0; i < radius * 2 + 1; i++) {
        for(j = 0; j < radius * 2 + 1; j++) {
            int tempX = playerX + (radius - j);
            int tempY = playerY + (radius - i);

            if(tempX < DUNGEON_WIDTH - 1 && tempX >= 0 && tempY < DUNGEON_HEIGHT - 1 && tempY >= 0) {
                dungeon->player->playerMap[tempY][tempX].character = dungeon->map[tempY][tempX].character;
            }
        }
    }
}

void teleportCheat(Dungeon *dungeon) {
    int tempX, tempY;
    int currX = dungeon->player->x;
    int currY = dungeon->player->y;

    while(true) {
        fogOfWar = false;
        printDungeon(dungeon, "Are you sure teleporting is wise?");
        mvaddch(currY + 1, currX, '*');
        refresh();

        int getChValue = getch();
        if(getChValue == '9' || getChValue == 'u' || getChValue == KEY_PPAGE) {
            // move character to the upper right
            tempX = currX + 1;
            tempY = currY - 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '8' || getChValue == 'k' || getChValue == KEY_UP) {
            // move character upward
            tempX = currX;
            tempY = currY - 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '7' || getChValue == 'y' || getChValue == KEY_HOME) {
            // move to the upper left
            tempX = currX - 1;
            tempY = currY - 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '6' || getChValue == 'l' || getChValue == KEY_RIGHT) {
            // move to the right
            tempX = currX + 1;
            tempY = currY;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '4' || getChValue == 'h' || getChValue == KEY_LEFT) {
            // move to the left
            tempX = currX - 1;
            tempY = currY;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '3' || getChValue == 'n' || getChValue == KEY_NPAGE) {
            // move to the lower right
            tempX = currX + 1;
            tempY = currY + 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '2' || getChValue == 'j' || getChValue == KEY_DOWN) {
            // move downward
            tempX = currX;
            tempY = currY + 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '1' || getChValue == 'b' || getChValue == KEY_END) {
            // move to the lower left
            tempX = currX - 1;
            tempY = currY + 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == 't') {
            movePlayerCharacter(dungeon, currX, currY);
            fogOfWar = true;
            return;
        } else if(getChValue == 'r') {
            movePlayerCharacter(dungeon, 1 + (rand() % 78), 1 + (rand() % 19));
            fogOfWar = true;
            return;
        }
    }
}

void displayCharacterInventory(Dungeon *dungeon) {
    std::vector<Item*> itemList = dungeon->player->inventory;

    while(true) {
        clear();

        mvprintw(0, 20, "Inventory");
        for(int i = 0; i < itemList.size(); i++) {
            std::string item;

            item += std::to_string(i);
            if(itemList.at(i) == nullptr) {
                item += ". No Item in Slot";
                mvprintw(i + 1, 20, item.c_str());
            } else {
                item += ". " + itemList.at(i)->getName();
            }
            mvprintw(i + 1, 20, item.c_str());
        }

        int getChValue = getch();

        if(getChValue == 27 || getChValue == 'q') {
            printDungeon(dungeon, "Exited Inventory ...");
            break;
        } 
    }
}

void displayCharacterEquipment(Dungeon *dungeon) {
    std::vector<Item*> equipmentList = dungeon->player->equipedItems;
    while(true) {
        clear();

        mvprintw(0, 20, "Equipped Items");
        for(int i = 0; i < equipmentList.size(); i++) {
            std::string item;

            item += (char)('a' + i);
            if(equipmentList.at(i) == nullptr) {
                item += ". No Item in Slot";
            } else {
                item += ". " + (std::to_string(equipmentList.at(i)->getType()) + " item: " + equipmentList.at(i)->getName());
            }
            mvprintw(i + 1, 20, item.c_str());
        }

        int getChValue = getch();

        if(getChValue == 27 || getChValue == 'q') {
            printDungeon(dungeon, "Exited Equipped Item List ...");
            break;
        } 
    }
}

void targetAndDisplayMonsterInfo(Dungeon *dungeon) {
    int tempX, tempY;
    int currX = dungeon->player->x;
    int currY = dungeon->player->y;
    // fogOfWar = false;
    strcpy(messageString, "Spying on some monsters, eh?");

    while(true) {
        printDungeon(dungeon, messageString);
        mvaddch(currY + 1, currX, '*');
        refresh();

        int getChValue = getch();
        if(getChValue == '9' || getChValue == 'u' || getChValue == KEY_PPAGE) {
            // move character to the upper right
            tempX = currX + 1;
            tempY = currY - 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '8' || getChValue == 'k' || getChValue == KEY_UP) {
            // move character upward
            tempX = currX;
            tempY = currY - 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '7' || getChValue == 'y' || getChValue == KEY_HOME) {
            // move to the upper left
            tempX = currX - 1;
            tempY = currY - 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '6' || getChValue == 'l' || getChValue == KEY_RIGHT) {
            // move to the right
            tempX = currX + 1;
            tempY = currY;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '4' || getChValue == 'h' || getChValue == KEY_LEFT) {
            // move to the left
            tempX = currX - 1;
            tempY = currY;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '3' || getChValue == 'n' || getChValue == KEY_NPAGE) {
            // move to the lower right
            tempX = currX + 1;
            tempY = currY + 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '2' || getChValue == 'j' || getChValue == KEY_DOWN) {
            // move downward
            tempX = currX;
            tempY = currY + 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == '1' || getChValue == 'b' || getChValue == KEY_END) {
            // move to the lower left
            tempX = currX - 1;
            tempY = currY + 1;
            if(isMovementPossible(dungeon, tempX, tempY, 1)) {
                currX = tempX;
                currY = tempY;
            }
        } else if(getChValue == 't') {
            bool found = false;
            for(auto monster : dungeon->newMonsters) {
                if(monster->x == currX && monster->y == currY) {
                    found = true;
                    clear();

                    mvprintw(0, 20, monster->getName().c_str());
                    for(int i = 0; i < monster->getDescription().size(); i++) {
                        std::string line = monster->getDescription().at(i);
                        mvprintw(i + 1, 0, line.c_str());
                    }

                    int getChVal = getch();

                    if(getChVal == 27 || getChVal == 'q') {
                        strcpy(messageString, "Select a monster or press q to exit!");
                    } 
                }
            }

            if(!found) {
                strcpy(messageString, "Select a monster or press q to quit!");
            }
        } else if(getChValue == 27 || getChValue == 'q') {
            printDungeon(dungeon, "Exited Monster Selection ...");
            break;
        } 
    }
}

void pickupItem(Dungeon *dungeon) {
    Item *tempItem = nullptr;
    for(int i = 0; i < (int)dungeon->items.size(); i++) {
        if(dungeon->items.at(i)->getX() == dungeon->player->x && dungeon->items.at(i)->getY() == dungeon->player->y) {
            tempItem = dungeon->items.at(i);
        } 
        
        if(tempItem == nullptr) {
            continue;
        }
        // strcpy(messageString, "test");
        // printDungeon(dungeon, messageString);
        
        for(int j = 0; j < (int)dungeon->player->inventory.size(); j++) {
        //     strcpy(messageString, "test");
        // printDungeon(dungeon, messageString);
            if(dungeon->player->inventory.at(j) == nullptr) {
                // sometime failing here.
                dungeon->items.erase(dungeon->items.begin() + i);
                dungeon->player->inventory.at(j) = tempItem;
                dungeon->player->inventory.at(j)->setPoint(-1, -1);
                std::string temp = "Picked up " + dungeon->player->inventory.at(j)->getName();
                strcpy(messageString, temp.c_str());
                return;
            }
        }
        strcpy(messageString, "You have no space in your inventory!");
        return;
    }
    strcpy(messageString, "There is no item here!");
}

void wearItem(Dungeon *dungeon) {
    std::vector<Item*> itemList = dungeon->player->inventory;

    while(true) {
        clear();

        mvprintw(0, 20, "Select an item to wear");
        for(int i = 0; i < itemList.size(); i++) {
            std::string item;

            item += std::to_string(i);
            if(itemList.at(i) == nullptr) {
                item += ". No Item in Slot";
                mvprintw(i + 1, 20, item.c_str());
            } else {
                item += ". " + itemList.at(i)->getName();
            }
            mvprintw(i + 1, 20, item.c_str());
        }

        int getChValue = getch();

        int getChValue2 = getChValue - '0';
        if(getChValue == 27 || getChValue == 'q') {
            strcpy(messageString, "You did not equip an item!");
            break;
        } else if (getChValue >= '0' && getChValue < ('0' + dungeon->player->inventory.size()) 
            && dungeon->player->inventory.at(getChValue2) != nullptr) {
            Item *equipItem = dungeon->player->inventory.at(getChValue2);
            int type = equipItem->getType();
            int slot;

            if(type & ParsedItem::WEAPON) {
                slot = 0;
            } else if(type & ParsedItem::OFFHAND) {
                slot = 1;
            } else if(type & ParsedItem::RANGED) {
                slot = 2;
            } else if(type & ParsedItem::ARMOR) {
                slot = 3;
            } else if(type & ParsedItem::HELMET) {
                slot = 4;
            } else if(type & ParsedItem::CLOAK) {
                slot = 5;
            } else if(type & ParsedItem::GLOVES) {
                slot = 6;
            } else if(type & ParsedItem::BOOTS) {
                slot = 7;
            } else if(type & ParsedItem::AMULET) {
                slot = 8;
            } else if(type & ParsedItem::LIGHT) {
                slot = 9;
            } else if(type & ParsedItem::RING) {
                // act differently due to two slots
                // for now, 
                slot = 10;
            } else {
                break;
            }

            dungeon->player->inventory.at(getChValue2) = dungeon->player->equipedItems.at(slot);
            dungeon->player->equipedItems.at(slot) = equipItem;
            strcpy(messageString, "Equipped item!");
            break;
        }
    }
}

void displayItemInfo(Dungeon *dungeon) {
    std::vector<Item*> itemList = dungeon->player->inventory;

    while(true) {
        clear();

        mvprintw(0, 20, "Select an item to view");
        for(int i = 0; i < itemList.size(); i++) {
            std::string item;

            item += std::to_string(i);
            if(itemList.at(i) == nullptr) {
                item += ". No Item in Slot";
                mvprintw(i + 1, 20, item.c_str());
            } else {
                item += ". " + itemList.at(i)->getName();
            }
            mvprintw(i + 1, 20, item.c_str());
        }

        int getChValue = getch();

        int getChValue2 = getChValue - '0';
        if(getChValue == 27 || getChValue == 'q') {
            strcpy(messageString, "Exiting Item Viewer ...");
            break;
        } else if (getChValue >= '0' && getChValue < ('0' + dungeon->player->inventory.size()) 
            && dungeon->player->inventory.at(getChValue2) != nullptr) {
           
                clear();
            Item *item = dungeon->player->inventory.at(getChValue2);

            mvprintw(0, 20, item->getName().c_str());
            for(int i = 0; i < item->getDescript().size(); i++) {
                std::string line = item->getDescript().at(i);
                mvprintw(i + 1, 0, line.c_str());
            }

            int getChVal = getch();

            if(getChVal == 27 || getChVal == 'q') {
                strcpy(messageString, "Exited item description ...");
            } 
        }
    }
}