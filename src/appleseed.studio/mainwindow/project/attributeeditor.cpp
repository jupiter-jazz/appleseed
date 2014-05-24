
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2013 Francois Beaune, Jupiter Jazz Limited
// Copyright (c) 2014 Francois Beaune, The appleseedhq Organization
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
#include "attributeeditor.h"

// appleseed.studio headers.
#include "utility/miscellaneous.h"

// Qt headers.
#include <QLayout>
#include <QObject>
#include <Qt>

using namespace foundation;
using namespace renderer;
using namespace std;

namespace appleseed {
namespace studio {

AttributeEditor::AttributeEditor(
    QWidget*    parent,
    Project&    project)
  : m_parent(parent)
  , m_project(project)
{
}

void AttributeEditor::clear()
{
    if (m_parent->layout())
    {
        clear_layout(m_parent->layout());
        delete m_parent->layout();
    }
}

void AttributeEditor::edit(
    auto_ptr<EntityEditor::IFormFactory>    form_factory,
    auto_ptr<EntityEditor::IEntityBrowser>  entity_browser,
    auto_ptr<IEntityEditorFactory>			entity_editor_factory,
    const Dictionary&                       values,
    QObject*                                receiver,
    const char*                             slot_apply)
{
    m_entity_editor = entity_editor_factory->create(
        m_parent,
        m_project,
        form_factory,
        entity_browser,
        values);

    m_entity_editor->initialize();

    QObject::connect(
        m_entity_editor.get(), SIGNAL(signal_applied(foundation::Dictionary)),
        receiver, slot_apply);
}

}   // namespace studio
}   // namespace appleseed
