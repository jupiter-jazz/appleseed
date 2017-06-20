
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

// appleseed.foundation headers.
#include "foundation/math/aabb.h"
#include "foundation/math/bvh.h"
#include "foundation/utility/test.h"

using namespace foundation;
using namespace bvh;
using namespace std;

TEST_SUITE(Foundation_Math_BVH_MiddlePartitioner)
{
    typedef std::vector<AABB2d> AABB2dVector;

    TEST_CASE(TestBBoxesOrderedAlongLongestDimension)
    {
        AABB2dVector bboxes = 
            {
                AABB2d(Vector2d(-7.0, -1.0 ), Vector2d( 1.0,  1.0 )),
                AABB2d(Vector2d( 1.0, -5.0 ), Vector2d( 3.0, -3.0 )),
                AABB2d(Vector2d( 3.0,  3.0 ), Vector2d( 5.0,  5.0 ))
            };
        
        MiddlePartitioner<AABB2dVector> partitioner(bboxes, 1);
        const AABB2d root_bbox(partitioner.compute_bbox(0, bboxes.size()));
        size_t pivot = partitioner.partition(0, bboxes.size(), root_bbox);

        // Expect to return the end.
        EXPECT_EQ(1, pivot);
    }

    TEST_CASE(TestBBoxesUnorderedAlongAllDimensions)
    {
        AABB2dVector bboxes =
            {
                AABB2d(Vector2d( 3.0,  13.0 ), Vector2d(5.0,  15.0 )),
                AABB2d(Vector2d( 1.0, -15.0 ), Vector2d(3.0, -13.0 )),
                AABB2d(Vector2d(-7.0,  -1.0 ), Vector2d(1.0,   1.0 ))
            };

        MiddlePartitioner<AABB2dVector> partitioner(bboxes, 1);
        const AABB2d root_bbox(partitioner.compute_bbox(0, bboxes.size()));

        size_t pivot = partitioner.partition(0, bboxes.size(), root_bbox);
        // Expect to return the end.
        EXPECT_EQ(1, pivot);
    }

    TEST_CASE(TestBBoxes3PointLightsTestScene)
    {
        AABB2dVector bboxes =
            {
                AABB2d(Vector2d( -0.01, 0.09 ), Vector2d( 0.01, 0.11 )),
                AABB2d(Vector2d( -0.01, 0.09 ), Vector2d( 0.01, 0.11 )),
                AABB2d(Vector2d( -0.49, 0.09 ), Vector2d( 0.51, 0.11 ))
            };

        MiddlePartitioner<AABB2dVector> partitioner(bboxes, 1);
        const AABB2d root_bbox(partitioner.compute_bbox(0, bboxes.size()));

        size_t pivot = partitioner.partition(0, bboxes.size(), root_bbox);
        // Expect to return the end.
        EXPECT_EQ(2, pivot);
    }

    TEST_CASE(TestBBoxesLightsFormingCube)
    {
        AABB2dVector bboxes =
            {
                AABB2d(Vector2d(-0.51,-0.51 ), Vector2d(-0.49,-0.49 )),
                AABB2d(Vector2d(-0.51,-0.51 ), Vector2d(-0.49,-0.49 )),
                AABB2d(Vector2d(-0.51, 0.49 ), Vector2d(-0.49, 0.51 )),
                AABB2d(Vector2d(-0.51, 0.49 ), Vector2d(-0.49, 0.51 )),
                AABB2d(Vector2d( 0.49,-0.51 ), Vector2d( 0.51,-0.49 )),
                AABB2d(Vector2d( 0.49,-0.51 ), Vector2d( 0.51,-0.49 )),
                AABB2d(Vector2d( 0.49, 0.49 ), Vector2d( 0.51, 0.51 )),
                AABB2d(Vector2d( 0.49, 0.49 ), Vector2d( 0.51, 0.51 ))
            };

        MiddlePartitioner<AABB2dVector> partitioner(bboxes, 1);
        const AABB2d root_bbox(partitioner.compute_bbox(0, bboxes.size()));
        
        size_t pivot = partitioner.partition(0, bboxes.size(), root_bbox);
        
        // Expect to return the end.
        EXPECT_EQ(4, pivot);
    }

    TEST_CASE(TestBBoxesOverlapping)
    {
        AABB2dVector bboxes =
            {
                AABB2d(Vector2d(-0.01,-0.01 ), Vector2d( 0.01, 0.01 )),
                AABB2d(Vector2d(-0.01,-0.01 ), Vector2d( 0.01, 0.01 )),
                AABB2d(Vector2d(-0.01,-0.01 ), Vector2d( 0.01, 0.01 )),
                AABB2d(Vector2d(-0.01,-0.01 ), Vector2d( 0.01, 0.01 ))
            };
        
        MiddlePartitioner<AABB2dVector> partitioner(bboxes, 1);
        const AABB2d root_bbox(partitioner.compute_bbox(0, bboxes.size()));
        
        size_t pivot = partitioner.partition(0, bboxes.size(), root_bbox);
        
        // Expect to return the end.
        EXPECT_EQ(1, pivot);
    }
}