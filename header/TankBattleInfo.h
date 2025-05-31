//
// Created by admin on 5/6/2025.
//

#ifndef TANKBATTLEINFO_H
#define TANKBATTLEINFO_H
#include "../common/BattleInfo.h"
#include "../common/GameObject.h"
#include <utility>
#include <vector>
#include <memory>
#include <map>

struct OppData{
    std::pair<int,int> opponentPos;
    Direction opponentDir = None;

    // OppData& operator=(const OppData& other) {
    //     if (this != &other) { // Self-assignment check
    //         opponentPos = other.opponentPos;
    //         opponentDir = other.opponentDir;
    //     }
    //     return *this; // Return *this to allow chaining
    // };
};

static const std::vector<std::pair<int, int>> offsets = {
    {-1, 0},  // U
    {-1, 1},  // UR
    { 0, 1},  // R
    { 1, 1},  // DR
    { 1, 0},  // D
    { 1, -1}, // DL
    { 0, -1}, // L
    {-1, -1}  // UL
};

class TankBattleInfo : public BattleInfo {
    int id = -1;
    int player_id = -1;
    Direction direction = None;
    std::pair<int, int> position;
    int remaining_shells = 0;
    int shoot_cooldown = 0;
    bool is_waiting_for_backward = false;
    int backward_cooldown = 0; // Number of turns left until backward move happens
    bool cancel_backward = false; // Whether the backward move was canceled
    bool moved_backward_last = false; // Indicates immediate next back is allowed
    std::vector<OppData> opponents;
    std::map<std::pair<int,int>, std::vector<GameObject*>> knownObjects;

    bool set_shells = false;
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
    bool isShellsSet() const;

    void setDirection(Direction direction);
    Direction getDirection() const;

    void setPosition(int x, int y);
    std::pair<int, int> getPosition() const;

    std::vector<OppData> getOpponents() const;
    // OppData* getOpponentById(int id) const;
    void setOpponents(std::vector<OppData> opps);
    void addOpponent(std::pair<int,int> position, Direction dir = None);

    GameObject* getObjectByPosition(std::pair<int,int> pos) const;

    std::map<std::pair<int, int>, std::vector<GameObject*> > getKnownObjects() const;
    void setKnownObjects(std::map<std::pair<int, int>, std::vector<GameObject*> > knownObjects);
};
#endif //TANKBATTLEINFO_H
