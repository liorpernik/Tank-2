#include "../header/BPlayer.h"
#include "../header/Shell.h"

BPlayer::BPlayer(int player_index, size_t map_width, size_t map_height,
             size_t max_steps, size_t num_shells) : MyPlayer(player_index, map_width, map_height,max_steps,num_shells) {
 }

void BPlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& view)  {
    auto knownShells = getShellsFromKnownObjects();
    getBattleInfoFromSatelliteView(view);
    tank.updateBattleInfo(*battle_info);
}

Direction BPlayer::calcShellsDirection(){

  return Direction::None;
}

vector<Shell*> BPlayer::getShellsFromKnownObjects() {
    auto knownObj = dynamic_cast<TankBattleInfo*>(battle_info.get())->getKnownObjects();
    vector<Shell*> shells;

    for (auto& [pos, objs] : knownObj) {
        for (auto& obj : objs) {
            if (obj->getSymbol() == '*') {
                auto shell = dynamic_cast<Shell*>(obj);
                if (shell) {
                    shells.push_back(shell);
                }
            }
        }
    }
    return shells;
}