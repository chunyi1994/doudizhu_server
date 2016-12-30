#ifndef CARDS_UTILS_H
#define CARDS_UTILS_H

#include "playing_cards.h"

namespace doudizhu {

//判断是否是等差数列
template<class InputIterator, class SubOperator>
static bool is_arithmetic_progression(InputIterator first,
                                      InputIterator last, int tolerance, SubOperator op) {
    auto pre = first;
    while (++first != last) {
        if(op(*first, *pre) != tolerance) return false;
        pre++;
    }
    return true;
}

template<class InputIterator>
static bool is_same_cards(InputIterator first, InputIterator last) {
    return is_arithmetic_progression(first, last, 0, card_subtraction);
}

//炸弹
static bool isBoom(const std::set<PlayingCards>& cards) {
    if (cards.size() == 2) {
        if(cards.count(JOKER_KING_SMALL) && cards.count(JOKER_KING_BIG)) {
            return true;
        }
    }else if(cards.size() == 4) {//数量为4，并且是公差为0的等差数列
        return is_same_cards(cards.begin(), cards.end());
    }
    return false;
}
//单牌
static bool isSingleCard(const std::set<PlayingCards>& cards) {
    return cards.size() == 1;
}

//对子
static bool isOnePair(const std::set<PlayingCards>& cards) {
    if (cards.size() != 2) return false;
    return is_same_cards(cards.begin(), cards.end());
}

//三张一样的牌
static bool isThreeSameCards(const std::set<PlayingCards>& cards) {
    if (cards.size() != 3) return false;
    return is_same_cards(cards.begin(), cards.end());
}

////三带1
static int isThreeWithOne(const std::set<PlayingCards>& cards) {
    if (cards.size() != 4) return -1;
    auto iter_begin2 = cards.begin();
    auto iter_end1 = cards.end();
    iter_end1--;
    ++iter_begin2;
    if (is_same_cards(cards.begin(), iter_end1)) {
        return 0;
    }
    if (is_same_cards(iter_begin2, cards.end())) {
        return 1;
    }
    return -1;
}

//三带二
static int isThreeWithTwo(const std::set<PlayingCards>& cards) {
    if(cards.size() != 5) return -1;
    auto iter_begin2 = cards.begin();
    auto iter_end1 = iter_begin2;
    std::advance(iter_end1, 3);
    std::advance(iter_begin2, 2);
    //前三个 后两个相等  或者 前两个后三个相等
    if (is_same_cards(cards.begin(), iter_end1) &&
            is_same_cards(iter_end1, cards.end())) {
        return 0;
    }else if (is_same_cards(iter_begin2, cards.end()) &&
              is_same_cards(cards.begin(), iter_begin2)  ) {
        return 2;
    }
    return -1;
}

//顺子
static bool isStraight(const std::set<PlayingCards>& cards) {
    if(cards.size() < 5) return false;
    if (cards.count(DIAMONDS_2) ||
            cards.count(HEARTS_2)||
            cards.count(SPADES_2)||
            cards.count(CLUBS_2) ||
            cards.count(JOKER_KING_SMALL)||
            cards.count(JOKER_KING_BIG)) {
        return false;
    }
    return doudizhu::is_arithmetic_progression(
                cards.begin(), cards.end(), 1, card_subtraction);
}
//双顺子
static bool isDoubleStraight(const std::set<PlayingCards>& cards) {
    if (cards.size() % 2 != 0 || cards.size() < 6) return false;
    if (cards.count(DIAMONDS_2) ||
            cards.count(HEARTS_2)||
            cards.count(SPADES_2)||
            cards.count(CLUBS_2)||
            cards.count(JOKER_KING_SMALL)||
            cards.count(JOKER_KING_BIG)) {
        return false;
    }
    auto iter_begin = cards.begin();
    auto iter_end = iter_begin;
    std::advance(iter_end, 2);
    while (iter_begin != cards.end()) {
        //两张两张地判断，是否相等
        if (!is_same_cards(iter_begin, iter_end) ){
            return false;
        }
        //相隔2张是否相减得1
        if(iter_end != cards.end() &&
                card_subtraction(*iter_end, *iter_begin) != 1) {
            return false;
        }
        iter_begin = iter_end;
        std::advance(iter_end, 2);
    }
    return true; //todo
}

//三顺子
static bool isTripleStraight(const std::set<PlayingCards>& cards) {
    if (cards.size() % 3 != 0 || cards.size() < 6) return false;
    if (cards.count(DIAMONDS_2) ||
            cards.count(HEARTS_2)||
            cards.count(SPADES_2)||
            cards.count(CLUBS_2) ||
            cards.count(JOKER_KING_SMALL)||
            cards.count(JOKER_KING_BIG)) {
        return false;
    }
    auto iter_begin = cards.begin();
    auto iter_end = iter_begin;
    std::advance(iter_end, 3);
    while (iter_begin != cards.end()) {
        //三张三张地判断，是否相等
        if (!is_same_cards(iter_begin, iter_end) ){
            return false;
        }
        //相隔3张是否相减得1
        if(iter_end != cards.end() &&
                card_subtraction(*iter_end, *iter_begin) != 1) {
            return false;
        }
        iter_begin = iter_end;
        std::advance(iter_end, 3);
    }
    return true; //todo
}

//飞机带翅膀
static bool isAirplaneWithWings(const std::set<PlayingCards>& cards, Airplane& airplane) {
    if (cards.size() < 8 || cards.size() > 16) return false;
    int airplane_start = -1;
    int airplane_num = 0;
    int i = 0;
    for(auto iter = cards.begin(); iter != cards.end(); ++i) {
        if (std::distance(iter, cards.end()) < 3) break;
        auto iter_end = iter;
        std::advance(iter_end, 3);
        if (!is_same_cards(iter, iter_end)){
            ++iter;
            continue;
        }
        if (card_equals(*iter, HEARTS_2)) break;
        if (airplane_start == -1) airplane_start = i;
        ++airplane_num;
        if (iter_end != cards.end() && card_subtraction(*iter_end, *iter) != 1) break;
        iter = iter_end;
    }//for
    if (airplane_num < 2) return false;
    //把飞机带的牌放到一个extra里
    //把飞机本体放到main里
    std::set<PlayingCards> extra_cards;
    std::set<PlayingCards> main_cards;

    auto iter = cards.begin();
    std::advance(iter, airplane_start);
    extra_cards.insert(cards.begin(), iter);
    auto iter_begin = iter;
    std::advance(iter, 3 * airplane_num);
    main_cards.insert(iter_begin, iter);
    extra_cards.insert(iter, cards.end());
    airplane.extra_num = extra_cards.size();
    airplane.airplane_num = airplane_num;
    airplane.main_min_card = *main_cards.begin();

    if (cards.size() % 4 == 0) {
        //牌数分别为８，１２，１６的情况
        return (std::size_t)airplane_num == extra_cards.size();
    } else {
        //牌数为１０，１４的情况
        auto iter_begin = extra_cards.begin();
        auto iter_end = iter_begin;
        std::advance(iter_end, 2);
        while (iter_begin != extra_cards.end()) {
            //两张两张地判断，是否相等
            if (!is_same_cards(iter_begin, iter_end) ){
                return false;
            }
            iter_begin = iter_end;
            std::advance(iter_end, 2);
        }
        return (std::size_t)airplane_num * 2 == extra_cards.size();
    }
    return false;
}

//3 3 3 4 4 4 5 6              8个
//3 3 3 4 4 4 5 5 6 6       10个
//3 3 3 4 4 4 5 5 5 6 7 8    12个
//3 3 3 4 4 4 5 5 5  33 66 88  14个
//3 3 3 4 4 4 5 5 5 6 6 6 7 8 9 10   16个


//四带二
static bool isFourWithTwo(const std::set<PlayingCards>& cards) {
    return false; //todo
}
}//namespace

#endif // CARDS_UTILS_H

