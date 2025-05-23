//
// Created by admin on 5/15/2025.
//

#ifndef BOARDSATELLITEVIEW_H
#define BOARDSATELLITEVIEW_H
#include <vector>
#include <string>

#include "../common/SatelliteView.h"

using namespace std;

class BoardSatelliteView : public SatelliteView {
    int cols;
    int rows;
    vector<vector<char>> map;
    const std::pair<int, int> requestingTankPos; // Position of tank making the request

public:
    BoardSatelliteView(std::pair<int, int> requesterPos)
        :  requestingTankPos(requesterPos) {}

    char getObjectAt(size_t x, size_t y) const override {

        // Convert size_t to int for comparison with tank positions
        int posX = static_cast<int>(x);
        int posY = static_cast<int>(y);

        // Check if location is outside the battlefield
        if (rows < posX || cols < posY) {
            return '&';
        }

        // Check if this is the requesting tank's position
        if (posX == requestingTankPos.first && posY == requestingTankPos.second) {
            return '%';
        }

        // Empty space
        return map[posX][posY];
    }

//    size_t getWidth() const override {
//        return boardData.cols;
//    }
//
//    size_t getHeight() const override {
//        return boardData.rows;
//    }
};
#endif //BOARDSATELLITEVIEW_H
