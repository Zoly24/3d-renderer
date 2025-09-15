#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

#include "geometry.h"
#include "line.h"
#include "triangle.h"

Triangle *create_triangle(SDL_Renderer *renderer, iVec2 *v1, iVec2 *v2, iVec2 *v3) {

    Triangle *triangle = (Triangle *)malloc(sizeof(Triangle));
    if (!triangle) {
        printf("Triangle could not be created");
        return NULL;
    }

    triangle->v1 = v1;
    triangle->v2 = v2;
    triangle->v3 = v3;

    render_line(renderer, v1, v2);
    render_line(renderer, v2, v3);
    render_line(renderer, v3, v1);
    // populate_uv_map(triangle);

    return triangle;
}

void draw_triangle(SDL_Renderer *renderer, iVec2 *v1, iVec2 *v2, iVec2 *v3) {
    render_line(renderer, v1, v2);
    render_line(renderer, v2, v3);
    render_line(renderer, v3, v1);
}

void batch_draw_triangles(SDL_Renderer *renderer, int size, iVec2 *point_arr) {
    for (int i = 0; i < size * 3; i += 3) {
        iVec2 p1 = point_arr[i + 0];
        iVec2 p2 = point_arr[i + 1];
        iVec2 p3 = point_arr[i + 2];

        render_line(renderer, &p1, &p2);
        render_line(renderer, &p2, &p3);
        render_line(renderer, &p3, &p1);
    }
}

void populate_uv_map(Triangle *triangle) {
    // int min_x = fmin(triangle->v1->x, fmin(triangle->v2->x, triangle->v3->x));
    // int max_x = fmax(triangle->v1->x, fmax(triangle->v2->x, triangle->v3->x));
    // int min_y = fmin(triangle->v1->y, fmin(triangle->v2->y, triangle->v3->y));
    // int max_y = fmax(triangle->v1->y, fmax(triangle->v2->y, triangle->v3->y));

    // float width = (float)fmax(1, max_x - min_x);
    // float height = (float)fmax(1, max_y - min_y);

    // triangle->v1->u = (triangle->v1->x - min_x) / width;
    // triangle->v1->v = (triangle->v1->y - min_y) / height;

    // triangle->v2->u = (triangle->v2->x - min_x) / width;
    // triangle->v2->v = (triangle->v2->y - min_y) / height;

    // triangle->v3->u = (triangle->v3->x - min_x) / width;
    // triangle->v3->v = (triangle->v3->y - min_y) / height;
}

void fill_triangle(SDL_Renderer *renderer, SDL_Color *color, Triangle *triangle, float *z_buffer, int width, int height) {

    int xa = triangle->v1->x;
    int xb = triangle->v2->x;
    int xc = triangle->v3->x;

    int ya = triangle->v1->y;
    int yb = triangle->v2->y;
    int yc = triangle->v3->y;

    int min_x = fmin(triangle->v1->x, fmin(triangle->v2->x, triangle->v3->x));
    int max_x = fmax(triangle->v1->x, fmax(triangle->v2->x, triangle->v3->x));
    int min_y = fmin(triangle->v1->y, fmin(triangle->v2->y, triangle->v3->y));
    int max_y = fmax(triangle->v1->y, fmax(triangle->v2->y, triangle->v3->y));

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            float alpha = (float)(((-(x - xb)) * (yc - yb)) + ((y - yb) * (xc - xb))) /
                          (((-(xa - xb)) * (yc - yb)) + ((ya - yb) * (xc - xb)));
            float beta = (float)(((-(x - xc)) * (ya - yc)) + ((y - yc) * (xa - xc))) /
                         (((-(xb - xc)) * (ya - yc)) + ((yb - yc) * (xa - xc)));
            float gamma = 1 - alpha - beta;

            if (gamma >= 0 && beta >= 0 && alpha >= 0) {

                SDL_SetRenderDrawColor(renderer, alpha * 255, beta * 255, gamma * 255, 255);
                SDL_RenderPoint(renderer, x, y);
            }
        }
    }
}
