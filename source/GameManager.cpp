#include "../header/GameManager.h"

/**
 * @brief Constructs the GameManager with player and tank algorithm factories.
 * @param playerFactory Unique pointer to a PlayerFactory object.
 * @param tankFactory Unique pointer to a TankAlgorithmFactory object.
 */
GameManager::GameManager(unique_ptr<PlayerFactory> playerFactory,unique_ptr<TankAlgorithmFactory> tankFactory)
	: player_factory(move(playerFactory)), tank_factory(move(tankFactory)) {}

/**
 * @brief Reads and parses the game board from the input file.
 * @details Parses metadata, validates input, initializes players and sets up output file.
 * @param filePath Path to the input file.
 * @throws invalid_argument if the input file cannot be opened.
 */
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

/**
 * @brief Validates the number of tanks for each player.
 * @details Throws exceptions if a player has zero tanks and logs the result.
 * @throws runtime_error if any player has zero tanks.
 */
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

/**
 * @brief Parses the first five lines of metadata from the input file.
 * @param file Input file stream.
 * @param hasErrors Reference to a boolean flag indicating if errors occurred.
 * @param errorLog Reference to the error log output stream.
 */
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

/**
 * @brief Checks if all required metadata values have been set properly.
 * @return true if max_steps, num_shells, rows, and cols are all greater than zero.
 */
bool GameManager::hasAllMetadata() const {
	return max_steps > 0 && num_shells > 0 && rows > 0 && cols > 0;
}

/**
 * @brief Attempts to parse a single metadata key-value pair from a line.
 * @param line Input line to parse.
 * @param key Metadata key to search for.
 * @param value Reference to store parsed integer value.
 * @param hasErrors Reference flag to mark error occurrence.
 * @param errorLog Output stream to log errors.
 * @return true if the key is found and value parsed successfully, false otherwise.
 */
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

/**
 * @brief Processes the map rows after metadata parsing.
 *        Constructs the internal map representation with game objects.
 * @param file Input file stream positioned at map rows.
 * @param hasErrors Reference flag for error detection.
 * @param errorLog Stream to log any parsing errors.
 */
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

/**
 * @brief Checks for and logs if a row has excess columns beyond expected count.
 * @param line The line representing a map row.
 * @param row Row index being checked.
 * @param hasErrors Reference flag to indicate errors.
 * @param errorLog Stream to log errors.
 */
void GameManager::checkExcessColumns(const string& line, size_t row,bool& hasErrors, ofstream& errorLog) {
	if (line.size() > cols) {
		hasErrors = true;
		errorLog << line.size() - cols << " excess columns in row " << row << ". Ignoring them.\n";

	}
}

/**
 * @brief Checks for extra rows beyond expected count and logs any found.
 * @param file Input file stream at end of reading map rows.
 * @param hasErrors Reference flag to indicate errors.
 * @param errorLog Stream to log errors.
 */
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

/**
 * @brief Processes each cell character in a map row and creates game objects accordingly.
 * @param line The string representing a row of the map.
 * @param row The row index.
 * @param map The map data structure being populated.
 * @param hasErrors Reference flag to mark errors.
 * @param errorLog Stream to log errors.
 */
void GameManager::processRowCells(const string& line, size_t row,vector<vector<vector<unique_ptr<GameObject>>>>& map, bool& hasErrors, ofstream& errorLog) {

    for (size_t col = 0; col < cols; ++col) {
        char symbol = (col < line.size()) ? line[col] : ' ';
        auto obj = processCell(symbol, row, col, hasErrors, errorLog);
    	map[row][col].push_back(move(obj));
    }
}

/**
 * @brief Processes a single map cell symbol and creates the appropriate GameObject.
 * @param symbol Character symbol representing the map cell.
 * @param row Row index of the cell.
 * @param col Column index of the cell.
 * @param hasErrors Reference flag for error detection.
 * @param errorLog Stream to log errors.
 * @return Unique pointer to the created GameObject or nullptr if empty/invalid.
 */
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

/**
 * @brief Handles creating a tank game object for the given player and position.
 * @param player_id The ID of the player (1 or 2).
 * @param row Row position of the tank.
 * @param col Column position of the tank.
 * @param hasErrors Reference flag for error detection.
 * @param errorLog Stream to log errors.
 * @return Unique pointer to the created Tank object or nullptr on failure.
 */
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

/**
 * @brief Initializes player instances using the player factory.
 */
void GameManager::initializePlayers() {
	players.push_back(player_factory->create(1,rows,cols, max_steps, num_shells));
	players.push_back(player_factory->create(2, rows,cols,max_steps, num_shells));
}

