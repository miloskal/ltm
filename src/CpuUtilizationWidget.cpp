
#define BASE_CPU_TEMPERATURE_PATH "/sys/class/hwmon"

#include "../include/CpuUtilizationWidget.h"
#include <unistd.h> // for sysconf, fork, pipe
#include <string.h> // strstr, strchr
#include <ctype.h> // isdigit
#include <sys/wait.h> // wait
#include <sys/types.h> // opendir
#include <dirent.h> // readdir
#include <sstream>
#include <iostream>
#include "../include/ShellCommands.h"
#include "../include/ErrorHandler.h"

CpuUtilizationWidget::
CpuUtilizationWidget(QWidget *parent) : AbstractGraph(parent)
{
  graph->addGraph();
  cpuCores = sysconf(_SC_NPROCESSORS_ONLN);
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

  conPen = new QPen(Qt::red);  
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
    error_fatal("cpuUnit");
  cpuCorrectionFactor = 100 / cpuUnit;

  getCpuTemperaturePath(cpuTemperatureFile);
  // printf("Cpu Temperature file: %s\n", cpuTemperatureFile);
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
  delete font;
  delete conPen;
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
  unsigned long long userTime, niceTime, systemTime, totalUtilization;
  char buf[BUFSIZE];
  FILE *f = fopen("/proc/stat", "r");
  if(!f)
    error_fatal("fopen");
  if(!fgets(buf, BUFSIZE, f)) 
    error_fatal("fgets");
  sscanf(buf, "cpu %llu %llu %llu", &userTime, &niceTime, &systemTime);
  totalUtilization = userTime + niceTime + systemTime;
  cpuUtilization = (totalUtilization - lastCpuUtilizationSample) / ((double)cpuCores) * cpuCorrectionFactor;
  if(cpuUtilization > 100)
    cpuUtilization = 100;
  lastCpuUtilizationSample = totalUtilization;
  fclose(f);
}

void CpuUtilizationWidget::
getCpuTemperaturePath(char *path)
{
  char path_temp[BUFSIZE], file_path[BUFSIZE], buf[BUFSIZE], *t;
  struct dirent *entry, *entry_temp;
  DIR *d, *d_temp;
  FILE *f;
  
  if((d = opendir(BASE_CPU_TEMPERATURE_PATH)) == NULL)
    error_fatal("opendir");

  while((entry = readdir(d)) != NULL){ // for each entry in /sys/class/hwmon
    if(!strstr(entry->d_name, "hwmon") || (entry->d_type != DT_DIR && entry->d_type != DT_LNK))
      continue;
    snprintf(path_temp, BUFSIZE, "%s/%s", BASE_CPU_TEMPERATURE_PATH, entry->d_name);
    if((d_temp = opendir(path_temp)) == NULL)
      error_fatal("opendir");
    while((entry_temp = readdir(d_temp)) != NULL){ // for each entry in /sys/class/hwmon/hwmonX
      if(!strstr(entry_temp->d_name, "label")) // we need file called '*label*' 
        continue;
      snprintf(file_path, BUFSIZE, "%s/%s", path_temp, entry_temp->d_name);
      if((f = fopen(file_path, "r")) == NULL)
        error_fatal("fopen");
      if(!fgets(buf, BUFSIZE, f))
        error_fatal("fgets");
      if(strstr(buf, "Tctl")){ // check if '*label*' file content is 'Tctl'
        strncpy(path, file_path, BUFSIZE);
        t = strstr(path, "label");
        strncpy(t, "input", 6);
        fclose(f);
        closedir(d_temp);
        closedir(d);
        return;
      }
      fclose(f);
    }
    closedir(d_temp);
  }
  closedir(d);
  error_fatal("Couldn't find file containing 'Tctl'");
}

void
CpuUtilizationWidget::
getCpuTemperature()
{
  FILE *f;
  char buf[BUFSIZE], *z;
  int n, whole, decimal;
  if((f = fopen(cpuTemperatureFile, "r")) == NULL)
    error_fatal("fopen");
  
  if(!fgets(buf, BUFSIZE, f))
    error_fatal("fgets");

  z = strchr(buf, '\n');
  *z = 0;
  n = atoi(buf);
  whole = n / 1000;
  decimal = n / 100 % 10;
  cpuTemperature = whole + (double)decimal / 10; 
  
  snprintf(buf, 9, "%.1lf °C", cpuTemperature); 
  cpuTemperatureVal->setText(QString::fromStdString(buf));
  
  fclose(f);
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

