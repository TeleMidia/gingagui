#ifndef RUNVIEWPLUGIN_H
#define RUNVIEWPLUGIN_H

#include <extensions/IPlugin.h>
#include <extensions/IPluginFactory.h>
#include <modules/ProjectControl.h>

#include "../include/gingaproxy.h"
#include "../include/gingaview.h"
#include "include/configdialog.h"

#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QToolButton>

using namespace cpr::core;

/*!
 * \brief Run View allows user to
 *        run the application from within composer."
 */
class RunViewPlugin : public IPlugin
{
  Q_OBJECT

public:
  explicit RunViewPlugin ();
  inline ~RunViewPlugin () { delete _runWidget; }
  inline QWidget *
  widget () override
  {
    return _runWidget;
  }

public slots:
  void init () override;
  void loadPreferences ();
  void playApplication ();
  void runPassiveDevice ();
  void runActiveDevice ();
  void execConfigDialog ();
  void updateGUI ();
  void handleError (QProcess::ProcessError error);

  inline bool
  eventFilter (QObject *obj, QEvent *event) override
  {
    if (event->type () == QEvent::Resize)
    {
      QString aspectRatio
          = _settings->value (Util::V_ASPECT_RATIO, "").toString ();
      if (aspectRatio == Util::WIDE)
      {
        _gingaView->setFixedHeight (_gingaView->width () * 9 / 16);
        return true;
      }
      else if (aspectRatio == Util::STANDARD)
      {
        _gingaView->setFixedHeight (_gingaView->width () * 3 / 4);
        return true;
      }
      else
      {
        _gingaView->setMinimumSize (0, 0);
        _gingaView->setMaximumSize (QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
      }
    }
    return IPlugin::eventFilter (obj, event);
  }

private:
  ProjectControl *_projectControl;
  Project *_currentProject;

  QWidget *_runWidget;
  GingaView *_gingaView;
  GingaProxy *_gingaProxy;

  QToolButton *_playButton;
  QMenu *menu_Multidevice;
  QPushButton *_stopButton;
  QLineEdit *_lineEdit;
  QSettings *_settings;
};

/*!
 * \brief Handles the creation and deletion of RunViewPlugin objects.
 */
class RunViewFactory : public QObject, public IPluginFactoryTpl<RunViewPlugin>
{
  Q_OBJECT
  CPR_PLUGIN_METADATA ("br.puc-rio.telemidia.run", "run-view.json")

public:
  RunViewFactory () {}
  ~RunViewFactory () { GingaProxy::deleteInstance (); }
};

inline Q_LOGGING_CATEGORY (CPR_PLUGIN_RUN, "cpr.plugin.run")

#endif // RUNVIEWPLUGIN_H
