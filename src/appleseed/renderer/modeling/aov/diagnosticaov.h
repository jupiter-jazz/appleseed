
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2018 Kevin Masson, The appleseedhq Organization
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

#ifndef APPLESEED_RENDERER_MODELING_AOV_DIAGNOSTICAOV_H
#define APPLESEED_RENDERER_MODELING_AOV_DIAGNOSTICAOV_H

// appleseed.renderer headers.
#include "renderer/kernel/aov/aovaccumulator.h"
#include "renderer/modeling/aov/aov.h"
#include "renderer/modeling/aov/iaovfactory.h"

// appleseed.foundation headers.
#include "foundation/math/aabb.h"
#include "foundation/utility/autoreleaseptr.h"

// appleseed.main headers.
#include "main/dllsymbol.h"

// Standard headers.
#include <cstddef>

// Forward declarations.
namespace foundation    { class Dictionary; }
namespace foundation    { class DictionaryArray; }
namespace renderer      { class ImageStack; }
namespace renderer      { class ParamArray; }

namespace renderer
{

//
// Diagnostic AOV.
//

class DiagnosticAOV
  : public AOV
{
  public:
    DiagnosticAOV(const char* name, const ParamArray& params);

    void release() override;

    size_t get_channel_count() const override;

    const char** get_channel_names() const override;

    bool has_color_data() const override;

    void create_image(
        const size_t canvas_width,
        const size_t canvas_height,
        const size_t tile_width,
        const size_t tile_height,
        ImageStack&  aov_images) override;

    void clear_image() override;

    foundation::auto_release_ptr<AOVAccumulator> create_accumulator() const override;
};


//
// Pixel Sample Count AOV.
//

class PixelSampleCountAOV
  : public DiagnosticAOV
{
  public:
    explicit PixelSampleCountAOV(const ParamArray& params);

    const char* get_model() const override;

    void post_process_image(const foundation::AABB2u& crop_window) override;

    void set_normalization_range(const size_t min_spp, const size_t max_spp);

  private:
    size_t m_min_spp;
    size_t m_max_spp;
};


//
// A factory for invalid sample AOVs.
//

class APPLESEED_DLLSYMBOL InvalidSamplesAOVFactory
  : public IAOVFactory
{
  public:
    // Delete this instance.
    void release() override;

    // Return a string identifying this AOV model.
    const char* get_model() const override;

    // Return metadata for this AOV model.
    foundation::Dictionary get_model_metadata() const override;

    // Return metadata for the inputs of this AOV model.
    foundation::DictionaryArray get_input_metadata() const override;

    // Create a new AOV instance.
    foundation::auto_release_ptr<AOV> create(
        const ParamArray&   params) const override;
};


//
// A factory for pixel sample count AOVs.
//

class APPLESEED_DLLSYMBOL PixelSampleCountAOVFactory
  : public IAOVFactory
{
  public:
    // Delete this instance.
    void release() override;

    // Return a string identifying this AOV model.
    const char* get_model() const override;

    // Return metadata for this AOV model.
    foundation::Dictionary get_model_metadata() const override;

    // Return metadata for the inputs of this AOV model.
    foundation::DictionaryArray get_input_metadata() const override;

    // Create a new AOV instance.
    foundation::auto_release_ptr<AOV> create(
        const ParamArray&   params) const override;
};


//
// A factory for pixel variation AOVs.
//

class APPLESEED_DLLSYMBOL PixelVariationAOVFactory
  : public IAOVFactory
{
  public:
    // Delete this instance.
    void release() override;

    // Return a string identifying this AOV model.
    const char* get_model() const override;

    // Return metadata for this AOV model.
    foundation::Dictionary get_model_metadata() const override;

    // Return metadata for the inputs of this AOV model.
    foundation::DictionaryArray get_input_metadata() const override;

    // Create a new AOV instance.
    foundation::auto_release_ptr<AOV> create(
        const ParamArray&   params) const override;
};

}       // namespace renderer

#endif  // !APPLESEED_RENDERER_MODELING_AOV_DIAGNOSTICAOV_H
