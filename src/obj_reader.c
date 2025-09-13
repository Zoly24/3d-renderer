#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "geometry.h"
#include "model.h"
#include "obj_reader.h"

FILE *open_file(char *filename) {
    FILE *ptr;
    ptr = fopen(filename, "r");

    if (!ptr) {
        printf("ERROR: Could not find file %s", filename);
        return NULL;
    }

    return ptr;
}

void generate_mesh(FILE *file, Mesh *mesh) {
    int *vertex_att = parse_vertex_attributes(file);

    // Store vertex attribute counts into the mesh
    mesh->vec_count = vertex_att[VERTEX];
    mesh->face_count = vertex_att[FACE];
    mesh->vec_texture_count = vertex_att[VERTEX_TEXTURE];
    mesh->vec_normal_count = vertex_att[VERTEX_NORMAL];

    // Make vec array for each vertex
    mesh->vec_arr = (fVec3 *)malloc(mesh->vec_count * sizeof(fVec3));

    // Initialize Linked List of VecConnectionsPoints
    mesh->head = NULL;

    // Restart and populate the verticies array
    rewind(file);
    populate_vertices_arr(file, mesh);

    // Restart and obtain the vertex connections
    rewind(file);
    parse_vertex_connections(file, mesh);
}

int *parse_vertex_attributes(FILE *file) {
    int *vertex_att = (int *)calloc(FACE + 1, sizeof(int));

    char buffer[MAX_BUFFER_SIZE];

    // Counter for each attribute
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (strncmp(buffer, "v ", 2) == 0) {
            vertex_att[VERTEX]++;
        }
        if (strncmp(buffer, "vt ", 3) == 0) {
            vertex_att[VERTEX_TEXTURE]++;
        }

        if (strncmp(buffer, "vn ", 3) == 0) {
            vertex_att[VERTEX_NORMAL]++;
        }

        if (strncmp(buffer, "f ", 2) == 0) {
            vertex_att[FACE]++;
        }
    }

    return vertex_att;
}

void populate_vertices_arr(FILE *file, Mesh *mesh) {
    char buffer[MAX_BUFFER_SIZE];

    float minx = FLT_MAX;
    float miny = FLT_MAX;
    float minz = FLT_MAX;

    float maxx = FLT_MIN;
    float maxy = FLT_MIN;
    float maxz = FLT_MIN;

    int i = 0;
    // Get x, y, and z
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (strncmp(buffer, "v ", 2) == 0) {
            float x;
            float y;
            float z;
            int parsed = sscanf(buffer, "v %f %f %f", &x, &y, &z);

            // Determine max and mins for bounding box
            determine_min_max(&minx, &miny, &minz, &maxx, &maxy, &maxz, x, y, z);

            // Put x, y, z into the mesh
            if (parsed == NUM_TRIANGLE_VERTEX) {
                (mesh->vec_arr + i)->x = x;
                (mesh->vec_arr + i)->y = y;
                (mesh->vec_arr + i)->z = z;
            }
            i++;
        }
    }

    // Define the boudning box
    define_bounding_box(mesh, minx, maxx, miny, maxy, minz, maxz);
}

void determine_min_max(float *minx, float *miny, float *minz, float *maxx, float *maxy, float *maxz, float x, float y, float z) {
    if (x < *minx) {
        *minx = x;
    }
    if (y < *miny) {
        *miny = y;
    }
    if (z < *minz) {
        *minz = z;
    }

    if (x > *maxx) {
        *maxx = x;
    }
    if (y > *maxy) {
        *maxy = y;
    }
    if (z > *maxz) {
        *maxz = z;
    }
}

void define_bounding_box(Mesh *mesh, float minx, float maxx, float miny, float maxy, float minz, float maxz) {
    // Egotistic code for calculating the bounding box using bit operations
    for (int i = 0; i < NUM_BOUNDING_BOX_VERTEX; i++) {
        mesh->bounding_box_vec[i] = create_fvec3(0, 0, 0);
        int maskx = 4;
        int masky = 2;
        int maskz = 1;

        if ((maskx & i) == 0) {
            mesh->bounding_box_vec[i]->x = minx;
        } else {
            mesh->bounding_box_vec[i]->x = maxx;
        }

        if ((masky & i) == 0) {
            mesh->bounding_box_vec[i]->y = miny;
        } else {
            mesh->bounding_box_vec[i]->y = maxy;
        }

        if ((maskz & i) == 0) {
            mesh->bounding_box_vec[i]->z = minz;
        } else {
            mesh->bounding_box_vec[i]->z = maxz;
        }
    }
}

