#ifndef ADVANCED_PLAYER_H
#define ADVANCED_PLAYER_H

#include "../header/MyPlayer.h"

class Shell;
using std::unique_ptr, std::vector;


class AdvancedPlayer : public MyPlayer {

	int last_battleInfo_step = steps_left;

   void calcShellsDirection(vector<Shell*> knownShells);

  public:
    AdvancedPlayer(int player_index, size_t map_width, size_t map_height,
             size_t max_steps, size_t num_shells);

    ~AdvancedPlayer() override = default;

    AdvancedPlayer(const AdvancedPlayer&) = delete;
    AdvancedPlayer& operator=(const AdvancedPlayer&) = delete;

    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& view) override;
    vector<Shell*> getShellsFromKnownObjects();
};
#endif //ADVANCED_PLAYER_H
