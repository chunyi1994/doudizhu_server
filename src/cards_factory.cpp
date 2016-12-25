#include "cards_factory.h"
#include "cards_algo.h"
namespace doudizhu {


Cards::Pointer CardsFactory::create(std::set<PlayingCards> cards) {
    if (isSingleCard(cards)) {
        utils::log("单排");
        return std::make_shared<SingleCard>(cards);
    }
    if (isOnePair(cards)) {
        utils::log("两张");
        return std::make_shared<OnePair>(cards);
    }
    if (isBoom(cards)) {
        utils::log("炸弹");
        return std::make_shared<Boom>(cards);
    }
    int n = -1;
    if ( (n = isThreeWithOne(cards)) != -1) {
        utils::log("三带一");
        auto iter = cards.begin();
        std::advance(iter, n);
        return std::make_shared<ThreeWithOne>(cards, *iter);
    }
    if ( (n = isThreeWithTwo(cards)) != -1) {
        utils::log("三带２");
        auto iter = cards.begin();
        std::advance(iter, n);
        return std::make_shared<ThreeWithTwo>(cards, *iter);
    }
    if (isThreeSameCards(cards)) {
        utils::log("三张牌");
        return std::make_shared<ThreeSameCards>(cards);
    }
    if (isStraight(cards)) {
        utils::log("顺子");
        return std::make_shared<Straight>(cards);
    }
    if (isDoubleStraight(cards)) {
        utils::log("双顺子");
        return std::make_shared<DoubleStraight>(cards);
    }
    if (isTripleStraight(cards)) { //todo
        return std::make_shared<TripleStraight>(cards);
    }
    Airplane airplane;
    if (isAirplaneWithWings(cards, airplane)) { //todo
        utils::log("飞机带翅膀");
        return std::make_shared<AirplaneWithWings>(cards, airplane);
    }
    if (isFourWithTwo(cards)) { //todo

    }
    return std::make_shared<WrongCards>(cards);
}

}//namespace
