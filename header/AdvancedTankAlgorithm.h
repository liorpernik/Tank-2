#ifndef ADVANCEDTANKALGORITHM_H
#define ADVANCEDTANKALGORITHM_H
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

class AdvancedTankAlgorithm : public MyTankAlgorithm {

  int current_turn = 0;
  int last_info_update = 0;

  vector<ActionRequest> rotations = {ActionRequest::RotateLeft45,ActionRequest::RotateLeft90, ActionRequest::RotateRight45, ActionRequest::RotateRight90};
  int countOpenSpaceInDirection(pair<int,int> pos);
  RotationOption rotationOption(ActionRequest rotation, Direction newDir,Direction oldDir );
  ActionRequest calculateBestEscapeRotation();
  bool canShootAfterRotate(Direction targetDir, OppData& opp);
  bool shouldShootOpponent(OppData& opp);
  Direction simulateRotation(ActionRequest act);
  ActionRequest decideAction() override;
  void moveKnownShells();

  public:
  AdvancedTankAlgorithm(int player_index, int tank_index);
  ~AdvancedTankAlgorithm() override = default;

  AdvancedTankAlgorithm(const AdvancedTankAlgorithm&) = delete;
  AdvancedTankAlgorithm& operator=(const AdvancedTankAlgorithm&) = delete;

  ActionRequest getAction() override;
};

#endif
