#include "line.h"
#include "constants.h"
#include "geometry.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

Line *create_line(iVec2 *start, iVec2 *end) {

    Line *line = (Line *)malloc(sizeof(Line));
    if (!line) {
        printf("Could not malloc mem for line");
        return NULL;
    }

    line->p0 = start;
    line->p1 = end;

    return line;
}

void render_line(SDL_Renderer *renderer, iVec2 *start, iVec2 *end) {
    int x0 = start->x;
    int x1 = end->x;
    int y0 = start->y;
    int y1 = end->y;

    if ((x0 < 0 && x1 < 0) || (x0 >= SCREEN_WIDTH && x1 >= SCREEN_WIDTH) ||
        (y0 < 0 && y1 < 0) || (y0 >= SCREEN_HEIGHT && y1 >= SCREEN_HEIGHT)) {
        return;
    }

    int dx = abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int error = dx + dy;

    while (1) {
        if (x0 >= 0 && x0 < SCREEN_WIDTH && y0 >= 0 && y0 < SCREEN_HEIGHT) {
            SDL_RenderPoint(renderer, x0, y0);
        }

        if (x0 == x1 && y0 == y1) {
            break;
        }

        int e2 = error << 1;
        if (e2 >= dy) {
            if (x0 == x1) {
            } else {
                error = error + dy;
                x0 = x0 + sx;
            }
        }
        if (e2 <= dx) {
            if (y0 == y1) {
            } else {
                error = error + dx;
                y0 = y0 + sy;
            }
        }
    }
}
