#include "ItemParser.h"

std::vector<ParsedItem> ItemParser::parseFile(std::string file) {
    std::ifstream fileStream;
    fileStream.open(file);

    if(fileStream.is_open()) {
        std::string temp;
        std::getline(fileStream, temp);
        if (temp != "RLG327 OBJECT DESCRIPTION 1") {
            std::cout << "error: Incorrect file passed into parser.";
            exit(EXIT_FAILURE);
        }

        std::getline(fileStream, temp);
        // Work until we reach the ENDofFILE
        while(!fileStream.eof()) {
            if(temp != "BEGIN OBJECT") {
                if (fileStream.eof()) {
                    break;
                }
                std::getline(fileStream, temp);
            } else if(temp == "BEGIN OBJECT") {
                ParsedItem parsedItem;
                std::getline(fileStream, temp);
                bool submit = true;
                std::vector<bool> didOccur(14, false);

                while(temp != "END") {
                    if(temp.substr(0, 5) == "NAME ") {
                        parsedItem.setName(temp.substr((5)));
                        if(didOccur[0]) {
                            submit = false;
                            break;
                        }
                        didOccur[0] = true;
                    } else if(temp == "DESC") {
                        parsedItem.setDescript(this->descriptionToVector(fileStream));
                        if(didOccur[1]) {
                            submit = false;
                            break;
                        }
                        didOccur[1] = true;
                    } else if(temp.substr(0, 6) == "COLOR ") {
                        parsedItem.setColor(temp.substr(6));
                        if(didOccur[2]) {
                            submit = false;
                            break;
                        }
                        didOccur[2] = true;
                    } else if(temp.substr(0, 4) == "HIT ") {
                        parsedItem.setHitBonus(this->hitToDice(temp.substr(4)));
                        if(didOccur[3]) {
                            submit = false;
                            break;
                        }
                        didOccur[3] = true;
                    } else if(temp.substr(0, 6) == "SPEED ") {
                        parsedItem.setSpeedBonus(this->speedToDice(temp.substr(6)));
                        if(didOccur[4]) {
                            submit = false;
                            break;
                        }
                        didOccur[4] = true;
                    } else if(temp.substr(0, 4) == "DAM ") {
                        parsedItem.setDamageBonus(this->damageToDice(temp.substr(4)));
                        if(didOccur[5]) {
                            submit = false;
                            break;
                        }
                        didOccur[5] = true;
                    } else if(temp.substr(0, 7) == "WEIGHT ") {
                        parsedItem.setWeight(this->weightToDice(temp.substr(7)));
                        if(didOccur[6]) {
                            submit = false;
                            break;
                        }
                        didOccur[6] = true;
                    } else if(temp.substr(0, 5) == "RRTY ") {
                        parsedItem.setRarity(this->rarityToInt(temp.substr(5)));
                        if(didOccur[7]) {
                            submit = false;
                            break;
                        }
                        didOccur[7] = true;
                    } else if(temp.substr(0, 4) == "DEF ") {
                        parsedItem.setDefenseBonus(this->defenseToDice(temp.substr(4)));
                        if(didOccur[8]) {
                            submit = false;
                            break;
                        }
                        didOccur[8] = true;
                    } else if(temp.substr(0, 4) == "VAL ") {
                        parsedItem.setValue(this->valueToDice(temp.substr(4)));
                        if(didOccur[9]) {
                            submit = false;
                            break;
                        }
                        didOccur[9] = true;
                    } else if(temp.substr(0, 6) == "DODGE ") {
                        parsedItem.setDodgeBonus(this->dodgeToDice(temp.substr(6)));
                        if(didOccur[10]) {
                            submit = false;
                            break;
                        }
                        didOccur[10] = true;
                    } else if(temp.substr(0, 4) == "ART ") {
                        parsedItem.setIsArtifact(this->artStatToBool(temp.substr(4)));
                        if(didOccur[11]) {
                            submit = false;
                            break;
                        }
                        didOccur[11] = true;
                    } else if(temp.substr(0, 5) == "ATTR ") {
                        parsedItem.setSpecAttr(this->specAttrToDice(temp.substr(5)));
                        if (didOccur[12]) {
                            submit = false;
                            break;
                        }
                        didOccur[12] = true;
                    } else if(temp.substr(0, 5) == "TYPE ") {
                        parsedItem.setType(this->typeToVector(temp.substr(5)));
                        if (didOccur[13]) {
                            submit = false;
                            break;
                        }
                        didOccur[13] = true;
                    }
                    std::getline(fileStream, temp);
                }

                // check if all fields have been filled!
                for(auto boolean : didOccur) {
                    if(!boolean) {
                        submit = false;
                        break;
                    }
                }

                if(submit) {
                    parsedItem.setIsUsed(false);
                    this->parsedItems.push_back(parsedItem);
                }
            }
        }
        return this->parsedItems;
    } else {
        std::cout << "error: Could not open item file.";
        exit(1);
    }
}

