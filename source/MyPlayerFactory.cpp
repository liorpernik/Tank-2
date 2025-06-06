#include "../header/MyPlayerFactory.h"

#include "../header/BPlayer.h"

unique_ptr<Player> MyPlayerFactory::create(int player_index, size_t x, size_t y,size_t max_steps, size_t num_shells) const {
    return player_index == 1 ? make_unique<BPlayer>(player_index, y, x, max_steps, num_shells)
            : make_unique<MyPlayer>(player_index, y, x, max_steps, num_shells);
}