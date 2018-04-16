
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2013 Francois Beaune, Jupiter Jazz Limited
// Copyright (c) 2014-2018 Francois Beaune, The appleseedhq Organization
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

#ifndef APPLESEED_RENDERER_MODELING_ENVIRONMENTSHADER_ENVIRONMENTSHADERTRAITS_H
#define APPLESEED_RENDERER_MODELING_ENVIRONMENTSHADER_ENVIRONMENTSHADERTRAITS_H

// appleseed.renderer headers.
#include "renderer/modeling/entity/entitytraits.h"
#include "renderer/modeling/environmentshader/environmentshader.h"
#include "renderer/modeling/scene/containers.h"

// appleseed.foundation headers.
#include "foundation/utility/autoreleaseptr.h"
#include "foundation/utility/containers/dictionary.h"

// Forward declarations.
namespace renderer  { class EnvironmentShaderFactoryRegistrar; }

namespace renderer
{

//
// Environment shader entity traits.
//

template <>
struct EntityTraits<EnvironmentShader>
{
    typedef EnvironmentShaderContainer ContainerType;
    typedef EnvironmentShaderFactoryRegistrar FactoryRegistrarType;

    static const char* get_entity_type_name()                           { return "environment_shader"; }
    static const char* get_human_readable_entity_type_name()            { return "Environment Shader"; }
    static const char* get_human_readable_collection_type_name()        { return "Environment Shaders"; }

    template <typename ParentEntity>
    static ContainerType& get_entity_container(ParentEntity& parent)    { return parent.environment_shaders(); }

    static foundation::Dictionary get_entity_values(const EnvironmentShader* entity)
    {
        return entity->get_parameters();
    }

    template <typename ParentEntity>
    static void insert_entity(
        foundation::auto_release_ptr<EnvironmentShader> entity,
        ParentEntity&                                   parent)
    {
        get_entity_container(parent).insert(entity);
    }

    template <typename ParentEntity>
    static void remove_entity(
        EnvironmentShader*                              entity,
        ParentEntity&                                   parent)
    {
        get_entity_container(parent).remove(entity);
    }
};

}       // namespace renderer

#endif  // !APPLESEED_RENDERER_MODELING_ENVIRONMENTSHADER_ENVIRONMENTSHADERTRAITS_H
