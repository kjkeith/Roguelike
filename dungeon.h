#ifndef DUNGEON_H
#define DUNGEON_H

class Monster;
class Character;

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "Dice.h"
#include "ItemParser.h"

#pragma once

#define DUNGEON_WIDTH 80
#define DUNGEON_HEIGHT 21

#define MIN_ROOM_WIDTH 4
#define MAX_ROOM_WIDTH 10
#define MIN_ROOM_HEIGHT 3
#define MAX_ROOM_HEIGHT 7

#define MIN_ROOM_COUNT 6 
#define MAX_ROOM_COUNT 10

#define NUM_GENERATED_ITEMS 10

class Room {
    public:
    int x, y, width, height;
};

class Stairs {
    public:
    int x, y;
};

class Map {
    public:
    char character;
    int hardness;
};

class Character {
    public:
    int x, y, speed, attributes;
    bool isLiving, isPlayer;
    char charRep;
    Map playerMap[DUNGEON_HEIGHT][DUNGEON_WIDTH];
    int hp;
    Dice damage;
    int turnPriority;
    std::vector<Item*> inventory;
    std::vector<Item*> equipedItems;
};

class ParsedMonster;

class Dungeon {
    public:
    Stairs downStairs, upStairs;
    int num_rooms, num_monsters;
    Map map[DUNGEON_HEIGHT][DUNGEON_WIDTH];
    Room *dungeonRooms;
    Character *player;
    std::vector<ParsedMonster> monsterTemplates;
    std::vector<Monster*> newMonsters;
    std::vector<ParsedItem> itemTemplates;
    std::vector<Item*> items;
};

enum MonsterAttributes{
    intelligent = 0x1,
    telepathic = 0x2,
    tunneling = 0x4,
    erratic = 0x8
};

// Dungeon Generation
void generateDungeon(Dungeon *dungeon, Character *player);
void generateRooms(Dungeon *dungeon);
void generatePaths(Dungeon *dungeon);
void createMonsters(Dungeon *dungeon);
void generateItems(Dungeon *dungeon);
void buildDistanceMap(Dungeon *dungeon, int tunneler, int distanceMap[DUNGEON_HEIGHT][DUNGEON_WIDTH]);
int doRoomsOverlap(Dungeon *dungeon, Room room, int currentRoomNum);
void printDungeon(Dungeon *dungeon, char messageString[DUNGEON_WIDTH]);

// File Manipulation
void saveDungeon(Dungeon *dungeon);
void loadDungeon(Dungeon *dungeon);
FILE *getSaveFile(char *mode);

// Movement / Character Handling
int isMovementPossible(Dungeon *dungeon, int x, int y, int isTunneler);
void moveCharacter(Character *character, Dungeon *dungeon, int tunnelerMap[DUNGEON_HEIGHT][DUNGEON_WIDTH], int nonTunnelersMap[DUNGEON_HEIGHT][DUNGEON_WIDTH]);
void movePlayerCharacter(Dungeon *dungeon, int x, int y);
int characterPlayerAction(Dungeon *dungeon, char messageString[DUNGEON_WIDTH]);
void updateFogOfWarMap(Dungeon *dungeon);
void attackCharacter(Dungeon *dungeon, Character *attacker, Character *defender);
void rangedAttack(Dungeon *dungeon);

// Misc functions
void stopProgram();
void teleportCheat(Dungeon *dungeon);
void displayMonsterList(Dungeon *dungeon);
void displayCharacterInventory(Dungeon *dungeon);
void displayCharacterEquipment(Dungeon *dungeon);
void targetAndDisplayMonsterInfo(Dungeon *dungeon);
void pickupItem(Dungeon *dungeon);
void wearItem(Dungeon *dungeon);
void displayItemInfo(Dungeon *dungeon);



#endif
