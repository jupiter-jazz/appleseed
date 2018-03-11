
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2017 Francois Beaune, The appleseedhq Organization
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
#include "entityfactoryregistrar.h"

// appleseed.renderer headers.
#include "renderer/utility/plugin.h"

// appleseed.foundation headers.
#include "foundation/utility/searchpaths.h"

// Standard headers.
#include <vector>

using namespace renderer;
using namespace std;

namespace renderer
{

struct EntityFactoryRegistrar::Impl
{
    vector<Plugin*> m_plugins;

    ~Impl()
    {
        clear();
    }

    void clear()
    {
        for (Plugin* plugin : m_plugins)
            plugin->release();

        m_plugins.clear();
    }
};

EntityFactoryRegistrar::EntityFactoryRegistrar()
  : impl(new Impl())
{
    //clearing register factory vector
    register_factories.clear();
}

EntityFactoryRegistrar::~EntityFactoryRegistrar()
{
    delete impl;
}

void EntityFactoryRegistrar::unload_all_plugins()
{
    impl->clear();
}

void EntityFactoryRegistrar::store_plugin(Plugin* plugin)
{
    impl->m_plugins.push_back(plugin);
}

}   // namespace renderer
