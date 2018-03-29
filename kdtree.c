#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "data_pass.h"
#include "probekins2.h"
#include "kdtree.h"

int visited;

int main(void)
{
    int shmid;
    int shmsz = 224;

    if ((shmid = shmget(SHMEM_KEY, shmsz, 0666)) == -1)
    {
        perror("failed"); exit(1);
    }

    char *shmptr;
    if ((shmptr = shmat(shmid, 0, 0)) == (void *)-1)
    {
        perror("shmat failed"); exit(1);
    }
    printf("shared memory attached from %p to %p\n", (void*)shmptr, (void*)shmptr+shmsz);

    mesh_struct* ms = (mesh_struct *)shmptr;
    face_t *ms_faces = (face_t*) &ms->vertices[ms->n_vertices];
    int* ms_face_groups_arr = (int*) &ms_faces[ms->n_faces];

    print_mesh_struct(ms);

    kd_node_t *nodes = calloc(ms->n_vertices, sizeof(kd_node_t));
    memset(nodes, 0, ms->n_vertices*sizeof(kd_node_t));

    // build local faces table
    face2_t *faces = calloc(ms->n_faces, sizeof(face2_t));
    memset(faces, 0, ms->n_faces*sizeof(face2_t));
    for (int i=0; i < ms->n_faces; ++i)
    {
        for (int j=0; j<3; ++j)
        {
            int vertex_idx = ms_faces[i][j];
            faces[i][j] = &ms->vertices[vertex_idx];
        }
    }

    // initialize node of tree
    int face_group_idx = 0;
    for (int i = 0; i < ms->n_vertices; ++i)
    {
        vertex_t *v = &ms->vertices[i];
        nodes[i].v = v;
        nodes[i].n_faces = v->face_group_size;

        // build face_goup table
        face2_ptr *face_group = calloc(v->face_group_size, sizeof(face2_ptr));
        memset(face_group, 0, v->face_group_size*sizeof(face2_ptr));

        for (int j=0; j < v->face_group_size; ++j)
        {
            int face_idx = ms_face_groups_arr[face_group_idx];
            ++face_group_idx;

            face_group[j] = &faces[face_idx];
        }
        nodes[i].face_group = face_group;
    }

    double x=0;
    double y=0;

    printf("Enter x y pair to search: ");
    while (scanf("%lf %lf", &x, &y) != 2)
    {
        printf("Input Error. Enter x y pair to search: ");
    }

    printf("%lf %lf\n", x, y);
    vertex_t testVertex = {
        .coord = {x, y, 0.0},
        .face_group_size = 0
    };

    kd_node_t testNode =
    {
        .v = &testVertex,
        .n_faces = 0,
        NULL,
        NULL,
        NULL
    };

    kd_node_t *root = NULL;
    kd_node_t *found = NULL;

    double best_dist;

    vertex_ptr target_vertex = testNode.v;

    printf(">> Searching for vertex (x=%f, y=%f, z=%f) in kd-tree\n",
            target_vertex->coord[0], target_vertex->coord[1], target_vertex->coord[2]);

    int n_nodes = ms->n_vertices;
    printf("Number of nodes = %d\n", n_nodes);

    root = make_tree(nodes, n_nodes, 0, TWO_DIMENSIONAL_TREE);

    find_nearest_node(root, &testNode, 0, TWO_DIMENSIONAL_TREE, &found, &best_dist);

    if (found)
    {
        vertex_ptr found_v_ptr = found->v;
        printf("found nearest vertex (x=%g, y=%g, z=%g) "
                "dist %g\n"
                "seen %d nodes\n",
                found_v_ptr->coord[0], found_v_ptr->coord[1], found_v_ptr->coord[2],
                sqrt(best_dist), visited);
        printf("this vertex belongs to faces:\n");
        for (int i=0; i<found->v->face_group_size; ++i)
        {
            face2_ptr fp = found->face_group[i];
            for (int j=0; j<3; ++j)
            {
                printf("(%f, %f, %f)\n",
                (*fp)[j]->coord[0],
                (*fp)[j]->coord[1],
                (*fp)[j]->coord[2]);
            }
            putchar('\n');
        }
    }
    else
    {
        printf("Didn't found any node");
    }


    for (int i = 0; i < ms->n_vertices; ++i)
    {
        free(nodes[i].face_group);
    }

    free(nodes);
    free(faces);

    if (shmdt(shmptr) == -1)
        perror("shmdt failed");
}

