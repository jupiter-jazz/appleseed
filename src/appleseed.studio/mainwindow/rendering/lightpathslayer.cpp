
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2018 Francois Beaune, The appleseedhq Organization
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

// Interface header.
#include "lightpathslayer.h"

// appleseed.studio headers.
#include "utility/miscellaneous.h"

// appleseed.renderer headers.
#include "renderer/api/camera.h"
#include "renderer/api/entity.h"
#include "renderer/api/object.h"
#include "renderer/api/project.h"
#include "renderer/api/rasterization.h"
#include "renderer/api/scene.h"
#include "renderer/api/utility.h"

// appleseed.foundation headers.
#include "foundation/image/color.h"
#include "foundation/image/colorspace.h"
#include "foundation/math/scalar.h"
#include "foundation/utility/api/apistring.h"
#include "foundation/utility/string.h"

// Qt headers.
#include <QKeyEvent>
#include <QOpenGLFunctions_3_3_Core>
#include <QSurfaceFormat>
#include <QString>

// Standard headers.
#include <algorithm>
#include <cmath>
#include <array>

using namespace foundation;
using namespace renderer;
using namespace std;

namespace appleseed {
namespace studio {

namespace {
    // Byte stride of a vec3.
    const size_t Vec3ByteStride = sizeof(float) * 3;

