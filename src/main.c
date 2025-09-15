/*
10 Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it freely. */
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <sys/types.h>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "constants.h"
#include "geometry.h"
#include "input.h"
#include "line.h"
#include "model.h"
#include "obj_reader.h"
#include "render_pipeline.h"
#include "triangle.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

float delta_tick;
float last_tick;
float current_tick;

UserCamera *camera;
UserInput *input;

float yaw = -90.0f;
float pitch = 0.0f;
int first_mouse_read = 1;

uint8_t *z_buffer;

Mesh *mesh;
ModelObject *model;

void update_user_input(UserInput *);
void run_program(void);
void test_functions(void);

static SDL_AppResult initialize_rendering_pipeline(void);
static SDL_AppResult initialize_user_input(void);
static SDL_AppResult initialize_camera(void);
static SDL_AppResult initialize_objects(void);

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) { /* Create the window */
    SDL_CreateWindowAndRenderer("Simulation", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE,
                                &window, &renderer);
    SDL_AppResult result = initialize_rendering_pipeline();
    if (result == SDL_APP_FAILURE) {
        return SDL_APP_FAILURE;
    }

    // Get mouse
    SDL_SetWindowMouseGrab(window, 1);
    SDL_SetWindowRelativeMouseMode(window, 1);

    // Initialize current tick
    current_tick = 0;

    // More initialization
    initialize_user_input();
    initialize_camera();

    // Initialize the model
    result = initialize_objects();

    if (result == SDL_APP_FAILURE) {
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

static SDL_AppResult initialize_rendering_pipeline() {
    z_buffer = (uint8_t *)calloc(SCREEN_WIDTH * SCREEN_HEIGHT, sizeof(int));

    if (!z_buffer) {
        printf("Could not allocate z-buffer mem, quitting.");
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

static SDL_AppResult initialize_user_input() {
    // Initialize the user's input
    input = create_user_input();
    if (!input) {
        printf("Could not allocate input mem, quitting");
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

static SDL_AppResult initialize_camera() {
    // Create a new user camera
    camera = create_camera();

    // Position and target
    camera->camera_position = create_fvec4(0.0f, 0.0f, 3.0f, 1.0f);
    camera->camera_front = create_fvec4(0.0f, 0.0f, -1.0f, 1.0f);
    camera->camera_up = create_fvec4(0.0f, 1.0f, 0.0f, 1.0f);

    // Tweaked look_at function
    camera_look_at_front(camera, camera->camera_position, camera->camera_front, camera->camera_up);

    if (!camera) {
        printf("Could not allocate mem for user camera");
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

static SDL_AppResult initialize_objects() {
    mesh = (Mesh *)malloc(sizeof(Mesh));
    if (!mesh) {
        printf("Could not allocate mem for mesh");
        return SDL_APP_FAILURE;
    }

    FILE *file = open_file("/home/zoly/Documents/3d-renderer/assets/Cube/Cube.obj");
    // FILE *file = open_file("/home/zoly/Documents/obj-assets/sword-futuristic/Futuristic_Sword_Upload.obj");
    if (!file) {
        printf("Could not open file");
        return SDL_APP_FAILURE;
    }

    // Create the new mesh
    generate_mesh(file, mesh);
    model = create_model_object(mesh);
    if (!model) {
        printf("Could not allocate mem for model");
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.scancode == SDL_SCANCODE_ESCAPE) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    last_tick = current_tick;
    current_tick = SDL_GetPerformanceCounter();

    delta_tick = (double)((current_tick - last_tick) * 1000 / (double)SDL_GetPerformanceFrequency());

    // Make sure the update what the user has done BEFORE updating screen
    update_user_input(input);
    run_program();

    return SDL_APP_CONTINUE;
}

void update_user_input(UserInput *input) {
    input->keyboard_state = SDL_GetKeyboardState(NULL);
    uint32_t mouse_button;

    // Get the mouse button pressed
    if (first_mouse_read) {
        mouse_button = SDL_GetMouseState(&input->cursorx, &input->cursory);
        first_mouse_read = 0;
    } else {
        mouse_button = SDL_GetMouseState(&input->cursorx, &input->cursory);
    }

    // Get how far the cursor went in x and y
    float xoffset = input->cursorx - SCREEN_WIDTH / 2.0f;
    float yoffset = SCREEN_HEIGHT / 2.0f - input->cursory;

    // Sensitivity for slower or faster movement
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Add to the current yaw and pitch of the UserCamera
    yaw += xoffset;
    pitch += yoffset;

    // Readjust back to the middle of the window
    SDL_WarpMouseInWindow(window, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);

    // Lock so that they do not further than usual up or down
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Calcuations for what we are now looking at
    camera->camera_front->x = cos(convert_deg_to_rad(yaw)) * cos(convert_deg_to_rad(pitch));
    camera->camera_front->y = sin(convert_deg_to_rad(pitch));
    camera->camera_front->z = sin(convert_deg_to_rad(yaw)) * cos(convert_deg_to_rad(pitch));

    // Normalize movement speed based on current tick
    float adjusted_movement_speed = delta_tick * MOVEMENT_SPEED_MULTIPLIER;

    if (input->keyboard_state[SDL_SCANCODE_W]) {
        camera->camera_position->x += camera->camera_front->x * adjusted_movement_speed;
        camera->camera_position->y += camera->camera_front->y * adjusted_movement_speed;
        camera->camera_position->z += camera->camera_front->z * adjusted_movement_speed;
    }

    if (input->keyboard_state[SDL_SCANCODE_A]) {
        fVec4 *right = cross_fvec4(camera->camera_front, camera->camera_up);
        normalize_fvec4(right);
        camera->camera_position->x -= right->x * adjusted_movement_speed;
        camera->camera_position->y -= right->y * adjusted_movement_speed;
        camera->camera_position->z -= right->z * adjusted_movement_speed;
        free(right);
    }

    if (input->keyboard_state[SDL_SCANCODE_S]) {
        camera->camera_position->x -= camera->camera_front->x * adjusted_movement_speed;
        camera->camera_position->y -= camera->camera_front->y * adjusted_movement_speed;
        camera->camera_position->z -= camera->camera_front->z * adjusted_movement_speed;
    }

    if (input->keyboard_state[SDL_SCANCODE_D]) {
        fVec4 *right = cross_fvec4(camera->camera_front, camera->camera_up);
        normalize_fvec4(right);
        camera->camera_position->x += right->x * adjusted_movement_speed;
        camera->camera_position->y += right->y * adjusted_movement_speed;
        camera->camera_position->z += right->z * adjusted_movement_speed;
        free(right);
    }

    // Update the new look at function with the new parameters from the buttons/movments
    camera_look_at_front(camera, camera->camera_position, camera->camera_front, camera->camera_up);
}

void test_functions() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    iVec2 *p0 = create_ivec2(2 * SCREEN_WIDTH / 3, 500);
    iVec2 *p1 = create_ivec2(SCREEN_WIDTH / 3, 500);
    iVec2 *p2 = create_ivec2(SCREEN_WIDTH / 2, 100);

    create_triangle(renderer, p2, p1, p0);

    SDL_RenderPresent(renderer);
}

void run_program() {
    clear_screen(renderer);
    update_fps();

    execute_render_pipeline(renderer, model, camera);

    SDL_RenderPresent(renderer);
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    if (camera->camera_mat) {
        free(camera->camera_mat);
        camera->camera_mat = NULL;
    }

    if (camera->projection_mat) {
        free(camera->projection_mat);
        camera->projection_mat = NULL;
    }

    if (mesh) {
        free_obj_reader(mesh);
        mesh = NULL;
    }

    if (model) {
        free(model);
        model = NULL;
    }

    if (z_buffer) {
        free(z_buffer);
        z_buffer = NULL;
    }
}
