#ifndef GAME_STATE_INC
#define GAME_STATE_INC
namespace doudizhu {
enum GameStates {
    STATE_WAITING_FOR_PLAYERS_READY, //等所有人准备
    STATE_ROB_THE_LANDLORD,  //抢地主
    STATE_PLAY_CARDS //出牌阶段
};
} // namespace
#endif // GAME_STATE_INC