std::vector<std::string> ItemParser::descriptionToVector(std::ifstream &file) {
    std::vector<std::string> returnDesc;
    std::string temp;
    std::getline(file, temp);

    if(temp.length() > 77) {
        std::cout << "Description does not fit the terminal" << std::endl;
        exit(0);
    }
    while(temp != ".") {
        returnDesc.push_back(temp);
        std::getline(file, temp);
        if(temp.length() > 77) {
            std::cout << "Description does not fit the terminal" << std::endl;
            exit(0);
        }
    }
    return returnDesc;
}

Dice ItemParser::stringToDice(std::string diceString) {
    size_t pos = 0;
    std::string base, count, side;

    pos = diceString.find("+");
    base = diceString.substr(0, pos);
    diceString.erase(0, pos + 1);

    pos = diceString.find("d");
    count = diceString.substr(0, pos);
    diceString.erase(0, pos + 1);

    side = diceString;

    Dice dice(std::stoi(base), std::stoi(count), std::stoi(side));

    return dice;
}

Dice ItemParser::speedToDice(std::string diceString) {
    Dice speedDie = stringToDice(diceString);
    return speedDie;
}

Dice ItemParser::damageToDice(std::string damString) {
    Dice damageDie = stringToDice(damString);
    return damageDie;
}

Dice ItemParser::hitToDice(std::string hitString) {
    Dice hitDie = stringToDice(hitString);
    return hitDie;
}

Dice ItemParser::dodgeToDice(std::string dodgeString) {
    Dice dodgeDie = stringToDice(dodgeString);
    return dodgeDie;
}

Dice ItemParser::defenseToDice(std::string defenseString) {
    Dice defenseDie = stringToDice(defenseString);
    return defenseDie;
}

Dice ItemParser::weightToDice(std::string weightString) {
    Dice weightDie = stringToDice(weightString);
    return weightDie;
}

Dice ItemParser::valueToDice(std::string valString) {
    Dice valDie = stringToDice(valString);
    return valDie;
}

Dice ItemParser::specAttrToDice(std::string specString) {
    Dice specDie = stringToDice(specString);
    return specDie;
}

std::vector<std::string> ItemParser::typeToVector(std::string typeString) {
    std::vector<std::string> typeVector;
    std::string delimiter = " ";

    size_t pos = 0;
    std::string token;
    while ((pos = typeString.find(delimiter)) != std::string::npos) {
        token = typeString.substr(0, pos);
        typeVector.push_back(token);

        typeString.erase(0, pos + delimiter.length());
    }
    token = typeString;
    typeVector.push_back(token);

    return typeVector;
}

bool ItemParser::artStatToBool(std::string boolString) {
    if(boolString == "TRUE") {
        return true;
    } else {
        return false;
    }
}

int ItemParser::rarityToInt(std::string rarityString) {
    return std::stoi(rarityString);
}

std::vector<ParsedItem> ItemParser::getParsedItems() {
    return parsedItems;
}

void ItemParser::printParsedData() {
    for(auto item : this->parsedItems) {
        std::string description;
        std::string type;
        for(std::string line : item.getDescript()) {
            description += (line + '\n');
        }

        for(std::string line : item.getType()) {
            type += (line = ' ');
        }

        std::cout << item.getName() << std::endl
                  << description << std::endl
                  << type << std::endl
                  << item.getColor() << std::endl
                  << item.getValue().getBase() << "+" << item.getValue().getCount() << "d" << item.getValue().getSides() << std::endl
                  << item.getHitBonus().getBase() << "+" << item.getHitBonus().getCount() << "d" << item.getHitBonus().getSides() << std::endl
                  << item.getSpeedBonus().getBase() << "+" << item.getSpeedBonus().getCount() << "d" << item.getSpeedBonus().getSides() << std::endl
                  << item.getDamageBonus().getBase() << "+" << item.getDamageBonus().getCount() << "d" << item.getDamageBonus().getSides() << std::endl
                  << item.getDodgeBonus().getBase() << "+" << item.getDodgeBonus().getCount() << "d" << item.getDodgeBonus().getSides() << std::endl
                  << item.getDefenseBonus().getBase() << "+" << item.getDefenseBonus().getCount() << "d" << item.getDefenseBonus().getSides() << std::endl
                  << item.getWeight().getBase() << "+" << item.getWeight().getCount() << "d" << item.getWeight().getSides() << std::endl
                  << item.getSpecAttr().getBase() << "+" << item.getSpecAttr().getCount() << "d" << item.getSpecAttr().getSides() << std::endl
                  << (item.getIsArtifact() ? "TRUE" : "FALSE") << std::endl
                  << item.getRarity() << std::endl << std::endl;
    }

}