TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS +=  -Wno-unused-parameter
QMAKE_CXXFLAGS +=  -Wno-unused-function

LIBS += -lpthread
LIBS += -lboost_coroutine
LIBS += -lboost_context
LIBS += -lboost_filesystem
LIBS += -lboost_thread
LIBS += -lboost_system



SOURCES += main.cpp \
    tcp_connection.cpp \
    tcp_server.cpp \
    game_manager.cpp \
    player.cpp \
    cards_factory.cpp \
    game_room.cpp

HEADERS += \
    utils.h \
    tcp_connection.h \
    tcp_server.h \
    playing_cards.h \
    simple_test.h \
    game_manager.h \
    game_states.h \
    player.h \
    cards_factory.h \
    database.h \
    game_room.h \
    cards_algo.h \
    unittest.h

