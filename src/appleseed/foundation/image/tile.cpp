
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2013 Francois Beaune, Jupiter Jazz Limited
// Copyright (c) 2014-2017 Francois Beaune, The appleseedhq Organization
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
#include "tile.h"

using namespace std;

namespace foundation
{

//
// Tile class implementation.
//

Tile::Tile(
    const size_t        width,
    const size_t        height,
    const size_t        channel_count,
    const PixelFormat   pixel_format,
    uint8*              storage)
  : m_width(width)
  , m_height(height)
  , m_channel_count(channel_count)
  , m_pixel_format(pixel_format)
  , m_pixel_count(width * height)
  , m_channel_size(Pixel::size(pixel_format))
  , m_pixel_size(m_channel_size * channel_count)
  , m_array_size(m_pixel_size * m_pixel_count)
{
    assert(m_width > 0);
    assert(m_height > 0);
    assert(m_channel_count > 0);

    if (storage)
    {
         m_pixel_array = storage;
         m_own_storage = false;
    }
    else
    {
        m_pixel_array = new uint8[m_array_size];
        m_own_storage = true;
    }

    m_sub_tiles = new Tile*[4];
    for (int i = 0; i < 4; ++i)
        m_sub_tiles[i] = nullptr;
}

Tile::Tile(
    const Tile&         tile,
    const PixelFormat   pixel_format,
    uint8*              storage)
  : m_width(tile.m_width)
  , m_height(tile.m_height)
  , m_channel_count(tile.m_channel_count)
  , m_pixel_format(pixel_format)
  , m_pixel_count(tile.m_pixel_count)
  , m_channel_size(Pixel::size(pixel_format))
  , m_pixel_size(m_channel_size * m_channel_count)
  , m_array_size(m_pixel_size * m_pixel_count)
{
    if (storage)
    {
         m_pixel_array = storage;
         m_own_storage = false;
    }
    else
    {
        m_pixel_array = new uint8[m_array_size];
        m_own_storage = true;
    }

    Pixel::convert(
        tile.m_pixel_format,                        // source format
        tile.m_pixel_array,                         // source begin
        tile.m_pixel_array + tile.m_array_size,     // source end
        1,                                          // source stride
        m_pixel_format,                             // destination format
        m_pixel_array,                              // destination
        1);                                         // destination stride
}

Tile::Tile(
    const Tile&         tile,
    const PixelFormat   pixel_format,
    const size_t*       shuffle_table,
    uint8*              storage)
  : m_width(tile.m_width)
  , m_height(tile.m_height)
  , m_channel_count(Pixel::get_dest_channel_count(tile.m_channel_count, shuffle_table))
  , m_pixel_format(pixel_format)
  , m_pixel_count(tile.m_pixel_count)
  , m_channel_size(Pixel::size(pixel_format))
  , m_pixel_size(m_channel_size * m_channel_count)
  , m_array_size(m_pixel_size * m_pixel_count)
{
    if (storage)
    {
         m_pixel_array = storage;
         m_own_storage = false;
    }
    else
    {
        m_pixel_array = new uint8[m_array_size];
        m_own_storage = true;
    }

    Pixel::convert_and_shuffle(
        tile.m_pixel_format,                        // source format
        tile.m_channel_count,                       // source channels
        tile.m_pixel_array,                         // source begin
        tile.m_pixel_array + tile.m_array_size,     // source end
        m_pixel_format,                             // destination format
        m_channel_count,                            // destination channels
        m_pixel_array,                              // destination
        shuffle_table);                             // channel shuffling table
}

Tile::Tile(const Tile& rhs)
  : m_width(rhs.m_width)
  , m_height(rhs.m_height)
  , m_channel_count(rhs.m_channel_count)
  , m_pixel_format(rhs.m_pixel_format)
  , m_pixel_count(rhs.m_pixel_count)
  , m_channel_size(rhs.m_channel_size)
  , m_pixel_size(rhs.m_pixel_size)
  , m_array_size(rhs.m_array_size)
{
    m_pixel_array = new uint8[m_array_size];
    m_own_storage = true;

    memcpy(m_pixel_array, rhs.m_pixel_array, m_array_size);
}

Tile::~Tile()
{
    if (m_own_storage)
        delete [] m_pixel_array;
}

void Tile::release()
{
    delete this;
}

size_t Tile::get_memory_size() const
{
    return sizeof(*this) + get_size();
}

void Tile::copy(const Tile& rhs)
{
    assert(m_width == rhs.m_width);
    assert(m_height == rhs.m_height);
    assert(m_channel_count == rhs.m_channel_count);

    Pixel::convert(
        rhs.m_pixel_format,                         // source format
        rhs.m_pixel_array,                          // source begin
        rhs.m_pixel_array + rhs.m_array_size,       // source end
        1,                                          // source stride
        m_pixel_format,                             // destination format
        m_pixel_array,                              // destination
        1);                                         // destination stride
}

void Tile::combine()
{
    //assert(!m_pixel_array);

    //assert(m_sub_tiles[0]);
    //assert(m_sub_tiles[1]);
    //assert(m_sub_tiles[2]);
    //assert(m_sub_tiles[3]);

    if (m_pixel_array)
        return;

    m_pixel_array = new uint8[m_array_size];

    const size_t m_width_half = m_width / 2;
    const size_t m_height_half = m_height / 2;

    for (int sub_tile_index = 0; sub_tile_index < 4; ++sub_tile_index)
    {
        const int sub_tile_x = sub_tile_index % 2;
        const int sub_tile_y = sub_tile_index / 2;

        const size_t sub_tile_width = sub_tile_x == 0 ? m_width_half : m_width - m_width_half;
        const size_t sub_tile_height = sub_tile_y == 0 ? m_height_half : m_height - m_height_half;

        //for (size_t x = 0; x < sub_tile_width; ++x)
        //{
        //    for (size_t y = 0; y < sub_tile_height; ++y)
        //    {
        //        const size_t offset_x = sub_tile_x == 0 ? 0 : m_width_half;
        //        const size_t offset_y = sub_tile_y == 0 ? 0 : m_height_half;

        //        const uint8* sub_pixel = m_sub_tiles[sub_tile_index]->pixel(x, y);

        //        set_pixel(x + offset_x, y + offset_y, sub_pixel);
        //    }
        //}
    }
}

void Tile::split()
{
    //assert(m_pixel_array);

    //assert(!m_sub_tiles[0]);
    //assert(!m_sub_tiles[1]);
    //assert(!m_sub_tiles[2]);
    //assert(!m_sub_tiles[3]);

    if (!m_pixel_array)
        return;

    const size_t m_width_half = m_width / 2;
    const size_t m_height_half = m_height / 2;

    for (int sub_tile_index = 0; sub_tile_index < 4; ++sub_tile_index)
    {
        const int sub_tile_x = sub_tile_index % 2;
        const int sub_tile_y = sub_tile_index / 2;

        const size_t sub_tile_width = sub_tile_x == 0 ? m_width_half : m_width - m_width_half;
        const size_t sub_tile_height = sub_tile_y == 0 ? m_height_half : m_height - m_height_half;

        m_sub_tiles[sub_tile_index] = 
            new Tile(
                sub_tile_width,
                sub_tile_height,
                m_channel_count,
                m_pixel_format);

        //for (size_t x = 0; x < sub_tile_width; ++x)
        //{
        //    for (size_t y = 0; y < sub_tile_height; ++y)
        //    {
        //        const size_t offset_x = sub_tile_x == 0 ? 0 : m_width_half;
        //        const size_t offset_y = sub_tile_y == 0 ? 0 : m_height_half;

        //        const uint8* base_pixel = pixel(x + offset_x, y + offset_y);

        //        m_sub_tiles[sub_tile_index]->set_pixel(x, y, base_pixel);
        //    }
        //}
    }

    delete[] m_pixel_array;
    m_pixel_array = nullptr;
}

}   // namespace foundation
