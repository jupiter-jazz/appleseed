
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

#ifndef APPLESEED_FOUNDATION_MATH_BVH_BVH_PARTITIONERBASE_H
#define APPLESEED_FOUNDATION_MATH_BVH_BVH_PARTITIONERBASE_H

// appleseed.renderer headers.
#include "renderer/global/globallogger.h"
// appleseed.foundation headers.
#include "foundation/core/concepts/noncopyable.h"
#include "foundation/math/bvh/bvh_bboxsortpredicate.h"
#include "foundation/platform/types.h"

// Standard headers.
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>

namespace foundation {
namespace bvh {

//
// A base class for BVH partitioners.
//

template <typename AABBVector>
class PartitionerBase
  : public NonCopyable
{
  public:
    typedef AABBVector AABBVectorType;
    typedef typename AABBVectorType::value_type AABBType;

    // Constructor.
    explicit PartitionerBase(
        const AABBVectorType&   bboxes);

    // Compute the bounding box of a given set of items.
    AABBType compute_bbox(
        const size_t            begin,
        const size_t            end) const;

    // Return the items ordering.
    const std::vector<size_t>& get_item_ordering() const;

  protected:
    static const size_t Dimension = AABBType::Dimension;

    const AABBVectorType&       m_bboxes;
    std::vector<size_t>         m_indices[Dimension];

    void sort_indices(
        const size_t            dimension,
        const size_t            begin,
        const size_t            end,
        const size_t            pivot,
        const bool              write = false);

  private:
    std::vector<size_t>         m_tmp;
    std::vector<uint8>          m_tags;
};


//
// PartitionerBase class implementation.
//

template <typename AABBVector>
PartitionerBase<AABBVector>::PartitionerBase(
    const AABBVectorType&       bboxes)
  : m_bboxes(bboxes)
{
    const size_t size = m_bboxes.size();

    FILE* f = fopen("sorting_test.txt", "at"); // in /sandbox/bin/Release/test.txt
    fprintf(f, "PartitionerBase constructor\n");
    for (size_t d = 0; d < Dimension; ++d)
    {
        fprintf(f, "DIMENSION d: %zu\n", d);
        std::vector<size_t>& indices = m_indices[d];

        // Identity ordering.
        indices.resize(size);
        for (size_t i = 0; i < size; ++i)
            indices[i] = i;

        fprintf(f, "m_indices before bbox sort: [ ");
        for(size_t m = 0; m < m_indices[d].size(); m++)
        {
            fprintf(f, "%lu ", m_indices[d][m]);
        }
        fprintf(f, "]\n");
        
        // Sort the items according to the centre of their bounding boxes.
        BboxSortPredicate<AABBVectorType> predicate(m_bboxes, d);
        std::sort(indices.begin(), indices.end(), predicate);
        
        fprintf(f, "m_indices after bbox sort: [ ");
        for(size_t m = 0; m < m_indices[d].size(); m++)
        {
            fprintf(f, "%lu ", m_indices[d][m]);
        }
        fprintf(f, "]\n");
    }
    fprintf(f, "PartitionerBase constructor end\n");
    fclose(f);

    m_tmp.resize(size);
    m_tags.resize(size);
}

template <typename AABBVector>
typename AABBVector::value_type PartitionerBase<AABBVector>::compute_bbox(
    const size_t                begin,
    const size_t                end) const
{
    const std::vector<size_t>& indices = m_indices[0];

    AABBType bbox;
    bbox.invalidate();

    for (size_t i = begin; i < end; ++i)
        bbox.insert(m_bboxes[indices[i]]);

    return bbox;
}

template <typename AABBVector>
void PartitionerBase<AABBVector>::sort_indices(
    const size_t                dimension,
    const size_t                begin,
    const size_t                end,
    const size_t                pivot,
    const bool                  write)
{
    FILE* f = fopen("sorting_test.txt", "at"); // in /sandbox/bin/Release/test.txt
    if(write)
    {
        fprintf(f, "\nSORTING INDICES...\n");
        fprintf(f, "DIMENSION argument: %zu\n", dimension);
        fprintf(f, "PIVOT: %zu\n", pivot);
    }

    const std::vector<size_t>& split_indices = m_indices[dimension];

    if(write)
    {
        fprintf(f, "split_indices: [ ");
        for(size_t m = 0; m < split_indices.size(); m++)
        {
            fprintf(f, "%lu ", split_indices[m]);
        }
        fprintf(f, "]\n");
    }

    enum { Left = 0, Right = 1 };

    for (size_t i = begin; i < pivot; ++i)
        m_tags[split_indices[i]] = Left;

    if(write)
    {
        fprintf(f, "\ntags: [ ");
        for(size_t m = 0; m < m_tags.size(); m++)
        {
            fprintf(f, "%d ", m_tags[m]);
        }
        fprintf(f, "]\n");
    }

    for (size_t i = pivot; i < end; ++i)
        m_tags[split_indices[i]] = Right;

    if(write)
    {
        fprintf(f, "\ntags: [ ");
        for(size_t m = 0; m < m_tags.size(); m++)
        {
            fprintf(f, "%d ", m_tags[m]);
        }
        fprintf(f, "]\n");
    }

    for (size_t d = 0; d < Dimension; ++d)
    {
        if(write)
        {
            fprintf(f, "\nDIMENSION d: %zu\n", d);
        }
        if (d != dimension)
        {
            fprintf(f, "\nd != dimension\n");
            std::vector<size_t>& indices = m_indices[d];

            if(write)
            {
                fprintf(f, "indices: [ ");
                for(size_t m = 0; m < indices.size(); m++)
                {
                    fprintf(f, "%lu ", indices[m]);
                }
                fprintf(f, "]\n");
            }

            size_t left = begin;
            size_t right = pivot;

            for (size_t i = begin; i < end; ++i)
            {
                const size_t index = indices[i];

                if(write)
                {
                    fprintf(f, "\nINDEX: %zu\n", i);
                }
                if (m_tags[index] == Left)
                {
                    assert(left < pivot);
                    m_tmp[left++] = index;
                    if(write)
                    {
                        fprintf(f, "\nleft: %zu\n", left);
                    }
                }
                else
                {
                    assert(right < end);
                    m_tmp[right++] = index;
                    if(write)
                    {
                        fprintf(f, "\nright: %zu\n", right);
                    }
                }

                if(write)
                {
                    fprintf(f, "\nm_tmp: [ ");
                    for(size_t m = 0; m < m_tmp.size(); m++)
                    {
                        fprintf(f, "%lu ", m_tmp[m]);
                    }
                    fprintf(f, "]\n");
                }
            }

            assert(left == pivot);
            assert(right == end);

            const size_t size = indices.size();

            if (end - begin > size / 2)
            {
                for (size_t i = 0; i < begin; ++i)
                    m_tmp[i] = indices[i];

                for (size_t i = end; i < size; ++i)
                    m_tmp[i] = indices[i];

                m_tmp.swap(indices);

                if(write)
                {
                    fprintf(f, "\nm_tmp: [ ");
                    for(size_t m = 0; m < m_tmp.size(); m++)
                    {
                        fprintf(f, "%lu ", m_tmp[m]);
                    }
                    fprintf(f, "]\n");
                }
            }
            else
            {
                for (size_t i = begin; i < end; ++i)
                    indices[i] = m_tmp[i];

                if(write)
                {
                    fprintf(f, "indices: [ ");
                    for(size_t m = 0; m < indices.size(); m++)
                    {
                        fprintf(f, "%lu ", indices[m]);
                    }
                    fprintf(f, "]\n");
                }
            }
        }
    }
    fclose(f);
}

template <typename Tree>
inline const std::vector<size_t>& PartitionerBase<Tree>::get_item_ordering() const
{
    return m_indices[0];
}

}       // namespace bvh
}       // namespace foundation

#endif  // !APPLESEED_FOUNDATION_MATH_BVH_BVH_PARTITIONERBASE_H
