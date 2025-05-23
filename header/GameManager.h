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

#include "./BoardManager.h"
#include "./BoardSatelliteView.h"

using std::string,std::unique_ptr,std::vector,std::map,std::pair,std::ifstream,std::ofstream;

class Tank;
class SatelliteView;
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
    int steps_since_no_shells=0;
    map<int, vector<unique_ptr<TankAlgorithm>>> player_tanks_algo;
    map<int, vector<pair<int,int>>> player_tanks_pos;
    vector<unique_ptr<Player>> players;
    unique_ptr<BoardManager> board;
    unique_ptr<SatelliteView> board_view;

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
    void applyAction();
    void updateTankWithBattleInfo();

    // Helper functions for readBoard
    void parseMetadata(ifstream& file, bool& hasErrors, ofstream& errorLog);
    bool tryParseMetadata(const string& line, const string& key,int& value, bool& hasErrors, ofstream& errorLog);
    bool hasAllMetadata() const;
    void processMapRows(ifstream& file, bool& hasErrors, ofstream& errorLog);
    void checkExcessColumns(const string& line, size_t row,bool& hasErrors, ofstream& errorLog);
    void checkExcessRows(ifstream& file, bool& hasErrors,ofstream& errorLog);
    void processRowCells(const string& line, size_t row,vector<vector<vector<unique_ptr<GameObject>>>>& map,bool& hasErrors, ofstream& errorLog);
    unique_ptr<GameObject> processCell(char symbol, size_t row, size_t col,bool& hasErrors, ofstream& errorLog);
    unique_ptr<GameObject> handleTank(int player_id, size_t row, size_t col,bool& hasErrors, ofstream& errorLog);
    void validateTankCounts();
    void initializePlayers();
    void setPlayerTankAlgorithms(int player_id);
    void setupOutputFile(const string& filePath);

    // Helper methods
    int count_alive_tanks(int player_id) const;
    vector<unique_ptr<Tank>> sortTanksByBoardPosition();
    bool isGameOver() const;


    // Log Functions
    string actionToString(ActionRequest action);
    string generateRoundOutput();
    string joinActions(const vector<string>& actions) ;
    void writeOutput();
    void logGameResult();
};

#endif