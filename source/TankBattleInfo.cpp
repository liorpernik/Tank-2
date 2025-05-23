//
// Created by admin on 5/20/2025.
//

#include "../header/TankBattleInfo.h"

TankBattleInfo::TankBattleInfo(int tank_index, int player_index) : id(tank_index), player_id(player_index) {}

void TankBattleInfo::decreaseBackwardCooldown(){ --backward_cooldown; }
void TankBattleInfo::increaseBackwardCooldown(int cooldown){ backward_cooldown += cooldown; }

void TankBattleInfo::setWaitingForBackward(bool is_waiting){ is_waiting_for_backward = is_waiting; }
bool TankBattleInfo::getWaitingForBackward(){ return is_waiting_for_backward; }

void TankBattleInfo::setMovedBackwardLast(bool moved){ moved_backward_last = moved; }
bool TankBattleInfo::getMovedBackwardLast(){ return moved_backward_last; }

void TankBattleInfo::decreaseShootCooldown(){ --shoot_cooldown; }
void TankBattleInfo::increaseShootCooldown(int cooldown){ shoot_cooldown += cooldown; }
bool TankBattleInfo::isWaitingToShoot() const { return shoot_cooldown > 0; }

bool TankBattleInfo::isWaitingToReverse() const { return is_waiting_for_backward && backward_cooldown > 0; }

void TankBattleInfo::decreaseRemainingShells(){ --remaining_shells; }
void TankBattleInfo::setRemainingShells(int num_of_shells) { remaining_shells = num_of_shells; }
int TankBattleInfo::getRemainingShells() const { return remaining_shells; }

void TankBattleInfo::setDirection(Direction dir){ direction = dir; }
Direction TankBattleInfo::getDirection() const { return direction; }

void TankBattleInfo::setPosition(int x, int y){ position.first = x; position.second = y; }
std::pair<int, int> TankBattleInfo::getPosition() const { return position; }


// std::vector<std::unique_ptr<OppData>> TankBattleInfo::getOpponents() const { return opponents; }

void TankBattleInfo::addOpponent(std::pair<int,int> position, Direction dir)
{
    opponents.emplace_back(OppData(std::make_pair(position.first, position.second), dir));
}

GameObject* TankBattleInfo::getObjectByPosition(std::pair<int,int> pos) const
{
    return knownObjects.contains(pos) ? knownObjects.at(pos).size() > 1 ? knownObjects.at(pos)[1] : knownObjects.at(pos)[0] : nullptr;
}
//OppData* TankBattleInfo::getOpponentAt() const{
//
//}
std::map<std::pair<int, int>, std::vector<GameObject*>> TankBattleInfo::getKnownObjects() const
{
    return knownObjects;
}

void TankBattleInfo::setKnownObjects(std::map<std::pair<int, int>, std::vector<GameObject*>> newKnownObjects)
{
    knownObjects = std::move(newKnownObjects);
}