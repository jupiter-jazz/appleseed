
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2013 Francois Beaune, Jupiter Jazz Limited
// Copyright (c) 2014-2018 Francois Beaune, The appleseedhq Organization
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

#ifndef APPLESEED_RENDERER_MODELING_BSDF_ASHIKHMINBRDF_H
#define APPLESEED_RENDERER_MODELING_BSDF_ASHIKHMINBRDF_H

// appleseed.renderer headers.
#include "renderer/global/globaltypes.h"
#include "renderer/modeling/bsdf/ibsdffactory.h"
#include "renderer/modeling/input/inputarray.h"

// appleseed.foundation headers.
#include "foundation/platform/compiler.h"
#include "foundation/utility/autoreleaseptr.h"

// appleseed.main headers.
#include "main/dllsymbol.h"

// Forward declarations.
namespace foundation    { class Dictionary; }
namespace foundation    { class DictionaryArray; }
namespace renderer      { class BSDF; }
namespace renderer      { class ParamArray; }

namespace renderer
{

//
// Ashikhmin-Shirley BRDF input values.
//

APPLESEED_DECLARE_INPUT_VALUES(AshikhminBRDFInputValues)
{
    Spectrum    m_rd;               // diffuse reflectance of the substrate
    float       m_rd_multiplier;    // diffuse reflectance multiplier
    Spectrum    m_rg;               // glossy reflectance at normal incidence
    float       m_rg_multiplier;    // glossy reflectance multiplier
    float       m_fr_multiplier;    // Fresnel multiplier
    float       m_nu;               // Phong-like exponent in first tangent direction
    float       m_nv;               // Phong-like exponent in second tangent direction
};


//
// Ashikhmin-Shirley BRDF factory.
//

class APPLESEED_DLLSYMBOL AshikhminBRDFFactory
  : public IBSDFFactory
{
  public:
    // Delete this instance.
    void release() override;

    // Return a string identifying this BSDF model.
    const char* get_model() const override;

    // Return metadata for this BSDF model.
    foundation::Dictionary get_model_metadata() const override;

    // Return metadata for the inputs of this BSDF model.
    foundation::DictionaryArray get_input_metadata() const override;

    // Create a new BSDF instance.
    foundation::auto_release_ptr<BSDF> create(
        const char*         name,
        const ParamArray&   params) const override;
};

}       // namespace renderer

#endif  // !APPLESEED_RENDERER_MODELING_BSDF_ASHIKHMINBRDF_H
