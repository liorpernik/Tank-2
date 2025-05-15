#ifndef MYTANKALGORITHMFACTORY_H
#define MYTANKALGORITHMFACTORY_H
#include "../common/TankAlgorithmFactory.h"
#include "MyTankAlgorithm.h"

class MyTankAlgorithmFactory : public TankAlgorithmFactory {
public:
    std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override;
};

#endif //MYTANKALGORITHMFACTORY_H
