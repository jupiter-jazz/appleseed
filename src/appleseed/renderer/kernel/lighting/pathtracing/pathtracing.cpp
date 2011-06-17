
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2011 Francois Beaune
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
#include "pathtracing.h"

// appleseed.renderer headers.
#include "renderer/kernel/lighting/directlighting.h"
#include "renderer/kernel/lighting/imagebasedlighting.h"
#include "renderer/kernel/lighting/lightsampler.h"
#include "renderer/kernel/lighting/pathtracer.h"
#include "renderer/kernel/shading/shadingcontext.h"
#include "renderer/kernel/shading/shadingpoint.h"
#include "renderer/modeling/bsdf/bsdf.h"
#include "renderer/modeling/edf/edf.h"
#include "renderer/modeling/environment/environment.h"
#include "renderer/modeling/environmentedf/environmentedf.h"
#include "renderer/modeling/input/inputevaluator.h"
#include "renderer/modeling/material/material.h"
#include "renderer/modeling/scene/scene.h"

// appleseed.foundation headers.
#include "foundation/math/basis.h"
#include "foundation/math/mis.h"
#include "foundation/math/population.h"
#include "foundation/utility/memory.h"
#include "foundation/utility/string.h"

// Forward declarations.
namespace renderer  { class EnvironmentEDF; }
namespace renderer  { class InputParams; }

using namespace foundation;
using namespace std;

namespace renderer
{

namespace
{
    //
    // Path Tracing lighting engine.
    //
    // Implementation of spectral, Monte Carlo backward path tracing,
    // optionally with next event estimation.
    //
    // Reference:
    //
    //   http://citeseer.ist.psu.edu/344088.html
    //

