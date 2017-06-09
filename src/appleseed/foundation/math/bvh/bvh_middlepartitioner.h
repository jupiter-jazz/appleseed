
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

#ifndef APPLESEED_FOUNDATION_MATH_BVH_BVH_MIDDLEPARTITIONER_H
#define APPLESEED_FOUNDATION_MATH_BVH_BVH_MIDDLEPARTITIONER_H

// appleseed.renderer headers.
#include "renderer/global/globallogger.h"
// appleseed.foundation headers.
#include "foundation/math/bvh/bvh_partitionerbase.h"

// Standard headers.
#include <cassert>
#include <cstddef>

namespace foundation {
namespace bvh {

//
// A BVH partitioner that use splitting in the middle of the bbox.
//

template <typename AABBVector>
class MiddlePartitioner
  : public PartitionerBase<AABBVector>
{
  public:
    typedef AABBVector AABBVectorType;
    typedef typename AABBVectorType::value_type AABBType;

    // Constructor.
    MiddlePartitioner(
        const AABBVectorType&   bboxes,
        const size_t            max_leaf_size = 1);

    // Partition a set of items into two distinct sets.
    size_t partition(
        const size_t            begin,
        const size_t            end,
        const AABBType&         bbox);

  private:
    const size_t                m_max_leaf_size;
};


//
// MiddlePartitioner class implementation.
//

template <typename AABBVector>
inline MiddlePartitioner<AABBVector>::MiddlePartitioner(
    const AABBVectorType&       bboxes,
    const size_t                max_leaf_size)
  : PartitionerBase<AABBVectorType>(bboxes)
  , m_max_leaf_size(max_leaf_size)
{
}

template <typename AABBVector>
inline size_t MiddlePartitioner<AABBVector>::partition(
    const size_t                begin,
    const size_t                end,
    const AABBType&             bbox)
{
    const size_t count = end - begin;
    assert(count > 1);
    
    const AABBVectorType& bboxes = PartitionerBase<AABBVector>::m_bboxes;

    // Don't split leaves containing less than a predefined number of items.
    if (count <= m_max_leaf_size)
        return end;

    // Split the longest dimension of the bounding box.
    const size_t split_dim = max_index(bbox.extent());
    const float center = bbox.center(split_dim);
    RENDERER_LOG_INFO("Partitioning new bbox");
    RENDERER_LOG_INFO("bbox center [%f %f %f]",
                                bbox.center()[0],
                                bbox.center()[1],
                                bbox.center()[2]);
    RENDERER_LOG_INFO("bbox max axis: %zu", split_dim);
    RENDERER_LOG_INFO("bbox max axis extent: [%f %f %f]",
                        bbox.extent()[0],
                        bbox.extent()[1],
                        bbox.extent()[2]);
    RENDERER_LOG_INFO("bbox min max along max axis: %f %f",
                        bbox.min[max_index(bbox.extent())],
                        bbox.max[max_index(bbox.extent())]);
    // Find the last bbox with center smaller than half and split at that point
    size_t split_point = begin;
    bool smaller = true;
    while(smaller)
    {
        smaller = bboxes[split_point].center(split_dim) < center;
        split_point++;
        if(split_point >= end)
        {
            split_point--;
            break;
        }
    };

    const size_t pivot = split_point;
    assert(pivot < end);

    PartitionerBase<AABBVector>::sort_indices(split_dim, begin, end, pivot);

    return pivot;
}

}       // namespace bvh
}       // namespace foundation

#endif  // !APPLESEED_FOUNDATION_MATH_BVH_BVH_MIDDLEPARTITIONER_H
