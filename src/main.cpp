#include "ltm.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  TaskManager tm;
  tm.setWindowIcon(QIcon(app.applicationDirPath() + "/icon.png"));
  tm.show();
  return app.exec();
}
