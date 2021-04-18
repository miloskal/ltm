#include "../include/ProcessTable.h"
#include <QList>
#include <QTimer>
#include <algorithm>
#include <bits/stdc++.h>
#include <unistd.h> // for sysconf()

#include "../include/Constants.h"
#include "../include/ShellCommands.h"

ProcessTable::
ProcessTable(QWidget *parent) : QTableView(parent)
{
  // core
  cpuCores = std::stoi(executeShellCommand(SHELLCMD_GET_NUMBER_OF_CPU_CORES));
  if(cpuCores == 0)
    exit(1);
  columnNames = new QStringList;
  *columnNames << "PID" << "Name" << "User" << "CPU %" << "Memory %";
  filter = "";
  processes = new std::vector<Process>();
  dataModel = new QStandardItemModel(this);
  setModel(dataModel);
  rowsVec = new QVector<QStandardItem*>();
  sortCriterion = SORT_NONE;
  long cpuUnit = sysconf(_SC_CLK_TCK);
  if(cpuUnit == 0)
    exit(34);
  cpuCorrectionFactor = 100 / cpuUnit;

  // vectors to store pointers to - avoiding memory leak
  lists = new QVector<QList<QStandardItem*>*>();
  toFree = new QVector<Process*>();

  // visual
  resize(1400,1000);
  setShowGrid(false);
  setMouseTracking(true);
  verticalHeader()->hide();
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::ExtendedSelection);
  setAlternatingRowColors(true);
  QHeaderView *horHeader = horizontalHeader();
  horHeader->setSectionResizeMode(QHeaderView::Stretch);
  rowHoverDelegate = new RowHoverDelegate;
  setItemDelegate(rowHoverDelegate);
  vSlider = verticalScrollBar();
  vSliderValue = 0;

  // actions
  killSelectedProcesses = new QAction("Kill selected processes", this);

  // timer and signal/slot mechanism
  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &ProcessTable::parseProcessesAndUpdateTable);
  connect(horHeader, &QHeaderView::sectionClicked, this, &ProcessTable::onClickedColumn);
  connect(this, &ProcessTable::hoverIndexChanged, rowHoverDelegate, &RowHoverDelegate::onHoverIndexChanged);
  connect(this, &ProcessTable::leaveTableEvent, rowHoverDelegate, &RowHoverDelegate::onLeaveTableEvent);
  connect(killSelectedProcesses, &QAction::triggered, this, &ProcessTable::onKillSelectedProcesses);
  
  // start
  networkDownloadBandwidth = 0;
  networkUploadBandwidth = 0;
  lastDownloadSample = 0;
  lastUploadSample = 0;
  lastCpuUtilizationSample = 0;
  getCpuUtilization();
  getNetworkBandwidth();
  parseProcessesAndUpdateTable();
  timer->start(TIMEOUT_INTERVAL);
}

ProcessTable::
~ProcessTable()
{
  delete timer;
  delete killSelectedProcesses;
  delete rowHoverDelegate;
  delete rowsVec;
  delete dataModel;
  delete processes;
  delete columnNames;
}

void 
ProcessTable::
setFilter(std::string filter)
{
  this->filter = filter;
}


QTimer*
ProcessTable::
getTimer()
{
  return timer;
}

void 
ProcessTable::
parseProcessesAndUpdateTable()
{
  parseProcesses();
  sortProcesses();
  updateTable();
}

void 
ProcessTable::
parseProcesses()
{
  //for(auto iter = processes->begin(); iter < processes->end(); iter++)
    //delete *iter;
  processes->clear();
  std::string allLines, line;
  if(filter == "")
    allLines = executeShellCommand(SHELLCMD_GET_PROCESSES_INFO);
  else{
    std::string s(SHELLCMD_GET_PROCESSES_INFO);
    s += " | grep -i " + filter;
    allLines = executeShellCommand(s.c_str());
  }
  
  std::istringstream stream(allLines);
  while(getline(stream, line))
    addProcessToVector(line);
}

