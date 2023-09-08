#include "ParsedItem.h"

ParsedItem::ParsedItem() {}

const std::string &ParsedItem::getName() const {
    return name;
}

void ParsedItem::setName(const std::string &name) {
    ParsedItem::name = name;
}

const std::vector<std::string> &ParsedItem::getDescript() const {
    return descript;
}

void ParsedItem::setDescript(const std::vector<std::string> &descript) {
    ParsedItem::descript = descript;
}

const std::vector<std::string> &ParsedItem::getType() const {
    return type;
}

void ParsedItem::setType(const std::vector<std::string> &type) {
    ParsedItem::type = type;
    this->binaryType = 0;

    for(auto i : this->type) {
        if(i == "WEAPON") {
            this->binaryType |= WEAPON;
        } else if(i == "OFFHAND") {
            this->binaryType |= OFFHAND;
        } else if(i == "RANGED") {
            this->binaryType |= RANGED;
        } else if(i == "ARMOR") {
            this->binaryType |= ARMOR;
        } else if(i == "HELMET") {
            this->binaryType |= HELMET;
        } else if(i == "CLOAK") {
            this->binaryType |= CLOAK;
        } else if(i == "GLOVES") {
            this->binaryType |= GLOVES;
        } else if(i == "BOOTS") {
            this->binaryType |= BOOTS;
        } else if(i == "RING") {
            this->binaryType |= RING;
        } else if(i == "AMULET") {
            this->binaryType |= AMULET;
        } else if(i == "LIGHT") {
            this->binaryType |= LIGHT;
        } else if(i == "SCROLL") {
            this->binaryType |= SCROLL;
        } else if(i == "BOOK") {
            this->binaryType |= BOOK;
        } else if(i == "FLASK") {
            this->binaryType |= FLASK;
        } else if(i == "GOLD") {
            this->binaryType |= GOLD;
        } else if(i == "AMMUNITION") {
            this->binaryType |= AMMUNITION;
        } else if(i == "FOOD") {
            this->binaryType |= FOOD;
        } else if(i == "WAND") {
            this->binaryType |= WAND;
        } else if(i == "CONTAINER") {
            this->binaryType |= CONTAINER;
        }
    }
}

const std::string &ParsedItem::getColor() const {
    return color;
}

void ParsedItem::setColor(const std::string &color) {
    ParsedItem::color = color;
}

const Dice &ParsedItem::getValue() const {
    return value;
}

void ParsedItem::setValue(const Dice &value) {
    ParsedItem::value = value;
}

const Dice &ParsedItem::getHitBonus() const {
    return hitBonus;
}

void ParsedItem::setHitBonus(const Dice &hitBonus) {
    ParsedItem::hitBonus = hitBonus;
}

const Dice &ParsedItem::getDamageBonus() const {
    return damageBonus;
}

void ParsedItem::setDamageBonus(const Dice &damageBonus) {
    ParsedItem::damageBonus = damageBonus;
}

const Dice &ParsedItem::getDodgeBonus() const {
    return dodgeBonus;
}

void ParsedItem::setDodgeBonus(const Dice &dodgeBonus) {
    ParsedItem::dodgeBonus = dodgeBonus;
}

const Dice &ParsedItem::getDefenseBonus() const {
    return defenseBonus;
}

void ParsedItem::setDefenseBonus(const Dice &defenseBonus) {
    ParsedItem::defenseBonus = defenseBonus;
}

const Dice &ParsedItem::getSpeedBonus() const {
    return speedBonus;
}

void ParsedItem::setSpeedBonus(const Dice &speedBonus) {
    ParsedItem::speedBonus = speedBonus;
}

Dice ParsedItem::getWeight() const {
    return weight;
}

void ParsedItem::setWeight(const Dice &weight) {
    ParsedItem::weight = weight;
}

Dice ParsedItem::getSpecAttr() const {
    return specAttr;
}

void ParsedItem::setSpecAttr(const Dice &specAttr) {
    ParsedItem::specAttr = specAttr;
}

bool ParsedItem::getIsArtifact() const {
    return isArtifact;
}

void ParsedItem::setIsArtifact(bool isArtifact) {
    ParsedItem::isArtifact = isArtifact;
}

bool ParsedItem::getIsUsed() const {
    return isUsed;
}

void ParsedItem::setIsUsed(bool isUsed) {
    ParsedItem::isUsed = isUsed;
}

int ParsedItem::getRarity() const {
    return rarity;
}

void ParsedItem::setRarity(int rarity) {
    ParsedItem::rarity = rarity;
}

Item* ParsedItem::generateItem() {
    ParsedItem::isUsed = true;
    return new Item(0, 0, name, descript, binaryType, color, value.rollDice(), hitBonus.rollDice(), damageBonus,
            dodgeBonus.rollDice(), defenseBonus.rollDice(), speedBonus.rollDice(), weight.rollDice(),
            specAttr.rollDice(), isArtifact);
}