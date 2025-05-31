#include "../header/MyPlayer.h"

#include "../header/Wall.h"
#include "../header/Mine.h"
#include "../header/Tank.h"
#include "../header/Shell.h"

#include "../header/BoardSatelliteView.h"

MyPlayer::MyPlayer(int player_index, size_t map_width, size_t map_height, size_t max_steps, size_t num_shells)
    : player_index(player_index), map_width(map_width), map_height(map_height),
        steps_left(static_cast<int>(max_steps)), shells_per_tank(num_shells), battle_info(unique_ptr<TankBattleInfo>())
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

    bool set_map_size = false;
    char symbol = ' ';
    int tank_i=0;
    auto* tank_info =  dynamic_cast<TankBattleInfo*>(battle_info.get());
    std::vector<OppData> opponents;
    std::map<std::pair<int,int>, std::vector<GameObject*>> knownObjects = tank_info->getKnownObjects();

    if (map_width == 0 || map_height == 0) { set_map_size=true; }
    if(!tank_info->isShellsSet()) tank_info->setRemainingShells(shells_per_tank);

   for ( i= 0; i < static_cast<int>(map_height) || set_map_size; i++)
   {
       for ( j = 0; j < static_cast<int>(map_width) || set_map_size; j++)
       {
           symbol = view.getObjectAt(i,j);
           if (symbol == '&')
           {
               if (set_map_size && map_width == 0) setWidth(j);
               if (set_map_size && map_width > 0) setWidth(i);
               break;
           }
           if (symbol == '%'){
               tank_info->setPosition(i,j);
               break;
           }

            std::pair pos = {i,j};
            std::vector<GameObject*> vec = knownObjects.at(pos);
            if (vec.empty())
            {
                if (symbol == '@')
                {
                    vec.push_back(std::make_unique<Wall>(pos).get());
                }else if (symbol == '#')
                {
                    vec.push_back(std::make_unique<Mine>(pos).get());
                }
            }

            else if (symbol == ' ' && !vec.empty())
            {
                vec.clear();
            }else if (!vec.empty() && symbol == '@' && symbol == '#')
            {
                vec.erase(vec.end() - 1);
            }else{
                if (symbol == '*'){
                    vec.push_back(std::make_unique<Shell>(pos, None, -1).get());
                }else{
                    int player = symbol - '0';
                    vec.push_back(std::make_unique<Tank>(pos,tank_i++,Direction::None,player_index,shells_per_tank).get());
                    if (player != player_index)
                    {
                        opponents.push_back(OppData(pos));
                    }
                }
            }
       }

       if (set_map_size && map_height > 0) break;
   }

    tank_info->setKnownObjects(knownObjects);
    tank_info->setOpponents(opponents);
}

void MyPlayer::setHeight(int height)
{
    map_height = static_cast<size_t>(height);
}

void MyPlayer::setWidth(int width)
{
    map_width = static_cast<size_t>(width);
}