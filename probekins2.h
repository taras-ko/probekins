#ifndef PROBEKINS2_H
#define PROBEKINS2_H

typedef struct {
    double coord[3];
    int face_group_size;
} vertex_t;

typedef int face_t[3];

typedef struct {
    int n_vertices;
    int n_faces;
    int n_face_groups;
    vertex_t vertices[1]; //...n_vertices
    // followed by:
    // face_t faces[1]; // 1..n fac
    // starting at &vertices[n_vertices]
    // followed by:
    // int face_groups[1]; // 1..n_face_groups
    // starting at &faces[n_faces]
} mesh_struct;

#endif //PROBEKINS2_H
