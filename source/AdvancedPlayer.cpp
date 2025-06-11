#include "../header/AdvancedPlayer.h"
#include "../header/Shell.h"

/**
 * Constructor for AdvancedPlayer.
 * Initializes the player with map size, max steps, and number of shells.
 */
AdvancedPlayer::AdvancedPlayer(int player_index, size_t map_width, size_t map_height,
                               size_t max_steps, size_t num_shells) : MyPlayer(player_index, map_width, map_height, max_steps, num_shells)
{
}

/**
 * @brief Updates the provided tank's battle info using the current satellite view.
 *
 * @param tank The tank algorithm instance to update.
 * @param view The satellite view containing the latest battlefield info.
 */
void AdvancedPlayer::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &view)
{
    auto knownShells = getShellsFromKnownObjects();
    getBattleInfoFromSatelliteView(view);

    calcShellsDirection(knownShells);

    tank.updateBattleInfo(*battle_info);
    last_battleInfo_step = steps_left;
}

/**
 * @brief Calculates the direction of shells that currently have unknown directions.
 * This method attempts to infer direction by checking where
 *
 * @param knownShells Vector of pointers to known Shell objects.
 */
void AdvancedPlayer::calcShellsDirection(vector<Shell *> knownShells)
{

    int steps_passed = 2 * (last_battleInfo_step - steps_left);
    vector<pair<int, int>> candidates;
    auto tank_info = dynamic_cast<TankBattleInfo *>(battle_info.get());
    pair<int, int> new_pos;

    auto calc_pos = [h = this->map_height, w = this->map_width](const pair<int, int> &pos,
                                                                const pair<int, int> &dir_offset,
                                                                int steps)
    {
        return make_pair(
            ((pos.first + dir_offset.first * steps) % h + h) % h,
            ((pos.second + dir_offset.second * steps) % w + w) % w);
    };

    for (auto shell : knownShells)
    {
        if (shell->getDirection() == Direction::None)
        {
            for (int dir = U; dir != None; dir++)
            {
                new_pos = calc_pos(shell->getPos(), offsets[dir], steps_passed);
                auto candidate = tank_info->getObjectByPosition(new_pos);
                if (candidate != nullptr && candidate->getSymbol() == '*')
                    candidates.push_back(candidate->getPos());
            }
        }
        else
        {
            new_pos = calc_pos(shell->getPos(), offsets[shell->getDirection()], steps_passed);
            auto candidate = tank_info->getObjectByPosition(new_pos);
            if (candidate == nullptr || (candidate != nullptr && candidate->getSymbol() != '*'))
            {
                for (int dir = U; dir != None; dir++)
                {
                    new_pos = calc_pos(shell->getPos(), offsets[dir], steps_passed);
                    auto candidate = tank_info->getObjectByPosition(new_pos);
                    if (candidate != nullptr && candidate->getSymbol() == '*')
                        candidates.push_back(candidate->getPos());
                }
            }
        }
        if (!candidates.empty())
        {
            auto [r, c] = shell->getPos();
            auto [nr, nc] = candidates[0];
            tank_info->updateObjectDirByPosition(new_pos, tank_info->calculateRealDirection(r, c, nr, nc));
            candidates.clear();
        }
    }
}

/**
 * @brief Retrieves a vector of pointers to all known Shell objects in the current battle info.
 *
 * @return Vector of Shell pointers representing all known shells.
 */
vector<Shell *> AdvancedPlayer::getShellsFromKnownObjects()
{
    auto knownObj = dynamic_cast<TankBattleInfo *>(battle_info.get())->getKnownObjects();
    vector<Shell *> shells;

    for (auto &[pos, objs] : knownObj)
    {
        for (auto &obj : objs)
        {
            if (obj->getSymbol() == '*')
            {
                auto shell = dynamic_cast<Shell *>(obj);
                if (shell)
                {
                    shells.push_back(shell);
                }
            }
        }
    }
    return shells;
}