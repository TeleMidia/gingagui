#ifndef UTIL_H
#define UTIL_H

#include <QStringList>
#include <QString>
#include <QVector>

typedef struct {
  QString command;
  QString code;
  QString messageKey;
  QVector <QString> data;

} GingaMessage;

class Util
{   
public:
  static void init ();
  static GingaMessage parseMessage (QString);
  static QString secondsToString (int);

  static int SCREEN_HEIGHT;
  static int SCREEN_WIDTH;

  //Util strings
  static QString VERSION;
  static QString CMD_PREFIX;

  //Ginga wilcard strings
  static QString GINGA_KEY_PREFIX;
  static QString GINGA_CLICK_PREFIX;
  static QString GINGA_COMMAND_PREFIX;
  static QString GINGA_QUIT;
  static QString GINGA_PAUSE_KEY;
  static QString GINGA_TS_FILE;

  //Preferences labels
  static QString PREFERENCES_ENVIRONMENT;
  static QString PREFERENCES_GINGA;
  static QString PREFERENCES_ADVANCED;

  //GingaGUI wilcard strings
  static QString GUI_NCL;
  static QString GUI_WID;
  static QString GUI_FILE;
  static QString GUI_SCREENSIZE;

  //GingaGui variables
  //"V" stands for "variable"
  static QString V_LOCATION;
  static QString V_CONTEXT_FILE;
  static QString V_PARAMETERS;
  static QString V_ASPECT_RATIO;
  static QString V_LAST_DIR;
  static QString V_FILES;
  static QString V_AUTOPLAY;
  static QString V_SCREENSIZE;
  static QString V_SCREENPOS;
  static QString V_PASSIVE;
  static QString V_DEVICE_PORT;
  static QString V_ENABLE_LOG;
  static QString V_LANG;
  static QString V_EMBEDDED;
  static QString V_RUN_AS;


  //Ginga constants
  static QString G_ON_BEGIN;
  static QString G_ON_END;

  //GingaGui variables values
  static QString TRUE_;
  static QString FALSE_;
  static QString WIDE;
  static QString STANDARD;
  static int DEFAULT_PORT;

  static QStringList split(QString parameters);
  static QString defaultParameters ();
};


#endif // UTIL_H
