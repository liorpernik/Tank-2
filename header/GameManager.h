#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include "../common/PlayerFactory.h"
#include "../common/TankAlgorithmFactory.h"
#include "../common/ActionRequest.h"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <fstream>
#include <algorithm>

#include "BoardManager.h"


using std::string,std::unique_ptr,std::vector,std::map,std::pair;

class Tank;

class GameManager {
public:

    GameManager(unique_ptr<PlayerFactory> playerFactory,unique_ptr<TankAlgorithmFactory> tankFactory);
    ~GameManager();
    GameManager(GameManager const&) = delete;
    GameManager& operator=(const GameManager&) = delete;
    void readBoard(const string& filePath);
    void run();

private:
    // Game state
    size_t cols=0,rows =0;
    int num_shells=0;
    int max_steps=0;
    int current_step = 0;
    std::map<int, std::vector<unique_ptr<TankAlgorithm>>> player_tanks_algo;
    std::map<int, std::vector<pair<int,int>>> player_tanks_pos;
    vector<unique_ptr<Player>> players;
    unique_ptr<BoardManager> board;

    vector<string> logs;
    string output_file;

    // Tank count tracking
    map<int, int> player_tank_count = {{1, 0}, {2, 0}};
    map<int, int> player_shell_count = {{1, 0}, {2, 0}};


    // Factories
    unique_ptr<PlayerFactory> player_factory;
    unique_ptr<TankAlgorithmFactory> tank_factory;

    //todo - Implement:
    void processRound();
    void moveShells(); // and all helpers
    void applyAction();

    // Helper functions for readBoard
    void parseMetadata(std::ifstream& file, bool& hasErrors, std::ofstream& errorLog);
    bool tryParseMetadata(const std::string& line, const std::string& key,int& value, bool& hasErrors, std::ofstream& errorLog);
    bool hasAllMetadata() const;
    void processMapRows(std::ifstream& file, bool& hasErrors, std::ofstream& errorLog);
    void checkExcessColumns(const std::string& line, size_t row,bool& hasErrors, std::ofstream& errorLog);
    void checkExcessRows(std::ifstream& file, bool& hasErrors,std::ofstream& errorLog);
    void processRowCells(const std::string& line, size_t row,vector<vector<vector<unique_ptr<GameObject>>>>& map,bool& hasErrors, std::ofstream& errorLog);
    unique_ptr<GameObject> processCell(char symbol, size_t row, size_t col,bool& hasErrors, std::ofstream& errorLog);
    unique_ptr<GameObject> handleTank(int player_id, size_t row, size_t col,bool& hasErrors, std::ofstream& errorLog);
    void validateTankCounts();
    void initializePlayers();
    void setPlayerTankAlgorithms(int player_id);
    void setupOutputFile(const std::string& filePath);

    // Helper methods
    int count_alive_tanks(int player_id) const;
    std::vector<unique_ptr<Tank>> sortTanksByBoardPosition();
    bool isGameOver() const;

    // Log Functions
    string actionToString(ActionRequest action);
    string generateRoundOutput();
    string joinActions(const vector<string>& actions) ;
    void writeOutput();
    void logGameResult();
};

#endif