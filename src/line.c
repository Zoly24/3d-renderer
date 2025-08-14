#include "line.h"
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

    int dx = abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;

    int dy = -abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;

    int error = dx + dy;

    while (1) {
        SDL_RenderPoint(renderer, x0, y0);
        int e2 = error << 1;
        if (e2 >= dy) {
            if (x0 == x1) {
                break;
            }
            error = error + dy;
            x0 = x0 + sx;
        }

        if (e2 <= dx) {
            if (y0 == y1) {
                break;
            }
            error = error + dx;
            y0 = y0 + sy;
        }
    }
}
