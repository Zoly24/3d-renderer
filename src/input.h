#ifndef INPUT_H
#define INPUT_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct UserInput {
    float cursorx;
    float cursory;

    uint8_t mouse_state[5];

    const _Bool *keyboard_state;
} UserInput;

UserInput *create_user_input(void);
SDL_Event get_user_input(UserInput *);

#endif
