#include "../header/BTankAlgorithm.h"

BTankAlgorithm::BTankAlgorithm(int player_index, int tank_index) : MyTankAlgorithm(player_index, tank_index){}

/**
 * @brief Determines the next move for the tank based on the opponent.
 *
 * @param opp The opponent's data.
 * @return The next decided Action.
 */
ActionRequest BTankAlgorithm::decideAction() {
	auto [currentRow, currentCol] = battle_info->getPosition();

	OppData opp = getClosestOpponent();
	if (opp.opponentPos == make_pair(-1,-1)) return ActionRequest::GetBattleInfo;

	auto [targetRow, targetCol] = opp.opponentPos;

	Direction currentDir = battle_info->getDirection();
	Direction desiredDir = calculateDirection(currentRow, currentCol, targetRow, targetCol);

	if(!battle_info->isWaitingToReverse() && battle_info->getWaitingForBackward())
		return ActionRequest::MoveBackward;

	// Try to dodge danger
	if (willBeHitIn(battle_info->getPosition().first,battle_info->getPosition().second,1)) {
		ActionRequest res= checkForEscape();
		if(res!=ActionRequest::DoNothing)
			return res;
		if(battle_info->getRemainingShells() > 0 && !battle_info->isWaitingToShoot())
			return ActionRequest::Shoot;
	}
	if(willBeHitIn(battle_info->getPosition().first,battle_info->getPosition().second,2)) {
		ActionRequest res;
		res =calculateBestEscapeRotation();
		if(res!=ActionRequest::DoNothing)
			return res;
		else {
			res=checkForEscape();
			if(res!=ActionRequest::DoNothing)
				return res;
		}
	}

    // Shoot if aligned and safe
    if (shouldShootOpponent(opp)) {
        return !battle_info->isWaitingToShoot() ? ActionRequest::Shoot : ActionRequest::GetBattleInfo;
    }
    if (currentDir != desiredDir) {
         ActionRequest rotate= determineRotation(currentDir, desiredDir);
         RotationOption option = rotationOption(rotate, desiredDir, currentDir);
         if((option.canMove || canShootAfterRotate(desiredDir, opp)) && option.safetyScore > 0)
         	return rotate;
    }

    // Move forward if not aligned/blocked
    pair<int,int> next = nextStep(true, battle_info->getPosition(), battle_info->getDirection());
    if ((currentRow != targetRow || currentCol != targetCol) && isOccupierFree(next)) {
        return ActionRequest::MoveForward;
    }

    // Default fallback ActionRequest
    ActionRequest r =calculateBestEscapeRotation();
    return r == ActionRequest::DoNothing ? ActionRequest::GetBattleInfo : r;
}

/**
 * @brief Simulates a rotation and checks if moving forward is possible afterward.
 *
 * @param act The rotation ActionRequest to simulate.
 * @return The new direction after the rotation, or Direction::None if not safe.
 */
Direction BTankAlgorithm::simulateRotation(ActionRequest act) {
	Direction currentDir = battle_info->getDirection();
	rotate(act);
	Direction newDir = battle_info->getDirection();
	if(canMoveFwd()) {
		battle_info->setDirection(currentDir);
		return newDir;
	}
	return Direction::None;
}

/**
 * @brief Checks whether the tank should shoot the opponent based on their predicted movement.
 *
 * @param opp The opponent's data.
 * @return True if shooting is advantageous, false otherwise.
 */
bool BTankAlgorithm::shouldShootOpponent(OppData& opp) {
	if (battle_info->isWaitingToShoot() || battle_info->getRemainingShells() <= 0) {
		return false;
	}

	auto [currentRow, currentCol] = battle_info->getPosition();
	auto [targetRow, targetCol] = opp.opponentPos;

	// Basic opponent always moves forward when possible, else rotates right
	Direction oppDir = opp.opponentDir;
	auto [dr, dc] = offsets[static_cast<int>(oppDir)];
	auto [h,w] = battle_info->getMapSize();

	// Predict opponent's next 2 positions
	vector<pair<int,int>> predictedPositions = {
		{targetRow, targetCol},  // Current position
		{wrap(targetRow + dr,h), wrap(targetCol + dc,w)} // Next position
		,{wrap(targetRow + dr*2,h), wrap(targetCol + dc*2,w)} // Position after next
	};

    // Check if we're aligned with any predicted position
    for (const auto& [r, c] : predictedPositions) {
        if (battle_info->getDirection() == calculateRealDirection(currentRow, currentCol, r, c)) {
            return true;
        }
    }
    return false;
}

