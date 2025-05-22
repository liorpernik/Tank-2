#ifndef MYTANKALGORITHM_H
#define MYTANKALGORITHM_H
#include "../common/TankAlgorithm.h"
#include "../common/ActionRequest.h"
#include "../common/GameObject.h"
#include "../header/TankBattleInfo.h"

#include <memory>

using std::unique_ptr,std::make_unique;

class MyTankAlgorithm : public TankAlgorithm {
    int player_index;
    int tank_index;

protected:
    unique_ptr<TankBattleInfo> battle_info;
    std::pair<int,int> nextStep(bool forward,const std::pair<int,int> pos, const Direction dir);
    Direction calculateRealDirection(int currRow, int currCol, int targetRow, int targetCol);
    ActionRequest determineRotation(Direction currentDir, Direction desiredDir);

    // Safety check for backward movement
    bool canSafelyBack(int backR, int backC);
    bool willBeHitIn(int row, int col, int t);
    bool isAlignedWithOpponent(std::pair<int, int> opponentPos) ;
    ActionRequest checkForEscape();

    // Common decision utilities
    bool isOccupierFree(std::pair<int, int> pos) ;
    bool shouldShootOpponent(const std::pair<int, int>& opponentPos) ;
    bool canMoveFwd() ;
    bool canMoveBack() ;
    void rotate(ActionRequest action);
    bool isValidMove(ActionRequest action);
    Direction calculateDirection(int currRow, int currCol, int targetRow, int targetCol);
    OppData getClosestOpponent();
    int calculateActionsToOpponent(const OppData& oppPos);

public:
    MyTankAlgorithm(int player_index, int tank_index);
    ~MyTankAlgorithm() override = default;
    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;
    // private:
//
//     ActionRequest decideNextAction();
};
#endif //MYTANKALGORITHM_H
