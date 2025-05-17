//
// Created by admin on 5/16/2025.
//

#ifndef SHELL_H
#define SHELL_H
#include "../common/GameObject.h"

class Shell : public GameObject {
public:
    // Constructor
    Shell(std::pair<int,int> pos, Direction dir, int owner);
    ~Shell() override= default;

    Shell(Shell const&) = delete;
    Shell& operator=(const Shell&) = delete;

    // Accessors
    Direction getDirection() const;
    int getOwnerId() const;
    bool isDestroyed() const override;

    // Mutators
    void destroy() override;

    // Symbol for representation on the board
    char getSymbol() const override;

private:
    Direction direction;
    int ownerId;
    bool destroyed;
    static const char SHELL_SYMBOL = '*';
};
#endif //SHELL_H
