#include "../include/MemoryWidget.h"
#include "../include/Constants.h"
#include "../include/ShellCommands.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <iostream>
#include <fstream>


MemoryWidget::
MemoryWidget(QWidget* parent) : AbstractGraph(parent)
{
  resize(800, 300);

  auto memW = new QWidget(this);
  memLbl = new QLabel("Memory Usage: ", memW);
  memVal = new QLabel("   ", memW);
  
  font = new QFont;
  font->setPointSize(14);
  memLbl->setFont(*font);
  memVal->setFont(*font);
  
  font->setPointSize(12);
  graph->xAxis->setLabelFont(*font);
  graph->yAxis->setLabelFont(*font);

  memLbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  memVal->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  QHBoxLayout *memLyt = new QHBoxLayout(memW);
  memLyt->addWidget(memLbl);
  memLyt->addWidget(memVal);
  
  std::string buf = executeShellCommand(SHELLCMD_GET_TOTAL_MEMORY);
  totalMemory = std::stoll(buf);

  if(totalMemory >= KILOBYTES_IN_GIGABYTE){
    graph->yAxis->setRange(0, totalMemory / KILOBYTES_IN_GIGABYTE + 1);
    yUnit = UNIT_GIGABYTE;
    totalMemoryQString = QString::number(totalMemory / KILOBYTES_IN_GIGABYTE, 'd', 1);
  }
  else if(totalMemory < KILOBYTES_IN_GIGABYTE && totalMemory >= KILOBYTES_IN_MEGABYTE){
    graph->yAxis->setRange(0, totalMemory / KILOBYTES_IN_MEGABYTE + 1);
    yUnit = UNIT_MEGABYTE;
    totalMemoryQString = QString::number(totalMemory / KILOBYTES_IN_MEGABYTE, 'd', 1);
  }
  else
    exit(23);
  QString lbl = "Usage (" + yUnit + ")";
  graph->yAxis->setLabel(lbl);
  lbl = tr("Time (sec)");
  graph->xAxis->setLabel(lbl);
  
  graph->graph(0)->setPen(QPen(Qt::blue));
  graph->graph(0)->setBrush(QBrush(QColor(0,0,255,20)));

  graph->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  auto layout = new QVBoxLayout(this);
  layout->addWidget(memW);
  layout->addWidget(graph);
 
  connect(timer, &QTimer::timeout, this, &MemoryWidget::getMemoryStatus);
  
  getMemoryStatus();
  timer->start(timeoutInterval);
}

MemoryWidget::
~MemoryWidget()
{
  delete font;
  delete memVal;
  delete memLbl;
}


void
MemoryWidget::
initializeVectors(int n)
{
  x.resize(n);
  y.resize(n);
  for(int i = 0; i < n; i++){
    x[i] = i;
    y[i] = 0;
  }
}


void 
MemoryWidget::
getMemoryStatus()
{
  long long newTemp;
  double newTemp2 = -1;
  std::string mem = executeShellCommand(SHELLCMD_GET_USED_MEMORY);
  newTemp = std::stoll(mem);
  if(yUnit == UNIT_GIGABYTE)
    newTemp2 = newTemp / KILOBYTES_IN_GIGABYTE;
  else if(yUnit == UNIT_MEGABYTE)
    newTemp2 = newTemp / KILOBYTES_IN_MEGABYTE;
  
  y.push_back(newTemp2);
  if(y.size() > frameSize)
    y.erase(y.begin());
  memVal->setText(QString::number(newTemp2, 'd', 1) + " / " + totalMemoryQString + " " + yUnit);
  graph->graph(0)->setData(x, y);
  graph->replot();
  graph->update();
}

void 
MemoryWidget::
updateGraph(int oldFrameSize, int newFrameSize)
{
  QVector<double> tmp;
  if(oldFrameSize < newFrameSize)
  {
    x.resize(newFrameSize);
    tmp.resize(oldFrameSize);
    for(int i = oldFrameSize - 1; i >= 0; i--)
      tmp[i] = y[i];

    y.resize(newFrameSize);
    for(int i = 0; i < newFrameSize; i++){
      y[i] = 0;
      if(i >= oldFrameSize)
        x[i] = i;
    }
 
    for(int i = oldFrameSize - 1, j = newFrameSize - 1; i >= 0; i--, j--)
      y[j] = tmp[i];
  }
  else if(oldFrameSize > newFrameSize)
  {
    x.resize(newFrameSize);
    tmp.resize(oldFrameSize);
    for(int i = oldFrameSize - 1, cnt = newFrameSize - 1; cnt >= 0; i--, cnt--)
      tmp[cnt] = y[i];
    
    y.resize(newFrameSize);
    for(int i = 0; i < newFrameSize; i++){
      y[i] = 0;
    }
    
    for(int i = newFrameSize - 1; i >= 0; i--)
      y[i] = tmp[i];
  }
  graph->graph(0)->setData(x, y);
  graph->xAxis->setRange(0, newFrameSize-1);
  graph->replot();
}
