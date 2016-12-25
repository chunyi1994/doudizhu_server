#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <unistd.h>
#include <map>
#include <functional>

#include "tcp_connection.h"
#include "game_manager.h"
namespace doudizhu {


class TcpServer  //nocopyable
{
public:
    typedef std::function<void(TcpConnection::Pointer)> ConnectionCallback;

public:
    TcpServer(io_service& ioservice, std::size_t port);

private:
    void startAccept();
    void handleAccept(TcpConnection::Pointer conn,  const boost::system::error_code &ec);
    void handleClose(TcpConnection::Pointer conn);
    void handleRead(TcpConnection::Pointer conn, std::size_t bytes_transfered);

    //void processMsg(const std::string& msg);
    void selectRoom(TcpConnection::Pointer conn, const std::string& msg);

private:
    TcpServer(const TcpServer&) = delete;
    TcpServer(TcpServer&&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

private:
    boost::asio::ip::tcp::acceptor acceptor_;
    std::shared_ptr<GameManager> gameManager_;
};


} // namespace



#endif // TCPSERVER_H



