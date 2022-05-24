#include "../include/ltm.h"
#include "../include/NetworkWidget.h"
#include "../include/CpuUtilizationWidget.h"

TaskManager::
TaskManager(QWidget *parent) : QMainWindow(parent)
{
  resize(800,800);
  tabs = new QTabWidget(this);
  setCentralWidget(tabs);
  processPage = new QWidget;
  auto processTab = new ProcessesTab(processPage);
  auto hb = new QHBoxLayout(processPage);
  hb->addWidget(processTab);
  performancePage = new QWidget;
  auto cpuUtilizationWidget = new CpuUtilizationWidget(performancePage);
  auto memoryTab = new MemoryWidget(performancePage);
  auto networkGraph = new NetworkWidget(performancePage);
  auto vb = new QVBoxLayout(performancePage);
  vb->addWidget(cpuUtilizationWidget);
  vb->addWidget(memoryTab);
  vb->addWidget(networkGraph);
  tabs->addTab(processPage, "Processes");
  tabs->addTab(performancePage, "Performance");
}

TaskManager::
~TaskManager()
{
  delete performancePage;
  delete processPage;
  delete tabs;
}
