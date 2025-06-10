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
//        auto playerFactory = std::make_unique<MyPlayerFactory>();
//        auto tankFactory = std::make_unique<MyTankAlgorithmFactory>();
//        GameManager game(std::move(playerFactory), std::move(tankFactory));
//        game.readBoard(filePath);
//        game.run();
//

    auto playerFactory = std::make_unique<MyPlayerFactory>();
    auto tankFactory = std::make_unique<MyTankAlgorithmFactory>();
    for(int i=6;i<7;i++){
      std::cout <<"test "<<i <<"\n";

        auto playerFactory = std::make_unique<MyPlayerFactory>();
        auto tankFactory = std::make_unique<MyTankAlgorithmFactory>();
        filePath = "./inputs/t" + to_string(i) + ".txt";
        GameManager game(std::move(playerFactory), std::move(tankFactory));
        game.readBoard(filePath);
        game.run();
        std::cout <<"done\n";
      }
    } catch (const std::exception& e) {
        std::clog << e.what() << std::endl;
        return 1;
    }

    return 0;
}