
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2017-2018 Gleb Mishchenko, The appleseedhq Organization
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
#include "pythoninterpreter.h"

// appleseed.studio headers.
#include "mainwindow/mainwindow.h"
#include "mainwindow/pythonconsole/outputredirector.h"

// appleseed.shared headers.
#include "application/application.h"

// appleseed.renderer headers.
#include "renderer/api/log.h"

// appleseed.foundation headers.
#include "foundation/core/exceptions/exception.h"
#include "foundation/platform/path.h"
#include "foundation/utility/string.h"

// Boost headers.
#include "boost/filesystem.hpp"

using namespace appleseed::shared;
using namespace foundation;
using namespace std;
namespace bpy = boost::python;
namespace bf = boost::filesystem;

// From appleseed.python module.cpp
extern "C" void bind_appleseed_python_classes();

BOOST_PYTHON_MODULE(_appleseedpythonbuiltin)
{
    bind_appleseed_python_classes();
}

// Generated by BOOST_PYTHON_MODULE() declarations in module.cpp files.
extern "C" void init_appleseedstudio();

namespace appleseed {
namespace studio {

//
// PythonInterpreter class implementation
//

PythonInterpreter& PythonInterpreter::instance()
{
    static PythonInterpreter interpreter;
    return interpreter;
}

void PythonInterpreter::set_main_window(MainWindow* main_window)
{
    m_main_window = main_window;
}

MainWindow* PythonInterpreter::get_main_window() const
{
    return m_main_window;
}

namespace
{
    bf::path compute_site_packages_path()
    {
        // Start with the absolute path to appleseed.studio's executable.
        bf::path base_path(get_executable_path());

        // Strip appleseed.studio's executable filename from the path.
        base_path = base_path.parent_path();

        // Go up in the hierarchy until bin/ is found.
        while (base_path.filename() != "bin")
            base_path = base_path.parent_path();

        // One more step up to reach the parent of bin/.
        base_path = base_path.parent_path();

        // Compute full path.
        const bf::path site_pkg_path = base_path / "lib" / "python" / "site-packages";

        return safe_canonical(site_pkg_path);
    }

    bf::path compute_appleseed_python_module_path()
    {
        // Start with the absolute path to appleseed.studio's executable.
        bf::path base_path(get_executable_path());
        bf::path lib_path;

        // Strip appleseed.studio's executable filename from the path.
        base_path = base_path.parent_path();

        // Go up in the hierarchy until bin/ is found.
        while (base_path.filename() != "bin")
        {
            lib_path = base_path.filename() / lib_path;
            base_path = base_path.parent_path();
        }

        // One more step up to reach the parent of bin/.
        lib_path = "lib" / lib_path;
        base_path = base_path.parent_path();

        // Compute full path.
        lib_path = base_path / lib_path / "python";

        return safe_canonical(lib_path);
    }

    bf::path compute_bundled_plugins_path()
    {
        // Start with the absolute path to appleseed.studio's executable.
        bf::path base_path(get_executable_path());

        // Strip appleseed.studio's executable filename from the path.
        base_path = base_path.parent_path();

        // Go up in the hierarchy until bin directory is found.
        while (base_path.filename() != "bin")
            base_path = base_path.parent_path();

        // One more step up to reach the parent of bin directory.
        base_path = base_path.parent_path();

        // Compute full path.
        bf::path plugins_path = base_path / "studio" / "plugins";

        return safe_canonical(plugins_path);
    }
}

void PythonInterpreter::initialize(OutputRedirector redirector)
{
    //
    // When using Boost.Python as a shared libray, Boost.Python's C++ <-> Python registry
    // is shared between all loaded Boost.Python extension modules, and objects can be
    // converted between modules.
    //
    // When using Boost.Python as a static library however, each module has its own
    // registry and attempting to convert objects between modules fails.
    //
    // To summarize:
    //  - When using compiled modules with static Boost.Python, there are 2 registries.
    //  - When using compiled modules with shared Boost.Python, there is 1 registry.
    //
    // The solution is to build all extension modules into appleseed.studio such that
    // we get a single registry. That way appleseed.studio module can find a converter
    // from e.g. renderer::Project to appleseedpython.Project.
    //

    PyImport_AppendInittab("_appleseedpythonbuiltin", init_appleseedpythonbuiltin);
    PyImport_AppendInittab("_appleseedstudio", init_appleseedstudio);
    Py_Initialize();

    bpy::object main_module = bpy::import("__main__");
    m_main_namespace = main_module.attr("__dict__");

    // Locate the path to Python's site-packages/ directory.
    const bf::path site_packages_path = compute_site_packages_path();
    if (bf::is_directory(site_packages_path))
    {
        RENDERER_LOG_INFO("Python's site-packages directory found at %s.",
            site_packages_path.string().c_str());
    }
    else
    {
        RENDERER_LOG_WARNING(
            "Python's site-packages directory does not exist: %s",
            site_packages_path.string().c_str());
    }

    // Locate the path to appleseed Python module's directory.
    const bf::path appleseed_python_module_path = compute_appleseed_python_module_path();
    if (bf::is_directory(appleseed_python_module_path))
    {
        RENDERER_LOG_INFO(
            "appleseed Python module's directory found at %s.",
            appleseed_python_module_path.string().c_str());
    }
    else
    {
        RENDERER_LOG_WARNING(
            "appleseed Python module's directory does not exist: %s",
            appleseed_python_module_path.string().c_str());
    }

    // Add paths to Python's site-packages/ and appleseed Python module's directory to sys.path.
    bpy::import("sys").attr("path").attr("append")(site_packages_path.string());
    bpy::import("sys").attr("path").attr("append")(appleseed_python_module_path.string());

    // Redirect stdout and stderr to the output panel.
    bpy::class_<OutputRedirector>("OutputRedirector", bpy::no_init)
        .def("write", &OutputRedirector::write);

    bpy::object sys_module = bpy::import("sys");
    sys_module.attr("stdout") = redirector;
    sys_module.attr("stderr") = redirector;

    // Import Python modules.
    import_python_module("appleseed", "asr");
    import_python_module("appleseed.studio", "studio");
}

void PythonInterpreter::import_python_module(const char* module_name, const char* alias_name)
{
    execute(format("import {0}\n{1} = {0}\n", module_name, alias_name));
}

void PythonInterpreter::load_plugins()
{
    execute(
        format(
            "import appleseed.studio.plugins\n"
            "appleseed.studio.plugins.load_plugins('{0}')\n",
            compute_bundled_plugins_path().string()));
}

bpy::object PythonInterpreter::execute(const string& command, const bool notify)
{
    bpy::object result;

    try
    {
        const string escaped_command = replace(command, "\\", "\\\\");
        result = bpy::exec(escaped_command.c_str(), m_main_namespace, m_main_namespace);
    }
    catch (const bpy::error_already_set&)
    {
        PyErr_Print();
        return bpy::object();
    }

    if (notify && m_main_window != nullptr)
        m_main_window->on_project_change();

    return result;
}

PythonInterpreter::PythonInterpreter()
  : m_main_window(nullptr)
{
}

PythonInterpreter::~PythonInterpreter()
{
    // Commented as Boost.Python doesn't support Py_Finalize call.
    // Py_Finalize();
}

}   // namespace studio
}   // namespace appleseed
