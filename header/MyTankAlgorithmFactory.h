#ifndef MYTANKALGORITHMFACTORY_H
#define MYTANKALGORITHMFACTORY_H
#include "../common/TankAlgorithmFactory.h"

using std::unique_ptr,std::make_unique;

class MyTankAlgorithmFactory : public TankAlgorithmFactory {
public:
    MyTankAlgorithmFactory() = default;
    ~MyTankAlgorithmFactory() override= default;
    unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override;
};

#endif //MYTANKALGORITHMFACTORY_H