void 
ProcessTable::
updateTable()
{ 
  // save selected items
  vSliderValue = vSlider->value();
  selected = selectedIndexes();
  
  // destroy every Process object from the last timeout instance
  for(auto iter = toFree->begin(); iter < toFree->end(); iter++)
  {
      delete *iter;
  }
  
  // call destructor of every QStandardItem object from the last timeout instance
  for(auto iter = rowsVec->begin(); iter < rowsVec->end(); iter++)
  {
      delete *iter;
  }
  toFree->clear();
  rowsVec->clear();
  dataModel->clear();

  // call destructor of every list (row) in 'lists'
  for(auto iter = lists->begin(); iter < lists->end(); iter++)
  {
      delete *iter;
  }
  lists->clear();
  

  dataModel->setHorizontalHeaderLabels(*columnNames);
  memoryUtilization = 0;
  for(auto iter = processes->begin(); iter < processes->end(); iter++)
  {
    memoryUtilization += iter->getMemUsage();

    auto pidItem = new QStandardItem(1);
    auto nameItem = new QStandardItem(1);
    auto userItem = new QStandardItem(1);
    auto cpuItem = new QStandardItem(1);
    auto memItem = new QStandardItem(1);
    pidItem->setEditable(false);
    nameItem->setEditable(false);
    userItem->setEditable(false);
    cpuItem->setEditable(false);
    memItem->setEditable(false);


    
    pidItem->setText(QString::number(iter->getPid()));
    nameItem->setText(iter->getName());
    userItem->setText(iter->getUser());
    cpuItem->setText(QString::number(iter->getCpuUsage(), 'f', 2));
    memItem->setText(QString::number(iter->getMemUsage(), 'f', 2));
    
    auto rowList = new QList<QStandardItem*>();
     
    rowList->append(pidItem);
    rowList->append(nameItem);
    rowList->append(userItem);
    rowList->append(cpuItem);
    rowList->append(memItem);
    
    dataModel->appendRow(*rowList);
    
    // save pointers to QStandardItem objects to be able to free memory later
    rowsVec->append(pidItem);
    rowsVec->append(nameItem);
    rowsVec->append(userItem);
    rowsVec->append(cpuItem);
    rowsVec->append(memItem);

    // same idea for list which contains pointers to items of current row
    lists->append(rowList);
  }

  setSelectionMode(QAbstractItemView::MultiSelection);

  // re-select rows
  for(auto selectIndex = selected.begin(); selectIndex < selected.end(); selectIndex += COLUMNS_NUM)
  {
    int row = selectIndex->row();
    selectRow(row);
  }

  getCpuUtilization();
  getNetworkBandwidth();

  // restore vertical slider position
  vSlider->setMaximum(dataModel->rowCount());
  vSlider->setValue(vSliderValue);

  setSelectionMode(QAbstractItemView::ExtendedSelection);
  
  // emit 'ready to report' signal
  emit cpuUtilizationChanged(cpuUtilization);
  emit memoryUtilizationChanged(memoryUtilization);
  emit networkDownloadBandwidthChanged(networkDownloadBandwidth);
  emit networkUploadBandwidthChanged(networkUploadBandwidth);
}


void
ProcessTable::
getNetworkBandwidth()
{
  DIR* dp;
  struct dirent *entry;
  char s[BUFSIZE];
  FILE* fp;
  long long sumDownload = 0, sumUpload = 0;

  dp = opendir(NETWORK_BASE_DIR);
  if(dp == nullptr)
    exit(5);

  while((entry = readdir(dp)) != nullptr)
  {
    if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
      continue;

    if(entry->d_type == DT_DIR || entry->d_type == DT_LNK)
    {
      strncpy(s, NETWORK_BASE_DIR, BUFSIZE);
      strcat(s, entry->d_name);
      strcat(s, "/");
      strcat(s, "statistics/");
      if(chdir(s) != 0)
        exit(8);
      fp = fopen("rx_bytes", "r");
      if(fp == nullptr)
        exit(12);
      fgets(s, BUFSIZE, fp);
      sumDownload += atoll(s);
      fclose(fp);
      fp = fopen("tx_bytes", "r");
      if(fp == nullptr)
        exit(13);
      fgets(s, BUFSIZE, fp);
      sumUpload += atoll(s);
      fclose(fp);
    }
  }
  closedir(dp);

  networkDownloadBandwidth = sumDownload - lastDownloadSample;
  networkUploadBandwidth = sumUpload - lastUploadSample;
  lastDownloadSample = sumDownload;
  lastUploadSample = sumUpload;
}


void
ProcessTable::
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
  emit cpuUtilizationChanged(cpuUtilization);
}

void 
ProcessTable::
addProcessToVector(std::string line)
{
  if(line.find(" ") == 0)
  {
    std::istringstream lineStream(line);
    std::string buf;
    std::vector<std::string> tokens;
    while(getline(lineStream, buf, ' '))
      tokens.push_back(buf);

    long pid = std::stol(tokens[1]);
    float cpuUsage = std::stof(tokens[9]) / cpuCores;
    float memUsage = std::stof(tokens[10]);
    int ntokens;
    if((ntokens = tokens.size()) > 13)
    {
      for(int i = 13; i < ntokens; i++){
        tokens[12] += " " + tokens[i];
      }
    }
    QString name = QString::fromStdString(tokens[12]);
    QString user = QString::fromStdString(tokens[2]);
    auto p = new Process(pid, name, user, cpuUsage, memUsage);
    toFree->push_back(p);
    processes->push_back(*p);
  }
  else
  {
    std::istringstream lineStream(line);
    std::string buf;
    std::vector<std::string> tokens;
    while(getline(lineStream, buf, ' '))
      tokens.push_back(buf);

    long pid = std::stol(tokens[0]);
    float cpuUsage = std::stof(tokens[8]) / cpuCores;
    float memUsage = std::stof(tokens[9]);
    int ntokens;
    if((ntokens = tokens.size()) > 12)
    {
      for(int i = 12; i < ntokens; i++){
        tokens[11] += " " + tokens[i];
      }
    }
    QString name = QString::fromStdString(tokens[11]);
    QString user = QString::fromStdString(tokens[1]);
    auto p = new Process(pid, name, user, cpuUsage, memUsage);
    toFree->push_back(p);
    processes->push_back(*p);
  }
}

