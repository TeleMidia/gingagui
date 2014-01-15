#include "include/runviewplugin.h"

#include <QVBoxLayout>
#include <QFileInfo>
#include <QMessageBox>
#include <QStringList>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QKeyEvent>

int Util::SCREEN_HEIGHT;
int Util::SCREEN_WIDTH;

RunViewPlugin::RunViewPlugin()
{
  _settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                            "telemidia", "gingagui", this);
  _gingaView = new QnplView;
  _gingaView->installEventFilter(this);

  _runWidget = new QWidget;
  _runWidget->setFocusPolicy(Qt::ClickFocus);

  _projectControl = ProjectControl::getInstance();
  _gingaProxy = GingaProxy::getInstance();

  Util::SCREEN_HEIGHT = QApplication::desktop()->height();
  Util::SCREEN_WIDTH = QApplication::desktop()->width();

  _playButton = new QPushButton();
  _playButton->setIcon(QIcon (":/icons/play"));
  _playButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

  _stopButton = new QPushButton();
  _stopButton->setIcon(QIcon (":/icons/stop"));
  _stopButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
  _stopButton->setEnabled(false);

  QPushButton *configButton = new QPushButton;
  configButton->setIcon(QIcon(":/icons/config"));
  configButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

  _lineEdit = new QLineEdit();
  _lineEdit->setEnabled(false);

  QHBoxLayout *bottomLayout = new QHBoxLayout;
  bottomLayout->addWidget(_playButton);
  bottomLayout->addWidget(_stopButton);
  bottomLayout->addWidget(_lineEdit);
  bottomLayout->addWidget(configButton);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(_gingaView);
  layout->addLayout(bottomLayout);

  _runWidget->setLayout(layout);
  _runWidget->setFocusProxy(_gingaView);

  connect (_playButton, SIGNAL(pressed()),
           SLOT(playApplication()));
  connect (_stopButton, SIGNAL(pressed()),
           _gingaProxy, SLOT(stop()));
  connect (configButton, SIGNAL(pressed()),
           SLOT(execConfigDialog()));
  connect (_gingaProxy, SIGNAL(gingaStarted()),
           SLOT(updateGUI()));
  connect (_gingaProxy, SIGNAL(gingaFinished(int,QProcess::ExitStatus)),
           SLOT(updateGUI()));
  connect (_gingaView, SIGNAL(selected(QString)),
           _gingaProxy, SLOT(sendCommand(QString)));

  QString aspectRatio = _settings->value(Util::V_ASPECT_RATIO, "").toString();
  if (aspectRatio == Util::WIDE)
    _gingaView->resize(_gingaView->width(),_gingaView->width() * 9 / 16);
  else if (aspectRatio == Util::STANDARD)
    _gingaView->resize(_gingaView->width(),_gingaView->width() * 3 / 4);
}

void RunViewPlugin::updateGUI()
{
  if (_gingaProxy->state() == QProcess::Running)
  {
    _playButton->setEnabled(false);
    _stopButton->setEnabled(true);
  }
  else
  {
    _playButton->setEnabled(true);
    _stopButton->setEnabled(false);
  }
}

void RunViewPlugin::playApplication()
{
  QString location = _currentProject->getLocation();
  QString nclFilePath = location.mid (0, location.lastIndexOf(".")) + ".ncl";

  if (_currentProject->isDirty())
  {
    int answer = QMessageBox::question(0, "Save project", "Would you like to "
                                       "save the project before run?",
                                       QMessageBox::Yes, QMessageBox::No);
    if (answer == QMessageBox::Yes)
    {
      _projectControl->saveProject(_currentProject->getLocation());
      QFile file(nclFilePath);
      if(file.open(QFile::WriteOnly | QIODevice::Truncate))
      {
        if(project->getChildren().size())
          file.write(project->getChildren().at(0)->toString(0,false).toLatin1());

        file.close();
      }
    }
  }

  QString gingaLocation = _settings->value(Util::V_LOCATION, "").toString();
  QString contextLocation = _settings->value(Util::V_CONTEXT_FILE, "").toString();
  QString args = _settings->value(Util::V_PARAMETERS,
                                  Util::defaultParameters()).toString();

  QStringList argsList = args.split(" ");

  QString vmode = QString::number(_gingaView->width()) + "x" +
      QString::number(_gingaView->height());

  argsList.replaceInStrings(Util::GUI_FILE, nclFilePath);
  argsList.replaceInStrings(Util::GUI_WID, QString::number((unsigned long long)
                                                      _gingaView->winId()));
  argsList.replaceInStrings(Util::GUI_SCREENSIZE, vmode);

  argsList << "--context-dir" << contextLocation;

  qDebug () << gingaLocation;
  qDebug () << nclFilePath;
  qDebug () << argsList;
  _gingaProxy->setBinaryPath(gingaLocation);
  _gingaProxy->run(argsList);
  _gingaView->setFocus();
}

void RunViewPlugin::execConfigDialog()
{
  ConfigDialog *configDialog = new ConfigDialog(_runWidget);
  configDialog->exec();

  delete configDialog;
}
