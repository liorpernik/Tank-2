#include "../header/MyPlayer.h"
#include "../header/BoardSatelliteView.h"

MyPlayer::MyPlayer(int player_index, size_t map_width, size_t map_height, size_t max_steps, size_t num_shells)
    : player_index(player_index), map_width(map_width), map_height(map_height),
        steps_left(static_cast<int>(max_steps)), shells_per_tank(num_shells), battle_info(unique_ptr<TankBattleInfo>())
{
}

void MyPlayer::addTank(unique_ptr<TankAlgorithm>& tank)
{
    tanks.push_back(std::move(tank));
}

int MyPlayer::getPlayerIndex() const { return player_index;}

void MyPlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& view)
{
    getBattleInfoFromSatelliteView(view);
    tank.updateBattleInfo(*battle_info);
}

void MyPlayer::getBattleInfoFromSatelliteView(SatelliteView& view) const
{
   //todo

    view.getObjectAt(0,0);
}