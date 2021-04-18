#include "ltm.h"
#include <QApplication>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  TaskManager tm;
  tm.show();
  return app.exec();
}
