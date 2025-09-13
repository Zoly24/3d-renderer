#ifndef MODEL_H
#define MODEL_H

#include "constants.h"
#include "geometry.h"
#include "transform.h"
#include "triangle.h"

typedef struct LLVecConnections {
    struct LLVecConnections *next;
    fVec3 *triangle_points[NUM_TRIANGLE_VERTEX];
    fVec3 *surface_normal;

} VecConnectionsPoints;

typedef struct Mesh {
    int face_count;
    int vec_count;
    int vec_texture_count;
    int vec_normal_count;
    int num_triangles;

    fVec3 *vec_arr;
    fVec3 *bounding_box_vec[NUM_BOUNDING_BOX_VERTEX];

    VecConnectionsPoints *head;
} Mesh;

typedef struct ModelObject {
    Mesh *mesh;
    fMatrix44 *model_mat;
    Transform transform;
} ModelObject;

ModelObject *create_model_object(Mesh *);
void update_model_mat(ModelObject *);

#endif
