#include "../header/MyTankAlgorithmFactory.h"
#include "../header/AdvancedTankAlgorithm.h"
#include "../header/BasicTankAlgorithm.h"

/**
 * @brief Factory method to create TankAlgorithm instances.
 * @param player_index The index of the player requesting the algorithm.
 * @param tank_index The index of the tank controlled by the player.
 * @return unique_ptr<TankAlgorithm> A unique pointer to the created TankAlgorithm instance.
 *
 * @note For player_index == 1, an AdvancedTankAlgorithm is created (the winning algorithm).
 *       For all other players, a BasicTankAlgorithm is created.
 */
unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const {
    if (player_index == 1)
        return make_unique<AdvancedTankAlgorithm>(player_index, tank_index); // Winning algorithm
    return make_unique<BasicTankAlgorithm>(player_index, tank_index); // Basic algorithm
}
