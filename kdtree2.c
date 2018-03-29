#include <math.h>
#include <string.h>

#include "probekins2.h"
#include "kdtree.h"

static double dist(kd_node_t *a, kd_node_t *b, int dim)
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

static inline void swap(kd_node_t *a_node, kd_node_t *b_node)
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
    if (end <= start)
        return NULL;

    if (end == start + 1)
        return start;

    kd_node_t *p, *store, *md = start + (end - start) / 2;
    double pivot;

    while (1)
    {
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

        if (store > md)
        {
            end = store;
        }
        else
        {
            start = store;
        }
    }
}

kd_node_t*
make_tree(kd_node_t *t, int len, int i, int dim)
{
    kd_node_t *n;

    if (!len) return 0;

    if ((n = find_median(t, t + len, i)))
    {
        i = (i + 1) % dim;
        n->left  = make_tree(t, n - t, i, dim);
        n->right = make_tree(n + 1, t + len - (n + 1), i, dim);
    }
    return n;
}

void find_nearest_node(kd_node_t *root, kd_node_t *nd, int i, int dim,
        kd_node_t **best, double *best_dist)
{
    double d, dx, dx2;

    if (!root) return;

    d = dist(root, nd, dim);

    vertex_ptr root_v_ptr = root->v;
    vertex_ptr nd_v_ptr = nd->v;

    dx = root_v_ptr->coord[i] - nd_v_ptr->coord[i];
    dx2 = dx * dx;

//    visited++;

    if (!*best || d < *best_dist) {
        *best_dist = d;
        *best = root;
    }

    /* if chance of exact match is high */
    if (!*best_dist) return;

    if (++i >= dim)
        i = 0;

    find_nearest_node(dx > 0 ? root->left : root->right, nd, i, dim, best, best_dist);

    if (dx2 >= *best_dist)
        return;

    find_nearest_node(dx > 0 ? root->right : root->left, nd, i, dim, best, best_dist);
}
