//
// Created by admin on 5/16/2025.
//
#include "../header/Shell.h"

Shell::Shell (std::pair<int,int> pos, Direction dir, int owner)
    : GameObject(pos), direction (dir), ownerId (owner), destroyed(false) {}

Direction Shell::getDirection () const {
    return direction;
}

void Shell::setDirection (Direction dir)
{
    direction = dir;
}

int Shell::getOwnerId() const {
    return ownerId;
}

void Shell::destroy() {
    destroyed=true;
}

bool Shell::isDestroyed () const {
    return destroyed;
}

char Shell::getSymbol() const {
     return SHELL_SYMBOL; // Using
}