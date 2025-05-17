#include "../header/MyTankAlgorithmFactory.h"
#include "../header/MyTankAlgorithm.h"

unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const {
    return make_unique<MyTankAlgorithm>(player_index, tank_index);
}