#include "../header/MyTankAlgorithm.h"
#include "../header/TankBattleInfo.h"
#include "../header/Shell.h"

using std::pair;

MyTankAlgorithm::MyTankAlgorithm(int player_index, int tank_index) : player_index(player_index), tank_index(tank_index) {
    battle_info = make_unique<TankBattleInfo>(tank_index, player_index);
}

void MyTankAlgorithm::updateBattleInfo(BattleInfo& info)
{
	auto tank_info = dynamic_cast<TankBattleInfo&>(info);
    battle_info->setOpponents(tank_info.getOpponents());
	battle_info->setPosition(tank_info.getPosition().first, tank_info.getPosition().second);
	battle_info->setDirection(tank_info.getDirection());
	battle_info->setKnownObjects(tank_info.getKnownObjects());
	battle_info->setRemainingShells(tank_info.getRemainingShells());
}

ActionRequest MyTankAlgorithm::getAction()
{
   return ActionRequest::DoNothing;
}

std::pair<int,int> MyTankAlgorithm::nextStep(bool forward,const std::pair<int,int> pos, const Direction dir){
    int side = forward ? 1: -1;
    int newRow = pos.first + offsets[dir].first*side;
    int newCol = pos.second + offsets[dir].second*side;
    return {newRow , newCol };
}

Direction MyTankAlgorithm::calculateRealDirection(int currRow, int currCol, int targetRow, int targetCol) {
    int rowDiff = targetRow - currRow;
    int colDiff = targetCol - currCol;

    // Normalize differences to identify direction only if strictly aligned
    if (rowDiff > 0 && colDiff == 0) return Direction::D;    // Down
    if (rowDiff < 0 && colDiff == 0) return Direction::U;    // Up
    if (rowDiff == 0 && colDiff > 0) return Direction::R;    // Right
    if (rowDiff == 0 && colDiff < 0) return Direction::L;    // Left
    if (rowDiff == colDiff && rowDiff > 0) return Direction::DR; // Down-Right (strict diagonal)
    if (rowDiff == -colDiff && rowDiff > 0) return Direction::DL; // Down-Left (strict diagonal)
    if (rowDiff == colDiff && rowDiff < 0) return Direction::UR; // Up-Right (strict diagonal)
    if (rowDiff == -colDiff && rowDiff < 0) return Direction::UL; // Up-Left (strict diagonal)

    return Direction::None; // None for non-aligned movement
}

void MyTankAlgorithm::rotate(ActionRequest action) {
    Direction dir = battle_info->getDirection();
    switch(action) {
    case ActionRequest::RotateLeft45:
        dir = static_cast<Direction>((dir + 7) % 8); // move 1 counter-clockwise
        break;
    case ActionRequest::RotateRight45:
        dir = static_cast<Direction>((dir + 1) % 8); // move 1 clockwise
        break;
    case ActionRequest::RotateLeft90:
        dir = static_cast<Direction>((dir + 6) % 8); // move 2 clockwise
        break;
    case ActionRequest::RotateRight90:
        dir = static_cast<Direction>((dir + 2) % 8); // move 2 counter-clockwise
        break;
    case ActionRequest::MoveForward:
    case ActionRequest::MoveBackward:
    case ActionRequest::Shoot:
    case ActionRequest::DoNothing:
    case ActionRequest::GetBattleInfo:
        break;
    }
    battle_info->setDirection(dir);
}

bool MyTankAlgorithm::isValidMove(ActionRequest action) {
    if (battle_info->isWaitingToReverse() && action != ActionRequest::MoveForward)
        return false;

    std::pair<int,int> newPos;
    std::pair<int,int> currPos = battle_info->getPosition();
    Direction currDir = battle_info->getDirection();

    switch (action) {
    case ActionRequest::Shoot:
        return !battle_info->isWaitingToShoot() && battle_info->getRemainingShells() > 0;
    case ActionRequest::MoveForward:
        newPos = nextStep(true,currPos, currDir);
        return isOccupierFree(newPos);
    case ActionRequest::MoveBackward:
        newPos = nextStep(false, currPos, currDir);
        return !battle_info->isWaitingToReverse() && isOccupierFree(newPos);
    case ActionRequest::RotateLeft45:
    case ActionRequest::RotateRight45:
    case ActionRequest::RotateLeft90:
    case ActionRequest::RotateRight90:
        return !battle_info->isWaitingToReverse();
    default:
        return false;
    }
}

