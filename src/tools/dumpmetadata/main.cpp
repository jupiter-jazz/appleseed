
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
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

// dumpmetadata headers.
#include "commandlinehandler.h"

// appleseed.shared headers.
#include "application/application.h"
#include "application/superlogger.h"

// appleseed.renderer headers.
#include "renderer/api/aov.h"
#include "renderer/api/bsdf.h"
#include "renderer/api/bssrdf.h"
#include "renderer/api/camera.h"
#include "renderer/api/color.h"
#include "renderer/api/edf.h"
#include "renderer/api/entity.h"
#include "renderer/api/environment.h"
#include "renderer/api/environmentedf.h"
#include "renderer/api/environmentshader.h"
#include "renderer/api/frame.h"
#include "renderer/api/light.h"
#include "renderer/api/material.h"
#include "renderer/api/object.h"
#include "renderer/api/postprocessing.h"
#include "renderer/api/project.h"
#include "renderer/api/surfaceshader.h"
#include "renderer/api/texture.h"
#include "renderer/api/volume.h"

// appleseed.foundation headers.
#include "foundation/core/appleseed.h"
#include "foundation/utility/api/specializedapiarrays.h"
#include "foundation/utility/containers/dictionary.h"
#include "foundation/utility/indenter.h"
#include "foundation/utility/log.h"
#include "foundation/utility/string.h"
#include "foundation/utility/xmlelement.h"

// Standard headers.
#include <cstddef>
#include <cstdio>
#include <ios>
#include <sstream>
#include <string>

using namespace appleseed::dumpmetadata;
using namespace appleseed::shared;
using namespace foundation;
using namespace renderer;
using namespace std;

namespace
{
    //
    // Dump as XML.
    //

    void dump_metadata_xml(
        const char*             entity_type,
        const char*             entity_model,
        const DictionaryArray&  metadata_array,
        FILE*                   file,
        Indenter&               indenter)
    {
        XMLElement entity_element("entity", file, indenter);
        entity_element.add_attribute("type", entity_type);
        entity_element.add_attribute("model", entity_model);
        entity_element.write(XMLElement::HasChildElements);

        for (size_t i = 0; i < metadata_array.size(); ++i)
        {
            Dictionary metadata = metadata_array[i];

            const string name = metadata.get<string>("name");
            metadata.strings().remove("name");

            Dictionary wrapped_metadata;
            wrapped_metadata.insert(name, metadata);

            write_dictionary(wrapped_metadata, file, indenter);
        }
    }

    template <typename EntityType>
    void dump_metadata_xml(FILE* file, Indenter& indenter)
    {
        typedef EntityTraits<EntityType> EntityTraitsType;
        typedef typename EntityTraitsType::FactoryRegistrarType FactoryRegistrarType;
        typedef typename FactoryRegistrarType::FactoryArrayType FactoryArrayType;

        FactoryRegistrarType registrar;
        const FactoryArrayType factories = registrar.get_factories();

        for (size_t i = 0; i < factories.size(); ++i)
        {
            dump_metadata_xml(
                EntityTraitsType::get_entity_type_name(),
                factories[i]->get_model(),
                factories[i]->get_input_metadata(),
                file,
                indenter);
        }
    }

    void dump_metadata_xml()
    {
        FILE* file = stdout;
        Indenter indenter(4);

        fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        fprintf(
            file,
            "<!-- File generated by %s. -->\n",
            Appleseed::get_synthetic_version_string());

        XMLElement metadata_element("metadata", file, indenter);
        metadata_element.write(XMLElement::HasChildElements);

        dump_metadata_xml<AOV>(file, indenter);
        dump_metadata_xml<BSDF>(file, indenter);
        dump_metadata_xml<BSSRDF>(file, indenter);
        dump_metadata_xml<Camera>(file, indenter);
        dump_metadata_xml("color", "color", ColorEntityFactory::get_input_metadata(), file, indenter);
        dump_metadata_xml<EDF>(file, indenter);
        dump_metadata_xml("environment", "generic_environment", EnvironmentFactory::get_input_metadata(), file, indenter);
        dump_metadata_xml<EnvironmentEDF>(file, indenter);
        dump_metadata_xml<EnvironmentShader>(file, indenter);
        dump_metadata_xml("frame", "frame", FrameFactory::get_input_metadata(), file, indenter);
        dump_metadata_xml<Light>(file, indenter);
        dump_metadata_xml<Material>(file, indenter);
        dump_metadata_xml<Object>(file, indenter);
        dump_metadata_xml<PostProcessingStage>(file, indenter);
        dump_metadata_xml<SurfaceShader>(file, indenter);
        dump_metadata_xml<Texture>(file, indenter);
        dump_metadata_xml<Volume>(file, indenter);
    }

    //
    // Dump as Markdown.
    //

    bool is_numeric_value(const string& s)
    {
        istringstream iss(s);

        double val;
        iss >> noskipws >> val;     // noskipws considers leading whitespace invalid

        return iss && iss.eof();
    }

