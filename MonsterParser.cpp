#include "MonsterParser.h"

std::vector<ParsedMonster> MonsterParser::parseFile(std::string file) {
    std::ifstream fileStream;
    fileStream.open(file);

    if(fileStream.is_open()) {
        std::string temp;
        std::getline(fileStream, temp);
        if (temp != "RLG327 MONSTER DESCRIPTION 1") {
            std::cout << "error: Incorrect file passed into parser.";
            exit(EXIT_FAILURE);
        }

        std::getline(fileStream, temp);
        // Work until we reach the ENDofFILE
        while(!fileStream.eof()) {
            if(temp != "BEGIN MONSTER") {
                if (fileStream.eof()) {
                    break;
                }
                std::getline(fileStream, temp);
            } else if(temp == "BEGIN MONSTER") {
                ParsedMonster parsedMonster;
                std::getline(fileStream, temp);
                bool submit = true;
                std::vector<bool> didOccur(9, false);

                while(temp != "END") {
                    if(temp.substr(0, 5) == "NAME ") {
                        parsedMonster.setName(temp.substr((5)));
                        if(didOccur[0]) {
                            submit = false;
                            break;
                        }
                        didOccur[0] = true;
                    } else if(temp == "DESC") {
                        parsedMonster.setDescript(this->descriptionToVector(fileStream));
                        if(didOccur[1]) {
                            submit = false;
                            break;
                        }
                        didOccur[1] = true;
                    } else if(temp.substr(0, 6) == "COLOR ") {
                        parsedMonster.setColors(this->colorsToVectorString(temp.substr(6)));
                        if(didOccur[2]) {
                            submit = false;
                            break;
                        }
                        didOccur[2] = true;
                    } else if(temp.substr(0, 5) == "SYMB ") {
                        parsedMonster.setSymbol(this->symbolToChar(temp.substr(5)));
                        if(didOccur[3]) {
                            submit = false;
                            break;
                        }
                        didOccur[3] = true;
                    } else if(temp.substr(0, 6) == "SPEED ") {
                        parsedMonster.setSpeed(this->speedToDice(temp.substr(6)));
                        if(didOccur[4]) {
                            submit = false;
                            break;
                        }
                        didOccur[4] = true;
                    } else if(temp.substr(0, 4) == "DAM ") {
                        parsedMonster.setDamage(this->damageToDice(temp.substr(4)));
                        if(didOccur[5]) {
                            submit = false;
                            break;
                        }
                        didOccur[5] = true;
                    } else if(temp.substr(0, 3) == "HP ") {
                        parsedMonster.setHp(this->hpToDice(temp.substr(3)));
                        if(didOccur[6]) {
                            submit = false;
                            break;
                        }
                        didOccur[6] = true;
                    } else if(temp.substr(0, 5) == "RRTY ") {
                        parsedMonster.setRarity(this->rarityToInt(temp.substr(5)));
                        if(didOccur[7]) {
                            submit = false;
                            break;
                        }
                        didOccur[7] = true;
                    } else if(temp.substr(0, 5) == "ABIL ") {
                        parsedMonster.setAbilities(this->abilitiestoStringVector(temp.substr(5)));
                        if(didOccur[8]) {
                            submit = false;
                            break;
                        }
                        didOccur[8] = true;

                        if(std::find(parsedMonster.getAbilities().begin(), parsedMonster.getAbilities().end(), "UNIQ")
                                != parsedMonster.getAbilities().end()) {
                            parsedMonster.setIsUnique(true);
                        } else {
                            parsedMonster.setIsUnique(false);
                        }
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
                    this->parsedMonsters.push_back(parsedMonster);
                }
            }
        }
        return this->parsedMonsters;
    } else {
        std::cout << "error: Could not open monster file.";
        exit(1);
    }
}

std::vector<std::string> MonsterParser::descriptionToVector(std::ifstream &file) {
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

std::vector<std::string> MonsterParser::colorsToVectorString(std::string colorString) {
    std::vector<std::string> returnColors;
    std::string delimiter = " ";

    size_t pos = 0;
    std::string token;
    while ((pos = colorString.find(delimiter)) != std::string::npos) {
        token = colorString.substr(0, pos);
        returnColors.push_back(token);

        colorString.erase(0, pos + delimiter.length());
    }
    token = colorString;
    returnColors.push_back(token);

    return returnColors;
}

char MonsterParser::symbolToChar(std::string symbString) {
    return symbString.at(0);
}

Dice MonsterParser::speedToDice(std::string diceString) {
    size_t pos = 0;
    std::string base, count, side;

    pos = diceString.find("+");
    base = diceString.substr(0, pos);
    diceString.erase(0, pos + 1);

    pos = diceString.find("d");
    count = diceString.substr(0, pos);
    diceString.erase(0, pos + 1);

    side = diceString;

    Dice speedDie(std::stoi(base), std::stoi(count), std::stoi(side));
    return speedDie;
}

Dice MonsterParser::damageToDice(std::string damString) {
    size_t pos = 0;
    std::string base, count, side;

    pos = damString.find("+");
    base = damString.substr(0, pos);
    damString.erase(0, pos + 1);

    pos = damString.find("d");
    count = damString.substr(0, pos);
    damString.erase(0, pos + 1);

    side = damString;

    Dice damageDie(std::stoi(base), std::stoi(count), std::stoi(side));
    return damageDie;
}

Dice MonsterParser::hpToDice(std::string hpString) {
    size_t pos = 0;
    std::string base, count, side;

    pos = hpString.find("+");
    base = hpString.substr(0, pos);
    hpString.erase(0, pos + 1);

    pos = hpString.find("d");
    count = hpString.substr(0, pos);
    hpString.erase(0, pos + 1);

    side = hpString;

    Dice hpDie(std::stoi(base), std::stoi(count), std::stoi(side));
    return hpDie;
}

int MonsterParser::rarityToInt(std::string rarityString) {
    return std::stoi(rarityString);
}

std::vector<std::string> MonsterParser::abilitiestoStringVector(std::string abilitiesString) {
    std::vector<std::string> abilitiesStringVector;
    std::string delimiter = " ";

    size_t pos = 0;
    std::string token;
    while ((pos = abilitiesString.find(delimiter)) != std::string::npos) {
        token = abilitiesString.substr(0, pos);
        abilitiesStringVector.push_back(token);

        abilitiesString.erase(0, pos + delimiter.length());
    }
    token = abilitiesString;
    abilitiesStringVector.push_back(token);

    return abilitiesStringVector;
}

std::vector<ParsedMonster> MonsterParser::getParsedMonsters() {
    return parsedMonsters;
}

void MonsterParser::printParsedData() {
    for(auto monster : this->parsedMonsters) {
        std::string description;
        std::string colors;
        std::string abilities;
        for(std::string line : monster.getDescript()) {
            description += (line + '\n');
        }
        for(std::string color : monster.getColors()) {
            colors += (color + ' ');
        }
        for(std::string ability : monster.getAbilities()) {
            abilities += (ability + ' ');
        }


        std::cout << monster.getName() << std::endl
                  << description << std::endl
                  << colors << std::endl
                  << monster.getSpeed().getBase() << "+" << monster.getSpeed().getCount() << "d" << monster.getSpeed().getSides() << std::endl
                  << abilities << std::endl
                  << monster.getHp().getBase() << "+" << monster.getHp().getCount() << "d" << monster.getHp().getSides() << std::endl
                  << monster.getDamage().getBase() << "+" << monster.getDamage().getCount() << "d" << monster.getDamage().getSides() << std::endl
                  << monster.getSymbol() << std::endl
                  << monster.getRarity() << std::endl << std::endl;
    }

}