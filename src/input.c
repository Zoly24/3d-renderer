#include "input.h"
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_scancode.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

UserInput *create_user_input() {
    UserInput *input = (UserInput *)malloc(sizeof(UserInput));
    if (!input) {
        printf("Could not allocate input mem");
        return NULL;
    }

    return input;
}
