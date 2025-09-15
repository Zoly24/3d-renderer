#include "render_pipeline.h"
#include "camera.h"
#include "constants.h"
#include "geometry.h"
#include "line.h"
#include "model.h"
#include <SDL3/SDL.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t last_frame_time = 0;
uint32_t frame_count = 0;
uint64_t last_fps_update = 0;
float fps = 0.0f;

// MAIN PIPELINE //

void execute_render_pipeline(SDL_Renderer *renderer, ModelObject *model, UserCamera *camera) {
    // Update camera matrix
    update_frustum_planes(camera);

    // Update model matrix
    update_model_space(model);

    // Being pipeline execution
    start_render(renderer, model, camera);
}

void start_render(SDL_Renderer *renderer, ModelObject *model, UserCamera *camera) {
    render_bounding_box(model, camera);
    if (!check_model_in_frustum(model, camera)) {
        return;
    }
    render_model_geometry(renderer, camera, model);
}

void render_model_geometry(SDL_Renderer *renderer, UserCamera *camera, ModelObject *model) {
    VecConnectionsPoints *triangle = model->mesh->head;

    int max_vertices = model->mesh->num_triangles * 27;
    iVec2 *batch_triangles = (iVec2 *)calloc(max_vertices, sizeof(iVec2));

    int triangle_idx = 0;

    while (triangle != NULL) {
        render_triangle_3d(renderer, camera, triangle, batch_triangles, &triangle_idx);
        triangle = triangle->next;
    }

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

    batch_draw_triangles(renderer, triangle_idx, batch_triangles);
    free(batch_triangles);
}

void render_triangle_3d(SDL_Renderer *renderer, UserCamera *camera, VecConnectionsPoints *triangle_data, iVec2 *batch_triangles, int *triangle_idx) {
    ClipVertex clip_triangle[3];

    // Convert triangle point from model to clip space
    for (int i = 0; i < 3; i++) {
        fVec4 *camera_space = create_fvec4(0, 0, 0, 0);
        multiply_fvec4_matrix44(triangle_data->triangle_points[i], camera_space, camera->camera_mat);
        multiply_fvec4_matrix44(camera_space, &clip_triangle[i].position, camera->projection_mat);
        free(camera_space);
    }

    ClipVertexList clipped_vertices;
    if (!clip_triangle_3d(clip_triangle, &clipped_vertices)) {
        return;
    }

    // Convert clipped vertices to screen space
    iVec2 screen_points[NUM_CLIP_TRIANLGE_VERTEX];
    int valid_screen_points = 0;

    for (int i = 0; i < clipped_vertices.count; i++) {
        if (clip_to_screen(&clipped_vertices.vertices[i], &screen_points[valid_screen_points])) {
            valid_screen_points++;
        }
    }

    if (valid_screen_points < 3) {
        return;
    }

    // Check backface culling on only first triangle
    if (!is_front_facing(screen_points)) {
        return;
    }

    // Triangulate the clipped polygon (fan triangulation)
    for (int i = 2; i < valid_screen_points; i++) {
        int new_idx_base = *triangle_idx * 3;
        batch_triangles[new_idx_base + 0] = screen_points[0];
        batch_triangles[new_idx_base + 1] = screen_points[i - 1];
        batch_triangles[new_idx_base + 2] = screen_points[i];

        render_normal_vector(renderer, camera, triangle_data, batch_triangles);
        (*triangle_idx)++;
    }
}

// CULLING AND VISIBILITY //

int check_model_in_frustum(ModelObject *model, UserCamera *camera) {
    int inside;
    // Make sure that the model is inside the camera frustum
    if ((inside = render_bounding_box(model, camera)) == INSIDE_FRUSTUM || inside == INTERSECT_FRUSTUM) {
        return 1;
    }

    return 0;
}

int render_bounding_box(ModelObject *model, UserCamera *camera) {
    int result = INSIDE_FRUSTUM;
    // IF one vec is out, then it intersects, if all, then it is outside frustum
    for (int i = 0; i < NUM_FRUSTUM_PLANES; i++) {
        int num_vertex_out = 0;
        int num_vertex_in = 0;
        for (int j = 0; j < NUM_BOUNDING_BOX_VERTEX && (num_vertex_out == 0 || num_vertex_in == 0); j++) {
            if (plane_distance_to_fvec4(camera->frustum.planes[i], model->mesh->bounding_box_vec[j]) == NEGATIVE_OF_PLANE) {
                num_vertex_out++;
            } else {
                num_vertex_in++;
            }
        }

        if (!num_vertex_in) {
            return OUTSIDE_FRUSTUM;
        } else if (num_vertex_out) {
            result = INTERSECT_FRUSTUM;
        }
    }

    return result;
}

int is_front_facing(iVec2 *screen_points) {
    int winding = determine_winding_order(screen_points);
    return winding < 0;
}

int determine_winding_order(iVec2 *arr) {
    return ((arr[0].x * arr[1].y) -
            (arr[1].x * arr[0].y)) +
           ((arr[1].x * arr[2].y) -
            (arr[2].x * arr[1].y)) +
           ((arr[2].x * arr[0].y) -
            (arr[0].x * arr[2].y));
}

// CLIPPING PIPELINE //

ClipVertex interpolate_clip_vertex(const ClipVertex *v1, const ClipVertex *v2, float t) {
    ClipVertex result;
    result.position.x = v1->position.x + t * (v2->position.x - v1->position.x);
    result.position.y = v1->position.y + t * (v2->position.y - v1->position.y);
    result.position.z = v1->position.z + t * (v2->position.z - v1->position.z);
    result.position.w = v1->position.w + t * (v2->position.w - v1->position.w);
    return result;
}

