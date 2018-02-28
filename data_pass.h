#ifndef DATA_PASS_H
#define DATA_PASS_H
#include "probekins2.h"

static inline void print_mesh_struct(mesh_struct *mp)
{
    int i;

    printf("Number of vertices: %d\n", mp->n_vertices);
    printf("Number of faces: %d\n", mp->n_faces);
    printf("Size of face groups array: %d\n", mp->n_face_groups);

    face_t* mp_faces = (face_t*) &mp->vertices[mp->n_vertices];
    int* mp_face_groups_arr = (int*) &mp_faces[mp->n_faces];
    int acc = 0;

    for (i = 0; i < mp->n_vertices; ++i)
    {
        printf("\tVertex %d\n", i);
        int face_group_size = mp->vertices[i].face_group_size;
        printf("\t face group size %d: ", face_group_size);

        printf("\t Belongs to faces: ");
        int j;
        for (j = 0; j < face_group_size; ++j)
            printf("%d ", mp_face_groups_arr[acc + j]);
        acc += face_group_size-1;
        printf("\n");
    }
}

#endif //DATA_PASS_H

