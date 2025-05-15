#ifndef MYPLAYERFACTORY_H
#define MYPLAYERFACTORY_H
#include "../common/PlayerFactory.h"
#include "./MyPlayer.h"
#include <memory>

using std::unique_ptr,std::make_unique;

class MyPlayerFactory : public PlayerFactory {
public:
    unique_ptr<Player> create(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) const override;
};

#endif //MYPLAYERFACTORY_H
