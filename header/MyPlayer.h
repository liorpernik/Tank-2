#ifndef MYPLAYER_H
#define MYPLAYER_H
#include "../common/Player.h"
#include <vector>
#include <memory>

class MyTankAlgorithm;
using std::unique_ptr, std::vector;
enum Direction { U, UR, R, DR, D, DL, L, UL, None};

class MyPlayer : public Player {
    int player_index;
    size_t map_width;
    size_t map_height;
    //BattleInfo last_two_battle_info[2] or vector<BattleInfo> battle_infos? TODO: class that implements BattleInfo
	vector<int> last_battle_info_reqs;
	vector<unique_ptr<MyTankAlgorithm>> tanks;

public:
    MyPlayer(int player_index, size_t map_width, size_t map_height,
             size_t max_steps, size_t num_shells);

    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& view) override;
	void setRoles();
    Direction calcShellDirection();
};

#endif //MYPLAYER_H
