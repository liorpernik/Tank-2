#include "../header/MyTankAlgorithmFactory.h"
#include "../header/BTankAlgorithm.h"

unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const {
    return make_unique<BTankAlgorithm>(player_index, tank_index);
}