
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

#pragma once

// appleseed.foundation headers.
#include "foundation/image/color.h"
#include "foundation/image/image.h"
#include "foundation/image/tile.h"
#include "foundation/math/aabb.h"

// Standard headers.
#include <vector>

namespace foundation
{

class ColorMap
{
  public:
  	static void find_min_max_red_channel(
		const Image*    image,
		const AABB2u&   crop_window,
		float&          min_val,
		float&          max_val);

	static void find_min_max_relative_luminance(
		Image*    	image,
		float&  	min_luminance,
		float&    	max_luminance);

    void set_palette_from_array(const float* values, const size_t entry_count);

    void set_palette_from_image_file(const std::string& filepath);

    void remap_red_channel(
		Image*            image,
		const AABB2u&     crop_window,
		const float       min_val,
		const float       max_val) const;
    
    void remap_relative_luminance(
		Image* 			image,
		const float 	min_luminance,
		const float 	max_luminance) const;

	Color3f evaluate_palette(float x) const;

  private:
    template <typename Func>
    static void for_each_pixel(Image* image, const Func& func);

    std::vector<Color3f> m_palette;
};

//
// ColorMap class implementation.
//

template <typename Func>
void ColorMap::for_each_pixel(Image* image, const Func& func)
{
	const foundation::CanvasProperties& image_props = image->properties();

	for (size_t ty = 0; ty < image_props.m_tile_count_y; ++ty)
	{
    	for (size_t tx = 0; tx < image_props.m_tile_count_x; ++tx)
    	{
        	foundation::Tile& tile = image->tile(tx, ty);

        	for (size_t y = 0, th = tile.get_height(); y < th; ++y)
        	{
                for (size_t x = 0, tw = tile.get_width(); x < tw; ++x)
                {
                	foundation::Color4f color;
                    tile.get_pixel(x, y, color);
                    func(color);
                    tile.set_pixel(x, y, color);
                }
            }
        }
    }
}

}   // namespace foundation