    class PTLightingEngine
      : public ILightingEngine
    {
      public:
        PTLightingEngine(
            const LightSampler&     light_sampler,
            const ParamArray&       params)
          : m_params(params)
          , m_light_sampler(light_sampler)
        {
            RENDERER_LOG_DEBUG(
                "path tracing settings:\n"
                "  next event est.  %s\n"
                "  min path length  %s\n"
                "  dl light samples %s\n"
                "  ibl bsdf samples %s\n"
                "  ibl env samples  %s",
                m_params.m_next_event_estimation ? "on" : "off",
                pretty_uint(m_params.m_minimum_path_length).c_str(),
                pretty_uint(m_params.m_dl_light_sample_count).c_str(),
                pretty_uint(m_params.m_ibl_bsdf_sample_count).c_str(),
                pretty_uint(m_params.m_ibl_env_sample_count).c_str());
        }

        ~PTLightingEngine()
        {
            RENDERER_LOG_DEBUG(
                "path tracing statistics:\n"
                "  paths            %s\n"
                "  path length      avg %.1f  min %s  max %s  dev %.1f\n",
                pretty_uint(m_stats.m_path_count).c_str(),
                m_stats.m_path_length.get_avg(),
                pretty_uint(m_stats.m_path_length.get_min()).c_str(),
                pretty_uint(m_stats.m_path_length.get_max()).c_str(),
                m_stats.m_path_length.get_dev());
        }

        virtual void release()
        {
            delete this;
        }

        virtual void compute_lighting(
            SamplingContext&        sampling_context,
            const ShadingContext&   shading_context,
            const ShadingPoint&     shading_point,
            Spectrum&               radiance)   // output radiance, in W.sr^-1.m^-2
        {
            typedef PathTracer<
                PathVisitor,
                BSDF::Diffuse | BSDF::Glossy | BSDF::Specular,
                false                           // not adjoint
            > PathTracer;

            PathVisitor path_visitor(
                m_params,
                m_light_sampler,
                shading_context,
                shading_point.get_scene(),
                radiance);

            PathTracer path_tracer(
                path_visitor,
                m_params.m_minimum_path_length);

            const size_t path_length =
                path_tracer.trace(
                    sampling_context,
                    shading_context.get_intersector(),
                    shading_context.get_texture_cache(),
                    shading_point);

            // Update statistics.
            ++m_stats.m_path_count;
            m_stats.m_path_length.insert(path_length);
        }

      private:
        struct Parameters
        {
            const bool          m_next_event_estimation;    // use next event estimation?
            const size_t        m_minimum_path_length;      // minimum path length before Russian Roulette is used
            const size_t        m_dl_light_sample_count;    // number of light samples used to estimate direct illumination
            const size_t        m_ibl_bsdf_sample_count;    // number of BSDF samples used to estimate IBL
            const size_t        m_ibl_env_sample_count;     // number of environment samples used to estimate IBL

            explicit Parameters(const ParamArray& params)
              : m_next_event_estimation ( params.get_optional<bool>("next_event_estimation", true) )
              , m_minimum_path_length   ( params.get_optional<size_t>("minimum_path_length", 3) )
              , m_dl_light_sample_count ( params.get_optional<size_t>("dl_light_samples", 1) )
              , m_ibl_bsdf_sample_count ( params.get_optional<size_t>("ibl_bsdf_samples", 1) )
              , m_ibl_env_sample_count  ( params.get_optional<size_t>("ibl_env_samples", 1) )
            {
            }
        };

        struct Statistics
        {
            uint64              m_path_count;
            Population<uint64>  m_path_length;

            Statistics()
              : m_path_count(0)
            {
            }
        };

        class PathVisitor
        {
          public:
            PathVisitor(
                const Parameters&       params,
                const LightSampler&     light_sampler,
                const ShadingContext&   shading_context,
                const Scene&            scene,
                Spectrum&               path_radiance)
              : m_params(params)
              , m_light_sampler(light_sampler)
              , m_shading_context(shading_context)
              , m_texture_cache(shading_context.get_texture_cache())
              , m_env_edf(scene.get_environment()->get_environment_edf())
              , m_path_radiance(path_radiance)
            {
                m_path_radiance.set(0.0f);
            }

            bool visit_vertex(
                SamplingContext&        sampling_context,
                const ShadingPoint&     shading_point,
                const Vector3d&         outgoing,
                const BSDF*             bsdf,
                const void*             bsdf_data,
                const BSDF::Mode        bsdf_mode,
                const double            bsdf_prob,
                const Spectrum&         throughput)
            {
                const Vector3d& point = shading_point.get_point();
                const Vector3d& geometric_normal = shading_point.get_geometric_normal();
                const Vector3d& shading_normal = shading_point.get_shading_normal();
                const Basis3d& shading_basis = shading_point.get_shading_basis();
                const Material* material = shading_point.get_material();
                const InputParams& input_params = shading_point.get_input_params();

                const EDF* edf = material->get_edf();
                const double cos_on = dot(outgoing, shading_normal);

                // Evaluate the input values of the EDF (if any).
                InputEvaluator edf_input_evaluator(m_texture_cache);
                const void* edf_data = edf
                    ? edf_input_evaluator.evaluate(
                        edf->get_inputs(),
                        input_params)
                    : 0;

                if (m_params.m_next_event_estimation)
                {
                    // Compute direct lighting.
                    Spectrum vertex_radiance;
                    compute_direct_lighting_light_sampling(
                        sampling_context,
                        m_shading_context,
                        m_light_sampler,
                        point,
                        geometric_normal,
                        shading_basis,
                        outgoing,
                        *bsdf,
                        bsdf_data,
                        1,
                        m_params.m_dl_light_sample_count,
                        vertex_radiance,
                        &shading_point);

                    if (m_env_edf)
                    {
                        // Compute image-based lighting.
                        Spectrum ibl_radiance;
                        compute_image_based_lighting(
                            sampling_context,
                            m_shading_context,
                            *m_env_edf,
                            point,
                            geometric_normal,
                            shading_basis,
                            outgoing,
                            *bsdf,
                            bsdf_data,
                            m_params.m_ibl_bsdf_sample_count,
                            m_params.m_ibl_env_sample_count,
                            ibl_radiance,
                            &shading_point);
                        vertex_radiance += ibl_radiance;
                    }

                    if (edf && cos_on > 0.0)
                    {
                        // Compute the emitted radiance.
                        Spectrum emitted_radiance;
                        edf->evaluate(
                            edf_data,
                            geometric_normal,
                            shading_basis,
                            outgoing,
                            emitted_radiance);

                        // Multiple importance sampling.
                        const double square_distance = square(shading_point.get_distance());
                        if (bsdf_mode != BSDF::Specular && square_distance > 0.0)
                        {
                            // Transform bsdf_prob to surface area measure (Veach: 8.2.2.2 eq. 8.10).
                            const double bsdf_point_prob = bsdf_prob * cos_on / square_distance;

                            // Compute the probability density wrt. surface area of choosing this point
                            // by sampling the light sources.
                            const double light_point_prob = m_light_sampler.evaluate_pdf(shading_point);

                            // Apply MIS.
                            const double mis_weight =
                                mis_power2(
                                    bsdf_point_prob,
                                    m_params.m_dl_light_sample_count * light_point_prob);
                            emitted_radiance *= static_cast<float>(mis_weight);
                        }

                        vertex_radiance += emitted_radiance;
                    }

                    // Update the path radiance.
                    vertex_radiance *= throughput;
                    m_path_radiance += vertex_radiance;
                }
                else
                {
                    if (edf && cos_on > 0.0)
                    {
                        // Compute the emitted radiance.
                        Spectrum emitted_radiance;
                        edf->evaluate(
                            edf_data,
                            geometric_normal,
                            shading_basis,
                            outgoing,
                            emitted_radiance);

                        // Update the path radiance.
                        emitted_radiance *= throughput;
                        m_path_radiance += emitted_radiance;
                    }
                }

                // Proceed with this path.
                return true;
            }

            void visit_environment(
                const ShadingPoint&     shading_point,
                const Vector3d&         outgoing,
                const Spectrum&         throughput)
            {
                if (m_params.m_next_event_estimation)
                    return;

                if (m_env_edf)
                {
                    // Evaluate the environment EDF.
                    InputEvaluator input_evaluator(m_texture_cache);
                    Spectrum environment_radiance;
                    m_env_edf->evaluate(
                        input_evaluator,
                        -outgoing,
                        environment_radiance);

                    // Update the path radiance.
                    environment_radiance *= throughput;
                    m_path_radiance += environment_radiance;
                }
            }

          private:
            const Parameters&       m_params;
            const LightSampler&     m_light_sampler;
            const ShadingContext&   m_shading_context;
            TextureCache&           m_texture_cache;
            const EnvironmentEDF*   m_env_edf;
            Spectrum&               m_path_radiance;
        };

        const Parameters        m_params;
        Statistics              m_stats;
        const LightSampler&     m_light_sampler;
    };
}


//
// PTLightingEngineFactory class implementation.
//

PTLightingEngineFactory::PTLightingEngineFactory(
    const LightSampler& light_sampler,
    const ParamArray&   params)
  : m_light_sampler(light_sampler)
  , m_params(params)
{
}

void PTLightingEngineFactory::release()
{
    delete this;
}

ILightingEngine* PTLightingEngineFactory::create()
{
    return new PTLightingEngine(m_light_sampler, m_params);
}

ILightingEngine* PTLightingEngineFactory::create(
    const LightSampler& light_sampler,
    const ParamArray&   params)
{
    return new PTLightingEngine(light_sampler, params);
}

}   // namespace renderer
