//
// Created by admin on 5/16/2025.
//
#include "../header/Wall.h"

Wall::Wall(std::pair<int,int> pos) : GameObject(pos), hitCount(0) {
}

int Wall::getHitCount() const {
    return hitCount;
}

void Wall::registerHit() {
    hitCount++;
}

void Wall::destroy() {
  destroyed = true;
}

bool Wall::isDestroyed() const {
    return hitCount >= WALL_MAX_HITS;
}

char Wall::getSymbol() const {
    return WALL_SYMBOL;
}