#ifndef CARDSFACTORY_H
#define CARDSFACTORY_H
#include <set>
#include "playing_cards.h"
namespace doudizhu {
class CardsFactory {
public:
    static Cards::Pointer create(std::set<PlayingCards> cards);
};
}//namespace
#endif // CARDSFACTORY_H
