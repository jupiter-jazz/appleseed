
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2019 Jonathan Dent, The appleseedhq Organization
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// appleseed.renderer headers.
#include "renderer/api/object.h"

// appleseed.foundation headers.
#include "foundation/platform/python.h"

// Standard headers.
#include <cstdint>

namespace bpy = boost::python;
using namespace foundation;
using namespace renderer;

namespace
{
    // Blender data structures.
    // https://developer.blender.org/diffusion/B/browse/master/source/blender/makesdna/DNA_meshdata_types.h

    // Blender 2.79 structures.
    struct MFace
    {
        unsigned int v[4];
        short mat_nr;
        char edcode, flag;
    };

    struct MVert
    {
        float co[3];
        short no[3];
        char flag, bweight;
    };

    struct MTFace
    {
        float uv[4][2];
        void *tpage;
        char flag, transp;
        short mode, tile, unwrap;
    };

    // Blender 2.8 structures.
    struct MLoop
    {
        // Vertex index.
        unsigned int v;
        unsigned int e;
    };

    struct MLoopTri
    {
        // Vertex id's.
        unsigned int tri[3];
        unsigned int poly;
    };

    struct MLoopUV
    {
        float uv[2];
        int flag;
    };

    struct MPoly
    {
        int loopstart;
        int totloop;
        short mat_nr;
        char flag, pad;
    };
}


//
// The following function takes a series of pointers to Blender mesh data
// and modifies the appleseed MeshObject entity.
//
//    Arguments:
//
//    blender_mesh: the appleseed MeshObject created earlier in the export process
//    bl_vert_count: the number of vertices in the mesh
//    bl_vert_ptr: a string pointer to the first element of the vertex array
//    (Same template applies to mesh faces.)
//

void export_mesh_blender79(
    MeshObject*         blender_mesh,
    const size_t        bl_vert_count,
    const uintptr_t     bl_vert_ptr,
    const size_t        bl_faces_count,
    const uintptr_t     bl_faces_ptr,
    const uintptr_t     bl_uv_ptr,
    const bool          export_normals,
    const bool          export_uvs)
{
    // Convert uintptr_t numbers to actual pointers.
    const MVert* bl_vertices = reinterpret_cast<MVert*>(bl_vert_ptr);
    const MFace* bl_faces = reinterpret_cast<MFace*>(bl_faces_ptr);
    const MTFace* bl_uv_faces = reinterpret_cast<MTFace*>(bl_uv_ptr);

    blender_mesh->reserve_vertices(bl_vert_count);
    blender_mesh->reserve_triangles(bl_faces_count);

    // Push vertices.
    for (size_t vertex_index = 0; vertex_index < bl_vert_count; ++vertex_index)
    {
        const MVert& vert = bl_vertices[vertex_index];
        blender_mesh->push_vertex(GVector3(vert.co[0], vert.co[1], vert.co[2]));
    }

    // Push normals.
    if (export_normals)
    {
        blender_mesh->reserve_vertex_normals(bl_vert_count);
        for (size_t vertex_index = 0; vertex_index < bl_vert_count; ++vertex_index)
        {
            const MVert& vert = bl_vertices[vertex_index];
            blender_mesh->push_vertex_normal(GVector3(vert.no[0], vert.no[1], vert.no[2]));
        }
    }

    // Push triangles.
    for (size_t face_index = 0; face_index < bl_faces_count; ++face_index)
    {
        const MFace& face = bl_faces[face_index];
        blender_mesh->push_triangle(Triangle(face.v[0], face.v[1], face.v[2], face.mat_nr));
    }

    // Tie vertex normals to mesh faces.
    if (export_normals)
    {
        for (size_t face_index = 0; face_index < bl_faces_count; ++face_index)
        {
            const MFace& face = bl_faces[face_index];
            Triangle& tri = blender_mesh->get_triangle(face_index);
            tri.m_n0 = face.v[0];
            tri.m_n1 = face.v[1];
            tri.m_n2 = face.v[2];
        }
    }

    // Tie uv coordinates to mesh faces.
    if (export_uvs)
    {
        uint32 uv_vertex_index = 0;
        blender_mesh->reserve_tex_coords(bl_vert_count);
        for (size_t face_index = 0; face_index < bl_faces_count; ++face_index)
        {
            const MTFace& tex_face = bl_uv_faces[face_index];
            Triangle& tri = blender_mesh->get_triangle(face_index);
            blender_mesh->push_tex_coords(GVector2(tex_face.uv[0][0], tex_face.uv[0][1]));
            tri.m_a0 = uv_vertex_index++;

            blender_mesh->push_tex_coords(GVector2(tex_face.uv[1][0], tex_face.uv[1][1]));
            tri.m_a1 = uv_vertex_index++;

            blender_mesh->push_tex_coords(GVector2(tex_face.uv[2][0], tex_face.uv[2][1]));
            tri.m_a2 = uv_vertex_index++;
        }
    }
}

