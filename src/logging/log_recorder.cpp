#include "log_recorder.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h> //for mkdir
#include <thread>
#include "singleton.h"
namespace logging{

const char* LOG_PATH_FOLDER = "log/";

LogRecorder::LogRecorder():
    queue_(),
    quit_(false)
{
    int isCreate = mkdir(LOG_PATH_FOLDER ,
                         S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    std::thread t([this](){
        while (!quit_) {
            std::string log_msg = queue_.pop();
            std::string path = LOG_PATH_FOLDER;
            path += "log.txt";
            std::ofstream ofs(path, std::ios::app);
            if (ofs) {
                std::cout<<log_msg<<std::endl;
                ofs << log_msg << std::endl;
            } else {
                std::cout<<"写入失败"<<std::endl;
            }
        }//while
    });
    t.detach();
}

void LogRecorder::submit(std::string msg) {
    queue_.push(msg);
}

void LogRecorder::stop() {
    quit_ = true;
}

Logger::OutputCallback LogRecorder::callback() {
    return [](std::string msg){
        Singleton<LogRecorder>::get_instance().submit(msg);
    };
}

}

