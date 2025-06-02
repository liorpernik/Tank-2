#include "../header/MyPlayerFactory.h"
#include "../header/MyPlayer.h"

unique_ptr<Player> MyPlayerFactory::create(int player_index, size_t x, size_t y,size_t max_steps, size_t num_shells) const {
    return make_unique<MyPlayer>(player_index, y, x, max_steps, num_shells);
}