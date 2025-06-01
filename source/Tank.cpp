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

void Tank::rotate(ActionRequest action) {

    switch(action) {
        case ActionRequest::RotateLeft45:
            direction = static_cast<Direction>((direction + 7) % 8); // move 1 counter-clockwise
        break;
        case ActionRequest::RotateRight45:
            direction = static_cast<Direction>((direction + 1) % 8); // move 1 clockwise
        break;
        case ActionRequest::RotateLeft90:
            direction = static_cast<Direction>((direction + 6) % 8); // move 2 clockwise
        break;
        case ActionRequest::RotateRight90:
            direction = static_cast<Direction>((direction + 2) % 8); // move 2 counter-clockwise
        break;
        case ActionRequest::MoveForward:
            case ActionRequest::MoveBackward:
        case ActionRequest::Shoot:
        case ActionRequest::DoNothing:
        case ActionRequest::GetBattleInfo:
            break;
    }
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