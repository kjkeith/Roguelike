#ifndef PARSEDITEM_H
#define PARSEDITEM_H

#include <string>
#include <vector>
#include "Dice.h"
#include "Item.h"

class Item;

class ParsedItem {
private:
    std::string name;
    std::vector<std::string> descript;
    std::vector<std::string> type;
    int binaryType;
    std::string color;
    Dice value;
    Dice hitBonus;
    Dice damageBonus;
    Dice dodgeBonus;
    Dice defenseBonus;
    Dice speedBonus;
    Dice weight;
    Dice specAttr;
    bool isArtifact;
    bool isUsed;
    int rarity;

public:
    enum item_type
    {
        WEAPON = 1,
        OFFHAND = 2,
        RANGED = 4,
        ARMOR = 8,
        HELMET = 16,
        CLOAK = 32,
        GLOVES = 64,
        BOOTS = 128,
        RING = 256,
        AMULET = 512,
        LIGHT = 1024,
        SCROLL = 2048,
        BOOK = 4096,
        FLASK = 8192,
        GOLD = 16384,
        AMMUNITION = 32768,
        FOOD = 65536,
        WAND = 131072,
        CONTAINER = 262144
    };

    ParsedItem();

    const std::string &getName() const;

    void setName(const std::string &name);

    const std::vector <std::string> &getDescript() const;

    void setDescript(const std::vector <std::string> &descript);

    const std::vector<std::string> &getType() const;

    void setType(const std::vector<std::string> &type);

    const std::string &getColor() const;

    void setColor(const std::string &color);

    const Dice &getValue() const;

    void setValue(const Dice &value);

    const Dice &getHitBonus() const;

    void setHitBonus(const Dice &hitBonus);

    const Dice &getDamageBonus() const;

    void setDamageBonus(const Dice &damageBonus);

    const Dice &getDodgeBonus() const;

    void setDodgeBonus(const Dice &dodgeBonus);

    const Dice &getDefenseBonus() const;

    void setDefenseBonus(const Dice &defenseBonus);

    const Dice &getSpeedBonus() const;

    void setSpeedBonus(const Dice &speedBonus);

    Dice getWeight() const;

    void setWeight(const Dice &weight);

    Dice getSpecAttr() const;

    void setSpecAttr(const Dice &specAttr);

    bool getIsArtifact() const;

    void setIsArtifact(bool isArtifact);

    bool getIsUsed() const;

    void setIsUsed(bool isUsed);

    int getRarity() const;

    void setRarity(int rarity);

    Item* generateItem();


};

#endif
