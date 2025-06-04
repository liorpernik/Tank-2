#include "../header/GameManager.h"

GameManager::GameManager(unique_ptr<PlayerFactory> playerFactory,unique_ptr<TankAlgorithmFactory> tankFactory)
	: player_factory(move(playerFactory)), tank_factory(move(tankFactory)) {}

void GameManager::readBoard(const string& filePath) {
    ifstream file(filePath);
	if (!file) {
		throw invalid_argument("File could not be opened");
	}

    ofstream errorLog("input_errors.txt");
    bool hasErrors = false;
    string line;

    // Parse metadata
    parseMetadata(file, hasErrors, errorLog);

    // Process map rows
    processMapRows(file, hasErrors, errorLog);

    // Clean up error log if no errors
    if (!hasErrors) {
        errorLog.close();
        remove("input_errors.txt");
    }

    // Initialize players and output
	initializePlayers();
	setupOutputFile(filePath);
}

void GameManager::validateTankCounts() {
	if (player_tank_count[1] == 0 && player_tank_count[2] == 0) {
		logs.push_back("Tie, both players have zero tanks");
		writeOutput();
		throw runtime_error("Game ended before start - both players have no tanks");
	}
	if (player_tank_count[1] == 0) {
		logs.push_back("Player 2 won with " + to_string(player_tank_count[2]) +
					  " tanks still alive (Player 1 had no tanks)");
		writeOutput();
		throw runtime_error("Game ended before start - Player 1 has no tanks");
	}
	if (player_tank_count[2] == 0) {
		logs.push_back("Player 1 won with " + to_string(player_tank_count[1]) +
					  " tanks still alive (Player 2 had no tanks)");
		writeOutput();
		throw runtime_error("Game ended before start - Player 2 has no tanks");
	}
}

void GameManager::parseMetadata(ifstream& file, bool& hasErrors, ofstream& errorLog) {
    string line;
    for (int i =0; i < 5; i++) {
    	getline(file, line);
        if (line.empty() || line[0] == ';') continue;
        if (tryParseMetadata(line, "MaxSteps", max_steps, hasErrors, errorLog)) continue;
        if (tryParseMetadata(line, "NumShells", num_shells, hasErrors, errorLog)) continue;

    	// For Rows/Cols, parse into temp int first to avoid size_t->int conversion issues
    	int temp;
    	if (tryParseMetadata(line, "Rows", temp, hasErrors, errorLog)) {
    		rows = static_cast<size_t>(temp);
    		continue;
    	}
    	if (tryParseMetadata(line, "Cols", temp, hasErrors, errorLog)) {
    		cols = static_cast<size_t>(temp);
    		continue;
    	}

    	if (hasAllMetadata()) break ;
    }

    if (!hasAllMetadata()) {
        throw invalid_argument("Missing required metadata in file");
    }
}

bool GameManager::hasAllMetadata() const {
	return max_steps > 0 && num_shells > 0 && rows > 0 && cols > 0;
}

bool GameManager::tryParseMetadata(const string& line, const string& key,int& value, bool& hasErrors, ofstream& errorLog) {
    if (line.find(key) != string::npos) {
        size_t equalPos = line.find('=');
        if (equalPos != string::npos) {
            try {
            	// stoi ignores whitespaces, so it'll get the number after '=' if the row exists
                value = stoi(line.substr(equalPos + 1));
                return true;
            } catch (...) {
                hasErrors = true;
                errorLog << "Invalid " << key << " value\n";
            }
        }
    }
    return false;
}

void GameManager::processMapRows(ifstream& file, bool& hasErrors, ofstream& errorLog) {
    string line;
	vector<vector<vector<unique_ptr<GameObject>>>> map(rows);

	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < cols; ++j) {
			map[i].emplace_back(); // empty vec
		}
	}

    for (size_t row = 0; row < rows; ++row) {
    	bool rowHadErrors = false;

        if (!getline(file, line)) {
        	hasErrors = true;
        	errorLog << "Row " << row << " is missing. Filling with empty cells.\n";
        	continue;
        }
        processRowCells(line, row,map, rowHadErrors, errorLog);
        checkExcessColumns(line, row, rowHadErrors, errorLog);

    	hasErrors |= rowHadErrors;

    }

    checkExcessRows(file, hasErrors, errorLog);
	board = make_unique<BoardManager>(move(map), rows, cols);
	board_view=make_unique<BoardSatelliteView>(rows,cols,board->objMapToCharMap());
}

void GameManager::checkExcessColumns(const string& line, size_t row,bool& hasErrors, ofstream& errorLog) {
	if (line.size() > cols) {
		hasErrors = true;
		errorLog << line.size() - cols << " excess columns in row " << row << ". Ignoring them.\n";

	}
}

void GameManager::checkExcessRows(ifstream& file, bool& hasErrors,ofstream& errorLog) {
	size_t extraRows = 0;
	string line;
	while (getline(file, line)) {
		extraRows++;
	}
	if (extraRows > 0) {
		hasErrors = true;
		errorLog << extraRows << " excess rows found. Ignoring them.\n";
	}
}

