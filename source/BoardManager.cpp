#include "../header/BoardManager.h"

// Constructor: Initializes board dimensions and takes ownership of the game map.
BoardManager::BoardManager(vector<vector<vector<unique_ptr<GameObject>>>> gameMap, int rows, int cols) :  height(rows), width(cols),game_map(move(gameMap)) {}// printBoard();}

/**
 * @brief Returns the top object at a given position, prioritizing the shell if multiple objects exist.
 *
 * @param x Row index.
 * @param y Column index.
 * @return Pointer to the GameObject at the specified position, or nullptr if out of bounds or empty.
 */
GameObject* BoardManager::getObjectAt(int x, int y) const {
	if (x >= height || y >= width) return nullptr;
	return !game_map[x][y].empty() ? game_map[x][y].size() > 1 ? game_map[x][y][1].get() : game_map[x][y][0].get() : nullptr; //if more than one then shell is last and more important
}

/**
 * @brief Stores a textual representation of the current board state.
 */
void BoardManager::printBoard()
{
    stringstream boardState;
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            GameObject* obj = getObjectAt(i,j);
            obj != nullptr && !obj->isDestroyed()? boardState << obj->getSymbol() : boardState << ' ';
        }
        boardState << "\n";
    }
	// Store the complete board state
    boardStates.push_back(boardState.str());
}

/**
 * @brief Writes all stored board states to a file.
 *
 * @param fileName Base file name for output file (prefixed with "gameSteps_").
 */
void BoardManager::writeBoardStates(string fileName)
{
	ofstream outFile("gameSteps_"+fileName);
	if (!outFile.is_open()) {
		cerr << "Error: Could not open file for writing!" << endl;
		return;
	}

	// Write header
	outFile << "=== GAME BOARD STATES ===\n\n";

	// Write each board state with turn number
	for (size_t i = 0; i < boardStates.size(); ++i) {
		outFile << "Turn " << (i + 1) << ":\n";
		outFile << boardStates[i] << "\n";
	}

	outFile.close();
	boardStates.clear(); // Clear logs after writing
}

/**
 * @brief Inserts a GameObject into the map at the given position.
 *
 * @param obj The object to move.
 * @param new_pos The target position, or (-1,-1) to indicate deferred removal.
 */
void BoardManager::updateMap(unique_ptr<GameObject> obj, pair<int,int> new_pos) {
    if (!obj) return;

    if (new_pos.first == -1 && new_pos.second == -1) {
        // deletion handled in cleanupDestroyedObjects
        return;
    }

    obj->setPos(new_pos);
    if (!game_map[new_pos.first][new_pos.second].empty() && game_map[new_pos.first][new_pos.second][0] == nullptr) game_map[new_pos.first][new_pos.second][0] = move(obj);
    else game_map[new_pos.first][new_pos.second].push_back(move(obj));
}

/**
 * @brief Retrieves all tanks currently on the board, sorted by appearance.
 *
 * @return A vector of pointers to Tank objects.
 */
vector<Tank*> BoardManager::getSortedTanks() {
	vector<Tank*> tanks;
	for (auto& rowVec : game_map) {
		for (auto& cellVec : rowVec) {
			for (auto& obj : cellVec) {
				if (obj && (obj->getSymbol() == '1' || obj->getSymbol() == '2')) {
					tanks.push_back(dynamic_cast<Tank*>(obj.get()));
				}
			}
		}
	}
	return tanks;
}

/**
 * @brief Moves all shells that have been fired up to 2 steps and handles collisions.
 */
