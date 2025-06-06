#ifndef BPLAYER_H
#define BPLAYER_H

#include "../header/MyPlayer.h"

class Shell;
using std::unique_ptr, std::vector;


class BPlayer : public MyPlayer {

	int last_battleInfo_step = steps_left;

   void calcShellsDirection(vector<Shell*> knownShells);

  public:
    BPlayer(int player_index, size_t map_width, size_t map_height,
             size_t max_steps, size_t num_shells);

    ~BPlayer() override = default;

    BPlayer(const BPlayer&) = delete;
    BPlayer& operator=(const BPlayer&) = delete;

    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& view) override;
    vector<Shell*> getShellsFromKnownObjects();
};
#endif //BPLAYER_H
