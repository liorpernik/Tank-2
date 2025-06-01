#ifndef MINE_H
#define MINE_H
#include "../common/GameObject.h"
using std::pair;

class Mine : public GameObject {
private:
    bool triggered;
    bool destroyed;
    static const char MINE_SYMBOL = '@'; // Assuming this is the intended symbol

public:
    Mine(pair<int,int> pos);
    ~Mine() override= default;

    Mine(Mine const&) = delete;
    Mine& operator=(const Mine&) = delete;

    void trigger();
    bool isTriggered() const;
    void destroy() override;
    bool isDestroyed() const override;
    char getSymbol() const override;
};
#endif //MINE_H
