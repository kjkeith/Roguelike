#include "ParsedMonster.h"

ParsedMonster::ParsedMonster() {}

const std::string &ParsedMonster::getName() const{
    return name;
}

void ParsedMonster::setName(const std::string &name) {
    ParsedMonster::name = name;
}

const std::vector<std::string> &ParsedMonster::getDescript() const {
    return descript;
}

void ParsedMonster::setDescript(const std::vector<std::string> &descript) {
    ParsedMonster::descript = descript;
}

const std::vector<std::string> &ParsedMonster::getColors() const {
    return colors;
}

void ParsedMonster::setColors(const std::vector<std::string> &colors) {
    ParsedMonster::colors = colors;
}

const Dice &ParsedMonster::getSpeed() const {
    return speed;
}

void ParsedMonster::setSpeed(const Dice &speed) {
    ParsedMonster::speed = speed;
}

std::vector<std::string> ParsedMonster::getAbilities() const {
    return abilities;
}

void ParsedMonster::setAbilities(std::vector<std::string> abilities) {
    ParsedMonster::abilities = abilities;
}

const Dice &ParsedMonster::getHp() const {
    return hp;
}

void ParsedMonster::setHp(const Dice &hp) {
    ParsedMonster::hp = hp;
}

const Dice &ParsedMonster::getDamage() const {
    return damage;
}

void ParsedMonster::setDamage(const Dice &damage) {
    ParsedMonster::damage = damage;
}

char ParsedMonster::getSymbol() const {
    return symbol;
}

void ParsedMonster::setSymbol(char symbol) {
    ParsedMonster::symbol = symbol;
}

int ParsedMonster::getRarity() const {
    return rarity;
}

void ParsedMonster::setRarity(int rarity) {
    ParsedMonster::rarity = rarity;
}

Monster* ParsedMonster::generateMonster() {
    ParsedMonster::isUsed = true;
    return new Monster(0, 0, name, descript, colors, speed.rollDice(), abilities, hp.rollDice(), damage, symbol);
}

bool ParsedMonster::getIsUnique() const {
    return isUnique;
}

void ParsedMonster::setIsUnique(bool isUnique) {
    ParsedMonster::isUnique = isUnique;
}

bool ParsedMonster::getIsUsed() const {
    return isUsed;
}

void ParsedMonster::setIsUsed(bool isUsed) {
    ParsedMonster::isUsed = isUsed;
}

