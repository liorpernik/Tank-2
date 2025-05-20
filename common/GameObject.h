//
// Created by admin on 5/16/2025.
//

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <utility>

enum Direction { U, UR, R, DR, D, DL, L, UL, None};

class GameObject {
    std::pair<int, int> pos;
  public:
    GameObject() = default;
    GameObject(std::pair<int, int> pos) : pos(pos) {}
    virtual ~GameObject() {};
    GameObject(GameObject const&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    virtual bool isDestroyed() const = 0;
    virtual void destroy() = 0;
    virtual std::pair<int, int> getPos() { return pos; }
    virtual void setPos(std::pair<int, int> pos) { this->pos = pos; }
    virtual char getSymbol() const { return ' '; }
};
#endif //GAMEOBJECT_H
