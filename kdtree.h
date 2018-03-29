#ifndef KDTREE_H
#define KDTREE_H

#include "probekins2.h"

#define TWO_DIMENSIONAL_TREE 2
typedef struct kd_node {
    vertex_ptr v;
    int n_faces;
    face2_ptr *face_group; // array of faces which node does include
    struct kd_node *left;
    struct kd_node *right;
} kd_node_t;

kd_node_t* make_tree(kd_node_t *root, int len, int i, int dim);
void find_nearest_node(kd_node_t *root, kd_node_t *nd, int i, int dim,
        kd_node_t **best, double *best_dist);

#endif //KDTREE_H