void BoardManager::moveFiredShells() {

    vector<int> index_of_delete;
    for (size_t i = 0; i < fired_shells.size(); i++) {
        auto& shell = fired_shells[i];
        bool shellDestroyed = false;

        for (int step = 0; step < 2 && !shellDestroyed; step++) {
            pair<int, int> oldPos = shell->getPos();
            pair<int, int> newPos = calculateNewPosition(oldPos, shell->getDirection());

            // Find the shell's unique_ptr in game_map
            unique_ptr<GameObject> shellPtr;
            if (game_map[oldPos.first][oldPos.second].size() > 1)
            {
                shellPtr = move(game_map[oldPos.first][oldPos.second][1]);
                game_map[oldPos.first][oldPos.second].erase(game_map[oldPos.first][oldPos.second].begin() + 1);
            }
            else shellPtr = move(game_map[oldPos.first][oldPos.second][0]);

            // Move to new position using updateMap
            updateMap(move(shellPtr), newPos);
            shell->setPos(newPos);
            handleAllCollisions();
            shellDestroyed = getObjectAt(newPos.first, newPos.second)->isDestroyed();
            if (shellDestroyed) index_of_delete.push_back(i);
        }
    }

    for (int i : index_of_delete)
    {
        fired_shells.erase(fired_shells.begin() + i);
    }
}

/**
 * @brief Extracts and removes a specific object from the map, returning ownership.
 *
 * @tparam T Type derived from GameObject.
 * @param object Pointer to the object to extract.
 * @return A unique_ptr to the removed object.
 */
template<typename T>
unique_ptr<GameObject> BoardManager::extractObjectFromMap(T* object) {
    if (!object) return nullptr;

    pair<int, int> pos = object->getPos();
    if (pos.first < 0 || pos.first >= height || pos.second < 0 || pos.second >= width) {
        return nullptr;
    }

    auto& cell = game_map[pos.first][pos.second];
    for (auto it = cell.begin(); it != cell.end(); ++it) {
        if (it->get() == object) { // move ownership and remove from map, later gets back to map in updateMap
            unique_ptr<GameObject> objPtr = move(*it);
            cell.erase(it);
            return objPtr;
        }
    }
    return nullptr;
}

/**
 * @brief Processes all cells with multiple objects and resolves collisions.
 */
void BoardManager::handleAllCollisions() {
    // Find all cells with multiple objects
    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
            auto& cell = game_map[x][y];
            if (cell.size() > 1) {
                // Check for special case: exactly 2 objects where one is shell and other is wall/mine
                if (cell.size() == 2) {
                    bool hasShell = false;
                    bool hasWallOrMine = false;

                    for (auto& obj : cell) {
                        if (dynamic_cast<Shell*>(obj.get())) {
                            hasShell = true;
                        }
                        else if (dynamic_cast<Wall*>(obj.get()) || dynamic_cast<Mine*>(obj.get())) {
                            hasWallOrMine = true;
                        }
                    }

                    // Skip collision processing for this case
                    if (hasShell && hasWallOrMine) {
                        continue;
                    }
                }

                // Normal collision processing for other cases
                vector<GameObject*> objects;
                for (auto& obj : cell) {
                    objects.push_back(obj.get());
                }

                processCollision(objects);
            }
        }
    }
}

/**
 * @brief Calculates a new position from a given direction, with wrap-around.
 */
pair<int, int> BoardManager::calculateNewPosition(pair<int, int> pos, Direction dir) const {
    const auto& offset = offsets[static_cast<int>(dir)];
    pos.first = (pos.first + offset.first + height) % height;
    pos.second = (pos.second + offset.second + width) % width;
    return pos;
}

/**
 * @brief Processes collision for a group of objects in a single cell.
 *
 * @param objects All objects occupying the same cell.
 */
