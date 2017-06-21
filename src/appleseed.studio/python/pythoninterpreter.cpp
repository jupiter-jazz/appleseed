
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2017 Gleb Mishchenko, The appleseedhq Organization
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

// appleseed.foundation headers.
#include "foundation/platform/path.h"
#include "foundation/platform/python.h"

// Boost headers.
#include "boost/filesystem.hpp"

namespace bpy = boost::python;
namespace bf = boost::filesystem;

// Function prototypes.
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

void PythonInterpreter::redirect_output(OutputRedirector redirector)
{
    bpy::class_<OutputRedirector>("OutputRedirector", bpy::no_init)
        .def("write", &OutputRedirector::write);

    bpy::object sys_module = bpy::import("sys");
    sys_module.attr("stdout") = redirector;
    sys_module.attr("stderr") = redirector;
}

void PythonInterpreter::execute_command(const char* command)
{
    PyRun_SimpleString(command);
}

PythonInterpreter::PythonInterpreter()
{
    PyImport_AppendInittab("_appleseedstudio", init_appleseedstudio);
    Py_Initialize();

    const bf::path bin(foundation::get_executable_directory());
    const bf::path configuration = bin.filename();
    const bf::path lib(bin.parent_path().parent_path()
                       / "lib" / configuration / "python2.7");

    // Add path to appleseed module to sys.path so python can find it.
    bpy::import("sys").attr("path").attr("append")(lib.string());

    // Imports appleseed module with 'asd' alias for it
    // and _appleseedstudio module with 'studio' alias for it.
    PyRun_SimpleString("import appleseed\n"
                       "import _appleseedstudio\n"
                       "asr = appleseed\n"
                       "studio = _appleseedstudio");
}

PythonInterpreter::~PythonInterpreter()
{
    Py_Finalize();
}

}   // namespace studio
}   // namespace appleseed
