#include "../header/BoardManager.h"

BoardManager::BoardManager(vector<vector<vector<unique_ptr<GameObject>>>> gameMap, int rows, int cols) :  height(rows), width(cols),game_map(move(gameMap)) {}// printBoard();}

GameObject* BoardManager::getObjectAt(int x, int y) const {
	if (x >= height || y >= width) return nullptr;
	return !game_map[x][y].empty() ? game_map[x][y].size() > 1 ? game_map[x][y][1].get() : game_map[x][y][0].get() : nullptr; //if more than one then shell is last and more important
}

void BoardManager::printBoard()
{
	stringstream boardState;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			GameObject* obj = getObjectAt(i,j);//map[i][j]
			obj != nullptr ?
				boardState << obj->getSymbol() : boardState << ' ';

		}
		boardState << "\n";
	}
	// Store the complete board state
	boardStates.push_back(boardState.str());
}

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

void BoardManager::updateMap(unique_ptr<GameObject> obj, pair<int,int> new_pos) {
    if (!obj) return;

    if (new_pos.first == -1 && new_pos.second == -1) {
        // deletion handled in cleanupDestroyedObjects
        return;
    }

    obj->setPos(new_pos);
    if (!game_map[new_pos.first][new_pos.second].empty() && game_map[new_pos.first][new_pos.second][0] == nullptr) game_map[new_pos.first][new_pos.second][0] = std::move(obj);
    else game_map[new_pos.first][new_pos.second].push_back(std::move(obj));
}

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

void BoardManager::moveFiredShells() {

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
            else shellPtr = move(game_map[oldPos.first][oldPos.second][0]); //extractObjectFromMap(shell.get());

            // Move to new position using updateMap
            updateMap(std::move(shellPtr), newPos);
            shell->setPos(newPos);
            handleAllCollisions();
            shellDestroyed = shell->isDestroyed();
        }
        if (shellDestroyed) fired_shells.erase(fired_shells.begin() + i);
    }
}

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
            unique_ptr<GameObject> objPtr = std::move(*it);
            cell.erase(it);
            return objPtr;
        }
    }
    return nullptr;
}

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

pair<int, int> BoardManager::calculateNewPosition(pair<int, int> pos, Direction dir) const {
    const auto& offset = offsets[static_cast<int>(dir)];
    pos.first = (pos.first + offset.first + height) % height;
    pos.second = (pos.second + offset.second + width) % width;
    return pos;
}

void BoardManager::processCollision(vector<GameObject*>& objects) {
    bool containsMine = false;
    bool containsTank = false;
    bool containsShell = false;
    bool containsWall = false;

    // Analyze collision group
    for (auto obj : objects) {
        if (dynamic_cast<Mine*>(obj)) containsMine = true;
        else if (dynamic_cast<Tank*>(obj)) containsTank = true;
        else if (dynamic_cast<Shell*>(obj)) containsShell = true;
        else if (dynamic_cast<Wall*>(obj)) containsWall = true;
    }

    // Handle special cases
    if (containsMine&&containsTank) {
        // Mine-Tank explosion destroys everything in the cell
        for (auto obj : objects) {
            obj->destroy();
        }
        return;
    }

    // Tank-Tank collision (all tanks die)
    if (containsTank && count_if(objects.begin(), objects.end(),
        [](GameObject* obj) { return dynamic_cast<Tank*>(obj); }) > 1) {
        for (auto obj : objects) {
            if (auto tank = dynamic_cast<Tank*>(obj)) {
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

void BoardManager::cleanupDestroyedObjects(pair<int,int> pos) {
    auto& cell = game_map[pos.first][pos.second];
    cell.erase(remove_if(cell.begin(), cell.end(),[](const unique_ptr<GameObject>& obj) {return obj && obj->isDestroyed();}),cell.end()); // releases ptr automaticly when erasing.
}

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

void BoardManager::applyMoves(map<Tank*, ActionRequest> moves) {
    for (auto& [tank, action] : moves) {
        if (!tank || tank->isDestroyed()) continue;

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