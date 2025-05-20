//
// Created by admin on 5/15/2025.
//
#include "../header/BoardManager.h"

BoardManager::BoardManager(vector<vector<unique_ptr<GameObject>>> map, int rows, int cols) :  height(rows), width(cols),map(std::move(map)) {}

GameObject* BoardManager::getObjectAt(int x, int y) const {
	if (x >= height || y >= width) return nullptr;
	return map[x][y].get();
}

// BoardManager::BoardManager(string filePath){
// 	BoardData boardData;
//     ifstream file(filePath);
//
// 	if (!file.is_open()) {
// 		throw invalid_argument("Error: Unable to open file at path" + filePath);
// 	}
//
// 	ofstream errorLog("input_errors.txt");
// 	bool hasErrors = false;
//
// 	// Read the dimensions of the board
// 	std::vector<std::string> lines;
// 	std::string line;
//
// 	while (std::getline(file, line)) {
// 		lines.push_back(line);
// 	}
// 	file.close();
//
// 	// Check if file has at least 5 lines (for the header info)
// 	if (lines.size() < 5) {
// 		throw invalid_argument("Error: File must have at least 5 lines, found " + std::to_string(lines.size()));
// 	}
//
// 	// Line 2: MaxSteps
// 	size_t eq_pos = lines[1].find('=');
// 	if (eq_pos == std::string::npos) {
// 		throw invalid_argument("Error: MaxSteps line missing '='");
// 	}
// 	boardData.max_steps = std::stoi(lines[1].substr(eq_pos + 1));
//
// 	// Line 3: NumShells
// 	eq_pos = lines[2].find('=');
// 	if (eq_pos == std::string::npos) {
// 		throw invalid_argument("Error: NumShells line missing '='");
// 	}
// 	boardData.num_shells = std::stoi(lines[2].substr(eq_pos + 1));
//
// 	// Line 4: Rows
// 	eq_pos = lines[3].find('=');
// 	if (eq_pos == std::string::npos) {
// 		throw invalid_argument("Error: Rows line missing '='");
// 	}
// 	boardData.rows = std::stoi(lines[3].substr(eq_pos + 1));
//
// 	// Line 5: Cols
// 	eq_pos = lines[4].find('=');
// 	if (eq_pos == std::string::npos) {
// 		throw invalid_argument("Error: Cols line missing '='");
// 	}
// 	boardData.cols = std::stoi(lines[4].substr(eq_pos + 1));
//
//
// 	// Check for negative values
// 	if (boardData.max_steps < 0 || boardData.num_shells < 0 || boardData.rows < 0 || boardData.cols < 0) {
// 		throw invalid_argument("Error: Negative values not allowed in metadata");
// 	}
//
// 	boardData.map.resize(boardData.rows, std::vector<char>(boardData.cols, ' '));
//
// 	// Start from line 6 (index 5) for map data
// 	for (int i = 0; i < boardData.rows; ++i) {
// 		int line_idx = 5 + i;
// 		if (line_idx < lines.size()) {
// 			const std::string& current_line = lines[line_idx];
// 			for (int j = 0; j < boardData.cols; ++j) {
// 				if (j < current_line.size()) {
// 					char c = current_line[j];
// 					if (c == '1') {
// 						boardData.player1_tanks.emplace_back(i, j);
// 						boardData.map[i][j] = ' ';
// 					} else if (c == '2') {
// 						boardData.player2_tanks.emplace_back(i, j);
// 						boardData.map[i][j] = ' ';
// 					} else if (c == '#' || c == '@') {
// 						boardData.map[i][j] = c;
// 					} else {
// 						boardData.map[i][j] = ' ';
// 					}
// 				} else {
// 					boardData.map[i][j] = ' ';
// 				}
// 			}
// 		} else {
// 			// Fill with empty spaces if line doesn't exist
// 			std::fill(boardData.map[i].begin(), boardData.map[i].end(), ' ');
// 		}
// 	}


//	// Handle excess rows
//	int rowIndex = height;
//	while (getline(file, line)) {
//		hasErrors = true;
//		++rowIndex;
//	}
//	if (errorLog && rowIndex - height >0) errorLog << rowIndex - height << " Excess rows " << " found. Ignoring it.\n";
//
//	if (!hasErrors && errorLog) {
//		errorLog.close(); // Delete file if no errors were logged
//		remove("input_errors.txt");
//	}
// }


void BoardManager::printBoard()
{
	stringstream boardState;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			GameObject* obj = getObjectAt(i,j);//map[i][j]
			obj ?
				boardState << obj->getSymbol() : boardState << ' ';

		}
		boardState << "\n";
	}
	// Store the complete board state
	boardStates.push_back(boardState.str());
}

void BoardManager::writeBoardStates(string fileName)
{
	std::ofstream outFile("gameSteps_"+fileName);
	if (!outFile.is_open()) {
		std::cerr << "Error: Could not open file for writing!" << std::endl;
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