int clip_triangle_3d(ClipVertex triangle[3], ClipVertexList *clipped_output) {
    ClipVertexList input;
    ClipVertexList temp;

    float plane_boundaries[6][4] = {{0, 0, 1, 1},
                                    {0, 0, -1, 1},
                                    {1, 0, 0, 1},
                                    {-1, 0, 0, 1},
                                    {0, 1, 0, 1},
                                    {0, -1, 0, 1}};

    // Initialize input with the triangle
    input.count = NUM_TRIANGLE_VERTEX;
    for (int i = 0; i < NUM_TRIANGLE_VERTEX; i++) {
        input.vertices[i] = triangle[i];
    }

    for (int i = 0; i < NUM_FRUSTUM_PLANES; i++) {
        clip_against_plane(&input, &temp, plane_boundaries[i]);
        if (i < NUM_FRUSTUM_PLANES - 1) {

            input = temp;

            if (input.count == 0) {
                return 0;
            }
        }
    }
    *clipped_output = temp;

    return clipped_output->count >= NUM_TRIANGLE_VERTEX;
}

int clip_to_screen(const ClipVertex *clip_vertex, iVec2 *screen_point) {
    // Check for valid w component
    if (clip_vertex->position.w <= 0) {
        return 0;
    }

    // Perform perspective division (NDC POINTS)
    float ndc_x = clip_vertex->position.x / clip_vertex->position.w;
    float ndc_y = clip_vertex->position.y / clip_vertex->position.w;

    // Convert to screen coordinates
    screen_point->x = (int)((ndc_x + 1.0f) * 0.5f * SCREEN_WIDTH);
    screen_point->y = (int)((1.0f - (ndc_y + 1.0f) * 0.5f) * SCREEN_HEIGHT);

    return 1;
}

void clip_against_plane(ClipVertexList *input, ClipVertexList *output, float boundary[4]) {
    output->count = 0;

    if (input->count == 0)
        return;

    ClipVertex *prev_vertex = &input->vertices[input->count - 1];
    float prev_distance = boundary[0] * prev_vertex->position.x + boundary[1] * prev_vertex->position.y +
                          boundary[2] * prev_vertex->position.z + boundary[3] * prev_vertex->position.w;

    for (int i = 0; i < input->count; i++) {
        ClipVertex *curr_vertex = &input->vertices[i];
        float curr_distance = boundary[0] * curr_vertex->position.x + boundary[1] * curr_vertex->position.y +
                              boundary[2] * curr_vertex->position.z + boundary[3] * curr_vertex->position.w;
        if (curr_distance >= 0 && prev_distance >= 0) {
            output->vertices[output->count] = *curr_vertex;
            output->count++;
        } else if (curr_distance >= 0 && prev_distance < 0) {
            float percentage_distance = prev_distance / (prev_distance - curr_distance);
            output->vertices[output->count] = interpolate_clip_vertex(prev_vertex, curr_vertex, percentage_distance);
            output->count++;

            output->vertices[output->count] = *curr_vertex;
            output->count++;
        } else if (curr_distance <= 0 && prev_distance >= 0) {
            float percentage_distance = prev_distance / (prev_distance - curr_distance);
            output->vertices[output->count] = interpolate_clip_vertex(prev_vertex, curr_vertex, percentage_distance);
            output->count++;
        }

        prev_vertex = curr_vertex;
        prev_distance = curr_distance;
    }
}

// UTILIY FUNCTIONS //

void clear_screen(SDL_Renderer *renderer) {
    // Wipe screen for new draw

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void update_fps() {
    // Calculate the current fps from this and prev tick times
    uint64_t current_time = SDL_GetTicksNS();
    last_frame_time = current_time;

    frame_count++;

    if (current_time - last_fps_update >= NS_TO_SEC_INT) {
        fps = (float)frame_count / ((current_time - last_fps_update) / NS_TO_SEC_FLOAT);
        printf("FPS: %.2f\n", fps);

        frame_count = 0;
        last_fps_update = current_time;
    }
}

void update_model_space(ModelObject *model) {
    // New function for future when getting more objects
    update_model_mat(model);
}

fVec4 *calculate_triangle_centroid(fVec4 *triangle_points[3]) {
    fVec4 *centroid = add_fvec4(triangle_points[0], triangle_points[1]);
    add_fvec4_in_place(centroid, triangle_points[2]);
    scale_fvec4_in_place(centroid, 1.0f / 3.0f);
    centroid->w = 1.0f;

    return centroid;
}

fVec4 *calculate_normal_endpoint(fVec4 *centroid, fVec4 *surface_normal) {
    fVec4 *scale_normal = scale_fvec4(surface_normal, NORMAL_VECTOR_LENGTH);
    fVec4 *endpoint = add_fvec4(centroid, scale_normal);
    endpoint->w = 1.0f;

    free(scale_normal);

    return endpoint;
}

void render_normal_vector(SDL_Renderer *renderer, UserCamera *camera, VecConnectionsPoints *triangle_data, iVec2 *screen_points) {
    // Get center of triangle and normalize it to length of one
    fVec4 *centroid_fvec4 = calculate_triangle_centroid(triangle_data->triangle_points);
    fVec4 *endpoint_fvec4 = calculate_normal_endpoint(centroid_fvec4, triangle_data->surface_normal);

    free(centroid_fvec4);
    free(endpoint_fvec4);
}