    // Struct of an element of the "others" vertex buffer
    struct OtherAttributes {
        GLfloat v_luminance;
        GLint   v_distance_start_end;
        GLfloat v_color[3];
        GLfloat v_surface_normal[3];
    };
}

LightPathsLayer::LightPathsLayer(
    const Project&             project,
    const size_t               width,
    const size_t               height)
  : m_project(project)
  , m_camera(*m_project.get_uncached_active_camera())
  , m_backface_culling_enabled(false)
  , m_selected_light_path_index(-1)
  , m_gl_initialized(false)
  , m_width(width)
  , m_height(height)
  , m_max_thickness(4.0f)
  , m_min_thickness(1.0f)
  , m_max_luminance(0.0f)
{

    const float time = m_camera.get_shutter_middle_time();
    set_transform(m_camera.transform_sequence().evaluate(time));
}

void LightPathsLayer::resize(const size_t width, const size_t height)
{
    m_width = width,
    m_height = height;
}

void LightPathsLayer::set_gl_functions(QOpenGLFunctions_3_3_Core* functions)
{
    m_gl = functions;
}

void LightPathsLayer::update_render_camera_transform()
{
    const float time = m_camera.get_shutter_middle_time();
    m_render_camera_matrix = m_camera.transform_sequence().evaluate(time).get_parent_to_local();
    m_gl_render_view_matrix = transpose(m_render_camera_matrix);
}

void LightPathsLayer::set_transform(const Transformd& transform)
{
    m_camera_matrix = transform.get_parent_to_local();
    m_gl_view_matrix = transpose(m_camera_matrix);
}

void LightPathsLayer::set_light_paths(const LightPathArray& light_paths)
{
    m_light_paths = light_paths;

    if (m_light_paths.size() > 1)
    {
        // Sort paths by descending radiance at the camera.
        const auto& light_path_recorder = m_project.get_light_path_recorder();
        sort(
            &m_light_paths[0],
            &m_light_paths[0] + m_light_paths.size(),
            [&light_path_recorder](const LightPath& lhs, const LightPath& rhs)
            {
                LightPathVertex lhs_v;
                light_path_recorder.get_light_path_vertex(lhs.m_vertex_end_index - 1, lhs_v);

                LightPathVertex rhs_v;
                light_path_recorder.get_light_path_vertex(rhs.m_vertex_end_index - 1, rhs_v);

                return
                    sum_value(Color3f::from_array(lhs_v.m_radiance)) >
                    sum_value(Color3f::from_array(rhs_v.m_radiance));
            });
    }

    // Display all paths by default.
    set_selected_light_path_index(-1);
    load_light_paths_data();
}

void LightPathsLayer::set_selected_light_path_index(const int selected_light_path_index)
{
    m_selected_light_path_index = selected_light_path_index;

    dump_selected_light_path();

    emit signal_light_path_selection_changed(
        m_selected_light_path_index,
        static_cast<int>(m_light_paths.size()));
}

void LightPathsLayer::slot_display_all_light_paths()
{
    if (m_selected_light_path_index > -1)
        set_selected_light_path_index(-1);
}

void LightPathsLayer::slot_display_previous_light_path()
{
    if (m_selected_light_path_index > -1)
        set_selected_light_path_index(m_selected_light_path_index - 1);
}

void LightPathsLayer::slot_display_next_light_path()
{
    if (m_selected_light_path_index < static_cast<int>(m_light_paths.size()) - 1)
        set_selected_light_path_index(m_selected_light_path_index + 1);
}

void LightPathsLayer::slot_toggle_backface_culling(const bool checked)
{
    m_backface_culling_enabled = checked;
}

void LightPathsLayer::slot_synchronize_camera()
{
    m_camera.transform_sequence().clear();
    m_camera.transform_sequence().set_transform(0.0f,
        Transformd::from_local_to_parent(inverse(m_camera_matrix)));
}

void LightPathsLayer::load_light_paths_data()
{
    m_index_offsets.clear();
    m_max_luminance = 0.0f;
    if (!m_light_paths.empty())
    {
        m_index_offsets.push_back(0);

        const auto& light_path_recorder = m_project.get_light_path_recorder();

        GLsizei total_index_count = 0;

        // Vertex count * 4 since we will be adding two vertices for each point along the line and will
        // be adding each point twice for the beginning and end parts of each segment
        // Add two because we need extra at the front and back for the extra 'previous' and 'next' attributes
        const size_t total_gl_vertex_count = 2 * (light_path_recorder.get_vertex_count() + 2);

        vector<unsigned int> indices;
        vector<float> positions_buffer;
        // * 3 since we want 3 floats per position
        positions_buffer.reserve(total_gl_vertex_count * 3);
        indices.reserve(total_gl_vertex_count);

        // Add an empty vertex at the beginning to serve as first 'previous'
        array<float, 6> empty_positions =
        {
            0.0, 0.0, 0.0,
            0.0, 0.0, 0.0,
        };
        positions_buffer.insert(
            positions_buffer.end(),
            empty_positions.begin(),
            empty_positions.end());
        
        vector<OtherAttributes> others_buffer;
        others_buffer.reserve(total_gl_vertex_count);
        array<OtherAttributes, 4> others;

        for (size_t light_path_idx = 0; light_path_idx < m_light_paths.size(); light_path_idx++)
        {
            const auto& path = m_light_paths[light_path_idx];
            assert(path.m_vertex_end_index - path.m_vertex_begin_index >= 2);

            LightPathVertex prev;
            light_path_recorder.get_light_path_vertex(path.m_vertex_begin_index, prev);
            for (size_t vertex_idx = path.m_vertex_begin_index + 1; vertex_idx < path.m_vertex_end_index; vertex_idx++)
            {
                LightPathVertex vert;
                light_path_recorder.get_light_path_vertex(vertex_idx, vert);

                auto piece_radiance = Color3f::from_array(vert.m_radiance);
                piece_radiance = linear_rgb_to_srgb(piece_radiance);
                auto piece_luminance = luminance(piece_radiance);
                m_max_luminance = max(m_max_luminance, piece_luminance);
                piece_radiance /= piece_luminance;
                //piece_radiance /= piece_radiance + Color3f(1.0);

                array<GLfloat, 12> positions_temp_store = {
                    prev.m_position[0], prev.m_position[1], prev.m_position[2],
                    prev.m_position[0], prev.m_position[1], prev.m_position[2],
                    vert.m_position[0], vert.m_position[1], vert.m_position[2],
                    vert.m_position[0], vert.m_position[1], vert.m_position[2],
                };
                positions_buffer.insert(
                    positions_buffer.end(),
                    positions_temp_store.begin(),
                    positions_temp_store.end());

                unsigned int start_index = static_cast<unsigned int>(others_buffer.size());
                
                GLint start = 2;
                GLint end = 4;

                others = {{
                    {
                        piece_luminance,
                        start,
                        {piece_radiance[0], piece_radiance[1], piece_radiance[2]},
                        {prev.m_surface_normal[0], prev.m_surface_normal[1], prev.m_surface_normal[2]}
                    },
                    {
                        piece_luminance,
                        1 | start,
                        {piece_radiance[0], piece_radiance[1], piece_radiance[2]},
                        {prev.m_surface_normal[0], prev.m_surface_normal[1], prev.m_surface_normal[2]}
                    },
                    {
                        piece_luminance,
                        end,
                        {piece_radiance[0], piece_radiance[1], piece_radiance[2]},
                        {vert.m_surface_normal[0], vert.m_surface_normal[1], vert.m_surface_normal[2]}
                    },
                    {
                        piece_luminance,
                        1 | end,
                        {piece_radiance[0], piece_radiance[1], piece_radiance[2]},
                        {vert.m_surface_normal[0], vert.m_surface_normal[1], vert.m_surface_normal[2]}
                    },
                }};
                others_buffer.insert(
                    others_buffer.end(),
                    others.begin(),
                    others.end());

                array<unsigned int, 6> indices_scratch = {
                    start_index, start_index + 1, start_index + 2,
                    start_index + 1, start_index + 2, start_index + 3,
                };
                indices.insert(
                    indices.end(),
                    indices_scratch.begin(),
                    indices_scratch.end());

                total_index_count += 6;
                prev = vert;
            }
            m_index_offsets.push_back(total_index_count);
        }

        // Add an empty vertex at the end to serve as last 'next'
        positions_buffer.insert(
            positions_buffer.end(),
            empty_positions.begin(),
            empty_positions.end());
        
        // Upload the data to the buffers
        m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_positions_vbo);
        m_gl->glBufferData(
            GL_ARRAY_BUFFER,
            positions_buffer.size() * sizeof(float),
            reinterpret_cast<const GLvoid*>(&positions_buffer[0]),
            GL_STATIC_DRAW);
        m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_others_vbo);
        m_gl->glBufferData(
            GL_ARRAY_BUFFER,
            others_buffer.size() * sizeof(OtherAttributes),
            reinterpret_cast<const GLvoid*>(&others_buffer[0]),
            GL_STATIC_DRAW);
        m_gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices_ebo);
        m_gl->glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(unsigned int),
            reinterpret_cast<const GLvoid*>(&indices[0]),
            GL_STATIC_DRAW);
    }
}

