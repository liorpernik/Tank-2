#ifndef MYTANKALGORITHM_H
#define MYTANKALGORITHM_H
#include "../common/TankAlgorithm.h"
#include <memory>

enum class ActionRequest : int;
using std::unique_ptr,std::make_unique;

class MyTankAlgorithm : public TankAlgorithm {
    int player_index;
    int tank_index;
    int shoot_cooldown = 0;
    int backward_cooldown = 0;
public:
    MyTankAlgorithm(int player_index, int tank_index);

    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;

private:

    ActionRequest decideNextAction();
};
#endif //MYTANKALGORITHM_H