/**
 * @brief Checks if a given board position is free of occupiers(Wall, Tank, Mine).
 *
 * @param pos Position to check.
 * @return True if the cell is free, false otherwise.
 */
bool MyTankAlgorithm::isOccupierFree(std::pair<int, int> pos)
{
    return battle_info->getObjectByPosition(pos) == nullptr;
}

/**
 * @brief Determines if the player should shoot at the opponent based on alignment and available shells.
 *
 * @param opponentPos The opponent's position.
 * @return True if the player should shoot, false otherwise.
 */
bool MyTankAlgorithm::shouldShootOpponent(const std::pair<int,int>& opponentPos)  {
	return battle_info->getRemainingShells() > 0 &&
	       isAlignedWithOpponent(opponentPos) &&
	       !battle_info->isWaitingToShoot();
}

/**
 * @brief Checks if the player can move forward safely.
 *
 * @return True if moving forward is possible and safe, false otherwise.
 */
bool MyTankAlgorithm::canMoveFwd()  {
	auto [r, c] = nextStep(true, battle_info->getPosition(), battle_info->getDirection());
	return isOccupierFree({r, c}) && !willBeHitIn(r,c,1);
}

/**
 * @brief Checks if the player can move backward safely.
 *
 * @return True if moving backward is possible and safe, false otherwise.
 */
bool MyTankAlgorithm::canMoveBack()  {
	if (!isValidMove( ActionRequest::MoveBackward)) return false;
	pair<int,int> next = nextStep(false, battle_info->getPosition(), battle_info->getDirection());
	return canSafelyBack(next.first, next.second) && isOccupierFree(next);
}

/**
 * @brief Determines if moving backward would be safe for the player.
 *
 * @param backR Row after moving backward.
 * @param backC Column after moving backward.
 * @return True if the move is safe, false otherwise.
 */
bool MyTankAlgorithm::canSafelyBack(int backR, int backC)  {
	auto [r,c]=battle_info->getPosition();
	// current position and backward cell are safe
	if (!battle_info->getMovedBackwardLast()) {
		if (willBeHitIn(r, c, 1) || willBeHitIn(r, c, 2) || willBeHitIn(backR, backC, 3)) {//
			return false; // Danger in current or backward position
		}
	}
	else {
		// If just moved back, only check immediate danger in next cell
		if (willBeHitIn(backR, backC, 1)) {
			return false;
		}
	}
	return true; // Safe to move backward
}

/**
 * @brief Chooses an escape action if the player is in danger.
 *
 * @param board The current game board.
 * @return Action::MoveFwd if forward is safe, Action::MoveBack if backward is safe, or Action::None if stuck.
 */
ActionRequest MyTankAlgorithm::checkForEscape() {
	if (canMoveFwd()) {
		return ActionRequest::MoveForward;
	}
	if (canMoveBack()) {
		return ActionRequest::MoveBackward;
	}
	return ActionRequest::DoNothing;
}

/**
 * @brief Predicts if a shell will hit a specific cell within a given number of game steps.
 *
 * @param row Row of the cell to check.
 * @param col Column of the cell to check.
 * @param t Number of game steps ahead.
 * @return True if a shell will hit, false otherwise.
 */
bool MyTankAlgorithm::willBeHitIn(int row, int col, int t) {
	auto knownObjects = battle_info->getKnownObjects();
	for (auto& [pos, objects]: knownObjects) {
		auto& object = objects.size() > 1 ? objects[1] :  objects[0];
		char symbol = object->getSymbol();
		if (symbol != '*') continue;

		auto [sr, sc] = pos;
		Direction dir = dynamic_cast<Shell*>(object)->getDirection();

		for(int i =0; i<= 2 && dir != None; i++) {

			auto [dr, dc] = offsets[static_cast<int>(dir)];

			// Calc forward `t` steps:
			int stepsAhead = (t-1)*2 + i;
			int pr = sr + dr * stepsAhead;
			int pc = sc + dc * stepsAhead;

			// // Wraparound edges
			// pr = battle_info->wrap(pr, battle_info->getHeight());
			// pc = battle_info->wrap(pc, battle_info->getWidth());

			if (pr == row && pc == col) {
				return true;  // A shell will hit the cell by that time
			}
		}
	}
	return false;
}

