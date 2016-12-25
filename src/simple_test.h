#ifndef SIMPLE_TEST_H
#define SIMPLE_TEST_H
#include <iostream>
#include <memory>
#include "unittest.h"
#include <set>
#include <vector>
//#include "doudizhu_game.h"
#include "playing_cards.h"
#include "tcp_connection.h"
#include "cards_algo.h"
#include "utils.h"
namespace doudizhu{

void test_cards_classes(){
    utils::log("test_cards_classes");
    std::set<PlayingCards> set1 = {CLUBS_3,DIAMONDS_3};
    std::set<PlayingCards> set2 = {CLUBS_4,DIAMONDS_4};
    std::set<PlayingCards> set3 = {CLUBS_3};
    std::set<PlayingCards> set4 = {CLUBS_4};

    Cards::Pointer card1 =  std::make_shared<OnePair>(set1);
    Cards::Pointer card2 =  std::make_shared<OnePair>(set2);
    Cards::Pointer card3 =  std::make_shared<SingleCard>(set3);
    Cards::Pointer card4 =  std::make_shared<SingleCard>(set4);

    EXCEPT_TRUE(card1->isTheSameType(card2));
    EXCEPT_TRUE(card3->isTheSameType(card4));
    EXCEPT_FALSE(card3->isTheSameType(card1));

    EXCEPT_TRUE(card2->greaterThan(card1));
    //EXCEPT_TRUE(card3->isTheSameType(card4));
    EXCEPT_FALSE(card3->greaterThan(card1));
}


void test_cards() {
    utils::log("test_cards");
    std::set<PlayingCards> set1 = {CLUBS_3,DIAMONDS_3,HEARTS_3,SPADES_3};
    EXCEPT_TRUE(isBoom(set1));
    EXCEPT_FALSE(isSingleCard(set1));

    set1 = {CLUBS_3,DIAMONDS_3,HEARTS_3,SPADES_4};
    EXCEPT_FALSE(isBoom(set1));

    set1 = {JOKER_KING_SMALL, JOKER_KING_BIG};
    EXCEPT_TRUE(isBoom(set1));

    set1 = {JOKER_KING_SMALL};
    EXCEPT_TRUE(isSingleCard(set1));

    set1 = {CLUBS_3,DIAMONDS_3,HEARTS_3,HEARTS_4};
    EXCEPT_TRUE(isThreeWithOne(set1) != -1);

    set1 = {CLUBS_3,DIAMONDS_3,HEARTS_3,HEARTS_4, DIAMONDS_4};
    EXCEPT_TRUE(isThreeWithTwo(set1) != -1);

    set1 = {CLUBS_3,DIAMONDS_4,HEARTS_5, CLUBS_6,DIAMONDS_7};
    EXCEPT_TRUE(isStraight(set1));

    set1 = {CLUBS_3,DIAMONDS_3,HEARTS_3,CLUBS_4,DIAMONDS_4,HEARTS_4};
    EXCEPT_TRUE(isTripleStraight(set1));

    set1 = {CLUBS_3,DIAMONDS_3,HEARTS_3,CLUBS_4,DIAMONDS_4,HEARTS_7};
    EXCEPT_FALSE(isTripleStraight(set1));


    //3 3 3 4 4 4 5 6              8个
    //3 3 3 4 4 4 5 5 6 6       10个
    //3 3 3 4 4 4 5 5 5 6 7 8    12个
    //3 3 3 4 4 4 5 5 5  33 66 88  15个
    //3 3 3 4 4 4 5 5 5 6 6 6 7 8 9 10   16个
    Airplane airplane;
    set1 = {CLUBS_3,DIAMONDS_3,HEARTS_3,CLUBS_4,DIAMONDS_4,HEARTS_4, HEARTS_5, HEARTS_6};
    EXCEPT_TRUE(isAirplaneWithWings(set1, airplane));

    set1 = {CLUBS_3,DIAMONDS_3,HEARTS_3,CLUBS_4,DIAMONDS_4,
            HEARTS_4, HEARTS_5, DIAMONDS_5,HEARTS_6, DIAMONDS_6};  //333 444 55 66
    EXCEPT_TRUE(isAirplaneWithWings(set1, airplane));

    set1 = {DIAMONDS_Q,
            HEARTS_Q,
            SPADES_Q,
            CLUBS_K,
            HEARTS_K,
            SPADES_K, CLUBS_3, DIAMONDS_3,CLUBS_4, CLUBS_5};  //QQQ KKK 33 45
    EXCEPT_FALSE(isAirplaneWithWings(set1, airplane));

    set1 = { (PlayingCards)0,(PlayingCards)1, (PlayingCards)4, (PlayingCards)8 ,(PlayingCards)37,
             (PlayingCards)38, (PlayingCards)39, (PlayingCards)40,(PlayingCards) 42,(PlayingCards) 43};  //QQQ KKK 33 45
    EXCEPT_FALSE(isAirplaneWithWings(set1, airplane));


    set1 = {CLUBS_3,DIAMONDS_3,HEARTS_3,CLUBS_4,DIAMONDS_4,
            HEARTS_4, CLUBS_5,DIAMONDS_5,HEARTS_5,HEARTS_6, DIAMONDS_7,HEARTS_8};
    EXCEPT_TRUE(isAirplaneWithWings(set1, airplane));

    set1 = {CLUBS_3,DIAMONDS_3,HEARTS_3,CLUBS_4,DIAMONDS_4,
            HEARTS_4, CLUBS_5,DIAMONDS_5,HEARTS_5,
            CLUBS_6,DIAMONDS_6,CLUBS_7,DIAMONDS_7, CLUBS_8,DIAMONDS_8};
    EXCEPT_TRUE(isAirplaneWithWings(set1, airplane));   // 333 444 555 66 77 88

    set1 = {CLUBS_3,DIAMONDS_3,HEARTS_3,CLUBS_4,DIAMONDS_4,
            HEARTS_4, CLUBS_5, HEARTS_5, DIAMONDS_5,HEARTS_6, DIAMONDS_6, CLUBS_6,
           DIAMONDS_7,HEARTS_8,DIAMONDS_9,HEARTS_10};
    EXCEPT_TRUE(isAirplaneWithWings(set1, airplane));
}

void test_string2int(){
    EXCEPT_EQ(utils::string2int(std::string("0")), 0);
    EXCEPT_EQ(utils::string2int(std::string("1")), 1);
    EXCEPT_EQ(utils::string2int(std::string("-1")), -1);
    EXCEPT_EQ(utils::string2int(std::string("no")), 0);
    EXCEPT_EQ(utils::string2int(std::string("")), 0);
}

void test_isDigit(){
    EXCEPT_TRUE(utils::isDigit(std::string("0")));
    EXCEPT_TRUE(utils::isDigit(std::string("1")));
    EXCEPT_TRUE(utils::isDigit(std::string("-1")));
    EXCEPT_FALSE(utils::isDigit(std::string("fdsfsgaa")));
    EXCEPT_FALSE(utils::isDigit(std::string("")));
}

void test_shuffleAndToDeal() {
    std::vector<std::set<PlayingCards> > playersCards_(3);
    std::vector<TcpConnection::Pointer> players_;
    std::vector<PlayingCards> cards(MAX_PLAYING_CARDS_NUM);//前三行建立一个从0到54的vector
    int n = 0;
    std::generate(cards.begin(), cards.end(), [&n]() { return PlayingCards(n++); });
    std::random_shuffle(cards.begin(), cards.end()); //打乱顺序
    int cards_num_per_player = MAX_PLAYING_CARDS_NUM / 3 - 1;
    auto iter_begin = cards.begin();
    auto iter_end = iter_begin;
    for (auto &item : playersCards_) { //把vector分成三段分别填充到三位玩家中
        iter_end = iter_begin + cards_num_per_player;
        item.insert(iter_begin, iter_end);
        iter_begin = iter_end;
    }
    for (auto &item : playersCards_) { //遍历，肉眼查看分牌结果
        std::copy(item.begin(), item.end(), std::ostream_iterator<PlayingCards>(std::cout, " "));
        std::cout<<std::endl;
    }
}
}//namespace
#endif // SIMPLE_TEST_H

