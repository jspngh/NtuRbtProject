#ifndef M_MESSAGE
#define M_MESSAGE

enum BehaviourState
{
    LOSING          = 0b00000001,
    WINNING         = 0b00000010,
    BUSY_MOVE       = 0b00000100,
    WAITING_VISION  = 0b00001000,
    WAITING_USER    = 0b00010000,
    BOARD_PROC_ERR  = 0b00100000,
    INVITE          = 0b01000000,
    COMPLIMENT      = 0b10000000,
};

enum VoiceCommand
{
    HELLO = 0,
    HOW_ARE_YOU = 1,
    COMPLIMENTING = 2,
    INSULT = 3,
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
