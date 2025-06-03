#ifndef MYPLAYER_H
#define MYPLAYER_H
#include "../common/Player.h"
#include "../header/TankBattleInfo.h"
#include "../common/TankAlgorithm.h"  // Add this include
#include <vector>
#include <memory>

using std::unique_ptr, std::vector;

class MyPlayer : public Player {

protected:
    int player_index;
    size_t map_width = 0;
    size_t map_height = 0;
	int steps_left;
	int shells_per_tank;
    // TODO: class that implements BattleInfo, here, DataStructure to handle last 2 sequential battleinfos(to determine direction)
	// vector<unique_ptr<TankAlgorithm>> tanks;
	unique_ptr<BattleInfo> battle_info;

public:
    MyPlayer(int player_index, size_t map_width, size_t map_height,
             size_t max_steps, size_t num_shells);

	~MyPlayer() override = default;

	// Delete copy operations
	MyPlayer(const MyPlayer&) = delete;
	MyPlayer& operator=(const MyPlayer&) = delete;

    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& view) override;
	// void addTank(unique_ptr<TankAlgorithm>& tank);
	int getPlayerIndex() const;

    void getBattleInfoFromSatelliteView(SatelliteView& view);
};

#endif //MYPLAYER_H