    void dump_entity_params_markdown(
        const DictionaryArray&  metadata_array,
        FILE*                   file)
    {
        fprintf(file, "| Parameter | Label | Presence | Default | Description |\n");
        fprintf(file, "| :-------- | :---- | :------- | :------ | :---------- |\n");

        for (size_t i = 0; i < metadata_array.size(); ++i)
        {
            Dictionary metadata = metadata_array[i];

            // Parameter, Label.
            const string param_name = metadata.get<string>("name");
            const string param_label = metadata.get<string>("label");

            // Presence.
            string param_use = metadata.get<string>("use");
            if (param_use == "required")
                param_use = format("**{0}**", param_use);

            // Default.
            string param_default = metadata.strings().exist("default") ? metadata.get<string>("default") : "";
            if (param_default.empty())
                param_default = "_None_";
            else if (!is_numeric_value(param_default))
                param_default = format("`{0}`", param_default);

            // Description.
            string param_desc = metadata.strings().exist("help") ? metadata.get<string>("help") : "";
            if (metadata.get<string>("type") == "enumeration")
            {
                if (!param_desc.empty() && !ends_with(param_desc, "."))
                    param_desc += ".";

                param_desc += " Possible values are: ";

                const StringDictionary& items = metadata.dictionary("items").strings();
                for (auto x : items)
                {
                    if (x != items.begin())
                        param_desc += ", ";
                    param_desc += format("`{0}` ({1})", x.value(), x.key());
                }

                param_desc += ".";
            }

            fprintf(
                file,
                "| `%s` | %s | %s | %s | %s |\n",
                param_name.c_str(),
                param_label.c_str(),
                param_use.c_str(),
                param_default.c_str(),
                param_desc.c_str());
        }

        fprintf(file, "\n");
    }

    void dump_metadata_markdown(
        const size_t            section_number,
        const size_t            subsection_number,
        const char*             entity_model,
        const DictionaryArray&  metadata_array,
        FILE*                   file)
    {
        fprintf(
            file,
            "#### " FMT_SIZE_T "." FMT_SIZE_T ". The `%s` Model\n\n",
            section_number,
            subsection_number,
            entity_model);

        if (!metadata_array.empty())
            dump_entity_params_markdown(metadata_array, file);
    }

    template <typename EntityType>
    void dump_metadata_markdown(
        const size_t            section_number,
        FILE*                   file)
    {
        typedef EntityTraits<EntityType> EntityTraitsType;
        typedef typename EntityTraitsType::FactoryRegistrarType FactoryRegistrarType;
        typedef typename FactoryRegistrarType::FactoryArrayType FactoryArrayType;

        fprintf(
            file,
            "### " FMT_SIZE_T ". Built-in %s Models\n\n",
            section_number,
            EntityTraitsType::get_human_readable_entity_type_name());

        FactoryRegistrarType registrar;
        const FactoryArrayType factories = registrar.get_factories();

        for (size_t i = 0; i < factories.size(); ++i)
        {
            dump_metadata_markdown(
                section_number,
                i + 1,
                factories[i]->get_model(),
                factories[i]->get_input_metadata(),
                file);
        }
    }

    void dump_metadata_markdown()
    {
        FILE* file = stdout;
        size_t section_number = 1;

        dump_metadata_markdown<AOV>(section_number++, file);
        dump_metadata_markdown<BSDF>(section_number++, file);
        dump_metadata_markdown<BSSRDF>(section_number++, file);
        dump_metadata_markdown<Camera>(section_number++, file);
        dump_metadata_markdown(section_number++, 1, "color", ColorEntityFactory::get_input_metadata(), file);
        dump_metadata_markdown<EDF>(section_number++, file);
        dump_metadata_markdown(section_number++, 1, "generic_environment", EnvironmentFactory::get_input_metadata(), file);
        dump_metadata_markdown<EnvironmentEDF>(section_number++, file);
        dump_metadata_markdown<EnvironmentShader>(section_number++, file);
        dump_metadata_markdown(section_number++, 1, "frame", FrameFactory::get_input_metadata(), file);
        dump_metadata_markdown<Light>(section_number++, file);
        dump_metadata_markdown<Material>(section_number++, file);
        dump_metadata_markdown<Object>(section_number++, file);
        dump_metadata_markdown<PostProcessingStage>(section_number++, file);
        dump_metadata_markdown<SurfaceShader>(section_number++, file);
        dump_metadata_markdown<Texture>(section_number++, file);
        dump_metadata_markdown<Volume>(section_number++, file);
    }

    void dump_metadata(const string& format, Logger& logger)
    {
        if (format == "xml")
            dump_metadata_xml();
        else if (format == "markdown")
            dump_metadata_markdown();
        else
            LOG_ERROR(logger, "unknown format: %s", format.c_str());
    }
}


//
// Entry point of dumpmetadata.
//

int main(int argc, const char* argv[])
{
    // Construct the logger that will be used throughout the program.
    SuperLogger logger;

    // Make sure this build can run on this host.
    Application::check_compatibility_with_host(logger);

    // Make sure appleseed is correctly installed.
    Application::check_installation(logger);

    // Parse the command line.
    CommandLineHandler cl;
    cl.parse(argc, argv, logger);

    // Load an apply settings from the settings file.
    Dictionary settings;
    Application::load_settings("appleseed.tools.xml", settings, logger);
    logger.configure_from_settings(settings);

    // Apply command line arguments.
    cl.apply(logger);

    dump_metadata(cl.m_format.value(), logger);

    return 0;
}
