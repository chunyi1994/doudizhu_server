#include "player.h"
#include "utils.h"
#include "../logging/logger.h"
namespace doudizhu {
Player::Player(TcpConnection::Pointer conn) :
    conn_(conn),
    roomID_(NO_ROOM_ID),
    username_("unknow_user"),
    isLogin_(false),
    playerNumInRoom_(-1){

}

void Player::send(const std::string &msg)
{
    auto conn = conn_.lock();
    if (conn) {
        conn->send(msg);
    } else {
        LOG_ERROR<<"player send error";
    }
}

}
