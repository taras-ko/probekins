#!/usr/bin/python2

import sysv_ipc
from struct import *
import struct

n_vertices = 5
n_faces = 3
n_face_groups = 9

Hdr = Struct('iii')
Vertex = Struct('dddi')

shm_sz = Hdr.size + Vertex.size

memory = sysv_ipc.SharedMemory(4713, flags=sysv_ipc.IPC_CREAT, size=shm_sz)

hdr = Hdr.pack(n_vertices, n_faces, n_face_groups)
vertex = Vertex.pack(-150.0, 111.0, 2.1, 2)

memory.write(hdr)
memory.write(vertex, Hdr.size)
#(n_vertices, n_faces, n_face_groups) = unpack_from('iii', buf)

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
#wait = input("PRESS ENTER TO CONTINUE")
memory.remove()
