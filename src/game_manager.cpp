#include "game_manager.h"
#include <utility>
#include "utils.h"
#include "game_room.h"
#include "database.h"
namespace doudizhu {
GameManager::GameManager() :
    players_(),
    rooms_()
{
    rooms_.reserve(MAX_ROOMS_NUM);
    for(int i = 0; i < MAX_ROOMS_NUM; i++) {
        GameRoom game(i);
        rooms_.push_back(game);
    }
}

void GameManager::addPlayer(TcpConnection::Pointer conn) {
    players_[conn] = std::make_shared<Player>(conn);
}

void GameManager::removePlayer(TcpConnection::Pointer conn) {
    auto iter = players_.find(conn);
    assert(iter != players_.end());
    removePlayerFromHisRoom(iter->second);
    players_.erase(iter);
}

//分发，处理客户端传来的各种信息
void GameManager::processClientMsg(TcpConnection::Pointer conn, const std::string &msg) {
    Player::Pointer player = players_[conn];
    if (utils::hasPrefix(msg, "ping")) { //心跳
        ping(player);
        return;
    }
    if (utils::hasPrefix(msg, "login")) { //登陆
        login(player, msg);
        return;
    }
    if (utils::hasPrefix(msg, "register")) { //登陆
        registerAccount(player, msg);
        return;
    }
    if (!player->isLogin()) {
        utils::log("没有登陆");
        return;
    }
    if (utils::hasPrefix(msg, "selectroom")) { //切换房间
        selectRoom(player, msg);
        return;
    }
    int room_id = players_[conn]->roomID();
    if (room_id == NO_ROOM_ID) {
        //如果这个用户没有加入房间，就忽略请求，
        //因为以下函数都需要用户进入一个房间
        utils::log("忽略一个没有房间号的用户的请求");
        return;
    }
    //游戏请求 分发到对应房间的DoudizhuGame这个类处理
    if (utils::hasPrefix(msg, "game")) {
        //游戏
        rooms_[room_id].processGameMsg(player, msg);
        return;
    }
}

void GameManager::removePlayerFromHisRoom(Player::Pointer player) {
    int old_room_id = player->roomID();
    if (old_room_id == NO_ROOM_ID) {
        return;
    }
    rooms_[old_room_id].removePlayer(player); //从老房间移走
    player->setRoomID(NO_ROOM_ID);
    utils::log("用户"+player->username()+"离开了" + utils::int2string(old_room_id) + "房间");
}

void GameManager::selectRoom(Player::Pointer player, const std::string &msg) {
    auto results = utils::split(msg, " ");
    if (results.size() < 2) {
        utils::log("selectroom错误的命令各式");
        return;
    }
    std::string room_num = results[1];
    if (!utils::isDigit(room_num)) {
        utils::log("selectroom错误的类型");
        return;
    }
    int room_int = utils::string2int(room_num);
    if (room_int >= MAX_ROOMS_NUM) {
        utils::log("selectroom错误的数字大小");
        return;
    }
   if (!changeRoom(player, room_int)) {
       player->send("selectroom error\r\n");
   } else {
       player->send("selectroom ok\r\n");
   }
}

bool GameManager::changeRoom(Player::Pointer player, int roomNum) {
    if (rooms_[roomNum].full()) {
        utils::log("selectroom 选择的房间满了");
        return false;
    }
    removePlayerFromHisRoom(player);  //把他从老房间移除
    rooms_[roomNum].addPlayer(player); //把他移进新的房间
    player->setRoomID(roomNum);
    std::string logmsg = "用户"+ player->username() +"来到了" + utils::int2string(roomNum) + "房间";
    utils::log(logmsg);
    return true;
}

void GameManager::login(Player::Pointer player, const std::string &msg) {
    auto results = utils::split(msg, " ");
    if (results.size() < 3) {
        utils::log("login错误的命令各式:少了字段");
        return;
    }
    std::string username = results[1];
    std::string password = results[2];
    if (!login_aux(username, password)) {
        player->send("login error\r\n");
        utils::log("登录失败");
        return;
    }
    player->setUsername(username);
    player->setLogin(true);
    player->send("login ok\r\n");
    utils::log("用户" + username + "已经登陆服务器");

    for (int i = 0; i < MAX_ROOMS_NUM; i++) {
        if (changeRoom(player, i)) {
            player->send("selectroom ok\r\n");
            return;
        }
    }
}

void GameManager::ping(Player::Pointer player) {
    player->send("pong\r\n");
}

void GameManager::registerAccount(Player::Pointer player,  const std::string &msg)
{
    auto results = utils::split(msg, " ");
    if (results.size() < 3) {
        utils::log("register错误的命令各式:少了字段");
        return;
    }
    std::string username = results[1];
    std::string password = results[2];
    if (!register_aux(username, password)) {
        player->send("register error\r\n");
        utils::log("注册失败");
        return;
    } else {
        utils::log("注册成功");
        player->send("register ok\r\n");
    }
}


}//namespace


