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
private:
    const std::pair<int, int> requestingTankPos; // Position of tank making the request

public:
    BoardSatelliteView(std::pair<int, int> requesterPos)
        :  requestingTankPos(requesterPos) {}

    char getObjectAt(size_t x, size_t y) const override {
        // Check if location is outside the battlefield
//        if (x >= boardData.cols || y >= boardData.rows) {
//            return '&';
//        }

        // Convert size_t to int for comparison with tank positions
        int posX = static_cast<int>(x);
        int posY = static_cast<int>(y);

        // Check if this is the requesting tank's position
        if (posX == requestingTankPos.first && posY == requestingTankPos.second) {
            return '%';
        }

//        // Check for artillery shells first (highest priority)
//        // Note: You'll need to add artillery shell tracking to your BoardData
//        for (const auto& shell : boardData.artilleryShells) {
//            if (shell.x == posX && shell.y == posY) {
//                return '*';
//            }
//        }
//
//        // Check for player tanks (excluding the requesting tank)
//        for (const auto& tank : boardData.player1_tanks) {
//            if (tank.second == posX && tank.first == posY) {
//                // Skip if this is the requesting tank
//                if (!(posX == requestingTankPos.first && posY == requestingTankPos.second)) {
//                    return '1';
//                }
//            }
//        }
//
//        for (const auto& tank : boardData.player2_tanks) {
//            if (tank.second == posX && tank.first == posY) {
//                // Skip if this is the requesting tank
//                if (!(posX == requestingTankPos.first && posY == requestingTankPos.second)) {
//                    return '2';
//                }
//            }
//        }
//
//        // Check the base map (walls and mines)
//        // Only check for mines if there isn't an artillery shell above them
//        char mapObject = boardData.map[posY][posX];
//        if (mapObject == '#') {
//            return '#';
//        }
//        else if (mapObject == '@') {
//            // Check if there's an artillery shell at this position
//            bool hasShell = false;
//            for (const auto& shell : boardData.artilleryShells) {
//                if (shell.x == posX && shell.y == posY) {
//                    hasShell = true;
//                    break;
//                }
//            }
//            return hasShell ? '*' : '@';
//        }

        // Empty space
        return ' ';
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