namespace {
    const string shader_kind_to_string(const GLint shader_kind)
    {
        switch (shader_kind) {
            case GL_VERTEX_SHADER:
                return "Vertex";
            case GL_FRAGMENT_SHADER:
                return "Fragment";
            default:
                return "Unknown Kind";
        }
    }

    void compile_shader(
        QOpenGLFunctions_3_3_Core* f,
        const GLuint               shader,
        const GLsizei              count,
        const GLchar**             src_string,
        const GLint*               length)
    {
        f->glShaderSource(shader, count, src_string, length);
        f->glCompileShader(shader);
        GLint success;
        f->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            char info_log[1024];
            f->glGetShaderInfoLog(shader, 1024, NULL, info_log);

            GLint shader_kind;
            f->glGetShaderiv(shader, GL_SHADER_TYPE, &shader_kind);
            string shader_kind_string = shader_kind_to_string(shader_kind);

            RENDERER_LOG_ERROR("opengl: %s shader compilation failed:\n%s", shader_kind_string.c_str(), info_log);
        }
    }

    void link_shader_program(
        QOpenGLFunctions_3_3_Core*  f,
        const GLuint                program,
        const GLuint                vert,
        const GLuint                frag)
    {
        f->glAttachShader(program, vert);
        f->glAttachShader(program, frag);
        f->glLinkProgram(program);

        GLint success;
        f->glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (!success)
        {
            char info_log[1024];
            f->glGetProgramInfoLog(program, 1024, NULL, info_log);
            RENDERER_LOG_ERROR("opengl: shader program linking failed:\n%s", info_log);
        }
    }

    void create_shader_program(
        QOpenGLFunctions_3_3_Core*  f,
        GLuint&                     program,
        const QByteArray&               vert_source,
        const QByteArray&               frag_source)
    {
        GLuint vert = f->glCreateShader(GL_VERTEX_SHADER);
        GLuint frag = f->glCreateShader(GL_FRAGMENT_SHADER);

        auto gl_vert_source = static_cast<const GLchar*>(vert_source.constData());
        auto gl_vert_source_length = static_cast<const GLint>(vert_source.size());

        auto gl_frag_source = static_cast<const GLchar*>(frag_source.constData());
        auto gl_frag_source_length = static_cast<const GLint>(frag_source.size());

        compile_shader(f, vert, 1, &gl_vert_source, &gl_vert_source_length);
        compile_shader(f, frag, 1, &gl_frag_source, &gl_frag_source_length);

        program = f->glCreateProgram();
        link_shader_program(f, program, vert, frag);

        f->glDeleteShader(vert);
        f->glDeleteShader(frag);
    }
}

