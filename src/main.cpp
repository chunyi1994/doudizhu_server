#include <iostream>
#include "simple_test.h"
#include "tcp_server.h"
#include "../logging/logger.h"
#include "../logging/log_recorder.h"
//#include <boost/coroutine/all.hpp>

using namespace std;
using namespace doudizhu;


int main()
{
    test_string2int();
    test_isDigit();
    test_cards();
    test_cards_classes();
    logging::Logger::set_output(logging::LogRecorder::callback());
    boost::asio::io_service ioservice;
    doudizhu::TcpServer server(ioservice, 23333);
    ioservice.run();



    cout << "Hello World!" << endl;
    return 0;
}

