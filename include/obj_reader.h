#ifndef OBJ_READER_H
#define OBJ_READER_H

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "geometry.h"
#include "model.h"
#include "triangle.h"

typedef enum AttributeType {
    VERTEX,
    VERTEX_NORMAL,
    VERTEX_TEXTURE,
    FACE
} VertexAttribute;

FILE *open_file(char *);
void generate_mesh(FILE *, Mesh *);
void populate_vertex_connections(int *, int, Mesh *);
void calculate_surface_normal(VecConnectionsPoints *, fVec3 *, fVec3 *, fVec3 *);
int *parse_vertex_attributes(FILE *);
void determine_min_max(float *, float *, float *, float *, float *, float *, float, float, float);
void define_bounding_box(Mesh *, float, float, float, float, float, float);
void parse_vertex_connections(FILE *, Mesh *);
void populate_vertices_arr(FILE *, Mesh *);
void free_obj_reader(Mesh *);

#endif