void LightPathsLayer::init_gl(QSurfaceFormat format)
{
    // If there was already previous data, clean up
    LightPathsLayer::cleanup_gl_data();

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    auto vertex_shader = load_gl_shader("lightpaths.vert");
    auto fragment_shader = load_gl_shader("lightpaths.frag");

    create_shader_program(
        m_gl,
        m_shader_program,
        vertex_shader,
        fragment_shader);

    m_view_mat_loc = m_gl->glGetUniformLocation(m_shader_program, "u_view");
    m_proj_mat_loc = m_gl->glGetUniformLocation(m_shader_program, "u_proj");
    m_res_loc = m_gl->glGetUniformLocation(m_shader_program, "u_res");
    m_max_luminance_loc = m_gl->glGetUniformLocation(m_shader_program, "u_max_luminance");
    m_max_thickness_loc = m_gl->glGetUniformLocation(m_shader_program, "u_max_thickness");
    m_min_thickness_loc = m_gl->glGetUniformLocation(m_shader_program, "u_min_thickness");

    const float z_near = 0.01f;
    const float z_far = 1000.0f;

    const auto& rc = m_camera.get_rasterization_camera();

    const float fy = tan(rc.m_hfov / rc.m_aspect_ratio * 0.5) * z_near;
    const float fx = fy * rc.m_aspect_ratio;

    const float shift_x = rc.m_shift_x * 2.0 * fx;
    const float shift_y = rc.m_shift_y * 2.0 * fy;

    const float left   = -fx + shift_x;
    const float right  =  fx + shift_x;
    const float top    = -fy + shift_y;
    const float bottom =  fy + shift_y;

    // Top and bottom are flipped because QOpenGLWidget draws to a framebuffer object and then blits
    // from the FBO to the default framebuffer, which flips the image.
    m_gl_proj_matrix = transpose(Matrix4f::make_frustum(top, bottom, left, right, z_near, z_far));

    GLuint temp_light_paths_vao = 0;
    m_gl->glGenVertexArrays(1, &temp_light_paths_vao);
    m_light_paths_vao = temp_light_paths_vao;

    GLuint temp_vbos[3] = {0, 0, 0};
    m_gl->glGenBuffers(3, &temp_vbos[0]);
    m_positions_vbo = temp_vbos[0];
    m_others_vbo = temp_vbos[1];
    m_indices_ebo = temp_vbos[2];

    m_gl->glBindVertexArray(m_light_paths_vao);

    m_gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices_ebo);

    // v_previous, v_position, and v_next all reference offsets into the same vertex data buffer.
    // v_previous is at vertex offset 0, v_position is at vertex offset 2 and v_next is at vertex offset 4.
    // See http://codeflow.org/entries/2012/aug/05/webgl-rendering-of-solid-trails/ for reference.
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_positions_vbo);

    // vec3 v_previous;
    m_gl->glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        Vec3ByteStride,
        reinterpret_cast<const GLvoid*>(0));
    m_gl->glEnableVertexAttribArray(0);

    // vec3 v_position;
    m_gl->glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        Vec3ByteStride,
        reinterpret_cast<const GLvoid*>(Vec3ByteStride * 2));
    m_gl->glEnableVertexAttribArray(1);

    // vec3 v_next;
    m_gl->glVertexAttribPointer(
        2,
        3,
        GL_FLOAT,
        GL_FALSE,
        Vec3ByteStride,
        reinterpret_cast<const GLvoid*>(Vec3ByteStride * 4));
    m_gl->glEnableVertexAttribArray(2);

    // The rest of the attributes are stored in a separate data buffer, interleaved.
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_others_vbo);

    // float v_thickness;
    m_gl->glVertexAttribPointer(
        3,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(OtherAttributes),
        reinterpret_cast<const GLvoid*>(0));
    m_gl->glEnableVertexAttribArray(3);

    // This attribute stores 3 bools:
    // bit 0: normal direction which maps to -1.0 (false) or 1.0 (true)
    // bit 1: whether this is the start vertex of a new path
    // bit 2: whether this is the end vertex of a path
    // int v_direction_start_end;
    m_gl->glVertexAttribIPointer(
        4,
        1,
        GL_INT,
        sizeof(OtherAttributes),
        reinterpret_cast<const GLvoid*>(sizeof(GLfloat)));
    m_gl->glEnableVertexAttribArray(4);

    // vec3 v_color;
    m_gl->glVertexAttribPointer(
        5,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(OtherAttributes),
        reinterpret_cast<const GLvoid*>(sizeof(GLfloat) + sizeof(GLint)));
    m_gl->glEnableVertexAttribArray(5);

    // vec3 v_surface_normal;
    m_gl->glVertexAttribPointer(
        6,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(OtherAttributes),
        reinterpret_cast<const GLvoid*>(sizeof(GLfloat) + sizeof(GLint) + 3 * sizeof(GLfloat)));
    m_gl->glEnableVertexAttribArray(6);

    m_gl->glBindVertexArray(0);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, 0);

    load_light_paths_data();

    m_gl_initialized = true;
}

