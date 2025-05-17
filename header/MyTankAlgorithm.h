#ifndef MYTANKALGORITHM_H
#define MYTANKALGORITHM_H
#include "../common/TankAlgorithm.h"
#include "../common/ActionRequest.h"
#include <memory>

using std::unique_ptr,std::make_unique;

class MyTankAlgorithm : public TankAlgorithm {
    int player_index;
    int tank_index;
    int remaining_tanks = 0;
public:
    MyTankAlgorithm(int player_index, int tank_index);
    ~MyTankAlgorithm() override = default;
    void setShells(int num_of_shells);
    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;

private:

    ActionRequest decideNextAction();
};
#endif //MYTANKALGORITHM_H
