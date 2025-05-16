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

enum Direction { U, UR, R, DR, D, DL, L, UL, None};
using std::string,std::unique_ptr,std::vector,std::map,std::pair;

class GameManager {
public:
    GameManager(unique_ptr<PlayerFactory> playerFactory,unique_ptr<TankAlgorithmFactory> tankFactory);
    void readBoard(const string& filePath);
    void run();

private:
    struct TankData {
        unique_ptr<TankAlgorithm> algorithm;
        int player_index;
        int tank_index;
        pair<size_t,size_t> pos; //{row,col}
        Direction dir;
        int remaining_shells;
        ActionRequest last_action=ActionRequest::DoNothing;
        bool last_action_success =true;
        bool alive = true;
        bool killed_this_round = false;
    };

    // Game state
    vector<TankData> tanks;
    vector<unique_ptr<Player>> players;
    int max_steps;
    int current_step = 0;
    int num_shells;
    int rows;
    int cols;
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
    void processRowCells(const std::string& line, size_t row,bool& hasErrors, std::ofstream& errorLog);
    void processCell(char symbol, size_t row, size_t col,bool& hasErrors, std::ofstream& errorLog);
    void handleTank(int player_id, size_t row, size_t col,bool& hasErrors, std::ofstream& errorLog);
    void validateTankCounts();
    void initializePlayers();
    void setupOutputFile(const std::string& filePath);

    // Helper methods
    int count_alive_tanks(int player_id) const;
    void sortTanksByBoardPosition();
    bool isGameOver() const;

    // Log Functions
    string actionToString(ActionRequest action);
    string generateRoundOutput();
    string joinActions(const vector<string>& actions) ;
    void writeOutput();
    void logGameResult();
};

#endif