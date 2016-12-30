#include "tcp_server.h"

#include <functional>
#include <utility>

#include "utils.h"
#include "../logging/logger.h"

namespace doudizhu {

TcpServer::TcpServer(io_service &ioservice, size_t port) :
    acceptor_(ioservice, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    gameManager_(std::make_shared<GameManager>())
{
    startAccept();
}


void TcpServer::startAccept() {
    TcpConnection::Pointer conn = TcpConnection::create(acceptor_.get_io_service());
    acceptor_.async_accept(conn->socket(), std::bind(
                               &TcpServer::handleAccept, this, conn, std::placeholders::_1));
}

void TcpServer::handleAccept(TcpConnection::Pointer conn,  const boost::system::error_code &ec) {
    if (!ec) {
        LOG_INFO<<"新连接来了";
        gameManager_->addPlayer(conn); //gameManager_里面有个成员 持有conn的引用计数，不会因此而析构
        conn->setCloseCallback(
                    std::bind(&TcpServer::handleClose, this, std::placeholders::_1));
        conn->setReadCallback(
                    std::bind(&TcpServer::handleRead, this,  std::placeholders::_1,  std::placeholders::_2));
        conn->start();
        gameManager_->processClientMsg(conn, "login gaochunyi gaochunji");
        gameManager_->processClientMsg(conn, "game ready");
        LOG_INFO<<"玩家自动登陆成功";
    } else {
        utils::log(ec.message());
    }
    startAccept();
}

void TcpServer::handleClose(TcpConnection::Pointer conn) {
    gameManager_->removePlayer(conn); //从manager中移除conn，conn将会析构
}

void sendFile(TcpConnection::Pointer conn, int fd){
    char buf[1024];
    int n = 0;
    while (1) {
        n = read(fd, buf, sizeof(buf));
        if(n <= 0)break;
        std::string msg(buf, n);
        conn->send(msg);
    }
}

void TcpServer::handleRead(TcpConnection::Pointer conn, std::size_t bytes_transfered) {
    std::string recv(conn->buf().data(), bytes_transfered);
    auto msgs = utils::split(recv, "\r\n");
    //这里并不能很好的处理粘包
    for (auto& msg : msgs) {
        gameManager_->processClientMsg(conn, msg);
    }
}


} //namespace
