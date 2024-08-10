#include "../include/NetworkWidget.h"
#include "../include/Constants.h"
#include "../include/ErrorHandler.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <iostream>
#include <fstream>

// for chdir(), opendir() and readdir()
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

NetworkWidget::
NetworkWidget(QWidget* parent) : AbstractGraph(parent)
{
  resize(800, 300);
  // add another graph for upload bandwidth 
  graph->addGraph();

  // labels above graph
  auto netW = new QWidget(this);
  downLbl = new QLabel("Download Speed: ", netW);
  downVal = new QLabel("   ", netW);
  upLbl = new QLabel("Upload Speed: ", netW);
  upVal = new QLabel("   ", netW);
  
  // set font
  font = new QFont;
  font->setPointSize(14);
  downLbl->setFont(*font);
  downVal->setFont(*font);
  upLbl->setFont(*font);
  upVal->setFont(*font);

  font->setPointSize(12);
  graph->xAxis->setLabelFont(*font);
  graph->yAxis->setLabelFont(*font);
  
  // set size policies for labels
  downLbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  downVal->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  upLbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  upVal->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  
  // put labels in horizontal box
  QHBoxLayout *netLyt = new QHBoxLayout(netW);
  netLyt->addWidget(downLbl);
  netLyt->addWidget(downVal);
  netLyt->addWidget(upLbl);
  netLyt->addWidget(upVal);
  
  // prepare to plot data
  lastDownloadSample = lastUploadSample = 0;
  max = -1;
  initializeVectors(frameSize);
  yUnit = UNIT_MEGABYTE;
  iteration = 0;
  getNetworkBandwidth();
  graph->yAxis->setRange(0,5);

  // set labels on graph
  QString lbl = "Bandwidth (" + yUnit + ")";
  graph->yAxis->setLabel(lbl);
  lbl = tr("Time (sec)");
  graph->xAxis->setLabel(lbl);
  
  // style graph
  graph->graph(0)->setPen(QPen(Qt::blue));
  graph->graph(0)->setBrush(QBrush(QColor(0,0,255,20)));
  graph->graph(1)->setPen(QPen(Qt::red));
  graph->graph(1)->setBrush(QBrush(QColor(255,0,0,20)));

  // expand graph as much as possible on this widget
  graph->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
  // put labels and graph to vertical layout
  auto layout = new QVBoxLayout(this);
  layout->addWidget(netW);
  layout->addWidget(graph);
 
  // connect signals with slots
  connect(timer, &QTimer::timeout, this, &NetworkWidget::getNetworkBandwidth);

  // start
  timer->start(timeoutInterval);
}

NetworkWidget::
~NetworkWidget()
{
  delete font;
  delete downLbl;
  delete downVal;
  delete upVal;
  delete upLbl;
}

void
NetworkWidget::
initializeVectors(int n)
{
  x.resize(n);
  y.resize(n);
  y1.resize(n);
  bandsInBytes.resize(n);
  for(int i = 0; i < n; i++){
    x[i] = i;
    y[i] = 0;
    y1[i] = 0;
    bandsInBytes[i] = 0;
  }
}

void 
NetworkWidget::
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
  graph->graph(1)->setData(x,y1);
  graph->xAxis->setRange(0, newFrameSize - 1);
  graph->replot();
}

void
NetworkWidget::
convertVectors(QString& oldUnit, QString& newUnit)
{
  // convert to bytes and store result to temp array
  // then convert these values to newUnit and restore them

  if(oldUnit == newUnit)
    return;

  QVector<double> temp;
  if(oldUnit == UNIT_MEGABYTE)
    for(auto iter = y.begin(); iter < y.end(); iter++)
      temp.push_back(*iter * BYTES_IN_MEGABYTE);
  else if(oldUnit == UNIT_KILOBYTE)
    for(auto iter = y.begin(); iter < y.end(); iter++)
      temp.push_back(*iter * BYTES_IN_KILOBYTE);
  
  else if(oldUnit == UNIT_BYTE)
    for(auto iter = y.begin(); iter < y.end(); iter++)
      temp.push_back(*iter);
  
  else return;

  y.clear();
  if(newUnit == UNIT_MEGABYTE)
    for(auto iter = temp.begin(); iter < temp.end(); iter++)
      y.push_back(*iter/BYTES_IN_MEGABYTE);
  else if(newUnit == UNIT_KILOBYTE)
    for(auto iter = temp.begin(); iter < temp.end(); iter++)
      y.push_back(*iter/BYTES_IN_KILOBYTE);
  else
    for(auto iter = temp.begin(); iter < temp.end(); iter++)
      y.push_back(*iter);

  temp.clear();

  if(oldUnit == UNIT_MEGABYTE)
    for(auto iter = y1.begin(); iter < y1.end(); iter++)
      temp.push_back(*iter * BYTES_IN_MEGABYTE);
  else if(oldUnit == UNIT_KILOBYTE)
    for(auto iter = y1.begin(); iter < y1.end(); iter++)
      temp.push_back(*iter * BYTES_IN_KILOBYTE);
  else if(oldUnit == UNIT_BYTE)
    for(auto iter = y1.begin(); iter < y1.end(); iter++)
      temp.push_back(*iter);
  else
    return;

  y1.clear();
  if(newUnit == UNIT_MEGABYTE)
    for(auto iter = temp.begin(); iter < temp.end(); iter++)
      y1.push_back(*iter/BYTES_IN_MEGABYTE);
  else if(newUnit == UNIT_KILOBYTE)
    for(auto iter = temp.begin(); iter < temp.end(); iter++)
      y1.push_back(*iter/BYTES_IN_KILOBYTE);
  else
    for(auto iter = temp.begin(); iter < temp.end(); iter++)
      y1.push_back(*iter);  
  
  oldUnit = newUnit;
}

