#ifndef ITEMPARSER_H
#define ITEMPARSER_H

#include <vector>
#include <fstream>
#include <iostream>
#include <ncurses.h>
#include "ParsedItem.h"

class ItemParser {
private:
    std::vector<ParsedItem> parsedItems;

public:
    std::vector<ParsedItem> parseFile(std::string file);
    std::vector<std::string> descriptionToVector(std::ifstream &file);
    Dice stringToDice(std::string diceString);
    Dice hitToDice(std::string hitString);
    Dice damageToDice(std::string damString);
    Dice dodgeToDice(std::string dodgeString);
    Dice defenseToDice(std::string defString);
    Dice speedToDice(std::string diceString);
    Dice weightToDice(std::string weightString);
    Dice valueToDice(std::string valString);
    Dice specAttrToDice(std::string specString);
    std::vector<std::string> typeToVector(std::string typeString);
        bool artStatToBool(std::string boolString);
    int rarityToInt(std::string rarityString);

    std::vector<ParsedItem> getParsedItems();

    void printParsedData();
};

#endif