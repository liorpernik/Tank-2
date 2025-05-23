//
// Created by admin on 5/15/2025.
//
#include "../header/BoardManager.h"

BoardManager::BoardManager(vector<vector<vector<unique_ptr<GameObject>>>> map, int rows, int cols) :  height(rows), width(cols),map(std::move(map)) {}

GameObject* BoardManager::getObjectAt(int x, int y) const {
	if (x >= height || y >= width) return nullptr;
	return !map[x][y].empty() ? map[x][y].size() > 1 ? map[x][y][1].get() : map[x][y][0].get() : nullptr; //if more than one then shell is last and more important
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