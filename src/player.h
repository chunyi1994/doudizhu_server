#ifndef PLAYER_H
#define PLAYER_H
#include <utility>
#include "tcp_connection.h"

namespace doudizhu {

class Player {
public:
    typedef std::shared_ptr<Player> Pointer;

public:
    explicit Player(TcpConnection::Pointer);

    int roomID() const { return roomID_; }
    void setRoomID(int id) { roomID_ = id; }
    std::string username() const { return username_; }
    void setUsername(std::string name) { username_ = std::move(name); }
    bool isLogin() const { return isLogin_; }
    void setLogin(bool login) { isLogin_ = login; }
    int playerNumInRoom() const { return playerNumInRoom_; }
    void setPlayerNumInRoom(int num) { playerNumInRoom_ = num; }

    void send(const std::string& msg);

private:
    TcpConnection::WeakPointer conn_;
    int roomID_;
    std::string username_;
    bool isLogin_;
    int playerNumInRoom_;
};


const int NO_ROOM_ID = -1; //表示没有房间

}//namespace
#endif // PLAYER_H
