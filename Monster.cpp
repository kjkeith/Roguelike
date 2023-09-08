#include "Monster.h"

Monster::Monster() = default;

Monster::Monster(int x, int y, std::string name, std::vector<std::string> description,
                 std::vector<std::string> colors, int speed, std::vector<std::string> abilities, int hp, Dice damage,
                 char charRep) {
    this->x = x;
    this->y = y;
    this->name = std::move(name);
    this->description = std::move(description);
    this->speed = speed;
    setAbilities(abilities);
    this->charRep = charRep;
    setColors(colors);
    this->hp = hp;
    this->damage = damage;

    this->isLiving = true;
    this->isPlayer = false;
}

const std::string &Monster::getName() const {
    return name;
}

void Monster::setName(const std::string &name) {
    Monster::name = name;
}

const std::vector<std::string> &Monster::getDescription() const {
    return description;
}

void Monster::setDescription(const std::vector<std::string> &description) {
    Monster::description = description;
}

const std::vector<int> &Monster::getColors() const {
    return colors;
}

void Monster::setColors(std::vector<std::string> colorStrings) {
    std::vector<int> returnColors;
    std::string delimiter = " ";
    for(std::string color : colorStrings) {
        size_t pos = 0;
        std::string token;
       // while ((pos = color.find(delimiter)) != std::string::npos) {
            token = color;
            if(token == "RED") {
                returnColors.push_back(COLOR_RED);
            } else if(token == "GREEN") {
                returnColors.push_back(COLOR_GREEN);
            } else if(token == "BLUE") {
                returnColors.push_back(COLOR_BLUE);
            } else if(token == "CYAN") {
                returnColors.push_back(COLOR_CYAN);
            } else if(token == "YELLOW") {
                returnColors.push_back(COLOR_YELLOW);
            } else if(token == "MAGENTA") {
                returnColors.push_back(COLOR_MAGENTA);
            } else if(token == "WHITE") {
                returnColors.push_back(COLOR_WHITE);
            } else if(token == "BLACK") {
                returnColors.push_back(COLOR_WHITE);
            }
            color.erase(0, pos + delimiter.length());
     //   }
    }
    Monster::colors = returnColors;
}

int Monster::getAbilities() const {
    return attributes;
}

void Monster::setAbilities(std::vector<std::string> abilities) {
    this->attributes = 0;
    for(auto ability : abilities) {
        if(ability == "SMART") {
            this->attributes |= Monster::SMART;
        } else if (ability == "TELE") {
            this->attributes |= Monster::TELE;
        } else if (ability == "TUNNEL") {
            this->attributes |= Monster::TUNNEL;
        }else if (ability == "ERRATIC") {
            this->attributes |= Monster::ERRATIC;
        }else if (ability == "PASS") {
            this->attributes |= Monster::PASS;
        }else if (ability == "PICKUP") {
            this->attributes |= Monster::PICKUP;
        }else if (ability == "DESTROY") {
            this->attributes |= Monster::DESTROY;
        }else if (ability == "UNIQ") {
            this->attributes |= Monster::UNIQ;
        }else if (ability == "BOSS") {
            this->attributes |= Monster::BOSS;
        }
    }
}

char Monster::getCharRep() const {
    return charRep;
}

void Monster::setCharRep(char charRep) {
    this->charRep = charRep;
}
