//
// Created by admin on 5/20/2025.
//

#ifndef BTANKALGORITHM_H
#define BTANKALGORITHM_H
#include <utility>
#include <optional>
#include <queue>
#include <algorithm>

#include "../common/ActionRequest.h"
#include "../header/MyTankAlgorithm.h"

using namespace std;

struct RotationOption {
    ActionRequest action;
    int safetyScore;
    bool canMove;
    Direction newDir;
  };

class BTankAlgorithm : public MyTankAlgorithm {

  vector<ActionRequest> rotations = {ActionRequest::RotateLeft45,ActionRequest::RotateLeft90, ActionRequest::RotateRight45, ActionRequest::RotateRight90};
  int countOpenSpaceInDirection(pair<int,int> pos);
  RotationOption rotationOption(ActionRequest rotation, Direction newDir,Direction oldDir );
  ActionRequest calculateBestEscapeRotation();
  bool canShootAfterRotate(Direction targetDir, OppData& opp);
  bool shouldShootOpponent(OppData& opp);
  Direction simulateRotation(ActionRequest act);
  ActionRequest decideAction();
  void moveKnownShells();
  public:
  BTankAlgorithm(int player_index, int tank_index);
  ~BTankAlgorithm() override = default;

  BTankAlgorithm(const BTankAlgorithm&) = delete;
  BTankAlgorithm& operator=(const BTankAlgorithm&) = delete;

  // void updateBattleInfo(BattleInfo& battleInfo) override;

  ActionRequest getAction() override;
};

#endif //BTANKALGORITHM_H