/**
 * @brief Checks if the player is aligned (same row or same column) with the opponent.
 *
 * @param opponentPos Opponent's current position.
 * @return True if aligned, false otherwise.
 */
bool MyTankAlgorithm::isAlignedWithOpponent(std::pair<int, int> opponentPos) {
	auto [r,c]= battle_info->getPosition();
	return r == opponentPos.first || c == opponentPos.second;
}

/**
 * @brief Determines the rotation action needed to align from the current direction to the desired direction.
 *
 * @param currentDir Current facing direction.
 * @param desiredDir Target direction.
 * @return Action representing the rotation to apply.
 */
ActionRequest MyTankAlgorithm::determineRotation(Direction currentDir, Direction desiredDir) {
	int rotationSteps = (static_cast<int>(desiredDir) - static_cast<int>(currentDir) + 8) % 8;
	if (rotationSteps == 1 || rotationSteps == 2) return ActionRequest::RotateRight45;
	if (rotationSteps == 6 || rotationSteps == 7) return ActionRequest::RotateLeft45;
	if (rotationSteps == 3) return ActionRequest::RotateRight90;
	if (rotationSteps == 5) return ActionRequest::RotateLeft90;

	// If no rotation required
	return ActionRequest::DoNothing; // Arbitrary fallback
}

/**
 * @brief Calculates the general direction from the current position to the target position.
 *
 * @param currRow Current row position.
 * @param currCol Current column position.
 * @param targetRow Target row position.
 * @param targetCol Target column position.
 * @return Direction to move toward the target, or current direction if already at the target.
 */
Direction MyTankAlgorithm::calculateDirection(int currRow, int currCol, int targetRow, int targetCol) {
	if (currRow < targetRow && currCol == targetCol) return Direction::D;
	if (currRow > targetRow && currCol == targetCol) return Direction::U;
	if (currRow == targetRow && currCol < targetCol) return Direction::R;
	if (currRow == targetRow && currCol > targetCol) return Direction::L;
	if (currRow < targetRow && currCol < targetCol) return Direction::DR;
	if (currRow < targetRow && currCol > targetCol) return Direction::DL;
	if (currRow > targetRow && currCol < targetCol) return Direction::UR;
	if (currRow > targetRow && currCol > targetCol) return Direction::UL;

	return None; // Default to current direction if no match
}

OppData MyTankAlgorithm::getClosestOpponent()
{
	std::vector<OppData> opponents = battle_info->getOpponents();
	int min_movements = 1e8;
	int min_pos = -1;
	int index = 0;
	for (auto opp : opponents)
	{
		int curr_moves = calculateActionsToOpponent(opp);
		if (min_movements > curr_moves)
		{
			min_pos = index;
			min_movements = curr_moves;
		}
		++index;
	}

	return opponents[min_pos];
}


int MyTankAlgorithm::calculateActionsToOpponent(const OppData& opp) {
	auto myPos = battle_info->getPosition();
	std::pair<int, int> oppPos = opp.opponentPos;

	int dx = oppPos.first - myPos.first;
	int dy = oppPos.second - myPos.second;

	Direction requiredDir = None;
	int movements = 0;
	int rotations = 0;

	if (dx == 0 && dy == 0) {
		return 0; // Already at opponent's position
	}

	// Determine the required direction and movements
	if (dx != 0) {
		requiredDir = calculateDirection(myPos.first,myPos.second,oppPos.first,oppPos.second);
		movements = std::abs(dx);
	} else if (dy != 0) {
		requiredDir = calculateDirection(myPos.first,myPos.second,oppPos.first,oppPos.second);
		movements = std::abs(dy);
	}

	// Calculate rotations to face the required direction
	if (requiredDir == opp.opponentDir) ++rotations;
	else rotations = 2;

	// Total actions = rotations + movements
	return rotations + movements;
}