void 
ProcessTable::
onKillSelectedProcesses()
{
  int row;
  for(auto selectIndex = selected.begin(); selectIndex < selected.end(); selectIndex += COLUMNS_NUM)
  {
    row = selectIndex->row();
    if(kill(processes->at(row).getPid(), SIGTERM) != 0)
      exit(1);
  }
}

void 
ProcessTable::
sortProcesses()
{
  
  if(sortCriterion == SORT_BY_PID_ASCENDING)
    std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getPid() < p2.getPid(); });
  
  else if(sortCriterion == SORT_BY_PID_DESCENDING)
    std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getPid() > p2.getPid(); });
  
  else if(sortCriterion == SORT_BY_CPU_ASCENDING)
    std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getCpuUsage() < p2.getCpuUsage(); });
  
  else if(sortCriterion == SORT_BY_CPU_DESCENDING)
    std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getCpuUsage() > p2.getCpuUsage(); });
  
  else if(sortCriterion == SORT_BY_MEM_ASCENDING)
    std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getMemUsage() < p2.getMemUsage(); });
  
  else if(sortCriterion == SORT_BY_MEM_DESCENDING)
    std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getMemUsage() > p2.getMemUsage(); });
  
  else if(sortCriterion == SORT_BY_NAME_ASCENDING)
    std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getName() < p2.getName(); });
  
  else if(sortCriterion == SORT_BY_NAME_DESCENDING)
    std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getName() > p2.getName(); });
  
  else if(sortCriterion == SORT_BY_USER_ASCENDING)
    std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getUser() < p2.getUser(); });
  
  else if(sortCriterion == SORT_BY_USER_DESCENDING)
    std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getUser() > p2.getUser(); });

  else
    return;
}


void 
ProcessTable::
onClickedColumn(int col)
{
  if(col == 0) // sort by pid
  {
    if(sortCriterion == SORT_BY_PID_ASCENDING)
    {
      sortCriterion = SORT_BY_PID_DESCENDING;
      std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getPid() > p2.getPid(); });
    }
    else
    {
      sortCriterion = SORT_BY_PID_ASCENDING;
      std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getPid() < p2.getPid(); });
    }
    updateTable();
  }
  else if(col == 1) // sort by process name
  {
    if(sortCriterion == SORT_BY_NAME_ASCENDING)
    {
      sortCriterion = SORT_BY_NAME_DESCENDING;
      std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getName() > p2.getName(); });
    }
    else
    {
      sortCriterion = SORT_BY_NAME_ASCENDING;
      std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getName() < p2.getName(); });
    }
    updateTable();
  }
  else if(col == 2) // sort by user
  {
    if(sortCriterion == SORT_BY_USER_ASCENDING)
    {
      sortCriterion = SORT_BY_USER_DESCENDING;
      std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getUser() > p2.getUser(); });
    }
    else
    {
      sortCriterion = SORT_BY_USER_ASCENDING;
      std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getUser() < p2.getUser(); });
    }
    updateTable();
  }
  else if(col == 3) // sort by cpu usage
  {
    if(sortCriterion == SORT_BY_CPU_DESCENDING)
    {
      sortCriterion = SORT_BY_CPU_ASCENDING;
      std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getCpuUsage() < p2.getCpuUsage(); });
    }
    else
    {
      sortCriterion = SORT_BY_CPU_DESCENDING;
      std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getCpuUsage() > p2.getCpuUsage(); });
    }
    updateTable();
  }
  else if(col == 4) // sort by memory usage
  {
    if(sortCriterion == SORT_BY_MEM_DESCENDING)
    {
      sortCriterion = SORT_BY_MEM_ASCENDING;
      std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getMemUsage() < p2.getMemUsage(); });
    }
    else
    {
      sortCriterion = SORT_BY_MEM_DESCENDING;
      std::sort(processes->begin(), processes->end(), [] (Process p1, Process p2) {return p1.getMemUsage() > p2.getMemUsage(); });
    }
    updateTable();
  }
  else
    return;
}

void 
ProcessTable::
mouseMoveEvent(QMouseEvent *event)
{
  // detect where the mouse cursor is relative to our custom table widget
  QModelIndex index = indexAt(event->pos());
  emit hoverIndexChanged(index);
}

void 
ProcessTable::
leaveEvent(QEvent *event)
{
  // detect when the mouse cursor leaves our custom table widget
  emit leaveTableEvent();
  viewport()->update();
}



#ifndef QT_NO_CONTEXTMENU
void
ProcessTable::
mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::RightButton)
  {
    mainMenu = new QMenu(this);
    mainMenu->addAction(killSelectedProcesses);
    mainMenu->exec(event->globalPos());
  }
  else if(event->button() == Qt::LeftButton && !(indexAt(event->pos()).isValid()))
  {
    clearSelection();
  } 
}

#endif
