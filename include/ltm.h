#include "MemoryWidget.h"
#include "ProcessesTab.h"
#include <QTabWidget>
#include <QMainWindow>

class TaskManager: public QMainWindow
{
public:
  TaskManager(QWidget *parent = nullptr);
  ~TaskManager();

private:
  QTabWidget *tabs;
  QWidget *processPage, *performancePage;
};
