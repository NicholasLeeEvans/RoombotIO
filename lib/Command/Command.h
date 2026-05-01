#ifndef COMMAND
#define COMMAND

struct Command {
    enum Type { STRAIGHT, ARC_TURN, SET_RPM, STOP, STATUS, SCAN};
    Type type;

    union {
        struct {int distance; } straight;
        struct {int angle; int radius; } arc_turn;
        struct {int rpm; } set_rpm;

    } params;
};




#endif