#ifndef BPLAYER_H
#define BPLAYER_H

#include "../header/MyPlayer.h"


using std::unique_ptr, std::vector;
class Shell;
class BPlayer : public MyPlayer {

   Direction calcShellsDirection();

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