void LightPathsLayer::cleanup_gl_data()
{
    if (m_shader_program != 0)
    {
        m_gl->glDeleteProgram(m_shader_program);
    }
}

void LightPathsLayer::draw_render_camera() const
{
    auto gl_view_matrix = const_cast<const GLfloat*>(&m_gl_render_view_matrix[0]);

    m_gl->glEnable(GL_BLEND);
    m_gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    render_scene(gl_view_matrix);
    m_gl->glDisable(GL_BLEND);
}

void LightPathsLayer::draw() const
{
    auto gl_view_matrix = const_cast<const GLfloat*>(&m_gl_view_matrix[0]);

    m_gl->glEnable(GL_BLEND);
    m_gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    render_scene(gl_view_matrix);
    m_gl->glDisable(GL_BLEND);
}

void LightPathsLayer::render_scene(const GLfloat* gl_view_matrix) const
{
    if (!m_gl_initialized)
        return;

    if (m_backface_culling_enabled)
        m_gl->glEnable(GL_CULL_FACE);
    else m_gl->glDisable(GL_CULL_FACE);

    m_gl->glDepthMask(GL_TRUE);
    m_gl->glEnable(GL_DEPTH_TEST);
    m_gl->glDepthFunc(GL_LEQUAL);

    if (m_index_offsets.size() > 1)
    {
        m_gl->glUseProgram(m_shader_program);

        m_gl->glUniformMatrix4fv(
            m_view_mat_loc,
            1,
            false,
            gl_view_matrix);
        m_gl->glUniformMatrix4fv(
            m_proj_mat_loc,
            1,
            false,
            const_cast<const GLfloat*>(&m_gl_proj_matrix[0]));
        m_gl->glUniform1f(
            m_max_luminance_loc,
            m_max_luminance);
        m_gl->glUniform1f(
            m_max_thickness_loc,
            m_max_thickness);
        m_gl->glUniform1f(
            m_min_thickness_loc,
            m_min_thickness);
        m_gl->glUniform2f(
            m_res_loc,
            (GLfloat)m_width,
            (GLfloat)m_height);

        m_gl->glBindVertexArray(m_light_paths_vao);

        GLint first;
        GLsizei count;
        assert(m_selected_light_path_index >= -1);
        if (m_selected_light_path_index == -1)
        {
            first = 0;
            count = m_index_offsets[m_index_offsets.size() - 1] / 3;
        }
        else
        {
            first = static_cast<GLint>(m_index_offsets[m_selected_light_path_index]);
            count = m_index_offsets[m_selected_light_path_index + 1] - first;
        }
        m_gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices_ebo);
        m_gl->glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(first * sizeof(unsigned int)));
        m_gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void LightPathsLayer::dump_selected_light_path() const
{
    if (m_selected_light_path_index == -1)
    {
        if (m_light_paths.empty())
            RENDERER_LOG_INFO("no light path to display.");
        else
        {
            RENDERER_LOG_INFO("displaying all %s light path%s.",
                pretty_uint(m_light_paths.size()).c_str(),
                m_light_paths.size() > 1 ? "s" : "");
        }
    }
    else
    {
        RENDERER_LOG_INFO("displaying light path %s:",
            pretty_int(m_selected_light_path_index + 1).c_str());

        const auto& light_path_recorder = m_project.get_light_path_recorder();
        const auto& path = m_light_paths[m_selected_light_path_index];

        for (size_t i = path.m_vertex_begin_index; i < path.m_vertex_end_index; ++i)
        {
            LightPathVertex v;
            light_path_recorder.get_light_path_vertex(i, v);

            const string entity_name =
                v.m_entity != nullptr
                    ? foundation::format("\"{0}\"", v.m_entity->get_path().c_str())
                    : "n/a";

            RENDERER_LOG_INFO("  vertex " FMT_SIZE_T ": entity: %s - position: (%f, %f, %f) - radiance: (%f, %f, %f) - total radiance: %f",
                i - path.m_vertex_begin_index + 1,
                entity_name.c_str(),
                v.m_position[0], v.m_position[1], v.m_position[2],
                v.m_radiance[0], v.m_radiance[1], v.m_radiance[2],
                v.m_radiance[0] + v.m_radiance[1] + v.m_radiance[2]);
        }
    }
}

}   // namespace studio
}   // namespace appleseed