void GameManager::processRowCells(const string& line, size_t row,vector<vector<vector<unique_ptr<GameObject>>>>& map, bool& hasErrors, ofstream& errorLog) {

    for (size_t col = 0; col < cols; ++col) {
        char symbol = (col < line.size()) ? line[col] : ' ';
        auto obj = processCell(symbol, row, col, hasErrors, errorLog);
    	map[row][col].push_back(move(obj));
    }
}

unique_ptr<GameObject> GameManager::processCell(char symbol, size_t row, size_t col,bool& hasErrors, ofstream& errorLog) {

	pair pos = {static_cast<int>(row), static_cast<int>(col) };
    switch (symbol) {
        case '1': case '2':
            return handleTank(symbol - '0', row, col, hasErrors, errorLog);
        case '@':
        	return make_unique<Mine>(pos);
		case '#':
			return make_unique<Wall>(pos);
    	case ' ':
            return nullptr; // Valid symbols, no action needed
        default:
        	hasErrors = true;
    		errorLog << "Unknown symbol '" << symbol << "' at (" << row << "," << col << "). Treating as empty space.\n";
    		break;
    }
	return nullptr;
}
unique_ptr<GameObject> GameManager::handleTank(int player_id, size_t row, size_t col,bool& hasErrors, ofstream& errorLog) {
	// Validate player ID
	if (player_id != 1 && player_id != 2) {
		hasErrors = true;
		errorLog << "Invalid player ID " << player_id  << " at (" << row << "," << col << "). Ignoring.\n";
		return nullptr;
	}
	// Create the tank
	try {
		int tank_index = player_tank_count[player_id]++;
		pair pos = {static_cast<int>(row), static_cast<int>(col) };
		player_tanks_algo[player_id].push_back(dynamic_cast<MyTankAlgorithmFactory*>(tank_factory.get())->create(player_id, tank_index));
		// player_tanks_algo[player_id].push_back(dynamic_cast<MyTankAlgorithmFactory*>(tank_factory.get())->create(player_id, tank_index));
		player_tanks_pos[player_id].push_back(pos);
		player_shell_count[player_id] += num_shells; //???
		return make_unique<Tank>(pos, tank_index , player_id == 1 ? Direction::L : Direction::R,player_id, num_shells);
	} catch (const exception& e) {
		hasErrors = true;
		errorLog << "Failed to create tank " << player_id << " at (" << row << "," << col << "): " << e.what() << "\n";
	}
	return nullptr;
}



void GameManager::initializePlayers() {
	players.push_back(player_factory->create(1,rows,cols, max_steps, num_shells));
	// setPlayerTankAlgorithms(1);
	players.push_back(player_factory->create(2, rows,cols,max_steps, num_shells));
	// setPlayerTankAlgorithms(2);
}

// void GameManager::setPlayerTankAlgorithms(int player_id)
// {
// 	for (size_t i = 0; i < player_tanks[player_id].size(); ++i)
// 	{
// 		int index = static_cast<int>(i);
// 		unique_ptr<TankAlgorithm> tank = tank_factory->create(player_id, index);
// 		dynamic_cast<MyTankAlgorithm*>(tank.get())->setShells(num_shells);
// 		dynamic_cast<MyPlayer*>(players[0].get())->addTank(tank);
// 	}
// }

void GameManager::setupOutputFile(const string& filePath) {
    size_t last_slash = filePath.find_last_of("/\\");
    output_file = "output_" + (last_slash == string::npos ? filePath : filePath.substr(last_slash + 1)); // string::npos = “not found”
}

void GameManager::run() {
	board->printBoard(); //initial board
	while (!isGameOver() && current_step < max_steps) {
		processRound();
		dynamic_cast<BoardSatelliteView*>(board_view.get())->update(board->objMapToCharMap());
		board->printBoard();

		current_step++;
		if (player_shell_count[0]==0&&player_shell_count[1]==0) {
			steps_since_no_shells++;
		}
	}
	logGameResult();
	writeOutput();
	board->writeBoardStates(output_file);
}

int GameManager::count_alive_tanks(int player_id) const {
	return player_tank_count.at(player_id);
}

void GameManager::processRound() {
	board->moveFiredShells();

	if(isGameOver()) {
		return;
	}
	vector<Tank*> tanks=board->getSortedTanks();
	vector<string> roundActions;
	map<Tank*,ActionRequest> actionRequests;

	// get all actions
	for (size_t i = 0; i < tanks.size(); i++) {

		auto tank = tanks[i];

		// Get action from algorithm
		TankAlgorithm* tankPtr = findTankAlgorithmById(tank);
		ActionRequest action = tankPtr->getAction();
		actionRequests[tank] = action;
	}
	board->applyMoves(actionRequests);
	updateTanksInfo(tanks);
	logs.push_back(generateRoundOutput(actionRequests));
	board->boardCleanup();
}

