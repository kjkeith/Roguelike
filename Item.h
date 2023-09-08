#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <vector>
#include <ncurses.h>
#include "Dice.h"



class Item {
private:
    int x, y;
    std::string name;
    std::vector<std::string> descript;
    int type;
    int color;
    int value;
    int hitBonus;
    Dice damageBonus;
    int dodgeBonus;
    int defenseBonus;
    int speedBonus;
    int weight;
    int specAttr;
    bool isArtifact;
    char charRep;

public:
    enum item_symbols {
        WEAPON = '|',
        OFFHAND = ')',
        RANGED = '}',
        ARMOR = '[',
        HELMET = ']',
        CLOAK = '(',
        GLOVES = '{',
        BOOTS = '\\',
        RING = '=',
        AMULET = '"',
        LIGHT = '_',
        SCROLL = '~',
        BOOK = '?',
        FLASK = '!',
        GOLD = '$',
        AMMUNITION = '/',
        FOOD = ',',
        WAND = '-',
        CONTAINER = '%',
        STACK = '&'
    };

    Item();

    Item(int x, int y, std::string name, std::vector<std::string> description, int type, std::string color,
            int value, int hitBonus, Dice damageBonus, int dodgeBonus, int defenseBonus, int speedBonus, int weight,
            int specAttr, bool isArtifact);

    int getX() const;

    int getY() const;

    void setPoint(int x, int y);

    const std::string &getName() const;

    const std::vector<std::string> &getDescript() const;

    const int &getType() const;

    int getColor() const;

    void setColor(std::string color);

    int getValue() const;

    int getHitBonus() const;

    Dice &getDamageBonus();

    int getDodgeBonus() const;

    int getDefenseBonus() const;

    int getSpeedBonus() const;

    int getWeight() const;

    int getSpecAttr() const;

    bool isArtifact1() const;

    char getCharRep() const;

    void setCharRep();
};


#endif
