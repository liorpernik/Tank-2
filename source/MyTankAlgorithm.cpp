#include "../header/MyTankAlgorithm.h"

#include "../header/TankBattleInfo.h"
MyTankAlgorithm::MyTankAlgorithm(int player_index, int tank_index) : player_index(player_index), tank_index(tank_index) {

}

void MyTankAlgorithm::setShells(int num_of_shells)
{
    remaining_tanks = num_of_shells;
}

void MyTankAlgorithm::updateBattleInfo(BattleInfo& info)
{
    (void)info;
}

ActionRequest MyTankAlgorithm::getAction()
{
   return decideNextAction();
}

ActionRequest MyTankAlgorithm::decideNextAction()
{
    return ActionRequest::DoNothing;
}