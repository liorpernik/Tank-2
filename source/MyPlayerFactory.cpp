#include "../header/MyPlayerFactory.h"

#include "../header/AdvancedPlayer.h"

/**
 * @brief Creates a Player instance based on the player index.
 * @param player_index The index of the player to create.
 * @param x Map width.
 * @param y Map height.
 * @param max_steps Maximum number of steps allowed.
 * @param num_shells Number of shells per tank.
 * @return A unique_ptr to the created Player instance.
 */
unique_ptr<Player> MyPlayerFactory::create(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) const
{
    return player_index == 1 ? make_unique<AdvancedPlayer>(player_index, y, x, max_steps, num_shells)
                             : make_unique<MyPlayer>(player_index, y, x, max_steps, num_shells);
}