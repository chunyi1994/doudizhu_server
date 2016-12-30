#include "cards_factory.h"
#include "cards_algo.h"
#include "../logging/logger.h"
namespace doudizhu {


Cards::Pointer CardsFactory::create(std::set<PlayingCards> cards) {
    if (isSingleCard(cards)) {
        LOG_DEBUG<<"单牌";
        return std::make_shared<SingleCard>(cards);
    }
    if (isOnePair(cards)) {
        LOG_DEBUG<<"两张";
        return std::make_shared<OnePair>(cards);
    }
    if (isBoom(cards)) {
        LOG_DEBUG<<"炸弹";
        return std::make_shared<Boom>(cards);
    }
    int n = -1;
    if ( (n = isThreeWithOne(cards)) != -1) {
        LOG_DEBUG<<"三带一";
        auto iter = cards.begin();
        std::advance(iter, n);
        return std::make_shared<ThreeWithOne>(cards, *iter);
    }
    if ( (n = isThreeWithTwo(cards)) != -1) {
        LOG_DEBUG<<"三带２";
        auto iter = cards.begin();
        std::advance(iter, n);
        return std::make_shared<ThreeWithTwo>(cards, *iter);
    }
    if (isThreeSameCards(cards)) {
        LOG_DEBUG<<"三张牌";
        return std::make_shared<ThreeSameCards>(cards);
    }
    if (isStraight(cards)) {
        LOG_DEBUG<<"顺子";
        return std::make_shared<Straight>(cards);
    }
    if (isDoubleStraight(cards)) {
        LOG_DEBUG<<"双顺子";
        return std::make_shared<DoubleStraight>(cards);
    }
    if (isTripleStraight(cards)) { //todo
        return std::make_shared<TripleStraight>(cards);
    }
    Airplane airplane;
    if (isAirplaneWithWings(cards, airplane)) { //todo
        LOG_DEBUG<<"飞机带翅膀";
        return std::make_shared<AirplaneWithWings>(cards, airplane);
    }
    if (isFourWithTwo(cards)) { //todo

    }
    LOG_DEBUG<<"非法牌";
    return std::make_shared<WrongCards>(cards);
}

}//namespace
