#define error_fatal(msg) do{fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, msg); \
                      exit(EXIT_FAILURE);} while(0)

#include "../include/CpuUtilizationWidget.h"
#include <unistd.h> // for sysconf, fork, pipe
#include <string.h> // strstr, strchr
#include <ctype.h> // isdigit
#include <sys/wait.h> // wait
#include <sstream>
#include <iostream>
#include "../include/ShellCommands.h"
#include "../include/Constants.h"

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
    exit(49);
  if(!fgets(buf, BUFSIZE, f)) 
    exit(50);
  sscanf(buf, "cpu %llu %llu %llu", &userTime, &niceTime, &systemTime);
  totalUtilization = userTime + niceTime + systemTime;
  cpuUtilization = (totalUtilization - lastCpuUtilizationSample) / ((double)cpuCores) * cpuCorrectionFactor;
  if(cpuUtilization > 100)
    cpuUtilization = 100;
  lastCpuUtilizationSample = totalUtilization;
  fclose(f);
}


void
CpuUtilizationWidget::
getCpuTemperature()
{
  int fd[2], ret;
  pid_t pid;
  char buf[BUFSIZE], res[BUFSIZE];
  char *p1, *p2;

  if(pipe(fd))
    error_fatal("pipe");

  pid = fork();
  if(pid < 0)
    error_fatal("fork");

  else if(pid > 0){ // parent
    ::close(fd[1]);
    memset(res, 0, BUFSIZE);
    wait(NULL); // wait for child to finish
    while((ret = read(fd[0], buf, BUFSIZE)) > 0){
      if((p1 = strstr(buf, "Tctl")) != NULL){
        p2 = strchr(p1, '.');
        if(!p2)
          error_fatal("strchr");
        while(!isdigit(*p1))
          ++p1;
        strncpy(res, p1, p2 - p1 + 5);
        *(p2 + 2) = 0;
        cpuTemperature = atof(p1);
        cpuTemperatureVal->setText(QString::fromStdString(res));
        ::close(fd[0]);
        return;
      }
    }
    if(ret < 0)
      error_fatal("read");
  }
  else{ // child
    ::close(fd[0]);
    if(dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO)
      error_fatal("dup2");
    ::close(fd[1]);
    char* const argv1[] = {"sensors", NULL};
    if(execvp("sensors", argv1) == -1)
      error_fatal("execvp");
  }
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

