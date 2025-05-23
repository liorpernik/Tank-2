#ifndef BOARDMANAGER_H
#define BOARDMANAGER_H
#include "../common/GameObject.h"
#include "../header/Tank.h"
#include "../header/Shell.h"
#include "../header/Mine.h"
#include "../header/Wall.h"
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>


using std::vector,std::unique_ptr,std::string,std::stringstream,std::map;

class BoardManager  {
  int height;
  int width;
  vector<vector<vector<unique_ptr<GameObject>>>> game_map;
    vector<unique_ptr<Shell>> fired_shells;
  vector<string> boardStates;

  public:
    BoardManager(const BoardManager&) = delete;
    BoardManager(vector<vector<vector<unique_ptr<GameObject>>>> map, int rows, int cols);
    // BoardManager(string filePath);
    ~BoardManager() = default;
    GameObject* getObjectAt(int x, int y) const;

    void updateBoard(size_t x, size_t y, char value);

    void printBoard();
    void writeBoardStates(string);
    //int wrap(int value, int size) const { return (value % size + size) % size; }

    void updateMap(pair<int, int> curr_pos,pair<int,int> new_pos);
    vector<Tank*> getSortedTanks();

    void moveFiredShells();
    pair<int, int> calculateNewPosition(pair<int, int> pos, Direction dir) const;// replaces wrap and returns new pos
    void processCollision(vector<GameObject*>& objects);
    void cleanupDestroyedObjects(pair<int,int>);

};
#endif //BOARDMANAGER_H
