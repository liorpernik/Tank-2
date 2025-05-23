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

void BoardManager::updateMap(pair<int, int> curr_pos,pair<int,int> new_pos) {
	vector<unique_ptr<GameObject>>& curr_cell=game_map[curr_pos.first][curr_pos.second];
	if (new_pos.first==-1 && new_pos.second==-1) { // GameObject destroyed
		if (curr_cell[0]->getSymbol()!='T') // if == > Tank Destroyed, keeping instance for logging
			curr_cell[0].reset();
	}
	else {
		vector<unique_ptr<GameObject>>& new_cell=game_map[new_pos.first][new_pos.second];
		if (curr_cell.size()>1) // passing shell in a cell with mine\wall\destroyed tank
			new_cell.push_back(move(curr_cell[1]));
		else
			new_cell.push_back(move(curr_cell[0]));
	}
}
vector<Tank*> BoardManager::getSortedTanks() {
	vector<Tank*> tanks;
	for (auto& rowVec : game_map) {
		for (auto& cellVec : rowVec) {
			for (auto& obj : cellVec) {
				if (obj->getSymbol() == 'T') {
					tanks.push_back(dynamic_cast<Tank*>(obj.get()));
				}
			}
		}
	}
	return tanks;
}
void BoardManager::moveFiredShells() {
    map<pair<int, int>, vector<GameObject*>> collisionGroups;

    // First pass: Move all shells and group objects by position
    for (size_t i = 0; i < fired_shells.size(); ) {
        auto& shell = fired_shells[i];
        bool shellDestroyed = false;

        for (int step = 0; step < 2 && !shellDestroyed; step++) {
            pair<int, int> oldPos = shell->getPos();
            pair<int, int> newPos = calculateNewPosition(oldPos, shell->getDirection());

            // Check for collisions at new position
            auto& cell = game_map[newPos.first][newPos.second];
            if (!cell.empty()) {
                collisionGroups[newPos].push_back(shell.get());
                for (auto& obj : cell) {
                    collisionGroups[newPos].push_back(obj.get());
                }
                shellDestroyed = true;
            }

            if (!shellDestroyed) {
                shell->setPos(newPos);
            }
        }

        if (shellDestroyed) {
            fired_shells.erase(fired_shells.begin() + i);
        } else {
            i++;
        }
    }

    // Second pass: Process all collisions
    for (auto& [position, objects] : collisionGroups) {
        processCollision(objects);
        cleanupDestroyedObjects(position);
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
        for (int y = 0; y < width; y++) {
            charMap.push_back(vector<char>());
            if (game_map[x][y].size()>0){
                auto obj=game_map[x][y].size() > 1 ? game_map[x][y][1].get() : game_map[x][y][0].get();
                charMap[x].push_back(obj->getSymbol());
            }
            else
                charMap[x].push_back(' ');

        }
    }
    return charMap;
}

void BoardManager::cleanupDestroyedObjects(pair<int,int> pos) {
    auto& cell = game_map[pos.first][pos.second];
    cell.erase(remove_if(cell.begin(), cell.end(),[](const unique_ptr<GameObject>& obj) {return obj->isDestroyed();}),cell.end()); // releases ptr automaticly when erasing.
}