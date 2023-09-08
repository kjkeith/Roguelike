#ifndef DICE_H
#define DICE_H

#include <stdlib.h>
#include <string>

class Dice {
private:
    int base;
    int count;
    int sides;
public:
    Dice(){};

    Dice(int base, int count, int sides) {
        this->base = base;
        this->count = count;
        this->sides = sides;
    };

    int rollDice() {
        int rollTotal = base;

        for(int i = 0; i < count; i++) {
            rollTotal +=  (rand() % sides + 1);
        }
        return rollTotal;
    };

    int getBase() const {
        return base;
    }

    void setBase(int base) {
        Dice::base = base;
    }

    int getCount() const {
        return count;
    }

    void setCount(int count) {
        Dice::count = count;
    }

    int getSides() const {
        return sides;
    }

    void setSides(int sides) {
        Dice::sides = sides;
    }
};

#endif
