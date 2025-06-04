#ifndef BASIC_TANK_ALGORITHM_H
#define BASIC_TANK_ALGORITHM_H

#include "MyTankAlgorithm.h"
#include <utility>


using std::pair,std::make_pair;

class BasicTankAlgorithm : public MyTankAlgorithm {
    int currentTurn = 0;
    int lastInfoUpdate = 0;

public:
    BasicTankAlgorithm(int player_index, int tank_index);
    ~BasicTankAlgorithm() override = default;

    BasicTankAlgorithm(const BasicTankAlgorithm&) = delete;
    BasicTankAlgorithm& operator=(const BasicTankAlgorithm&) = delete;

    //ActionRequest getAction() override;
    ActionRequest decideAction() override;
};

#endif