/**
 * @brief Sets up the output file path based on the input file path.
 * @param inputFilePath The path to the input file.
 */
void GameManager::setupOutputFile(const string& filePath) {
    size_t last_slash = filePath.find_last_of("/\\");
    output_file = "output_" + (last_slash == string::npos ? filePath : filePath.substr(last_slash + 1)); // string::npos = “not found”
}

/**
 * @brief Runs the game loop until termination conditions are met.
 *        Calls each player's turn to decide tank actions.
 *        Updates the board and logs events.
 */
void GameManager::run() {

	board->printBoard(); //initial board

	while (!isGameOver()) {
		processRound();
		dynamic_cast<BoardSatelliteView*>(board_view.get())->update(board->objMapToCharMap());
		board->printBoard();
		//todo: send curr steps to players
		++current_step;
		if (player_shell_count[0]==0&&player_shell_count[1]==0) {
			++steps_since_no_shells;
		}

		for (auto& player : players)
		{
			dynamic_cast<MyPlayer*>(player.get())->updateStepsLeft();
		}

	}

	logGameResult();
	writeOutput();
	board->writeBoardStates(output_file);
}

/**
 * @brief Counts the number of tanks still alive for a given player.
 * @param player_id The ID of the player (1 or 2).
 * @return The count of alive tanks for the player.
 */
int GameManager::count_alive_tanks(int player_id) const {
	return player_tank_count.at(player_id);
}

/**
 * @brief Processes a single round of the game.
 *        Moves fired shells, collects tank actions, applies moves, updates tank info, and logs actions.
 */
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

/**
 * @brief Updates internal information about tanks after a round.
 *        Updates tank positions, tank counts, shell counts, and processes battle info requests.
 * @param tanks Vector of pointers to Tank objects.
 */
void GameManager::updateTanksInfo(vector<Tank*> tanks) {
	int index;
	TankAlgorithm* tankPtr;
	for (Tank* tank :tanks) {
		tankPtr= findTankAlgorithmById(tank);
		index=getTankIndex(tankPtr);
		player_tanks_pos[tank->getOwnerId()][index]=tank->getPos();  // {-1,-1} if killed
		if (tank->isKilledThisRound())
			--player_tank_count[tank->getOwnerId()];
		if (tank->getLastAction()==ActionRequest::Shoot)
			--player_shell_count[tank->getOwnerId()];
		if (tank->getLastAction()==ActionRequest::GetBattleInfo){
			dynamic_cast<BoardSatelliteView*>(board_view.get())->setRequestingTankPos(tank->getPos());
			players.at(tank->getOwnerId()-1)->updateTankWithBattleInfo(*tankPtr,*board_view);
		}
	}
}

/**
 * @brief Retrieves the index of a TankAlgorithm within the player's tank algorithm vector.
 * @param t Pointer to the TankAlgorithm to find.
 * @return Index of the tank algorithm or -1 if not found.
 */
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

/**
 * @brief Finds the TankAlgorithm object corresponding to a Tank by its ID and owner.
 * @param tankData Pointer to the Tank object.
 * @return Pointer to the TankAlgorithm matching the tank or nullptr if not found.
 */
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

/**
 * @brief Converts the actions of tanks for a round into a formatted string output.
 * @param tankActions Map of Tank pointers to their respective ActionRequests.
 * @return String summarizing the actions performed by each tank.
 */
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

/**
 * @brief Joins a vector of action strings into a single comma-separated string.
 * @param actions Vector of action strings.
 * @return Single string with actions separated by commas.
 */
string GameManager::joinActions(const vector<string>& actions) {
    string result;
    for (size_t i = 0; i < actions.size(); ++i) {
        if (i != 0) result += ", ";
        result += actions[i];
    }
    return result;
}

/**
 * @brief Logs the game result based on remaining alive tanks.
 *        Adds a summary message to the logs vector.
 */
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

	cerr << logs[logs.size()-1] << endl;
}

/**
 * @brief Converts an ActionRequest enum to its string representation.
 * @param action The ActionRequest to convert.
 * @return The string corresponding to the action.
 */
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

/**
 * @brief Writes the logs collected during the game to the output file.
 *        Appends each log entry on a new line.
 */
void GameManager::writeOutput() {
	ofstream out(output_file);
	for (const auto& line : logs) {
		out << line << "\n";
	}
}

/**
 * @brief Checks if the game is over.
 * @return True if the game ended either because max steps reached or one player has no tanks.
 */
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
