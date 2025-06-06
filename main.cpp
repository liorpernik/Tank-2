//
// Created by admin on 5/6/2025.
//
#include <sstream>
#include <string>
#include <iostream>
#include <memory>

#include "./header/GameManager.h"
#include "./header/MyPlayerFactory.h"
#include "./header/MyTankAlgorithmFactory.h"

int main(int argc, char** argv)
{
    try
    {
        string filePath = "";
        if(argc > 1) {
            filePath = argv[1];
        }
        auto playerFactory = std::make_unique<MyPlayerFactory>();
        auto tankFactory = std::make_unique<MyTankAlgorithmFactory>();
        GameManager game(std::move(playerFactory), std::move(tankFactory));

        for(int i =3; i <4; i++){
            filePath = "inputs/board"+ to_string(i)+ ".txt";
            game.readBoard(filePath);
            game.run();
        }
    } catch (const std::exception& e) {
        std::clog << e.what() << std::endl;
        return 1;
    }

    return 0;
}