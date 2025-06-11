#include "../header/BasicTankAlgorithm.h"

// Constructor initializes the tank algorithm with player and tank indices
BasicTankAlgorithm::BasicTankAlgorithm(int player_index, int tank_index) : MyTankAlgorithm(player_index, tank_index) {}

/**
 * @brief Determines the next move for the tank based on current gathered info.
 *
 * @return The next decided Action.
 */
ActionRequest BasicTankAlgorithm::decideAction()
{
    current_turn++;

    // Request initial battle info if not yet received
    if (battle_info->getPosition() == make_pair(-1, -1))
    {
        last_info_update = current_turn;
        return ActionRequest::GetBattleInfo;
    }

    // Periodically update battle info (every 4 turns)
    if (current_turn - last_info_update > 3)
    {
        last_info_update = current_turn;
        return ActionRequest::GetBattleInfo;
    }

    // Escape if in immediate danger
    if (willBeHitIn(battle_info->getPosition().first, battle_info->getPosition().second, 1))
    {
        ActionRequest escape = checkForEscape();
        if (escape != ActionRequest::DoNothing)
        {
            return escape;
        }
        // If no escape, try to shoot
        if (!battle_info->isWaitingToShoot() && battle_info->getRemainingShells() > 0)
        {
            battle_info->setShootCooldown();
            battle_info->decreaseRemainingShells();
            return ActionRequest::Shoot;
        }
    }

    // Shoot if aligned with opponent and have shells
    OppData closestOpp = getClosestOpponent();
    if (closestOpp.opponentPos != make_pair(-1, -1))
    {
        if (isAlignedWithOpponent(closestOpp.opponentPos) &&
            !battle_info->isWaitingToShoot() &&
            battle_info->getRemainingShells() > 0)
        {
            return ActionRequest::Shoot;
        }

        // Rotate toward opponent if not aligned
        Direction currentDir = battle_info->getDirection();
        Direction desiredDir = calculateDirection(battle_info->getPosition().first, battle_info->getPosition().second,
                                                  closestOpp.opponentPos.first, closestOpp.opponentPos.second);
        if (currentDir != desiredDir)
        {
            return determineRotation(currentDir, desiredDir);
        }
    }

    // Move forward if path is clear
    if (canMoveFwd())
    {
        return ActionRequest::MoveForward;
    }

    // Move backward if forward blocked and backward is safe
    if (canMoveBack())
    {
        return ActionRequest::MoveBackward;
    }

    // Default rotation when stuck
    return ActionRequest::RotateRight45;
}
