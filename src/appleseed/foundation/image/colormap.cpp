
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2019 Achal Pandey, The appleseedhq Organization
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
#include "colormap.h"

// appleseed.foundation headers.
#include "foundation/image/colorspace.h"
#include "foundation/image/genericimagefilereader.h"

// Standard headers.
#include <cassert>
#include <cstddef>
#include <limits>
#include <memory>
#include <string>

namespace foundation
{

void ColorMap::set_palette_from_array(const float* values, const size_t entry_count)
{
    m_palette.resize(entry_count);

    for (size_t i = 0; i < entry_count; ++i)
    {
        m_palette[i] =
            Color3f(
                values[i * 3 + 0],
                values[i * 3 + 1],
                values[i * 3 + 2]);
    }
}

void ColorMap::set_palette_from_image_file(const std::string& filepath)
{
    GenericImageFileReader reader;
    std::unique_ptr<Image> image(reader.read(filepath.c_str()));

    const size_t image_width = image->properties().m_canvas_width;
    m_palette.resize(image_width);

    for (size_t i = 0; i < image_width; ++i)
        image->get_pixel(i, 0, m_palette[i]);
}

void ColorMap::remap_red_channel(
    Image*              image,
    const AABB2u&       crop_window,
    const float         min_value,
    const float         max_value) const
{
    if (max_value == 0.0f)
    {
        for (size_t y = crop_window.min.y; y <= crop_window.max.y; ++y)
        {
            for (size_t x = crop_window.min.x; x <= crop_window.max.x; ++x)
                image->set_pixel(x, y, evaluate_palette(0.0f));
        }
        return;
    }

    assert(max_value > min_value);

    for (size_t y = crop_window.min.y; y <= crop_window.max.y; ++y)
    {
        for (size_t x = crop_window.min.x; x <= crop_window.max.x; ++x)
        {
            Color3f value;
            image->get_pixel(x, y, value);

            const float c = saturate(fit(value[0], min_value, max_value, 0.0f, 1.0f));

            image->set_pixel(x, y, evaluate_palette(c));
        }
    }
}

void ColorMap::remap_relative_luminance(
    Image*          image,
    const float     min_luminance, 
    const float     max_luminance) const
{
    if (min_luminance == max_luminance)
    {
        for_each_pixel(image, [this](Color4f& color)
        {
            color.rgb() = evaluate_palette(0.0f);
        });
    }

    else
    {
        for_each_pixel(image, [this, min_luminance, max_luminance](Color4f& color)
        {
            const float col_luminance = luminance(color.rgb());

            const float x =
                saturate(
                    inverse_lerp(
                        min_luminance,
                        max_luminance,
                        col_luminance));

                color.rgb() = evaluate_palette(x);
        });
    }
}

Color3f ColorMap::evaluate_palette(float x) const
{
    // When image contains a single channel we do not set the color palette.
    if (m_palette.empty())
        return Color3f(x);

    x *= m_palette.size() - 1;

    const size_t ix = min(truncate<size_t>(x), m_palette.size() - 2);
    const float w = x - ix;

    return lerp(m_palette[ix], m_palette[ix + 1], w);
}

void ColorMap::find_min_max_red_channel(
    const Image*    image,
    const AABB2u&   crop_window,
    float&          min_val,
    float&          max_val)
{
    max_val = 0.0f;
    min_val = 0.0f;

    for (size_t y = crop_window.min.y; y <= crop_window.max.y; ++y)
    {
        for (size_t x = crop_window.min.x; x <= crop_window.max.x; ++x)
        {
            Color3f val;
            image->get_pixel(x, y, val);
            max_val = max(val[0], max_val);
        }
    }
}

void ColorMap::find_min_max_relative_luminance(
    Image*      image,
    float&      min_luminance,
    float&      max_luminance)
{
    Color4f min_color = Color4f(+std::numeric_limits<float>::max());
    Color4f max_color = Color4f(-std::numeric_limits<float>::max());

    for_each_pixel(image, [&min_color, &max_color](Color4f& color)
    {
        min_color = component_wise_min(min_color, color);
        max_color = component_wise_max(max_color, color);
    });

    min_luminance = luminance(min_color.rgb());
    max_luminance = luminance(max_color.rgb());
}

}   // namespace foundation
