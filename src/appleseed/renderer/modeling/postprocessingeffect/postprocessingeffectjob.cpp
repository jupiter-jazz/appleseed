
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2020 Tiago Chaves, The appleseedhq Organization
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
#include "postprocessingeffectjob.h"

// appleseed.renderer headers.
#include "renderer/modeling/frame/frame.h"

// appleseed.foundation headers.
#include "foundation/image/canvasproperties.h"
#include "foundation/image/image.h"
#include "foundation/image/tile.h"
#include "foundation/math/ordering.h"

// Standard headers.
#include <cassert>
#include <exception>

using namespace foundation;

namespace renderer
{

//
// EffectJob class implementation.
//

EffectJob::EffectJob(
    const EffectApplierVector&  effect_appliers,
    const Frame&                frame,
    const size_t                tile_x,
    const size_t                tile_y,
    const size_t                thread_count,
    IAbortSwitch&               abort_switch)
  : m_effect_appliers(effect_appliers)
  , m_frame(frame)
  , m_tile_x(tile_x)
  , m_tile_y(tile_y)
  , m_thread_count(thread_count)
  , m_abort_switch(abort_switch)
{
}

void EffectJob::execute(const size_t thread_index)
{
    assert(thread_index < m_effect_appliers.size());

    // Apply the post-processing effect to the tile.
    m_effect_appliers[thread_index]->apply(
        m_frame,
        m_tile_x,
        m_tile_y,
        m_abort_switch);
}


//
// EffectJobFactory class implementation.
//

void EffectJobFactory::create(
    const Frame&                            frame,
    const EffectJob::EffectApplierVector&   effect_appliers,
    const size_t                            thread_count,
    EffectJobVector&                        effect_jobs,
    IAbortSwitch&                           abort_switch)
{
    // Retrieve frame properties.
    const CanvasProperties& props = frame.image().properties();

    // Generate tiles in linear order.
    std::vector<size_t> tiles;
    linear_ordering(tiles, props.m_tile_count);

    // Make sure the right number of tiles was created.
    assert(tiles.size() == props.m_tile_count);

    // Create effect jobs, one per tile.
    for (size_t i = 0; i < props.m_tile_count; ++i)
    {
        // Compute coordinates of the tile in the frame.
        const size_t tile_index = tiles[i];
        const size_t tile_x = tile_index % props.m_tile_count_x;
        const size_t tile_y = tile_index / props.m_tile_count_x;
        assert(tile_x < props.m_tile_count_x);
        assert(tile_y < props.m_tile_count_y);

        // Create the tile job.
        effect_jobs.push_back(
            new EffectJob(
                effect_appliers,
                frame,
                tile_x,
                tile_y,
                thread_count,
                abort_switch));
    }
}

}   // namespace renderer