void
NetworkWidget::
getNetworkBandwidth()
{
  DIR* dp;
  struct dirent *entry;
  char s[BUFSIZE];
  FILE* fp;
  long long sumDownload = 0, sumUpload = 0;
  double newDownValue = 0, newUpValue = 0;

  dp = opendir(NETWORK_BASE_DIR);
  if(dp == nullptr)
    error_fatal("opendir");

  while((entry = readdir(dp)) != nullptr)
  {
    if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
      continue;

    if(entry->d_type == DT_DIR || entry->d_type == DT_LNK)
    {
      strncpy(s, NETWORK_BASE_DIR, BUFSIZE);
      strncat(s, entry->d_name, BUFSIZE);
      strncat(s, "/statistics/", BUFSIZE);
      if(chdir(s) != 0)
        error_fatal("chdir");
      fp = fopen("rx_bytes", "r");
      if(fp == nullptr)
        error_fatal("fopen");
      if (fgets(s, BUFSIZE, fp) == nullptr)
          error_fatal("fgets");
      sumDownload += atoll(s);
      fclose(fp);
      fp = fopen("tx_bytes", "r");
      if(fp == nullptr)
        error_fatal("fopen");
      memset(s, 0, BUFSIZE);
      if (fgets(s, BUFSIZE, fp) == nullptr)
          error_fatal("fgets");
      sumUpload += atoll(s);
      fclose(fp);
    }
  }
  closedir(dp);

  networkDownloadBandwidth = sumDownload - lastDownloadSample;
  networkUploadBandwidth = sumUpload - lastUploadSample;
  biggerBandwidth = networkDownloadBandwidth > networkUploadBandwidth 
                  ? networkDownloadBandwidth : networkUploadBandwidth;
  lastDownloadSample = sumDownload;
  lastUploadSample = sumUpload;
  
  if(iteration > 0){
    bandsInBytes.push_back(biggerBandwidth);
    max = max > biggerBandwidth ? max : biggerBandwidth;
  }

  if(bandsInBytes.size() > frameSize){
    if(max == bandsInBytes[0])
      max = *std::max_element(bandsInBytes.begin() + 1, bandsInBytes.end());
    bandsInBytes.erase(bandsInBytes.begin());
  }
  QString newUnit;
  if(max < BYTES_IN_KILOBYTE)
    newUnit = UNIT_BYTE;
  else if(max >= BYTES_IN_KILOBYTE && max < BYTES_IN_MEGABYTE)
    newUnit = UNIT_KILOBYTE;
  else if(max >= BYTES_IN_MEGABYTE)
    newUnit = UNIT_MEGABYTE;

  convertVectors(yUnit, newUnit);
  if(iteration > 0)
    scaleGraph(max);

  // add new values to vectors y(download) and y1(upload)
  // in appropriate unit
  if(yUnit == UNIT_MEGABYTE){
    newDownValue = networkDownloadBandwidth / BYTES_IN_MEGABYTE;
    newUpValue = networkUploadBandwidth / BYTES_IN_MEGABYTE;
  }
  else if(yUnit == UNIT_KILOBYTE){
    newDownValue = networkDownloadBandwidth / BYTES_IN_KILOBYTE;
    newUpValue = networkUploadBandwidth / BYTES_IN_KILOBYTE;
  }
  else if(yUnit == UNIT_BYTE){
    newDownValue = networkDownloadBandwidth;
    newUpValue = networkUploadBandwidth;
  }
  if(iteration > 0){
    y.push_back(newDownValue);
    y1.push_back(newUpValue);
  }
  
  // if vectors reached frameSize elements, remove first element
  if(y.size() > frameSize)
    y.erase(y.begin());
  if(y1.size() > frameSize)
    y1.erase(y1.begin());
  graph->graph(0)->setData(x,y);
  graph->graph(1)->setData(x,y1);
  graph->replot();
  graph->update();
  downVal->setText(QString::number(newDownValue, 'd', 2) + " " + yUnit + "/s");
  upVal->setText(QString::number(newUpValue, 'd', 2) + " " + yUnit + "/s");

  if(iteration > 150)
    iteration = 1;
  else
    iteration++;
}


void NetworkWidget::
scaleGraph(long long bytes)
{
  if(bytes >= BYTES_IN_MEGABYTE) // convert to megabytes
    graph->yAxis->setRange(0, bytes/BYTES_IN_MEGABYTE + 1);
  
  else if(bytes < BYTES_IN_MEGABYTE && bytes >= BYTES_IN_KILOBYTE) // to kilobytes
    graph->yAxis->setRange(0, bytes/BYTES_IN_KILOBYTE + 1);
    
  else // to bytes
    graph->yAxis->setRange(0, BYTES_IN_KILOBYTE);
    
  QString lbl = "Bandwidth (" + yUnit + ")";
  graph->yAxis->setLabel(lbl);
}