void parse_vertex_connections(FILE *file, Mesh *mesh) {
    char buffer[MAX_BUFFER_SIZE];

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if ((strncmp(buffer, "f ", 2)) == 0) {
            // Duplicate string twice
            char *line1 = strdup(buffer + 2);
            char *line2 = strdup(buffer + 2);

            // Create two save pointers
            char *saveptr1;
            char *saveptr2;

            int vertex_groups = 0;

            // Count number of groups in current line (connections)
            char *token1 = strtok_r(line1, " \n\r", &saveptr1);
            while (token1 != NULL) {
                vertex_groups++;
                token1 = strtok_r(NULL, " \n\r", &saveptr1);
            }

            int *v_att_arr = (int *)malloc(sizeof(int) * vertex_groups);
            int *vn_att_arr = (int *)malloc(sizeof(int) * vertex_groups);
            int *vt_att_arr = (int *)malloc(sizeof(int) * vertex_groups);

            if (!v_att_arr || !vn_att_arr || !vt_att_arr) {
                printf("Vertex group arrays couldn't get malloced.");
                break;
            }

            int temp_idx = 0;
            char *token2 = strtok_r(line2, " \n\r", &saveptr2);
            while (token2 != NULL) {
                char *sub_saveptr;
                char *sub_token = strtok_r(token2, "/", &sub_saveptr);

                int v_idx = 0;
                int vt_idx = 0;
                int vn_idx = 0;
                // If statement for all cases that the attributes can be in
                if (sscanf(sub_token, "%d/%d/%d", &v_idx, &vt_idx, &vn_idx) == 3) {
                    v_att_arr[temp_idx] = v_idx;
                    vt_att_arr[temp_idx] = vt_idx;
                    vn_att_arr[temp_idx] = vn_idx;
                } else if (sscanf(sub_token, "%d/%d", &v_idx, &vt_idx) == 2) {
                    v_att_arr[temp_idx] = v_idx;
                    vt_att_arr[temp_idx] = vt_idx;
                    vn_att_arr[temp_idx] = NO_ATTRIBUTE;
                } else if (sscanf(sub_token, "%d//%d", &v_idx, &vn_idx) == 2) {
                    v_att_arr[temp_idx] = v_idx;
                    vt_att_arr[temp_idx] = NO_ATTRIBUTE;
                    vn_att_arr[temp_idx] = vn_idx;
                } else if (sscanf(sub_token, "%d", &v_idx) == 1) {
                    v_att_arr[temp_idx] = v_idx;
                    vt_att_arr[temp_idx] = NO_ATTRIBUTE;
                    vn_att_arr[temp_idx] = NO_ATTRIBUTE;
                }

                token2 = strtok_r(NULL, " \n\r", &saveptr2);
                temp_idx++;
            }

            // Number of triangles is the number of vertecies - 2
            mesh->num_triangles += temp_idx - 2;

            populate_vertex_connections(v_att_arr, vertex_groups, mesh);

            free(line1);
            free(line2);

            free(v_att_arr);
            free(vt_att_arr);
            free(vn_att_arr);
        }
    }
}

void populate_vertex_connections(int *v_att_arr, int vertex_groups, Mesh *mesh) {
    // Create a VecConnectionsPoints and add it to the linked list
    for (int i = 2; i < vertex_groups; i++) {
        VecConnectionsPoints *current_vec = (VecConnectionsPoints *)malloc(sizeof(VecConnectionsPoints));
        // attributes have 1 based indexing and vec_arr is 0 based indexing
        current_vec->triangle_points[0] = &mesh->vec_arr[v_att_arr[0] - 1];
        current_vec->triangle_points[1] = &mesh->vec_arr[v_att_arr[i - 1] - 1];
        current_vec->triangle_points[2] = &mesh->vec_arr[v_att_arr[i] - 1];

        calculate_surface_normal(current_vec, current_vec->triangle_points[0], current_vec->triangle_points[1], current_vec->triangle_points[2]);

        current_vec->next = mesh->head;
        mesh->head = current_vec;
    }
}

void calculate_surface_normal(VecConnectionsPoints *current_vec, fVec3 *a, fVec3 *b, fVec3 *c) {
    fVec3 *v1 = sub_fvec3(a, b);
    fVec3 *v2 = sub_fvec3(a, c);

    current_vec->surface_normal = cross_fvec3(v1, v2);
    normalize_fvec3(current_vec->surface_normal);

    free(v1);
    free(v2);
}

void free_obj_reader(Mesh *mesh) {
    if (mesh == NULL) {
        return;
    }

    VecConnectionsPoints *temp;
    while (mesh->head != NULL) {
        temp = mesh->head;
        mesh->head = mesh->head->next;

        free(temp->surface_normal);
        free(temp);
    }
    mesh->head = NULL;

    if (mesh->vec_arr) {
        free(mesh->vec_arr);
        mesh->vec_arr = NULL;
    }

    mesh->vec_arr = NULL;

    free(mesh);
    mesh = NULL;
}
