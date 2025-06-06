//
// Created by admin on 5/20/2025.
//

#include "../header/TankBattleInfo.h"

#include "../header/Shell.h"

TankBattleInfo::TankBattleInfo(int tank_index, int player_index) : id(tank_index), player_id(player_index)
{
    direction = player_id == 1 ? L : R;
}


void TankBattleInfo::setBackwardCooldown(int cooldown){ backward_cooldown = cooldown; }

void TankBattleInfo::setWaitingForBackward(bool is_waiting){ is_waiting_for_backward = is_waiting; }
bool TankBattleInfo::getWaitingForBackward(){ return is_waiting_for_backward; }

void TankBattleInfo::setMovedBackwardLast(bool moved){ moved_backward_last = moved; }
bool TankBattleInfo::getMovedBackwardLast(){ return moved_backward_last; }


void TankBattleInfo::setShootCooldown(int cooldown){ shoot_cooldown = cooldown; }
bool TankBattleInfo::isWaitingToShoot() const { return shoot_cooldown > 0; }

bool TankBattleInfo::isWaitingToReverse() const { return is_waiting_for_backward && backward_cooldown > 0; }

void TankBattleInfo::decreaseRemainingShells(){ --remaining_shells; }
void TankBattleInfo::setRemainingShells(int num_of_shells) { remaining_shells = num_of_shells; set_shells = true; }
int TankBattleInfo::getRemainingShells() const { return remaining_shells; }
bool TankBattleInfo::isShellsSet() const {return  set_shells;}

void TankBattleInfo::setDirection(Direction dir){ direction = dir; }
Direction TankBattleInfo::getDirection() const { return direction; }

void TankBattleInfo::setPosition(int x, int y){ position.first = x; position.second = y; }
std::pair<int, int> TankBattleInfo::getPosition() const { return position; }


std::vector<OppData> TankBattleInfo::getOpponents() const { return opponents; }
void TankBattleInfo::setOpponents(std::vector<OppData> opps) { opponents = opps; }

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


void TankBattleInfo::setMapSize(int h, int w)
{
    map_size = std::make_pair(h,w);
}

pair<int, int> TankBattleInfo::getMapSize() const
{
    return map_size;
}

void TankBattleInfo::updateObjectDirByPosition(pair<int,int> pos, Direction dir)
{
    if (!knownObjects[pos].empty())
    {
        if (knownObjects[pos].size() > 1 && knownObjects[pos][1]->getSymbol() == '*')
        {
            dynamic_cast<Shell*>(knownObjects[pos][1])->setDirection(dir);
        }else if (knownObjects[pos][0]->getSymbol() == '*')
        {
            dynamic_cast<Shell*>(knownObjects[pos][0])->setDirection(dir);
        }
    }
}

Direction TankBattleInfo::calculateRealDirection(int currRow, int currCol, int targetRow, int targetCol) {
    int rowDiff = targetRow - currRow;
    int colDiff = targetCol - currCol;

    // Normalize differences to identify direction only if strictly aligned
    if (rowDiff > 0 && colDiff == 0) return Direction::D;    // Down
    if (rowDiff < 0 && colDiff == 0) return Direction::U;    // Up
    if (rowDiff == 0 && colDiff > 0) return Direction::R;    // Right
    if (rowDiff == 0 && colDiff < 0) return Direction::L;    // Left
    if (rowDiff == colDiff && rowDiff > 0) return Direction::DR; // Down-Right (strict diagonal)
    if (rowDiff == -colDiff && rowDiff > 0) return Direction::DL; // Down-Left (strict diagonal)
    if (rowDiff == colDiff && rowDiff < 0) return Direction::UR; // Up-Right (strict diagonal)
    if (rowDiff == -colDiff && rowDiff < 0) return Direction::UL; // Up-Left (strict diagonal)

    return Direction::None; // None for non-aligned movement
}