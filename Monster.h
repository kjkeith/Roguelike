#ifndef MONSTER_H
#define MONSTER_H
#pragma once

#include <string>
#include <vector>
#include <ncurses.h>
#include "Dice.h"
#include "dungeon.h"

class Dungeon;

class Monster : public Character {
private:
    std::string name;
    // using vector as array
    std::vector<std::string> description;
    std::vector<int> colors;

public:
    enum monster_abilities {
        SMART = 1,
        TELE = 2,
        TUNNEL = 4,
        ERRATIC = 8,
        PASS = 16,
        PICKUP = 32,
        DESTROY = 64,
        UNIQ = 128,
        BOSS = 256
    };

    Monster();

    Monster(int x, int y, std::string name, std::vector<std::string> description,
            std::vector<std::string> colors, int speed, std::vector<std::string> abilities, int hp,
            Dice damage, char charRep);

    const std::string &getName() const;

    void setName(const std::string &name);

    const std::vector<std::string> &getDescription() const;

    void setDescription(const std::vector<std::string> &description);

    const std::vector<int> &getColors() const;

    void setColors(std::vector<std::string> colors);

    int getAbilities() const;

    void setAbilities(std::vector<std::string> abilities);

    char getCharRep() const;

    void setCharRep(char charRep);
};

#endif