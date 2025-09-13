#ifndef LINE_H
#define LINE_H

#include "geometry.h"
#include <SDL3/SDL.h>

typedef struct Line {
    iVec2 *p0;
    iVec2 *p1;

} Line;

Line *create_line(iVec2 *, iVec2 *);
void render_line(SDL_Renderer *, iVec2 *, iVec2 *);

#endif
