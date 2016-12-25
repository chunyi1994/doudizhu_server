#include "game_room.h"

#include <algorithm>
#include <iterator>
#include "utils.h"
#include "game_manager.h"
#include "playing_cards.h"
#include "cards_factory.h"
#include "cards_algo.h"
namespace doudizhu {

GameRoom::GameRoom(int id) :
    id_(id),
    gameState_(STATE_WAITING_FOR_PLAYERS_READY),
    playersCards_(MAX_PLAYERS_NUM),
    players_(),
    landlordExtraCards_(MAX_PLAYERS_NUM),
    robTheLandlordOrder_(),
    readyPlayersNum_(0),
    landlordRoomId_(NO_LANDLORD),
    turnPlayRoomId_(NO_ONE),
    lastCardPlayerRoomId_(NO_ONE),
    multiplyingPower_(0),
    whoFirstRobLandlord_(0),
    lastCards_(nullptr)
{}

//处理各种开头为game的命令
void GameRoom::processGameMsg(Player::Pointer player, const std::string &msg) {
    std::vector<std::string> results = utils::split(msg, " ");
    if (results.size() < 2) return;
    std::string cmd = results[1];

    if (cmd == "ready") {
        handleCmdReady(player);
    } else if (cmd == "rob") {
        handleCmdRob(player, results, true); //true代表抢
    } else if (cmd == "norob") {
        handleCmdRob(player, results, false); //false代表不抢
    } else if (cmd == "showcards") {
        handleCmdShowCards(player);
    } else if (cmd == "play") {
        handleCmdPlay(player, results);
    } else if (cmd == "noplay") {
        handleCmdNoPlay(player);
    } else if (cmd == "chat") {
        handleCmdChat(player, results);
    }
}

void GameRoom::handleCmdReady(Player::Pointer player) {
    if (gameState_ != STATE_WAITING_FOR_PLAYERS_READY) {
        return;
    }
    readyPlayersNum_++;
    sendToClientsSomeoneIsReady(player);

    if (allPlayersAreReady()) {
        startRobLandlord();
    }
}

void GameRoom::handleCmdShowCards(Player::Pointer player) {
    sendChineseNameCardToClient(player);
}

void GameRoom::handleCmdPlay(Player::Pointer player, std::vector<std::string> &results) {
    if (gameState_ != STATE_PLAY_CARDS) {
        utils::log("play:不是出牌的时候");
        return;
    }
    if (results.size() <= 2) {
        utils::log("play:参数个数错误");
        return;
    }
    if(player->playerNumInRoom() != turnPlayRoomId_) {
        utils::log("play:还没轮到他出牌");
        return;
    }
    std::set<PlayingCards> playCards;
    for(int i = 2; i < (int)results.size(); i++) {
        if (!utils::isDigit(results[i])) {
            utils::log("play:参数错误1：参数应该为数字");
            return;
        }
        int card_num = utils::string2int(results[i]);
        if (!isPlayCardsEnumNum(card_num)) {
            utils::log("play:参数错误2：参数数字不能代表牌型");
            return;
        }
        playCards.insert((PlayingCards)card_num);
    }
    Cards::Pointer cards = CardsFactory::create(playCards);
    if (isWrongCards(cards)) {
        utils::log("play:错误的牌：斗地主没有这样的牌型");
        return;
    }
    //判断出牌是否是自己手牌里有的，防止造假
    if (!isHisCards(player, cards)) {
        utils::log("play:出了自己没有的牌");
        return;
    }
    //如果是压别人的牌，需要判断牌型和大小
    if (lastCards_) {
        if (!lastCards_->isTheSameType(cards) && !isBoom(playCards)) {
            utils::log("play:出牌类型和上家不一样");
            return;
        }
        if (!cards->greaterThan(lastCards_)) {
            utils::log("play:出了更小的牌");
            return;
        }
    }
    int num = player->playerNumInRoom();
    lastCards_ = cards;
    lastCardPlayerRoomId_ = num;
    for (auto card : cards->cards()) {  //从手牌中清除
        playersCards_[num].erase(card);
    }
    sendToClientsSomeonePlay(player, cards);
    if (playersCards_[num].empty()) {
        //没手牌，赢了
        sendToClientsSomeoneWin(player);
        clear();
    } else { //让下家出牌
        turnPlayRoomId_ = nextPlayerRoomId();
        sendToClientPleasePlay(players_[turnPlayRoomId_]);
    }
}

void GameRoom::handleCmdNoPlay(Player::Pointer player) {
    if (gameState_ != STATE_PLAY_CARDS) {
        utils::log("play:不是出牌的时候");
        return;
    }
    if (player->playerNumInRoom() != turnPlayRoomId_) {
        utils::log("play:还没轮到他出牌");
        return;
    }
    utils::log("1现在轮到：" + utils::int2string(turnPlayRoomId_));
    turnPlayRoomId_ = nextPlayerRoomId();
    utils::log("1现在改成：" + utils::int2string(turnPlayRoomId_));
    if (turnPlayRoomId_ == lastCardPlayerRoomId_) lastCards_ = nullptr;
    sendToClientPleasePlay(players_[turnPlayRoomId_]);
}

void GameRoom::handleCmdChat(Player::Pointer player, std::vector<std::string> &results) {
    if (results.size() != 3) {
        utils::log("chat 参数错误");
        return;
    }
    std::string msg = results[2];
    //doudizhu::trimQuo(msg);
    broadcast(player->username() + ":" + msg + "\r\n");
}

//洗牌发牌
void GameRoom::shuffleAndToDeal() {
    //前三行建立一个从0到52的vector，还差一个大王没插入
    std::vector<PlayingCards> cards(MAX_PLAYING_CARDS_NUM - 1);
    int n = 0;
    std::generate(cards.begin(), cards.end(), [&n]() { return PlayingCards(n++); });
    cards.push_back(JOKER_KING_BIG);
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    std::random_shuffle(cards.begin(), cards.end()); //打乱顺序
    int cards_num_per_player = MAX_PLAYING_CARDS_NUM / MAX_PLAYERS_NUM - 1;
    auto iter_begin = cards.begin();
    auto iter_end = iter_begin;
    for (auto &item : playersCards_) {
        //把vector分成三段分别填充到三位玩家手牌中
        item.clear();
        iter_end = iter_begin + cards_num_per_player;
        item.insert(iter_begin, iter_end);
        iter_begin = iter_end;
    }
    //把没发完的牌存在地主额外牌里
    std::copy(cards.end() - MAX_PLAYERS_NUM, cards.end(),
              landlordExtraCards_.begin());
}

void GameRoom::handleCmdRob(
        Player::Pointer player,
        std::vector<std::string>& results,
        bool rob) {// rob真代表抢，否则不抢
    if (gameState_ != STATE_ROB_THE_LANDLORD) {
        return;
    }
    if (robTheLandlordOrder_.front() != player->playerNumInRoom()) {
        return; //如果抢地主的顺序还没轮到他，就退出
    }
    if (rob) {
        //抢地主
        playerRob(results, player);
    } else {
        playerNoRob(player);
    }
}

void GameRoom::playerRob(std::vector<std::string>& results, Player::Pointer player) {
    if (results.size() != 3) {
        utils::log("抢地主参数错误");
        return;
    }
    //倍数
    int multiplying_power_int = 0;
    //string不是数字默认是0
    multiplying_power_int = utils::string2int(results[2]);
    //申请的倍数不可以比别人压的更小
    if (multiplying_power_int < multiplyingPower_) {
        utils::log("抢地主倍数错误");
        return;
    }
    sendToClientsSomeoneRobLandlord(player, results[2]);
    //记录新地主
    landlordRoomId_ = player->playerNumInRoom();
    //记录新倍率
    multiplyingPower_ = multiplying_power_int;
    //如果有一个人压了3倍，或者他是最后一个压的，都是他当地主
    if (MAX_MULTIPLYING_POWER == multiplying_power_int ||
            robTheLandlordOrder_.size() == 1) {
        robTheLandlordOrder_ = std::queue<int>();
        giveLandlordTheRestOfCards(); //把剩余牌发给地主
        sendTheRestOfCardsToClients();  //公示地主额外牌
        startPlayCards(); //开始出牌
    } else { //继续抢地主
        robTheLandlordOrder_.pop();
        robTheLandlordOrder_.push(player->playerNumInRoom());
        auto nextPlayer = players_[robTheLandlordOrder_.front()];
        sendToClientIfRobTheLandlord(nextPlayer);
    }
}

void GameRoom::playerNoRob(Player::Pointer player){
    robTheLandlordOrder_.pop();
    sendToClientsSomeoneNoRob(player);
    if (robTheLandlordOrder_.empty()) {
        //如果没有人抢地主，重新发牌
        startRobLandlord();
    } else {
        if(robTheLandlordOrder_.front() == landlordRoomId_) {
            giveLandlordTheRestOfCards(); //把剩余牌发给地主
            sendTheRestOfCardsToClients();  //公示地主额外牌
            startPlayCards(); //开始出牌
            return;
        }
        auto nextPlayer = players_[robTheLandlordOrder_.front()];
        sendToClientIfRobTheLandlord(nextPlayer);
    }
}

bool GameRoom::isPlayCardsEnumNum(int n) const {
    if ((n >= 0 && n <= 52) || n == 56) {
        return true;
    }
    return false;
}

int GameRoom::nextPlayerRoomId() const {
    int result = turnPlayRoomId_ + 1;
    if (result >= (int)players_.size()) {
        result = 0;
    }
    return result;
}

void GameRoom::removePlayer(Player::Pointer player) {
    clear();
    auto iter = std::find(players_.begin(), players_.end(), player);
    assert(iter != players_.end());
    players_.erase(iter);

    for (int i = 0; i < (int)players_.size(); i++) {
        players_[i]->setPlayerNumInRoom(i);
    }
}

void GameRoom::addPlayer(Player::Pointer player) {
    if (full()) {
        utils::log("addPlayer:超过了本房间最大容纳的人数");
        return;
    }
    player->setPlayerNumInRoom(players_.size());
    sendToClientsSomeoneCome(player);
    players_.push_back(player);
}

void GameRoom::giveLandlordTheRestOfCards() {
    for (auto card : landlordExtraCards_) {
        playersCards_[landlordRoomId_].insert(card);
    }
}

//开始出牌
void GameRoom::startPlayCards() {
    gameState_ = STATE_PLAY_CARDS;
    sendToLandlordClientExtraCards();
    //让地主先出牌
    turnPlayRoomId_ = landlordRoomId_;
    sendToClientPleasePlay(players_[turnPlayRoomId_]);
}

//开始抢地主
void GameRoom::startRobLandlord() {
    gameState_ = STATE_ROB_THE_LANDLORD;
    //服务器分牌
    shuffleAndToDeal();
    sendCardsToClients();
    //把抢地主顺序设定好
    setupRobOrder();
    //询问第一位候选人是否抢地主
    auto firstPlayer = players_[robTheLandlordOrder_.front()];
    //询问是否抢地主
    sendToClientIfRobTheLandlord(firstPlayer);
}

void GameRoom::clear() {
    gameState_ = STATE_WAITING_FOR_PLAYERS_READY;
    landlordRoomId_ = NO_LANDLORD;
    turnPlayRoomId_ = NO_ONE;
    lastCardPlayerRoomId_=NO_ONE;
    multiplyingPower_ = 0;
    whoFirstRobLandlord_ = 0;
    robTheLandlordOrder_ = std::queue<int>();
    landlordExtraCards_.clear();
    readyPlayersNum_ = 0;
    lastCards_ = nullptr;
    clearCards();
}

void GameRoom::setupRobOrder() {
    if (!robTheLandlordOrder_.empty()) {
        robTheLandlordOrder_ = std::queue<int>();
    }
    for (int i = whoFirstRobLandlord_; i <(int)players_.size(); i++) {
        robTheLandlordOrder_.push(players_[i]->playerNumInRoom());
    }
    for (int i = 0; i < whoFirstRobLandlord_; i++) {
        robTheLandlordOrder_.push(players_[i]->playerNumInRoom());
    }
}

//清理牌
void GameRoom::clearCards() {
    for(auto& item : playersCards_) {
        item.clear();
    }
}

bool GameRoom::isWrongCards(const Cards::Pointer cards) const {
    return typeid(*(cards.get())) == typeid(WrongCards);
}

//查看出的牌是不是自己手牌里的，防止外挂作弊
bool GameRoom::isHisCards(Player::Pointer player, const Cards::Pointer cards) const {
    int num = player->playerNumInRoom();
    std::vector<PlayingCards> resultvec(cards->size());
    auto vec_end = std::set_intersection(
                playersCards_[num].begin(), playersCards_[num].end(),
                cards->cards().begin(), cards->cards().end(), resultvec.begin());

    if ((std::size_t)std::distance(resultvec.begin(), vec_end) != cards->size()) {
        return false;
    }
    return true;
}

std::size_t GameRoom::roomPlayersNum() const {
    return players_.size();
}

bool GameRoom::full() const {
    return players_.size() >= MAX_PLAYERS_NUM;
}

std::string GameRoom::playerCardString(int num) {
    return cardString(playersCards_[num]);
}

std::string GameRoom::cardString(std::set<PlayingCards>& cards) {
    std::string send_msg;
    for (auto card : cards) {
        int card_int = card;
        send_msg = send_msg + " " + doudizhu::cardChineseName(card) + "（"+ utils::int2string(card_int)+ "）";
    }

    send_msg += "\r\n-----------------------------------------------\r\n";
    for (auto card : cards) {
        send_msg = send_msg + " " + doudizhu::cardChineseName(card, false);
    }
    send_msg += "\r\n-----------------------------------------------\r\n";
    return send_msg;
}

bool GameRoom::allPlayersAreReady() const {
    return readyPlayersNum_ == MAX_PLAYERS_NUM;
}


void GameRoom::broadcast(const std::string& msg, int pos) {
    for (int i = 0; i < (int)players_.size(); i++) {
        if (i == pos) { //跳过pos的
            continue;
        }
        players_[i]->send(msg);
    }
}

void GameRoom::sendToClientPleasePlay(Player::Pointer player) {
    player->send("请你出牌\r\n");
}

void GameRoom::sendToLandlordClientExtraCards() {
    std::string send_msg = "你的新牌:\r\n" +playerCardString(landlordRoomId_) + "\r\n";
    players_[landlordRoomId_]->send(send_msg);
}

void GameRoom::sendToClientsSomeoneIsReady(Player::Pointer player) {
    std::string broacast_string = "玩家" + player->username()  + "已准备.\r\n";
    broadcast(broacast_string, player->playerNumInRoom());
    player->send("ready ok\r\n");
}

void GameRoom::sendToClientsSomeoneWin(Player::Pointer player) {
    broadcast("玩家" + player->username() + "胜利！\r\n");
}

void GameRoom::sendToClientIfRobTheLandlord(Player::Pointer player) {
    player->send("你是否要抢地主？\r\n");
}

void GameRoom::sendCardsToClients() {
    for (auto& player : players_) {
        int num = player->playerNumInRoom();
        //把set类型的牌转换成人类能看得懂的牌的string类型
        std::string send_msg = playerCardString(num) + "\r\n";
        player->send(send_msg);
    }
}

void GameRoom::sendToClientsStartPlaying() {
    broadcast("所有人都准备好了，开始游戏\r\n");
}

void GameRoom::sendToClientsSomeoneCome(Player::Pointer player) {
    std::string broacast_string = "有一位新玩家" + player->username()  + "入场了.\r\n";
    broadcast(broacast_string, player->playerNumInRoom());
}

void GameRoom::sendTheRestOfCardsToClients() {
    std::string send_msg;
    for (auto card : landlordExtraCards_) {
        int card_int = card;
        send_msg = send_msg + " " + doudizhu::cardChineseName(card) +
                "（"+ utils::int2string(card_int)+ "）";
    }
    broadcast(send_msg + "\r\n");
}

void GameRoom::sendChineseNameCardToClient(Player::Pointer player) {
    std::string sendMsg = playerCardString(player->playerNumInRoom()) + "\r\n";
    player->send(sendMsg);
}

void GameRoom::sendToClientsSomeoneRobLandlord(Player::Pointer player,const std::string& power) {
    broadcast("玩家："+player->username() +"抢地主，倍率:" + power +"\r\n");
}

void GameRoom::sendToClientsSomeoneNoRob(Player::Pointer player) {
    broadcast("玩家："+player->username() +"不抢\r\n");
}

void GameRoom::sendToClientsSomeonePlay(Player::Pointer player, const Cards::Pointer cards) {
    broadcast("玩家" + player->username() + "出的牌：" + cardString(cards->cards()) + "\r\n");
}

} // namespace
