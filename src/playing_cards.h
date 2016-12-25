#ifndef CARDS_H
#define CARDS_H
#include <set>
#include <typeinfo>
#include <memory>
#include <assert.h>
#include "utils.h"
namespace doudizhu {
//clubs 梅花,三叶草
//diamonds 方块,红方,钻石
//hearts 红桃,红心
//spades 黑桃,剑
enum PlayingCards {
    CLUBS_3 = 0,
    DIAMONDS_3,
    HEARTS_3,
    SPADES_3,
    CLUBS_4,
    DIAMONDS_4,
    HEARTS_4,
    SPADES_4,
    CLUBS_5,
    DIAMONDS_5,
    HEARTS_5,
    SPADES_5,
    CLUBS_6,
    DIAMONDS_6,
    HEARTS_6,
    SPADES_6,
    CLUBS_7,
    DIAMONDS_7,
    HEARTS_7,
    SPADES_7,
    CLUBS_8,
    DIAMONDS_8,
    HEARTS_8,
    SPADES_8,
    CLUBS_9,
    DIAMONDS_9,
    HEARTS_9,
    SPADES_9,
    CLUBS_10,
    DIAMONDS_10,
    HEARTS_10,
    SPADES_10,
    CLUBS_J,
    DIAMONDS_J,
    HEARTS_J,
    SPADES_J,
    CLUBS_Q,
    DIAMONDS_Q,
    HEARTS_Q,
    SPADES_Q,
    CLUBS_K,
    DIAMONDS_K,
    HEARTS_K,
    SPADES_K,
    CLUBS_A,
    DIAMONDS_A,
    HEARTS_A,
    SPADES_A,
    CLUBS_2,
    DIAMONDS_2,
    HEARTS_2,
    SPADES_2,
    JOKER_KING_SMALL, //52
    JOKER_KING_BIG = 56
};

static std::string cardChineseName(PlayingCards card, bool need_color = true) {
    if(card == JOKER_KING_BIG) return "大王";
    if(card == JOKER_KING_SMALL) return "小王";
    int n = card;
    std::string color;
    if (need_color) {
        switch (n % 4) {
        case 0: color = "梅花"; break;
        case 1: color = "方块"; break;
        case 2: color = "红桃"; break;
        case 3: color = "黑桃"; break;
        default: break;
        }
    }
    std::string num;
    if (n / 4 <= 7) num = utils::int2string(n/4 + 3);
    else if(n / 4 == 8) num = "J";
    else if(n / 4 == 9) num = "Q";
    else if(n / 4 == 10) num = "K";
    else if(n / 4 == 11) num = "A";
    else if(n / 4 == 12) num = "2";
    return color + num;
}

const int MAX_PLAYING_CARDS_NUM = 54;

static int card_subtraction(PlayingCards card1, PlayingCards card2) {
    return (card1 / 4) - (card2 / 4);
}

static bool card_equals(PlayingCards card1, PlayingCards card2) {
    return card1 / 4 == card2 / 4;
}

static bool card_less_than(PlayingCards card1, PlayingCards card2) {
    return card1 / 4 < card2 / 4;
}

static bool card_greater_than(PlayingCards card1, PlayingCards card2) {
    return card1 / 4 > card2 / 4;
}

//---------------------------------------------------------------------------------------------------------
//card一系列类
class Cards {
public:
    typedef std::shared_ptr<Cards> Pointer;

public:
    Cards(std::set<PlayingCards> cards) : cards_(std::move(cards)) {}
    //对于大部分的牌，在牌的数量相等的情况下，只需要判断第一个牌谁大谁小就行
    virtual bool greaterThan(const Cards::Pointer other) const {
        return card_greater_than( *(this->cards_.begin()),*(other->cards_.begin()) );
    }
    virtual bool lessThan(const Cards::Pointer other) const {
        return card_less_than( *(this->cards_.begin()),*(other->cards_.begin()) );
    }

    virtual bool isTheSameType(const Cards::Pointer other) const {
        return typeid(*this) == typeid(*(other.get())) && size() == other->size();
    }

    std::size_t size() const { return cards_.size(); }
    const std::set<PlayingCards>& cards() const { return cards_; }
    std::set<PlayingCards>& cards() { return cards_; }

protected:
    std::set<PlayingCards> cards_;
};

//炸弹
class Boom : public Cards{
public:
    Boom(std::set<PlayingCards> cards) : Cards(cards) {}
    //和炸弹一样类型，就比大小，如果和炸弹类型不一样，肯定炸弹大
    virtual bool greaterThan(const Cards::Pointer other) const {
        if (typeid(*this) == typeid(*(other.get()))) {
            return Cards::greaterThan(other);
        }
        return true;
    }

    virtual bool lessThan(const Cards::Pointer other) const {
        if (typeid(*this) == typeid(*(other.get()))) {
            return Cards::lessThan(other);
        }
        return false;
    }

};

//单牌
class SingleCard : public Cards{
public:
    SingleCard(std::set<PlayingCards> cards) : Cards(cards) {}
};

//对子
class OnePair : public Cards {
public:
    OnePair(std::set<PlayingCards> cards) : Cards(cards) {}

};

//三张一样的牌
class ThreeSameCards : public Cards {
public:
    ThreeSameCards(std::set<PlayingCards> cards) : Cards(cards) {}
};

//三带1
class ThreeWithOne : public Cards {
public:
    ThreeWithOne(std::set<PlayingCards> cards, PlayingCards compareCard) : Cards(cards), compareCard_(compareCard) {}

