//
// Created by admin on 5/16/2025.
//

#ifndef WALL_H
#define WALL_H

#include "../common/GameObject.h"

class Wall : public GameObject {
private:
    int hitCount;
    static const int WALL_MAX_HITS = 2; // Assuming this is the intended max hits
    static const char WALL_SYMBOL = '#'; // Assuming this is the intended symbol
    bool destroyed;
public:
    Wall(std::pair<int,int> pos);
    ~Wall() override= default;

    Wall(Wall const&) = delete;
    Wall& operator=(const Wall&) = delete;

    int getHitCount() const;
    void registerHit();
    void destroy() override;
    bool isDestroyed() const override;
    char getSymbol() const override;
};
#endif //WALL_H