// New helper function
bool BTankAlgorithm::canShootAfterRotate(Direction targetDir, OppData& opp) {
    // Simulate rotation
    Direction currentDir = battle_info->getDirection();
    battle_info->setDirection(targetDir);

    // Check if we could shoot after rotating
    bool canShoot = shouldShootOpponent(opp);

    // Restore direction
    battle_info->setDirection(currentDir);
    return canShoot;
}

/**
 * @brief Calculates the best rotation to escape from danger.
 *
 * @return The best escape rotation ActionRequest, or ActionRequest::None if none found.
 */
ActionRequest BTankAlgorithm::calculateBestEscapeRotation() {
	Direction currentDir = battle_info->getDirection();

	vector<RotationOption> options;
	RotationOption option;
	// Evaluate each rotation option
	for (ActionRequest rotation : rotations) {
		// Simulate rotation
		rotate(rotation);
		Direction newDir = battle_info->getDirection();

		option = rotationOption(rotation, newDir, currentDir);
		options.push_back(option);
	}

    // Sort options by safety score (descending)
    std::sort(options.begin(), options.end(), [](const RotationOption& a, const RotationOption& b) {
        if (a.safetyScore == b.safetyScore) {
            return a.canMove > b.canMove; // Prefer options that allow movement
        }
        return a.safetyScore > b.safetyScore;
    });

	return options.empty() ? ActionRequest::DoNothing : options[0].action;
}

/**
 * @brief Evaluates a rotation option, assigning a safety score.
 *
 * @param rotation The rotation action to consider.
 * @param newDir The new direction after rotation.
 * @param oldDir The old direction before rotation.
 * @return A RotationOption containing safety evaluation.
 */
RotationOption BTankAlgorithm::rotationOption(ActionRequest rotation, Direction newDir,Direction oldDir ) {
	RotationOption option;
	option.action = rotation;
	option.newDir = newDir;
	battle_info->setDirection(newDir);
	option.canMove = canMoveFwd();

	// Calculate safety score (higher is better)
	option.safetyScore = 0;

	// Check immediate safety
	auto [nextRow, nextCol] = nextStep(true, battle_info->getPosition(), battle_info->getDirection());
	if (!willBeHitIn(nextRow, nextCol, 1)) option.safetyScore += 2;
	if (!willBeHitIn(nextRow, nextCol, 2)) option.safetyScore += 1;

	// Bonus for moving towards open space
	if (option.canMove) {
		option.safetyScore += 2;

		// Check if this direction leads to more open space
		int openSpace = countOpenSpaceInDirection({nextRow, nextCol});
		option.safetyScore += openSpace;
	}
	// Reset direction
	battle_info->setDirection(oldDir);
	return option;
}

/**
 * @brief Counts the number of open (safe) spaces around a given position.
 *
 * @param pos The position to check around.
 * @return The number of open adjacent cells.
 */
int BTankAlgorithm::countOpenSpaceInDirection(pair<int,int> pos) {
	auto [row, col] = pos;
	int openCount = 0;
	for(auto [dr, dc] : offsets) {
		int newRow = row + dr;
		int newCol = col + dc;

        if (isOccupierFree({newRow, newCol})){
            openCount++;
         }
  	}
        return openCount;
  }


// void BTankAlgorithm::updateBattleInfo(BattleInfo& battleInfo)
// {
// 	battle_info->setDirection(dynamic_cast<TankBattleInfo*>(&battleInfo)->getDirection());
// }