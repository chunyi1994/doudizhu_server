#ifndef DOUDIZHU_GAME_H
#define DOUDIZHU_GAME_H
#include <set>
#include <vector>
#include <memory>
#include <queue>
#include "tcp_connection.h"
#include "playing_cards.h"
#include "game_states.h"
#include "player.h"
namespace doudizhu {

class GameRoom
{
public:
    explicit GameRoom(int id);
    std::size_t roomPlayersNum() const; //判断是否满人
    bool full() const;
    void removePlayer(Player::Pointer);
    void addPlayer(Player::Pointer);
    void processGameMsg(Player::Pointer, const std::string& msg);

private:
    //处理客户端包
    void handleCmdReady(Player::Pointer);
    void handleCmdRob(Player::Pointer player,  std::vector<std::string>&, bool rob); // rob真代表抢，否则不抢
    void handleCmdShowCards(Player::Pointer);
    void handleCmdPlay(Player::Pointer, std::vector<std::string>&);
    void handleCmdNoPlay(Player::Pointer);
    void handleCmdChat(Player::Pointer, std::vector<std::string>& msg);

    //游戏逻辑处理
    void shuffleAndToDeal();     //洗牌，发牌
    void startPlayCards();          //开始出牌
    void startRobLandlord();     //开始抢地主
    void setupRobOrder();        //设置好抢地主顺序
    void giveLandlordTheRestOfCards();
    void playerRob(std::vector<std::string>& results, Player::Pointer player);
    void playerNoRob(Player::Pointer player);
    int nextPlayerRoomId() const;
    void clear();     //清空状态
    void clearCards();

    std::string playerCardString(int num);//把所有牌从int类型翻译成中文的string
    std::string cardString(std::set<PlayingCards>& cards);

    //游戏逻辑bool判断
    bool allPlayersAreReady() const;
    bool isPlayCardsEnumNum(int n) const;   //判断数字是否在PlayCards这个enum的范围内
    bool isWrongCards(const Cards::Pointer) const;
    bool isHisCards(Player::Pointer player, const Cards::Pointer) const;

    //发送包给客户端
    void broadcast(const std::string& msg, int pos = -1); //向所有人发送消息,但是跳过下标为pos的那个用户
    void sendToClientsSomeoneIsReady(Player::Pointer player);
    void sendToClientsSomeoneWin(Player::Pointer player);
    void sendToClientPleasePlay(Player::Pointer player);
    void sendToClientIfRobTheLandlord(Player::Pointer player);
    void sendToLandlordClientExtraCards();
    void sendCardsToClients();
    void sendToClientsSomeoneCome(Player::Pointer player);
    void sendToClientsStartPlaying();
    void sendTheRestOfCardsToClients(); //把地主明牌给所有人看
    void sendChineseNameCardToClient(Player::Pointer player);
    void sendToClientsSomeoneRobLandlord(Player::Pointer player, const std::string& power);
    void sendToClientsSomeoneNoRob(Player::Pointer player);
    void sendToClientsSomeonePlay(Player::Pointer player, const Cards::Pointer);

private:
    int id_;
    GameStates gameState_;
    std::vector<std::set<PlayingCards> > playersCards_;
    std::vector<Player::Pointer> players_;
    std::vector<PlayingCards> landlordExtraCards_;
    std::queue<int> robTheLandlordOrder_; //抢地主的顺序
    int readyPlayersNum_;           //当等于全部人数，则说明全部准备了
    int landlordRoomId_;             //地主在房间的id数字
    int turnPlayRoomId_;             //轮到谁出牌了
    int lastCardPlayerRoomId_;  //上一次牌是谁出的
    int multiplyingPower_;          //倍率
    int whoFirstRobLandlord_;   //抢地主
    Cards::Pointer lastCards_;     //上一次打了什么牌
};

const int MAX_PLAYERS_NUM = 3;   //最大游戏参与者人数
const int MAX_ROOMS_NUM = 10;  //最多的房间容量

const int NO_LANDLORD = -1;
const int NO_ONE = -1;
const int MAX_MULTIPLYING_POWER = 3;
}//namespace
#endif // DOUDIZHU_GAME_H
