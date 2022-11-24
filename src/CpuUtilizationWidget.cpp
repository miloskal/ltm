#include "../include/CpuUtilizationWidget.h"
#include <unistd.h> // for sysconf()
#include <sstream>
#include <iostream>
#include "../include/ShellCommands.h"

CpuUtilizationWidget::
CpuUtilizationWidget(QWidget *parent) : AbstractGraph(parent)
{
  graph->addGraph();
  cpuCores = std::stoi(executeShellCommand(SHELLCMD_GET_NUMBER_OF_CPU_CORES));
  graph->yAxis->setRange(0, 100);
  graph->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  auto labelsWidget = new QWidget(this);
  cpuUtilizationLbl = new QLabel("CPU Utilization:", labelsWidget);
  cpuUtilizationVal = new QLabel("", labelsWidget);
  cpuTemperatureLbl = new QLabel("CPU Temperature:", labelsWidget);
  cpuTemperatureVal = new QLabel("", labelsWidget);
  cpuUtilizationLbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  cpuUtilizationVal->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  cpuTemperatureLbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  cpuTemperatureVal->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  auto conPen = new QPen(Qt::red);  
  conPen->setStyle(Qt::DashLine);
  conPen->setColor(QColor(255,0,0,100));
  graph->graph(0)->setPen(QPen(Qt::blue));
  graph->graph(0)->setBrush(QBrush(QColor(0,0,255,20)));
  graph->graph(1)->setPen(*conPen);
  graph->graph(1)->setBrush(QBrush(QColor(255,0,0,20)));

  auto labelsHb = new QHBoxLayout(labelsWidget);
  labelsHb->addWidget(cpuUtilizationLbl);
  labelsHb->addWidget(cpuUtilizationVal);
  labelsHb->addWidget(cpuTemperatureLbl);
  labelsHb->addWidget(cpuTemperatureVal);

  auto vb = new QVBoxLayout(this);
  vb->addWidget(labelsWidget);
  vb->addWidget(graph);

  font = new QFont;
  font->setPointSize(14);
  cpuUtilizationLbl->setFont(*font);
  cpuUtilizationVal->setFont(*font);
  cpuTemperatureLbl->setFont(*font);
  cpuTemperatureVal->setFont(*font);

  font->setPointSize(12);
  graph->yAxis->setLabelFont(*font);
  graph->xAxis->setLabelFont(*font);

  QString lbl = "Cpu % / Heat";
  graph->yAxis->setLabel(lbl);
  lbl = tr("Time (sec)");
  graph->xAxis->setLabel(lbl);

  long cpuUnit = sysconf(_SC_CLK_TCK);
  if(cpuUnit == 0)
    exit(34);
  cpuCorrectionFactor = 100 / cpuUnit;

  initializeVectors(frameSize);
  lastCpuUtilizationSample = 0;
  getCpuUtilization();
  cpuTemperature = 0;
  getCpuTemperature();

  connect(timer, &QTimer::timeout, this, &CpuUtilizationWidget::updateWidget);

  timer->start(timeoutInterval);
}

CpuUtilizationWidget::
~CpuUtilizationWidget()
{
  delete cpuUtilizationVal;
  delete cpuUtilizationLbl;
}

void
CpuUtilizationWidget::
initializeVectors(int n)
{
  x.resize(n);
  y.resize(n);
  y1.resize(n);
  for(int i = 0; i < n; i++){
    x[i] = i;
    y[i] = 0;
    y1[i] = 0;
  }
}

void
CpuUtilizationWidget::
getCpuUtilization()
{
  long long userTime, niceTime, systemTime, totalUtilization;

  std::string s = executeShellCommand(SHELLCMD_GET_CPU_USAGE);
  std::istringstream ss(s);
  ss >> userTime >> niceTime >> systemTime;
  totalUtilization = userTime + niceTime + systemTime;
  cpuUtilization = (totalUtilization - lastCpuUtilizationSample) / ((double)cpuCores) * cpuCorrectionFactor;
  if(cpuUtilization > 100)
    cpuUtilization = 100;
  lastCpuUtilizationSample = totalUtilization;
}

void
CpuUtilizationWidget::
getCpuTemperature()
{
  std::string s = executeShellCommand(SHELLCMD_GET_CPU_TEMPERATURE);
  QString q = QString::fromStdString(s).remove("\n");
  cpuTemperatureVal->setText(q);
  std::string s2 = s.substr(0,4);
  q = QString::fromStdString(s2);
  cpuTemperature = q.toDouble();
}

void
CpuUtilizationWidget::
updateWidget()
{
  getCpuUtilization();
  y.push_back(cpuUtilization);
  if(y.size() > frameSize)
    y.erase(y.begin());
  getCpuTemperature();
  y1.push_back(cpuTemperature);
  if(y1.size() > frameSize)
    y1.erase(y1.begin());
  graph->graph(0)->setData(x,y);
  graph->graph(1)->setData(x,y1);
  graph->replot();
  graph->update();
  cpuUtilizationVal->setText(QString::number(cpuUtilization, 'd', 2) + " %");
}

void
CpuUtilizationWidget::
updateGraph(int oldFrameSize, int newFrameSize)
{
  QVector<double> tmp, tmp1;
  if(oldFrameSize < newFrameSize)
  {
    x.resize(newFrameSize);
    tmp.resize(oldFrameSize);
    tmp1.resize(oldFrameSize);
    for(int i = oldFrameSize - 1; i >= 0; i--){
      tmp[i] = y[i];
      tmp1[i] = y1[i];
    }

    y.resize(newFrameSize);
    y1.resize(newFrameSize);
    for(int i = 0; i < newFrameSize; i++){
      y[i] = 0;
      y1[i] = 0;
      if(i >= oldFrameSize)
        x[i] = i;
    }
 
    for(int i = oldFrameSize - 1, j = newFrameSize - 1; i >= 0; i--, j--){
      y[j] = tmp[i];
      y1[j] = tmp1[i];
    }

  }
  else if(oldFrameSize > newFrameSize)
  {
    x.resize(newFrameSize);
    tmp.resize(oldFrameSize);
    tmp1.resize(oldFrameSize);
    for(int i = oldFrameSize - 1, cnt = newFrameSize - 1; cnt >= 0; i--, cnt--){
      tmp[cnt] = y[i];
      tmp1[cnt] = y1[i];
    }
    
    y.resize(newFrameSize);
    y1.resize(newFrameSize);
    for(int i = 0; i < newFrameSize; i++){
      y[i] = 0;
      y1[i] = 0;
    }
    
    for(int i = newFrameSize - 1; i >= 0; i--){
      y[i] = tmp[i];
      y1[i] = tmp1[i];
    }
  }
  graph->graph(0)->setData(x, y);
  graph->graph(1)->setData(x, y1);
  graph->xAxis->setRange(0, newFrameSize-1);
  graph->replot();
}

