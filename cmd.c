typedef enum {Nothing = 0, Continue = 1, Exit = 2} State;

struct Command {
    char* str;
    State state;
};