void export_mesh_blender79_pose(
    MeshObject*         blender_mesh,
    const size_t        pose,
    const size_t        bl_vert_count,
    const uintptr_t     bl_vert_ptr,
    const bool          export_normals)
{
    // Convert uintptr_t numbers to actual pointers.
    const MVert* bl_vertices = reinterpret_cast<MVert*>(bl_vert_ptr);

    // Push vertices.
    for (size_t vertex_index = 0; vertex_index < bl_vert_count; ++vertex_index)
    {
        const MVert& vert = bl_vertices[vertex_index];
        blender_mesh->set_vertex_pose(vertex_index, pose, GVector3(vert.co[0], vert.co[1], vert.co[2]));
    }

    // Push normals.
    if (export_normals)
    {
        for (size_t vertex_index = 0; vertex_index < bl_vert_count; ++vertex_index)
        {
            const MVert& vert = bl_vertices[vertex_index];
            blender_mesh->set_vertex_normal_pose(vertex_index, pose, GVector3(vert.no[0], vert.no[1], vert.no[2]));
        }
    }
}

void export_mesh_blender80(
    MeshObject*         blender_mesh,
    const size_t        bl_looptri_count,
    const uintptr_t     bl_looptri_ptr,
    const size_t        bl_loop_count,
    const uintptr_t     bl_loops_ptr,
    const uintptr_t     bl_polys_ptr,
    const uintptr_t     bl_vertices_ptr,
    const uintptr_t     bl_loops_uv_ptr,
    const bool          export_normals,
    const bool          export_uvs)
{
    // Convert uintptr_t numbers to actual pointers.
    const MLoopTri* bl_looptri_array = reinterpret_cast<MLoopTri*>(bl_looptri_ptr);
    const MLoop* bl_loop_array = reinterpret_cast<MLoop*>(bl_loops_ptr);
    const MPoly* bl_poly_array = reinterpret_cast<MPoly*>(bl_polys_ptr);
    const MVert* bl_vert_array = reinterpret_cast<MVert*>(bl_vertices_ptr);
    const MLoopUV* bl_loop_uv_array = reinterpret_cast<MLoopUV*>(bl_loops_uv_ptr);

    // Push vertices.
    blender_mesh->reserve_vertices(bl_loop_count);

    for (size_t loop_index = 0; loop_index < bl_loop_count; ++loop_index)
    {
        const MLoop& bl_loop = bl_loop_array[loop_index];
        const MVert& bl_vert = bl_vert_array[bl_loop.v];
        blender_mesh->push_vertex(GVector3(bl_vert.co[0], bl_vert.co[1], bl_vert.co[2]));
    }

    // Push faces.
    blender_mesh->reserve_triangles(bl_looptri_count);

    for (size_t looptri_index = 0; looptri_index < bl_looptri_count; ++looptri_index)
    {
        const MLoopTri& bl_looptri = bl_looptri_array[looptri_index];
        short mat_index = bl_poly_array[bl_looptri.poly].mat_nr;
        blender_mesh->push_triangle(Triangle(bl_looptri.tri[0], bl_looptri.tri[1], bl_looptri.tri[2], mat_index));
    }

    // Push normals.
    if (export_normals)
    {
        for (size_t loop_index = 0; loop_index < bl_loop_count; ++loop_index)
        {
            const MLoop& bl_loop = bl_loop_array[loop_index];
            const MVert& bl_vert = bl_vert_array[bl_loop.v];
            blender_mesh->push_vertex_normal(normalize(GVector3(bl_vert.no[0], bl_vert.no[1], bl_vert.no[2])));
        }

        for (size_t looptri_index = 0; looptri_index < bl_looptri_count; ++looptri_index)
        {
            const MLoopTri& bl_looptri = bl_looptri_array[looptri_index];
            Triangle& as_tri = blender_mesh->get_triangle(looptri_index);
            as_tri.m_n0 = bl_looptri.tri[0];
            as_tri.m_n1 = bl_looptri.tri[1];
            as_tri.m_n2 = bl_looptri.tri[2];
        }
    }

    // Push UV coordinates.
    if (export_uvs)
    {
        for (size_t loop_index = 0; loop_index < bl_loop_count; ++loop_index)
        {
            const MLoopUV& bl_loop_uv = bl_loop_uv_array[loop_index];
            blender_mesh->push_tex_coords(GVector2(bl_loop_uv.uv[0], bl_loop_uv.uv[1]));
        }

        for (size_t looptri_index = 0; looptri_index < bl_looptri_count; ++looptri_index)
        {
            const MLoopTri& bl_looptri = bl_looptri_array[looptri_index];
            Triangle& as_tri = blender_mesh->get_triangle(looptri_index);
            as_tri.m_a0 = bl_looptri.tri[0];
            as_tri.m_a1 = bl_looptri.tri[1];
            as_tri.m_a2 = bl_looptri.tri[2];
        }
    }
}

