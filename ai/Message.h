#ifndef M_MESSAGE
#define M_MESSAGE

enum BehaviourState
{
    INVITE,
    WINNING,
    WON,
    LOSING,
    LOST,
    BUSY_MOVE,
    MOVE_DONE,
    MOVE_BEGIN,
    WAITING_VISION,
    WAITING_USER,
    BOARD_PROC_ERR,
    COMPLIMENT,
    INSULT,
};

enum VoiceCommand
{
    HELLO = 0,
    HOW_ARE_YOU = 1,
    COMPLIMENTING = 2,
    INSULTING = 3,
    TOO_EASY = 4,
    TOO_HARD = 5,
};

struct Message
{
    int type; // 0 for VoiceCommand, 1 for BehaviourState
    union
    {
        VoiceCommand c;
        BehaviourState s;
    };
};

#endif