void GameManager::updateTanksInfo(vector<Tank*> tanks) {
	int index;
	TankAlgorithm* tankPtr;
	for (Tank* tank :tanks) {
		tankPtr= findTankAlgorithmById(tank);
		index=getTankIndex(tankPtr);
		player_tanks_pos[tank->getOwnerId()][index]=tank->getPos();  // {-1,-1} if killed
		if (tank->isDestroyed())
			--player_tank_count[tank->getOwnerId()];
		if (tank->getLastAction()==ActionRequest::Shoot)
			--player_shell_count[tank->getOwnerId()];
		if (tank->getLastAction()==ActionRequest::GetBattleInfo){
			dynamic_cast<BoardSatelliteView*>(board_view.get())->setRequestingTankPos(tank->getPos());
			players.at(tank->getOwnerId()-1)->updateTankWithBattleInfo(*tankPtr,*board_view);
		}
	}
}
int GameManager::getTankIndex(TankAlgorithm* t) {
	MyTankAlgorithm* tank=dynamic_cast<MyTankAlgorithm*>(t);
	const auto& vec=player_tanks_algo[tank->getOwnerId()];
	for (size_t i = 0; i <vec.size(); ++i) {
		if (dynamic_cast<MyTankAlgorithm*>(vec[i].get())->getTankId() == tank->getTankId()) {
			return static_cast<int>(i);
		}
	}
	return -1; // Not found
}

TankAlgorithm* GameManager::findTankAlgorithmById(Tank* tankData) {
	auto it = player_tanks_algo.find(tankData->getOwnerId());
	if (it != player_tanks_algo.end()) {
		const auto& tankVec = it->second;
		for (const auto& tankPtr : tankVec) {
			int id = dynamic_cast<MyTankAlgorithm*>(tankPtr.get())->getTankId();
			if (id == tankData->getId()) {
				return tankPtr.get();
			}
		}
	}
	return nullptr;
}


string GameManager::generateRoundOutput(map<Tank*,ActionRequest> tankActions) {
	vector<string> actions;
	//vector<Tank*> tanks=board->getSortedTanks();
	for (const auto& [tank,acts] : tankActions) {
		string move=actionToString(acts);
		if (tank->isDestroyed()) {
			if (tank->isKilledThisRound()) {
				actions.push_back(tank->getActionSuccess()?move + " (ignored) (killed)": " (killed)");
				tank->setKilledThisRound(false);
			}
			else {
				actions.push_back("killed");
			}
			continue;
		}
		actions.push_back(tank->getActionSuccess() ? move : move + " (ignored)");
	}
	return joinActions(actions);
}

string GameManager::joinActions(const vector<string>& actions) {
    string result;
    for (size_t i = 0; i < actions.size(); ++i) {
        if (i != 0) result += ", ";
        result += actions[i];
    }
    return result;
}

void GameManager::logGameResult() {
	int p1 = count_alive_tanks(1);
	int p2 = count_alive_tanks(2);

	if (p1 > 0 && p2 == 0) {
		logs.push_back("Player 1 won with " + to_string(p1) + " tanks still alive");
	} else if (p2 > 0 && p1 == 0) {
		logs.push_back("Player 2 won with " + to_string(p2) + " tanks still alive");
	} else if (current_step >= max_steps) {
		logs.push_back("Tie, reached max steps = " + to_string(max_steps) +
					  ", player 1 has " + to_string(p1) +
					  " tanks, player 2 has " + to_string(p2) + " tanks");
	} else {
		logs.push_back("Tie, both players have zero tanks");
	}
}

string GameManager::actionToString(ActionRequest action) {
	switch (action) {
		case ActionRequest::MoveForward: return "MoveForward";
		case ActionRequest::MoveBackward: return "MoveBackward";
		case ActionRequest::RotateLeft90: return "RotateLeft90";
		case ActionRequest::RotateRight90: return "RotateRight90";
		case ActionRequest::RotateLeft45: return "RotateLeft45";
		case ActionRequest::RotateRight45: return "RotateRight45";
		case ActionRequest::Shoot: return "Shoot";
		case ActionRequest::GetBattleInfo: return "GetBattleInfo";
		case ActionRequest::DoNothing: return "DoNothing";
		default: return "Unknown";
	}
}

void GameManager::writeOutput() {
	ofstream out(output_file);
	for (const auto& line : logs) {
		out << line << "\n";
	}
}

bool GameManager::isGameOver() const {
	int p1_tanks = count_alive_tanks(1);
	int p2_tanks = count_alive_tanks(2);

	return p1_tanks == 0 ||   // Player 1 eliminated
		   p2_tanks == 0 ||   // Player 2 eliminated
		   current_step >= max_steps|| // Timeout
			   steps_since_no_shells==40;
}

GameManager::~GameManager()
{
	//todo: check what to delete
}