void export_mesh_blender80_pose(
    MeshObject*         blender_mesh,
    const size_t        pose,
    const uintptr_t     bl_loop_ptr,
    const size_t        bl_loop_count,
    const uintptr_t     bl_vert_ptr,
    const bool          export_normals)
{
    // Convert uintptr_t numbers to actual pointers.
    const MVert* bl_vert_array = reinterpret_cast<MVert*>(bl_vert_ptr);
    const MLoop* bl_loop_array = reinterpret_cast<MLoop*>(bl_loop_ptr);

    // Push vertices.
    for (size_t loop_index = 0; loop_index < bl_loop_count; ++loop_index)
    {
        const MLoop& bl_loop = bl_loop_array[loop_index];
        const MVert& bl_vert = bl_vert_array[bl_loop.v];
        blender_mesh->set_vertex_pose(loop_index, pose, GVector3(bl_vert.co[0], bl_vert.co[1], bl_vert.co[2]));
    }

    // Push normals.
    if (export_normals)
    {
        for (size_t loop_index = 0; loop_index < bl_loop_count; ++loop_index)
        {
            const MLoop& bl_loop = bl_loop_array[loop_index];
            const MVert& bl_vert = bl_vert_array[bl_loop.v];
            blender_mesh->set_vertex_normal_pose(loop_index, pose, normalize(GVector3(bl_vert.no[0], bl_vert.no[1], bl_vert.no[2])));
        }
    }
}

void bind_blender_mesh_converter()
{
    bpy::def("export_mesh_blender79", &export_mesh_blender79);
    bpy::def("export_mesh_blender79_pose", &export_mesh_blender79_pose);
    bpy::def("export_mesh_blender80", &export_mesh_blender80);
    bpy::def("export_mesh_blender80_pose", &export_mesh_blender80_pose);
}
