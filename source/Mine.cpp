//
// Created by admin on 5/16/2025.
//
#include "../header/Mine.h"

Mine::Mine(std::pair<int,int> pos) : GameObject(pos), triggered(false), destroyed(false) {
}

void Mine::trigger() {
    triggered = true;
}

bool Mine::isTriggered() const {
    return triggered;
}

void Mine::destroy() {
    destroyed = true;
}

bool Mine::isDestroyed() const {
    return destroyed;
}

char Mine::getSymbol() const {
    return MINE_SYMBOL;
}