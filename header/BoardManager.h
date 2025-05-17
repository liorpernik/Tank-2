//
// Created by admin on 5/15/2025.
//
#ifndef BOARDMANAGER_H
#define BOARDMANAGER_H
#include "../common/GameObject.h"
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;
// : public SatelliteView
class BoardManager  {
  int height;
  int width;
  vector<vector<unique_ptr<GameObject>>> map;
  vector<string> boardStates;

  public:
    BoardManager(const BoardManager&) = delete;
    BoardManager(vector<vector<unique_ptr<GameObject>>> map, int rows, int cols);
    // BoardManager(string filePath);
    ~BoardManager() = default;
    GameObject& getObjectAt(int x, int y) const;

    void updateBoard(size_t x, size_t y, char value);

    void printBoard();
    void writeBoardStates(string);
    // int wrap(int value, int size) const { return (value % size + size) % size; }
};
#endif //BOARDMANAGER_H