void BoardManager::processCollision(vector<GameObject*>& objects) {
    bool containsMine = false;
    bool containsTank = false;
    bool containsShell = false;
    bool containsWall = false;

    // Analyze collision group
    for (auto obj : objects) {
        if (!obj->isDestroyed()) {
            if (dynamic_cast<Mine*>(obj)) containsMine = true;
            else if (dynamic_cast<Tank*>(obj)) containsTank = true;
            else if (dynamic_cast<Shell*>(obj)) containsShell = true;
            else if (dynamic_cast<Wall*>(obj)) containsWall = true;
        }
    }

    // Handle special cases
    if (containsMine&&containsTank) {
        // Mine-Tank explosion destroys everything in the cell
        for (auto obj : objects) {
            if (!obj->isDestroyed()) //ignore destroyed objects
                obj->destroy();
        }
        return;
    }

    // Tank-Tank collision (all tanks die)
    if (containsTank && count_if(objects.begin(), objects.end(),
        [](GameObject* obj) { return dynamic_cast<Tank*>(obj); }) > 1) {
        for (auto obj : objects) {
            if (auto tank = dynamic_cast<Tank*>(obj)){
                if (!tank->isDestroyed()) //ignore destroyed tanks
                tank->destroy();
            }
        }
        return;
        }

    // Shell-specific collisions
    for (auto obj : objects) {
        if (auto shell = dynamic_cast<Shell*>(obj)) {
            bool destroyShell = false;
            // Shell-Wall collision
            if (containsWall) {
                for (auto other : objects) {
                    if (auto wall = dynamic_cast<Wall*>(other)) {
                        wall->damage();
                    }
                }
            }
            // Shell-Tank collision
            if (containsTank) {
                for (auto other : objects) {
                    if (auto tank = dynamic_cast<Tank*>(other)) {
                        if (!tank->isDestroyed()) //ignore destroyed tanks
                            tank->destroy();
                        destroyShell = true;
                    }
                }
            }
            // Shell-Shell collision
            if (containsShell) {
                for (auto other : objects) {
                    if (dynamic_cast<Shell*>(other)) {
                        other->destroy(); // Destroy all shells
                        destroyShell = true;
                    }
                }
            }
            if (destroyShell) {
                shell->destroy(); // Destroy shell
            }
        }
    }
}

/**
 * @brief Converts the object map into a character representation.
 *
 * @return 2D vector of characters for display/debugging.
 */
vector<vector<char>> BoardManager::objMapToCharMap() {
    vector<vector<char>> charMap;
    for (int x = 0; x < height; x++) {
        charMap.push_back(vector<char>());
        for (int y = 0; y < width; y++) {
            if (game_map[x][y].size()>0){
                auto obj=game_map[x][y].size() > 1 ? game_map[x][y][1].get() : game_map[x][y][0].get();
                charMap[x].push_back(obj ? obj->getSymbol(): ' ');
            }
            else //shouldn't reach here
                charMap[x].push_back(' ');

        }
    }
    return charMap;
}

/**
 * @brief Iterates over the board and removes destroyed objects.
 */
void BoardManager::boardCleanup()
{
    for (int x = 0; x < height; x++)
    {
        for (int y = 0; y < width; y++)
        {
            cleanupDestroyedObjects({x,y});
        }
    }
}

/**
 * @brief Removes destroyed objects from a specific cell.
 *
 * @param pos Coordinates of the cell to clean.
 */
void BoardManager::cleanupDestroyedObjects(pair<int,int> pos) {
    auto& cell = game_map[pos.first][pos.second];
    cell.erase(remove_if(cell.begin(), cell.end(),[](const unique_ptr<GameObject>& obj) {return obj && obj->isDestroyed()&& !(obj->getSymbol()!='2'||obj->getSymbol()!='1');}),cell.end()); // releases ptr automaticly when erasing.
}

/**
 * @brief Checks if a move is valid for a given tank.
 *
 * @param tank Pointer to the tank attempting the move.
 * @param move The intended direction of movement.
 * @return true if the move is valid (e.g., not a wall); false otherwise.
 */
