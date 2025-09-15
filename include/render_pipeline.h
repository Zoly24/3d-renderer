#ifndef RENDER_PIPELINE_H
#define RENDER_PIPELINE_H

#include "camera.h"
#include "model.h"
#include <SDL3/SDL.h>

typedef struct {
    fVec4 position;
} ClipVertex;

typedef struct {
    ClipVertex vertices[NUM_CLIP_TRIANLGE_VERTEX];
    int count;
} ClipVertexList;

// Main pipeline
void execute_render_pipeline(SDL_Renderer *, ModelObject *, UserCamera *);
void start_render(SDL_Renderer *, ModelObject *, UserCamera *);
void render_model_geometry(SDL_Renderer *, UserCamera *, ModelObject *);
void render_triangle_3d(SDL_Renderer *, UserCamera *, VecConnectionsPoints *, iVec2 *, int *);

// Culling and Visibility
int check_model_in_frustum(ModelObject *, UserCamera *);
int render_bounding_box(ModelObject *, UserCamera *);
int is_front_facing(iVec2 *);
int determine_winding_order(iVec2 *);

// Clipping Pipleline
ClipVertex interpolate_clip_vertex(const ClipVertex *, const ClipVertex *, float);
int clip_triangle_3d(ClipVertex[3], ClipVertexList *);
int clip_to_screen(const ClipVertex *, iVec2 *);
void clip_against_plane(ClipVertexList *, ClipVertexList *, float[4]);

// Utility Functions
void clear_screen(SDL_Renderer *);
void update_fps(void);
void update_model_space(ModelObject *);
fVec4 *calculate_triangle_centroid(fVec4 *[3]);
fVec4 *calculate_normal_endpoint(fVec4 *, fVec4 *);
void render_normal_vector(SDL_Renderer *, UserCamera *, VecConnectionsPoints *, iVec2 *);

#endif
