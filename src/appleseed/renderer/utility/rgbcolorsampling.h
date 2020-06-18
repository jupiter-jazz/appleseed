
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

#pragma once

// appleseed.foundation headers.
#include "foundation/image/color.h"

// Standard headers.
#include <cstddef>

// Forward declarations.
namespace foundation    { class Image; }

namespace renderer
{

// Bilinear interpolation of RGB colors.
foundation::Color3f blerp(
    const foundation::Image&        image,
    const foundation::Color3f&      c00,
    const foundation::Color3f&      c10,
    const foundation::Color3f&      c01,
    const foundation::Color3f&      c11,
    const float                     fx,
    const float                     fy);

foundation::Color3f blerp(
    const foundation::Image&    image,
    const std::size_t           x0,
    const std::size_t           y0,
    const std::size_t           x1,
    const std::size_t           y1,
    const float                 fx,
    const float                 fy);

// Returns the weighted average of the four pixels closest to the image coordinate (fx, fy).
// Note: assumes fx and fy are valid coordinates (i.e. they are inside the image boundaries).
foundation::Color3f box_sample(
    const foundation::Image& image,
    const float fx,
    const float fy);

// Returns the weighted average of the four pixels closest to the image coordinate (fx, fy).
// Samples the edge-most pixels when coordinates are outside the image (i.e. texture clamping).
foundation::Color3f clamped_box_sample(
    const foundation::Image& image,
    const float fx,
    const float fy);

//
// Sampling filter from Masaki Kawase's GDC2003 Presentation:
// "Frame Buffer Postprocessing Effects in DOUBLE-S.T.E.A.L (Wreckless)".
//
// References:
//
//   http://www.daionet.gr.jp/~masa/archives/GDC2003_DSTEAL.ppt
//   https://developer.amd.com/wordpress/media/2012/10/Oat-ScenePostprocessing.pdf
//

foundation::Color3f kawase_sample(
    const foundation::Image&    image,
    const std::size_t           x,
    const std::size_t           y,
    const std::size_t           offset);

//
// Sampling filters based on Marius Bjørge's SIGGRAPH2015 Presentation:
// "Bandwidth-Efficient Rendering".
//
// Reference:
//
//   https://community.arm.com/cfs-file/__key/communityserver-blogs-components-weblogfiles/00-00-00-20-66/siggraph2015_2D00_mmg_2D00_marius_2D00_notes.pdf
//
//

foundation::Color3f dual_filter_downsample(
    const foundation::Image&    image,
    const float                 fx,
    const float                 fy);

foundation::Color3f dual_filter_upsample(
    const foundation::Image&    image,
    const float                 fx,
    const float                 fy);

//
// Sampling filters from Jorge Jimenez's SIGGRAPH2014 Presentation:
// "Next Generation Post Processing in Call of Duty: Advanced Warfare".
//
// References:
//
//   http://advances.realtimerendering.com/s2014/#_NEXT_GENERATION_POST
//   http://wwwvis.informatik.uni-stuttgart.de/~kraus/preprints/grapp2007.pdf
//

// 3x3 triangular (Bartlett) filter.
foundation::Color3f box_9tap_upsample(
    const foundation::Image&    image,
    const float                 fx,
    const float                 fy);

// 5x5 custom filter for temporal stability.
foundation::Color3f box_13tap_downsample(
    const foundation::Image&    image,
    const float                 fx,
    const float                 fy);

}   // namespace renderer
