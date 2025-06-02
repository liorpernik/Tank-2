#include "../header/MyPlayer.h"

#include "../header/Wall.h"
#include "../header/Mine.h"
#include "../header/Tank.h"
#include "../header/Shell.h"

#include "../header/BoardSatelliteView.h"

MyPlayer::MyPlayer(int player_index, size_t map_width, size_t map_height, size_t max_steps, size_t num_shells)
    : player_index(player_index), map_width(map_width), map_height(map_height),
        steps_left(static_cast<int>(max_steps)), shells_per_tank(num_shells), battle_info(std::make_unique<TankBattleInfo>(-1,player_index))
{
}

void MyPlayer::addTank(unique_ptr<TankAlgorithm>& tank)
{
    tanks.push_back(std::move(tank));
}

int MyPlayer::getPlayerIndex() const { return player_index;}

void MyPlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& view)
{
    getBattleInfoFromSatelliteView(view);
    tank.updateBattleInfo(*battle_info);
}

void MyPlayer::getBattleInfoFromSatelliteView(SatelliteView& view)
{
    int i=0,j=0;

    char symbol = ' ';
    int tanki_1=0, tanki_2=0;
    auto* tank_info =  dynamic_cast<TankBattleInfo*>(battle_info.get());

    std::vector<OppData> opponents;
    std::map<std::pair<int,int>, std::vector<GameObject*>> knownObjects = tank_info->getKnownObjects();
    // This will own all the game objects
    static std::vector<std::unique_ptr<GameObject>> objectStorage;

    if(!tank_info->isShellsSet()) tank_info->setRemainingShells(shells_per_tank);
    if (tank_info->getMapSize().first == -1) tank_info->setMapSize(map_height, map_width);

   for ( i= 0; i < static_cast<int>(map_height); i++)
   {
       for ( j = 0; j < static_cast<int>(map_width) ; j++)
       {
           symbol = view.getObjectAt(i,j);
           if (symbol == '&')
           {
               break;
           }
           if (symbol == '%'){
               tank_info->setPosition(i,j);
               break;
           }
            pair pos = {i,j};

            if (symbol == ' ')
            {
                if (knownObjects.contains(pos)) knownObjects.erase(pos);
                continue;
            }
            vector<GameObject*> vec = knownObjects[pos];
            if (vec.empty())
            {
                unique_ptr<GameObject> obj;
                if (symbol == '@')
                {
                    obj = make_unique<Mine>(pos);
                }else if (symbol == '#')
                {
                    obj = make_unique<Wall>(pos);
                }else if (symbol == '*'){
                   obj = make_unique<Shell>(pos, None, -1);
                }else if (isdigit(symbol)){
                    int player = symbol - '0';
                    obj = std::make_unique<Tank>(pos,player_index == 1 ? tanki_1++ : tanki_2++,Direction::None,player_index,shells_per_tank);
                    if (player != player_index)
                    {
                        opponents.push_back(OppData(pos));
                    }
                }
                knownObjects[pos].push_back(obj.get());
                objectStorage.push_back(std::move(obj));
            }else if (vec.size() > 1 && (symbol == '@' || symbol == '#'))
            {
                vec.erase(vec.end() - 1);
            }
       }
   }

    tank_info->setKnownObjects(knownObjects);
    tank_info->setOpponents(opponents);
}