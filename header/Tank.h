#ifndef TANK_H
#define TANK_H

#include "../common/GameObject.h"
#include <string> // for to_string()
#include "../common/ActionRequest.h"
using std::pair;

class Tank : public GameObject {
    int tank_index;
    Direction direction;
    int ownerId;
    bool destroyed;
    bool killed_this_round=false;
    int remaining_shells = 0;
    int shoot_cooldown = 0;
    int backward_cooldown = 0;
    ActionRequest last_action;
    bool last_action_success;

public:
    Tank(pair<int,int> pos,int tank_index,  Direction dir, int owner, int num_of_shells);
    ~Tank() override= default;

    Tank(Tank const&) = delete;
    Tank& operator=(const Tank&) = delete;

    void destroy() override;
    bool isDestroyed() const override;

    Direction getDirection() const;
    void setDirection(Direction dir);

    int getOwnerId() const;
    char getSymbol() const override;

    void setNumOfShells(int shells);
    int getNumOfRemainingShells() const;

    void setShootCooldown(int cooldown);
    int getShootCooldown() const;

    void setKilledThisRound(bool killed_now){killed_this_round=killed_now;}
    bool isKilledThisRound() const{return killed_this_round;}

    void setLastAction(ActionRequest action){last_action=action;}
    ActionRequest getLastAction() const{return last_action;}

    void setActionSuccess(bool success){last_action_success=success;}
    bool getActionSuccess () const {return last_action_success;}

    void setBackwardCooldown(int cooldown);
    int getBackwardCooldown() const;
};
#endif //TANK_H
