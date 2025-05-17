//
// Created by admin on 5/16/2025.
//
#include "../header/Tank.h"

Tank::Tank (std::pair<int,int> pos,int tank_index,  Direction dir, int owner, int num_of_shells)
    : GameObject(pos), tank_index(tank_index), direction (dir), ownerId (owner), destroyed(false), remaining_shells(num_of_shells) {}

Direction Tank::getDirection () const {
    return direction;
}

void Tank::setDirection(Direction dir){
    direction = dir;
}

int Tank::getOwnerId() const {
    return ownerId;
}

void Tank::destroy() {
    destroyed=true;
}

bool Tank::isDestroyed () const {
    return destroyed;
}

char Tank::getSymbol() const {
    return ownerId + '0'; // Using
}

void Tank::setNumOfShells(int shells){
    remaining_shells = shells;
}

int Tank::getNumOfRemainingShells() const{
    return remaining_shells;
}

void  Tank::setShootCooldown(int cooldown)
{
    shoot_cooldown = cooldown;
}

int  Tank::getShootCooldown() const
{
    return shoot_cooldown;
}

void  Tank::setBackwardCooldown(int cooldown)
{
    backward_cooldown = cooldown;
}
int  Tank::getBackwardCooldown() const
{
    return backward_cooldown;
}