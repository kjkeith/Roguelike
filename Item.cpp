#include "Item.h"
#include "ParsedItem.h"

Item::Item() = default;

Item::Item(int x, int y, std::string name, std::vector<std::string> description, int type, std::string color,
     int value, int hitBonus, Dice damageBonus, int dodgeBonus, int defenseBonus, int speedBonus, int weight,
     int specAttr, bool isArtifact) {
    this->x = x;
    this->y = y;
    this->name = std::move(name);
    this->descript = std::move(description);
    this->type = type;
    setColor(color);
    this->value = value;
    this->hitBonus = hitBonus;
    this->damageBonus = damageBonus;
    this->dodgeBonus = dodgeBonus;
    this->defenseBonus = defenseBonus;
    this->speedBonus = speedBonus;
    this->weight = weight;
    this->specAttr = specAttr;
    this->isArtifact = isArtifact;
    this->setCharRep();
}

int Item::getX() const {
    return x;
}

int Item::getY() const {
    return y;
}

void Item::setPoint(int x, int y) {
    Item::x = x;
    Item::y = y;
}

const std::string &Item::getName() const {
    return name;
}

const std::vector<std::string> &Item::getDescript() const {
    return descript;
}

const int &Item::getType() const {
    return type;
}

int Item::getColor() const {
    return color;
}

void Item::setColor(std::string color) {
    if(color == "RED") {
        this->color = COLOR_RED;
    } else if(color == "GREEN") {
        this->color = COLOR_GREEN;
    } else if(color == "BLUE") {
        this->color = COLOR_BLUE;
    } else if(color == "CYAN") {
        this->color = COLOR_CYAN;
    } else if(color == "YELLOW") {
        this->color = COLOR_YELLOW;
    } else if(color == "MAGENTA") {
        this->color = COLOR_MAGENTA;
    } else if(color == "WHITE") {
        this->color = COLOR_WHITE;
    } else if(color == "BLACK") {
        this->color = COLOR_WHITE;
    }
}

int Item::getValue() const {
    return value;
}

int Item::getHitBonus() const {
    return hitBonus;
}

Dice &Item::getDamageBonus() {
    return damageBonus;
}

int Item::getDodgeBonus() const {
    return dodgeBonus;
}

int Item::getDefenseBonus() const {
    return defenseBonus;
}

int Item::getSpeedBonus() const {
    return speedBonus;
}

int Item::getWeight() const {
    return weight;
}

int Item::getSpecAttr() const {
    return specAttr;
}

bool Item::isArtifact1() const {
    return isArtifact;
}

char Item::getCharRep() const {
    return charRep;
}

void Item::setCharRep() {
    if (type & ParsedItem::WEAPON) {
        charRep = WEAPON;
    } else if(type & ParsedItem::OFFHAND) {
        charRep = OFFHAND;
    } else if(type & ParsedItem::RANGED) {
        charRep = RANGED;
    } else if(type & ParsedItem::ARMOR) {
        charRep = ARMOR;
    } else if(type & ParsedItem::HELMET) {
        charRep = HELMET;
    } else if(type & ParsedItem::CLOAK) {
        charRep = CLOAK;
    } else if(type & ParsedItem::GLOVES) {
        charRep = GLOVES;
    } else if(type & ParsedItem::BOOTS) {
        charRep = BOOTS;
    } else if(type & ParsedItem::RING) {
        charRep = RING;
    } else if(type & ParsedItem::AMULET) {
        charRep = AMULET;
    } else if(type & ParsedItem::LIGHT) {
        charRep = LIGHT;
    } else if(type & ParsedItem::SCROLL) {
        charRep = SCROLL;
    } else if(type & ParsedItem::BOOK) {
        charRep = BOOK;
    } else if(type & ParsedItem::FLASK) {
        charRep = FLASK;
    } else if(type & ParsedItem::GOLD) {
        charRep = GOLD;
    } else if(type & ParsedItem::AMMUNITION) {
        charRep = AMMUNITION;
    } else if(type & ParsedItem::FOOD) {
        charRep = FOOD;
    } else if(type & ParsedItem::WAND) {
        charRep = WAND;
    } else if(type & ParsedItem::CONTAINER) {
        charRep = CONTAINER;
    }

}
