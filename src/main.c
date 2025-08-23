/*
10 Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
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

uint64_t last_frame_time = 0;
uint32_t frame_count = 0;
uint64_t last_fps_update = 0;
float fps = 0.0f;

Mesh *mesh;
ModelObject *model;

void update_user_input(UserInput *);
void run_program(void);
void test_functions(void);

void clear_screen(void);
void update_fps(void);
void update_model_transforms(void);
void render_scene(void);
int is_model_in_frustum(ModelObject *);
int is_point_in_frustum(fVec3 *);
fVec3 *transform_point_to_clip_space(fVec3 *);
void render_bounding_box(ModelObject *);
void render_model_geometry(ModelObject *);
void render_triangle(VecConnectionsPoints *, iVec2 **, int *);
int is_front_facing(iVec2 **);
void render_normal_vector(VecConnectionsPoints *, iVec2 **);
fVec3 *calculate_triangle_centroid(fVec3 *[3]);
fVec3 *calculate_normal_endpoint(fVec3 *, fVec3 *);

iVec2 *project_3d_to_2d(fVec3 *);
int determine_winding_order(iVec2 **);
int determine_bounding_box(ModelObject *);

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

    SDL_SetWindowMouseGrab(window, 1);
    SDL_SetWindowRelativeMouseMode(window, 1);

    current_tick = 0;

    initialize_user_input();
    initialize_camera();

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
    input = create_user_input();
    if (!input) {
        printf("Could not allocate input mem, quitting");
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

static SDL_AppResult initialize_camera() {
    camera = create_camera();
    // Position and target
    camera->camera_position = create_fvec3(0.0f, 0.0f, 3.0f);
    camera->camera_front = create_fvec3(0.0f, 0.0f, -1.0f);
    camera->camera_up = create_fvec3(0.0f, 1.0f, 0.0f);

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

    FILE *file = open_file("/home/zoly/Documents/3d-renderer/assets/one-triangle/triangle.obj");
    if (!file) {
        printf("Could not open file");
        return SDL_APP_FAILURE;
    }

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

    update_user_input(input);
    run_program();
    return SDL_APP_CONTINUE;
}

void update_user_input(UserInput *input) {
    input->keyboard_state = SDL_GetKeyboardState(NULL);
    uint32_t mouse_button;
    float lastx;
    float lasty;
    if (first_mouse_read) {
        mouse_button = SDL_GetMouseState(&input->cursorx, &input->cursory);
        lastx = input->cursorx;
        lasty = input->cursory;
        first_mouse_read = 0;
    } else {
        lastx = input->cursorx;
        lasty = input->cursory;
        mouse_button = SDL_GetMouseState(&input->cursorx, &input->cursory);
    }

    float xoffset = input->cursorx - SCREEN_WIDTH / 2.0f;
    float yoffset = SCREEN_HEIGHT / 2.0f - input->cursory;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    SDL_WarpMouseInWindow(window, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    camera->camera_front->x = cos(convert_deg_to_rad(yaw)) * cos(convert_deg_to_rad(pitch));
    camera->camera_front->y = sin(convert_deg_to_rad(pitch));
    camera->camera_front->z = sin(convert_deg_to_rad(yaw)) * cos(convert_deg_to_rad(pitch));

    float adjusted_movement_speed = delta_tick * MOVEMENT_SPEED_MULTIPLIER;

    if (input->keyboard_state[SDL_SCANCODE_W]) {
        camera->camera_position->x += camera->camera_front->x * adjusted_movement_speed;
        camera->camera_position->y += camera->camera_front->y * adjusted_movement_speed;
        camera->camera_position->z += camera->camera_front->z * adjusted_movement_speed;
    }

    if (input->keyboard_state[SDL_SCANCODE_A]) {
        fVec3 *right = cross_fvec3(camera->camera_front, camera->camera_up);
        normalize_fvec3(right);
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
        fVec3 *right = cross_fvec3(camera->camera_front, camera->camera_up);
        normalize_fvec3(right);
        camera->camera_position->x += right->x * adjusted_movement_speed;
        camera->camera_position->y += right->y * adjusted_movement_speed;
        camera->camera_position->z += right->z * adjusted_movement_speed;
        free(right);
    }

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
    clear_screen();
    update_fps();
    update_model_transforms();
    render_scene();
    SDL_RenderPresent(renderer);
}

void clear_screen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void update_fps() {
    uint64_t current_time = SDL_GetTicksNS();
    uint64_t delta_time_ns = current_time - last_frame_time;
    last_frame_time = current_time;

    frame_count++;

    if (current_time - last_fps_update >= NS_TO_SEC_INT) {
        fps = (float)frame_count / ((current_time - last_fps_update) / NS_TO_SEC_FLOAT);
        printf("FPS: %.2f\n", fps);

        frame_count = 0;
        last_fps_update = current_time;
    }
}

void update_model_transforms() {
    update_model_mat(model);
}

void render_scene() {

    render_bounding_box(model);

    render_model_geometry(model);
}

void render_bounding_box(ModelObject *model) {
    for (int i = 0; i < NUM_BOUNDING_BOX_VERTEX; i++) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        iVec2 *screen_pos = project_3d_to_2d(model->mesh->bounding_box_vec[i]);
        if (screen_pos) {
            SDL_RenderPoint(renderer, screen_pos->x, screen_pos->y);
            free(screen_pos);
        }
    }
}

void render_model_geometry(ModelObject *model) {
    VecConnectionsPoints *triangle = model->mesh->head;
    iVec2 **batch_triangles = calloc(model->mesh->num_triangles, sizeof(iVec2 *));
    for (int i = 0; i < model->mesh->num_triangles; i++) {
        batch_triangles[i] = calloc(3, sizeof(iVec2));
    }

    int triangle_idx = 0;

    while (triangle != NULL) {
        render_triangle(triangle, batch_triangles, &triangle_idx);
        triangle = triangle->next;
    }

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    batch_draw_triangles(renderer, triangle_idx, batch_triangles);
    for (int i = 0; i < model->mesh->num_triangles; i++) {
        free(batch_triangles[i]);
    }
    free(batch_triangles);
}

void render_triangle(VecConnectionsPoints *triangle_data, iVec2 **batch_triangles, int *triangle_idx) {
    iVec2 *screen_points[NUM_TRIANGLE_VERTEX];
    for (int i = 0; i < NUM_TRIANGLE_VERTEX; i++) {
        screen_points[i] = project_3d_to_2d(triangle_data->triangle_points[i]);
        if (!screen_points[i]) {
            for (int j = 0; j < i; j++) {
                free(screen_points[j]);
            }
            return;
        }
    }

    if (is_front_facing(screen_points)) {
        batch_triangles[*triangle_idx][0] = *screen_points[0];
        batch_triangles[*triangle_idx][1] = *screen_points[1];
        batch_triangles[*triangle_idx][2] = *screen_points[2];

        render_normal_vector(triangle_data, screen_points);
        *triangle_idx += 1;
    }

    for (int i = 0; i < NUM_TRIANGLE_VERTEX; i++) {
        free(screen_points[i]);
    }
}

int is_front_facing(iVec2 **screen_points) {
    int winding = determine_winding_order(screen_points);
    return winding < 0;
}

int determine_winding_order(iVec2 **arr) {
    return ((arr[0]->x * arr[1]->y) -
            (arr[1]->x * arr[0]->y)) +
           ((arr[1]->x * arr[2]->y) -
            (arr[2]->x * arr[1]->y)) +
           ((arr[2]->x * arr[0]->y) -
            (arr[0]->x * arr[2]->y));
}

void render_normal_vector(VecConnectionsPoints *triangle_data, iVec2 **screen_points) {
    fVec3 *centroid_fvec3 = calculate_triangle_centroid(triangle_data->triangle_points);
    fVec3 *endpoint_fvec3 = calculate_normal_endpoint(centroid_fvec3, triangle_data->surface_normal);

    iVec2 *centroid_ivec2 = project_3d_to_2d(centroid_fvec3);
    iVec2 *endpoint_ivec2 = project_3d_to_2d(endpoint_fvec3);

    if (centroid_ivec2 && endpoint_ivec2) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        render_line(renderer, centroid_ivec2, endpoint_ivec2);
    }

    free(centroid_fvec3);
    free(endpoint_fvec3);
    free(centroid_ivec2);
    free(endpoint_ivec2);
}

fVec3 *calculate_triangle_centroid(fVec3 *triangle_points[3]) {
    fVec3 *centroid = add_fvec3(triangle_points[0], triangle_points[1]);
    add_fvec3_in_place(centroid, triangle_points[2]);
    scale_fvec3_in_place(centroid, 1.0f / 3.0f);

    return centroid;
}

fVec3 *calculate_normal_endpoint(fVec3 *centroid, fVec3 *surface_normal) {
    fVec3 *scale_normal = scale_fvec3(surface_normal, NORMAL_VECTOR_LENGTH);
    fVec3 *endpoint = add_fvec3(centroid, scale_normal);

    free(scale_normal);

    return endpoint;
}

iVec2 *project_3d_to_2d(fVec3 *p) {
    fVec3 *camera_space = create_fvec3(0, 0, 0);

    multiply_fvec3_matrix44(p, camera_space, camera->camera_mat);

    // Prevents from going into model and flipping inverted
    if (camera_space->z > camera->settings.near) {
        free(camera_space);
        return NULL;
    }

    fVec3 *clip_space = create_fvec3(0, 0, 0);
    multiply_fvec3_matrix44(camera_space, clip_space, camera->projection_mat);

    printf("clip_space: %.2f, %.2f, %.2f\n", clip_space->x, clip_space->y, clip_space->z);

    // NOTE: Clip_space is normalized to -1, 1 for x and -1, 1 for y (kinda like NDC space)
    if (clip_space->x < -1 ||
        clip_space->x > 1 ||
        clip_space->y < -1 ||
        clip_space->y > 1) {
        free(camera_space);
        free(clip_space);
        return NULL;
    }

    int x = (int)((clip_space->x + 1.0f) * 0.5f * SCREEN_WIDTH);
    int y = (int)((1.0f - (clip_space->y + 1.0f) * 0.5f) * SCREEN_HEIGHT);

    x = fmax(0, fmin(SCREEN_WIDTH - 1, x));
    y = fmax(0, fmin(SCREEN_HEIGHT - 1, y));

    z_buffer[y * SCREEN_WIDTH + x] = (int)(clip_space->z * 255);

    free(camera_space);
    free(clip_space);

    return create_ivec2(x, y);
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
