#!/usr/bin/python2

import sysv_ipc
from struct import *
import struct

n_vertices = 5
n_faces = 3
n_face_groups = 9

Hdr = Struct('iii')
Vertex = Struct('dddi')
Face = Struct('iii')

shm_sz = \
    Hdr.size + \
    n_vertices*Vertex.size + \
    n_faces*Face.size + \
    n_face_groups*calcsize('i')
print "Allocating {} bytes in shared memory".format(shm_sz)

memory = sysv_ipc.SharedMemory(4714, flags=sysv_ipc.IPC_CREAT, size=shm_sz)

hdr = Hdr.pack(n_vertices, n_faces, n_face_groups)
vertices = \
    Vertex.pack(-150.0, 111.0, 2.1, 2) + \
    Vertex.pack(-12.0, 180.0, 1.4, 3) + \
    Vertex.pack(-44.0, -121.0, 1.2, 2) + \
    Vertex.pack(210.0, -120.0, 0.2, 1) + \
    Vertex.pack(-100.0, 200.0, 0.5, 1)
faces = \
    Face.pack(0, 1, 2) + \
    Face.pack(1, 3, 2) + \
    Face.pack(4, 1, 0)

face_groups = pack('9i', 0, 2, 0, 1, 2, 0, 1, 1, 2)

offset = 0
memory.write(hdr)

offset = Hdr.size
memory.write(vertices, offset)

offset += n_vertices*Vertex.size
memory.write(faces, offset)

offset += n_faces*Face.size
memory.write(face_groups, offset)

read_buf = memory.read()

(n1,n2,n3) = Hdr.unpack_from(read_buf)
(v1,v2,v3,fgs) = Vertex.unpack_from(read_buf, Hdr.size)

print "Number of vertices: {}".format(n1)
print "Number of faces: {}".format(n2)
print "Number of face groups: {}".format(n3)

#vertex_fmt = 'dddi'
n = 0
##for n in xrange(0,n_vertices):
#(v1,v2,v3,fgs) = unpack_from(vertex_fmt, buf, )
#offset = offset + calcsize(vertex_fmt)
print "Vertex {} ({},{},{}), face group size: {}".format(n, v1,v2,v3,fgs)

raw_input("PRESS ENTER TO CONTINUE")

memory.remove()
