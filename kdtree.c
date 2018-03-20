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

#define TWO_DIMENSIONAL_TREE 2
typedef struct kd_node {
    vertex_ptr v;
    int n_faces;
    face2_ptr *face_group; // array of faces which node does include
    struct kd_node *left, *right;
} kd_node_t;

double
dist(kd_node_t *a, kd_node_t *b, int dim)
{
    double t, d = 0;
    while (dim--)
    {
        vertex_ptr a_ptr = a->v;
        vertex_ptr b_ptr = b->v;

        t = a_ptr->coord[dim] - b_ptr->coord[dim];
        d += t * t;
    }
    return d;
}

void swap(kd_node_t *a_node, kd_node_t *b_node)
{
    kd_node_t tmp_node;

    memcpy(&tmp_node,  a_node, sizeof(kd_node_t));
    memcpy(a_node,     b_node, sizeof(kd_node_t));
    memcpy(b_node,  &tmp_node, sizeof(kd_node_t));
}

/* see quickselect method */
kd_node_t*
find_median(kd_node_t *start, kd_node_t *end, int idx)
{
    if (end <= start) return NULL;
    if (end == start + 1)
        return start;

    kd_node_t *p, *store, *md = start + (end - start) / 2;
    double pivot;
    while (1) {
        vertex_ptr v_ptr = md->v;
        pivot = v_ptr->coord[idx];

        swap(md, end - 1);
        for (store = p = start; p < end; p++)
        {
            vertex_ptr v_ptr = p->v;
            if (v_ptr->coord[idx] < pivot) {
                if (p != store)
                    swap(p, store);
                store++;
            }
        }
        swap(store, end - 1);

        /* median has duplicate values */
        v_ptr = md->v;
        vertex_ptr store_v_ptr = store->v;
        if (store_v_ptr->coord[idx] == v_ptr->coord[idx])
            return md;

        if (store > md) end = store;
        else        start = store;
    }
}

kd_node_t*
make_tree(kd_node_t *t, int len, int i, int dim)
{
    kd_node_t *n;

    if (!len) return 0;

    if ((n = find_median(t, t + len, i))) {
        i = (i + 1) % dim;
        n->left  = make_tree(t, n - t, i, dim);
        n->right = make_tree(n + 1, t + len - (n + 1), i, dim);
    }
    return n;
}

/* global variable, so sue me */
int visited;

void nearest(kd_node_t *root, kd_node_t *nd, int i, int dim,
        kd_node_t **best, double *best_dist)
{
    double d, dx, dx2;

    if (!root) return;

    d = dist(root, nd, dim);

    vertex_ptr root_v_ptr = root->v;
    vertex_ptr nd_v_ptr = nd->v;

    dx = root_v_ptr->coord[i] - nd_v_ptr->coord[i];
    dx2 = dx * dx;

    visited++;

    if (!*best || d < *best_dist) {
        *best_dist = d;
        *best = root;
    }

    /* if chance of exact match is high */
    if (!*best_dist) return;

    if (++i >= dim) i = 0;

    nearest(dx > 0 ? root->left : root->right, nd, i, dim, best, best_dist);
    if (dx2 >= *best_dist) return;
    nearest(dx > 0 ? root->right : root->left, nd, i, dim, best, best_dist);
}

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
        //free some memory

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
    scanf("%f%f", &x, &y);

    vertex_t testVertex = {x, y, 0.0};
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

    printf(">> Searching for vertex (x=%g, y=%g, z=%g) in kd-tree\n",
            target_vertex->coord[0], target_vertex->coord[1],
            target_vertex->coord[2]);

    int n_nodes = ms->n_vertices;
    printf("Number of nodes = %d\n", n_nodes);

    root = make_tree(nodes, n_nodes, 0, TWO_DIMENSIONAL_TREE);

    nearest(root, &testNode, 0, 2, &found, &best_dist);

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
    free(nodes);
    free(faces);

    if (shmdt(shmptr) == -1)
        perror("shmdt failed");
}

