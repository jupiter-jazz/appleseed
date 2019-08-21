
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2018 Francois Beaune, The appleseedhq Organization
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

#pragma once

// appleseed.studio headers.
#include "mainwindow/rendering/cameracontroller.h"
#include "mainwindow/rendering/lightpathspickinghandler.h"
#include "mainwindow/rendering/renderclipboardhandler.h"
#include "mainwindow/rendering/viewportwidget.h"
#include "utility/lpevalidator.h"
#include "utility/mousecoordinatestracker.h"
#include "utility/scrollareapanhandler.h"
#include "utility/widgetzoomhandler.h"

// appleseed.renderer headers.
#include "renderer/api/rendering.h"

// Qt headers.
#include <QObject>
#include <QWidget>

// Standard headers.
#include <memory>

// Forward declarations.
namespace appleseed { namespace studio { class ViewportTab; } }
namespace renderer  { class ParamArray; }
namespace renderer  { class Project; }
class QLabel;
class QLineEdit;
class QPoint;
class QRect;
class QScrollArea;
class QToolBar;
class QToolButton;

namespace appleseed {
namespace studio {

//
// Manages the state of the light paths overlay.
//

class LightPathsViewportManager
  : public QObject
{
    Q_OBJECT

  public:
    LightPathsViewportManager(
        ViewportTab*            viewport_tab,
        renderer::Project*      project,
        renderer::ParamArray&   settings);

    void reset(renderer::Project* project);

    QToolBar* toolbar() const;

    void set_enabled(const bool enabled);
    void set_picking_enabled(const bool enabled);
    void set_display_enabled(const bool enabled);

  signals:
    void signal_should_display(const bool should_display);

  public slots:
    void slot_entity_picked(const renderer::ScenePicker::PickingResult& result);
    void slot_rectangle_selection(const QRect& rect);
    void slot_light_paths_display_toggled(const bool active);

  private slots:
    void slot_base_layer_changed(const ViewportWidget::BaseLayer layer);
    void slot_light_path_selection_changed(
        const int               selected_light_path_index,
        const int               total_light_paths);
    void slot_save_light_paths();
    void slot_camera_changed();
    void slot_lpe_input_editing_finished();

  private:
    bool                                        m_enabled;
    bool                                        m_picking_enabled;
    bool                                        m_paths_display_active;

    renderer::Project*                          m_project;
    renderer::ParamArray&                       m_settings;
    ViewportTab*                                m_viewport_tab;
    QToolBar*                                   m_toolbar;
    QToolButton*                                m_prev_path_button;
    QToolButton*                                m_next_path_button;
    QLabel*                                     m_info_label;
    QLabel*                                     m_lpe_label;
    QLineEdit*                                  m_lpe_input;

    LpeValidator*                               m_lpe_validator;

    std::unique_ptr<ScrollAreaPanHandler>       m_pan_handler;
    std::unique_ptr<MouseCoordinatesTracker>    m_mouse_tracker;
    std::unique_ptr<LightPathsPickingHandler>   m_screen_space_paths_picking_handler;
    std::unique_ptr<LightPathsPickingHandler>   m_world_space_paths_picking_handler;

    void create_toolbar();
    void recreate_handlers();
};

}   // namespace studio
}   // namespace appleseed
