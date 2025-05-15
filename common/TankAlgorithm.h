#ifndef TANKALGORITHM_H
#define TANKALGORITHM_H
class ActionRequest;
class BattleInfo;

class TankAlgorithm {
public:
    virtual ~TankAlgorithm() {}
    virtual ActionRequest getAction() = 0;
    virtual void updateBattleInfo(BattleInfo& info) = 0;
};

#endif //TANKALGORITHM_H
