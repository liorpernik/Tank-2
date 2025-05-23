#ifndef TANKBATTLEINFO_H
#define TANKBATTLEINFO_H
#include "../common/BattleInfo.h"
#include "../common/GameObject.h"
#include <utility>
#include <vector>
#include <memory>
#include <map>

using std::map,std::vector,std::pair;
struct OppData{
    pair<int,int> opponentPos;
    Direction opponentDir = None;
};

class TankBattleInfo : public BattleInfo {
    int id = -1;
    int player_id = -1;
    Direction direction = None;
    pair<int, int> position;
    int remaining_shells = 0;
    int shoot_cooldown = 0;
    bool is_waiting_for_backward = false;
    int backward_cooldown = 0; // Number of turns left until backward move happens
    bool cancel_backward = false; // Whether the backward move was canceled
    bool moved_backward_last = false; // Indicates immediate next back is allowed
    vector<OppData> opponents;
    map<pair<int,int>, vector<GameObject*>> knownObjects;

public:

    TankBattleInfo(int tank_index, int player_id);
    ~TankBattleInfo() override = default;

    void decreaseBackwardCooldown();
    void increaseBackwardCooldown(int cooldown = 2);

    void setWaitingForBackward(bool);
    bool getWaitingForBackward();

    void setMovedBackwardLast(bool);
    bool getMovedBackwardLast();

    void decreaseShootCooldown();
    void increaseShootCooldown(int cooldown = 4);

    bool isWaitingToReverse() const;
    bool isWaitingToShoot() const;

    void decreaseRemainingShells();
    void setRemainingShells(int num_of_shells);
    int getRemainingShells() const;

    void setDirection(Direction direction);
    Direction getDirection() const;

    void setPosition(int x, int y);
    pair<int, int> getPosition() const;

    vector<OppData> getOpponents() const;
    // OppData* getOpponentById(int id) const;
    void addOpponent(pair<int,int> position, Direction dir = None);

    GameObject* getObjectByPosition(pair<int,int> pos) const;

    map<pair<int, int>, vector<GameObject*> > getKnownObjects() const;
    void setKnownObjects(map<pair<int, int>, vector<GameObject*> > knownObjects);
};
#endif //TANKBATTLEINFO_H
