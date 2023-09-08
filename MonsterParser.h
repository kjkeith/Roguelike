#ifndef MONSTERPARSER_H
#define MONSTERPARSER_H

#include <vector>
#include <fstream>
#include <iostream>
#include <ncurses.h>
#include "ParsedMonster.h"

class MonsterParser {
private:
    std::vector<ParsedMonster> parsedMonsters;

public:
    std::vector<ParsedMonster> parseFile(std::string file);
    std::vector<std::string> descriptionToVector(std::ifstream &file);
    std::vector<std::string> colorsToVectorString(std::string colorString);
    Dice speedToDice(std::string diceString);
    std::vector<std::string> abilitiestoStringVector(std::string abilitiesString);
    Dice hpToDice(std::string hpString);
    Dice damageToDice(std::string damString);
    char symbolToChar(std::string symbString);
    int rarityToInt(std::string rarityString);
    std::vector<ParsedMonster> getParsedMonsters();

    void printParsedData();
};


#endif