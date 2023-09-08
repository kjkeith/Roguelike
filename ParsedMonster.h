#ifndef PARSEDMONSTER_H
#define PARSEDMONSTER_H

#include <string>
#include <vector>
#include "Dice.h"
#include "Monster.h"

class ParsedMonster {
private:
    std::string name;
    std::vector<std::string> descript;
    std::vector<std::string> colors;
    Dice speed;
    std::vector<std::string> abilities;
    Dice hp;
    Dice damage;
    char symbol;
    int rarity;

    bool isUnique;
    bool isUsed;

public:
    ParsedMonster();

    const std::string &getName() const;

    void setName(const std::string &name);

    const std::vector<std::string> &getDescript() const;

    void setDescript(const std::vector<std::string> &descript);

    const std::vector<std::string> &getColors() const;

    void setColors(const std::vector<std::string> &colors);

    const Dice &getSpeed() const;

    void setSpeed(const Dice &speed);

    std::vector<std::string> getAbilities() const;

    void setAbilities(std::vector<std::string> abilities);

    const Dice &getHp() const;

    void setHp(const Dice &hp);

    const Dice &getDamage() const;

    void setDamage(const Dice &damage);

    char getSymbol() const;

    void setSymbol(char symbol);

    int getRarity() const;

    void setRarity(int rarity);

    Monster* generateMonster();

    bool getIsUnique() const;

    void setIsUnique(bool isUnique);

    bool getIsUsed() const;

    void setIsUsed(bool isUsed);


};

#endif