#include "../header/MyPlayer.h"

#include "../header/Wall.h"
#include "../header/Mine.h"
#include "../header/Tank.h"
#include "../header/Shell.h"

#include "../header/BoardSatelliteView.h"

/**
 * @brief Constructs a MyPlayer instance with game parameters.
 * @param player_index Index of this player.
 * @param map_width Width of the game map.
 * @param map_height Height of the game map.
 * @param max_steps Maximum allowed steps for the player.
 * @param num_shells Number of shells per tank.
 */
MyPlayer::MyPlayer(int player_index, size_t map_width, size_t map_height, size_t max_steps, size_t num_shells)
    : player_index(player_index), map_width(map_width), map_height(map_height),
      steps_left(static_cast<int>(max_steps)), shells_per_tank(num_shells), battle_info(make_unique<TankBattleInfo>(-1, player_index)) {}

/**
 * @brief Returns the player index.
 * @return Player's index.
 */
int MyPlayer::getPlayerIndex() const { return player_index; }

/**
 * @brief Updates the given tank's battle info based on the current satellite view.
 * @param tank Reference to a tank algorithm to update.
 * @param view Current satellite view of the board.
 */
void MyPlayer::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &view)
{
    getBattleInfoFromSatelliteView(view);
    tank.updateBattleInfo(*battle_info);
}

// todo: check after another get battle info that all was updated
/**
 * @brief Extracts and updates battle info from the satellite view.
 * @param view Current satellite view of the board.
 */
void MyPlayer::getBattleInfoFromSatelliteView(SatelliteView &view)
{
    int i = 0, j = 0;

    char symbol = ' ';
    int tanki_1 = 0, tanki_2 = 0;
    auto *tank_info = dynamic_cast<TankBattleInfo *>(battle_info.get());

    vector<OppData> opponents;
    map<pair<int, int>, vector<GameObject *>> knownObjects = tank_info->getKnownObjects();
    // This will own all the game objects
    static vector<unique_ptr<GameObject>> objectStorage;

    if (!tank_info->isShellsSet())
        tank_info->setRemainingShells(shells_per_tank);
    if (tank_info->getMapSize().first == -1)
        tank_info->setMapSize(map_height, map_width);

    for (i = 0; i < static_cast<int>(map_height); i++)
    {
        for (j = 0; j < static_cast<int>(map_width); j++)
        {
            symbol = view.getObjectAt(i, j);
            if (symbol == '&')
            {
                break;
            }
            if (symbol == '%')
            {
                tank_info->setPosition(i, j);
                continue;
            }
            pair pos = {i, j};

            if (symbol == ' ')
            {
                if (knownObjects.contains(pos))
                    knownObjects.erase(pos);
                continue;
            }
            vector<GameObject *> vec = knownObjects[pos];

            if (vec.size() > 1 && (symbol == '@' || symbol == '#'))
            {
                knownObjects[pos].erase(knownObjects[pos].end() - 1);
            }
            else
            {
                unique_ptr<GameObject> obj;
                if (symbol == '@' && vec.empty())
                {
                    obj = make_unique<Mine>(pos);
                }
                else if (symbol == '#' && vec.empty())
                {
                    obj = make_unique<Wall>(pos);
                }
                else if (symbol == '*')
                {
                    obj = make_unique<Shell>(pos, None, -1);
                }
                else if (isdigit(symbol))
                {
                    int player = symbol - '0';
                    obj = make_unique<Tank>(pos, player_index == 1 ? tanki_1++ : tanki_2++, Direction::None, player, shells_per_tank);
                    if (player != player_index)
                    {
                        opponents.push_back(OppData(pos));
                    }
                }
                if (obj)
                {
                    knownObjects[pos].push_back(obj.get());
                    objectStorage.push_back(move(obj));
                }
            }
        }
    }

    tank_info->setKnownObjects(knownObjects);
    tank_info->setOpponents(opponents);
}

/**
 * @brief Decrements the number of steps left for the player.
 */
void MyPlayer::updateStepsLeft()
{
    --steps_left;
}