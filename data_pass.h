#ifndef DATA_PASS_H
#define DATA_PASS_H
#include "probekins2.h"

static inline void print_mesh_struct(mesh_struct *mp)
{
    printf("Number of vertices: %d\n", mp->n_vertices);
    printf("Number of faces: %d\n", mp->n_faces);
    printf("Size of face groups array: %d\n", mp->n_face_groups);

    face_t* mp_faces = (face_t*) &mp->vertices[mp->n_vertices];
    int* mp_face_groups_arr = (int*) &mp_faces[mp->n_faces];
    int base_offset = 0;

    printf("Faces are: \n");
    for (int i = 0; i < mp->n_faces; ++i)
    {
        printf("\tFace%d (v1=%d, v2=%d, v3=%d\n", i,
                mp_faces[i][0],
                mp_faces[i][1],
                mp_faces[i][2]);
    }

    for (int i = 0; i < mp->n_vertices; ++i)
    {
        printf("Vertex%d (%f, %f, %f)\n", i,
                mp->vertices[i].coord[0],
                mp->vertices[i].coord[1],
                mp->vertices[i].coord[2]);
        int face_group_size = mp->vertices[i].face_group_size;
        printf("\t Face group size %d: \n", face_group_size);

        printf("\t Belongs to faces: ");
        int j;

        int temp_offset = base_offset;
        for (j = 0; j < face_group_size; ++j)
        {
            printf("%d ", mp_face_groups_arr[temp_offset + j]);
            ++base_offset;
        }
        printf("\n");
    }
}

#endif //DATA_PASS_H