bool BoardManager::isValidMove(Tank* tank, ActionRequest action) {
    Direction dir=tank->getDirection();
    if (!tank || tank->isDestroyed()) {
        return false;
    }
    if (tank->isWaitingToBackward() && action!=ActionRequest::MoveForward) // only fwd move can cancel while waiting for the back move
            return false;

    if (action == ActionRequest::Shoot && tank->isWaitingToShoot()) {
        return false;
    }

    // For movement actions, check the target position
    if (action == ActionRequest::MoveForward || action == ActionRequest::MoveBackward) {
        if (action == ActionRequest::MoveBackward){
            dir=static_cast<Direction>((static_cast<int>(tank->getDirection()) + 4) % 8);
        }
        auto [newX, newY] = calculateNewPosition(tank->getPos(),dir);

        // Check if the new position is occupied
        GameObject* obj = !game_map[newX][newY].empty() ? game_map[newX][newY][0].get() : nullptr;
        if (obj && !obj->isDestroyed() && obj->getSymbol()=='#') { //not valid only if it's a wall
            return false;  // Position is blocked
        }
    }
    return true; // all other actions are allways valid
}

/**
 * @brief Applies a tank's move actions to the game board.
 *
 * @param moves A map from Tank pointer to the action it wants to do.
 */
void BoardManager::applyMoves(map<Tank*, ActionRequest> moves) {
    for (auto& [tank, action] : moves) {
        if (!tank || tank->isDestroyed()) continue;

        bool isValid = isValidMove(tank, action);
        tank->setActionSuccess(isValid);
        tank->setLastAction(action);

        if (!isValid) continue;

        pair<int, int> newPos = {-1, -1};
        if (tank->isWaitingToBackward()&&tank->getBackwardCooldown()==0)
            action=ActionRequest::MoveBackward; // back after 3 rounds, no matter what the other action now is, it is ignored?
        switch (action) {
            case ActionRequest::MoveForward:
                if (tank->isWaitingToBackward()) {
                    tank->setBackwardCooldown(0);
                    tank->setWaitingForBackward(false);
                    // Tank stays in place
                } else {
                    newPos = calculateNewPosition(tank->getPos(), tank->getDirection());
                    unique_ptr<GameObject> tankPtr = extractObjectFromMap(tank);
                    tank->setPos(newPos);

                    updateMap(move(tankPtr), newPos);
                }
            break;

            case ActionRequest::MoveBackward:
                if (tank->isBackLastMove()||(tank->isWaitingToBackward()&&tank->getBackwardCooldown()==0)){// Instant backward move
                    newPos = calculateNewPosition(tank->getPos(), static_cast<Direction>((static_cast<int>(tank->getDirection()) + 4) % 8));
                    tank->setWaitingForBackward(false);
                    unique_ptr<GameObject> tankPtr = extractObjectFromMap(tank);
                    tank->setPos(newPos);

                    updateMap(move(tankPtr), newPos);
                }
                else if (!tank->isWaitingToBackward()) {
                    // Start backward cooldown (waiting for 2 steps)
                    tank->setBackwardCooldown(2);
                    tank->setWaitingForBackward(true);
                }
            break;

            case ActionRequest::Shoot:
                if (!tank->isWaitingToShoot() && tank->getNumOfRemainingShells() > 0) {
                    tank->setNumOfShells(tank->getNumOfRemainingShells() - 1);
                    tank->setShootCooldown(4);
                    pair shell_pos = calculateNewPosition(tank->getPos(), tank->getDirection());
                    fired_shells.push_back(make_unique<Shell>(shell_pos, tank->getDirection(),tank->getOwnerId()));
                    updateMap(make_unique<Shell>(shell_pos, tank->getDirection(),tank->getOwnerId()),shell_pos);
                }
            break;

            case ActionRequest::RotateLeft45:
            case ActionRequest::RotateRight45:
            case ActionRequest::RotateLeft90:
            case ActionRequest::RotateRight90:
            tank->rotate(action);
            break;

            case ActionRequest::DoNothing:
            case ActionRequest::GetBattleInfo: //handled in GameMmanager
                break;
        }
        tank->setLastAction(action);

        // Handle cooldowns
        if (action != ActionRequest::Shoot && tank->isWaitingToShoot()) {
            tank->setShootCooldown(tank->getShootCooldown() - 1);
        }
        if (action!=ActionRequest::MoveBackward&& tank->isWaitingToBackward())
            tank->setBackwardCooldown(tank->getBackwardCooldown() - 1);
    }

    handleAllCollisions();
}