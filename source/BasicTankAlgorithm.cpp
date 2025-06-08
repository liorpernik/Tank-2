#include "../header/BasicTankAlgorithm.h"

BasicTankAlgorithm::BasicTankAlgorithm(int player_index, int tank_index) : MyTankAlgorithm(player_index, tank_index){}

ActionRequest BasicTankAlgorithm::decideAction() {
	if (battle_info->getPosition()== make_pair(-1,-1)) return ActionRequest::GetBattleInfo;
    // Periodically update battle info (every 5 turns)

    if (currentTurn - lastInfoUpdate > 3) {
        lastInfoUpdate = currentTurn;
        return ActionRequest::GetBattleInfo;
    }
    currentTurn++;

    // Escape if in immediate danger
    if(willBeHitIn(battle_info->getPosition().first, battle_info->getPosition().second, 1)) {
        ActionRequest escape = checkForEscape();
        if(escape != ActionRequest::DoNothing) {
            return escape;
        }
        // If no escape, try to shoot
        if(!battle_info->isWaitingToShoot() && battle_info->getRemainingShells() > 0) {
            battle_info->setShootCooldown();
            battle_info->decreaseRemainingShells();
            return ActionRequest::Shoot;
        }
    }

    // Shoot if aligned with opponent and have shells
    OppData closestOpp = getClosestOpponent();
    if(closestOpp.opponentPos != make_pair(-1,-1)) {
        if(isAlignedWithOpponent(closestOpp.opponentPos) &&
           !battle_info->isWaitingToShoot() && 
           battle_info->getRemainingShells() > 0) {
            return ActionRequest::Shoot;
        }
        
        // Rotate toward opponent if not aligned
        Direction currentDir = battle_info->getDirection();
        Direction desiredDir = calculateDirection(battle_info->getPosition().first,battle_info->getPosition().second,
                                                closestOpp.opponentPos.first,closestOpp.opponentPos.second);
        if(currentDir != desiredDir) {
            return determineRotation(currentDir, desiredDir);
        }
    }

    // Move forward if path is clear
    if(canMoveFwd()) {
        return ActionRequest::MoveForward;
    }

    // Move backward if forward blocked and backward is safe
    if(canMoveBack()) {
        return ActionRequest::MoveBackward;
    }

    // Default rotation when stuck
    return ActionRequest::RotateRight45;
}