    virtual bool greaterThan(const Cards::Pointer other) const {
        const Cards* other_cards = other.get();
        const ThreeWithOne* other_raw = dynamic_cast<const ThreeWithOne*>(other_cards);
        assert(other_raw);
        return card_greater_than(compareCard_, other_raw->compareCard_);
    }
    virtual bool lessThan(const Cards::Pointer other) const {
        const ThreeWithOne* other_raw = dynamic_cast<const ThreeWithOne*>(other.get());
        assert(other_raw);
        return card_less_than(compareCard_, other_raw->compareCard_);
    }

protected:
    PlayingCards compareCard_;
};

//三带二
class ThreeWithTwo : public ThreeWithOne {
public:
    ThreeWithTwo(std::set<PlayingCards> cards, PlayingCards compareCard) : ThreeWithOne(cards, compareCard) {}

    virtual bool greaterThan(const Cards::Pointer other) const {
        const ThreeWithTwo* other_raw = dynamic_cast<const ThreeWithTwo*>(other.get());
        assert(other_raw);
        return card_greater_than(compareCard_, other_raw->compareCard_);
    }

    virtual bool lessThan(const Cards::Pointer other) const {
        const  ThreeWithTwo* other_raw = dynamic_cast<const ThreeWithTwo*>(other.get());
        assert(other_raw);
        return card_less_than(compareCard_, other_raw->compareCard_);
    }
};

//顺子
class Straight : public Cards {
public:
    Straight(std::set<PlayingCards> cards) : Cards(cards) {}
};

//双顺子
class DoubleStraight : public Straight {
public:
    DoubleStraight(std::set<PlayingCards> cards) : Straight(cards) {}
};

//三顺子
class TripleStraight : public Straight {
public:
    TripleStraight(std::set<PlayingCards> cards) : Straight(cards) {}

};

struct Airplane {
    int main_min_card;
    int airplane_num;
    int extra_num;
};

//飞机带翅膀
class AirplaneWithWings : public Cards {
public:
    AirplaneWithWings(std::set<PlayingCards> cards, Airplane airplane) :
        Cards(std::move(cards)),airplane_(std::move(airplane)) {
    }

    virtual bool greaterThan(const Cards::Pointer other) const {
        const AirplaneWithWings* other_raw = dynamic_cast<const AirplaneWithWings*>(other.get());
        return airplane_.extra_num > other_raw->airplane_.extra_num;
    }
    virtual bool lessThan(const Cards::Pointer other) const {
        const AirplaneWithWings* other_raw = dynamic_cast<const AirplaneWithWings*>(other.get());
        return airplane_.extra_num < other_raw->airplane_.extra_num;
    }
private:
    Airplane airplane_;
};

//四带二
class FourWithTwo : public Cards {
public:
    //    virtual bool greaterThan(const Cards::Pointer cards) const { return false; }
    //    virtual bool lessThan(const Cards::Pointer cards) const { return false; }
    //    virtual std::size_t size() const { return 0; }
};

class WrongCards : public Cards{
public:
    WrongCards(std::set<PlayingCards> cards) : Cards(cards) {}
};

}//namespace
#endif // CARDS_H

//火箭：即双王（大王和小王），最大的牌。
//炸弹：四张同数值牌（如四个 7 ）。
//单牌：单个牌（如红桃 5 ）。
//对牌：数值相同的两张牌（如梅花 4+ 方块 4 ）。
//三张牌：数值相同的三张牌（如三个 J ）。
//三带一：数值相同的三张牌 + 一张单牌或一对牌。例如： 333+6 或 444+99
//单顺：五张或更多的连续单牌（如： 45678 或 78910JQK ）。不包括 2 点和双王。
//双顺：三对或更多的连续对牌（如： 334455 、7788991010JJ ）。不包括 2 点和双王。
//三顺：二个或更多的连续三张牌（如： 333444 、 555666777888 ）。不包括 2 点和双王。
//飞机带翅膀：三顺＋同数量的单牌（或同数量的对牌）。
//如： 444555+79 或 333444555+7799JJ

//四带二是个什么鬼？？没玩过
//四带二：四张牌＋两手牌。（注意：四带二不是炸弹）。
//如： 5555 ＋ 3 ＋ 8 或 4444 ＋ 55 ＋ 77 。

//经过一番百度，我放弃了用英文表示斗地主术语的念头
//enum CardsType {
//    TYPE_ZHADAN,   //炸弹
//    TYPE_DANPAI,   //单牌
//    TYPE_DUIPAI,    //对牌
//    TYPE_3ZHANGPAI, //三张牌
//    TYPE_3DAI1,      //3带1
//    TYPE_3DAI2,
//    TYPE_DANSHUN,   //单顺
//    TYPE_SHUANGSHUN, //双顺
//    TYPE_3SHUN,     //三顺
//    TYPE_FEIJIDAICHIBANG_DANPAI, //飞机带翅膀-单牌
//    TYPE_FEIJIDAICHIBANG_DUIPAI  //飞机带翅膀-对牌
//};

