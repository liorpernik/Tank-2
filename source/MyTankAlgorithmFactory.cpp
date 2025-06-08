#include "../header/MyTankAlgorithmFactory.h"
#include "../header/BTankAlgorithm.h"
#include "../header/BasicTankAlgorithm.h"


unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const {
    if (player_index==1)
        return make_unique<BTankAlgorithm>(player_index, tank_index); //winning algo
    return make_unique<BasicTankAlgorithm>(player_index, tank_index); //basic algo
}