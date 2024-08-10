#include "ProcessesTab.h"

ProcessesTab::
ProcessesTab(QWidget *parent) : QWidget(parent)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
  // filter prompt
  lineEdit = new QLineEdit(this);
  lineEdit->setPlaceholderText("Quick search (by pid, name, user...)");

  // table
  processTable = new ProcessTable(this);
  
  // labels beneath table
  QWidget *utilizationWidget = new QWidget(this);
  cpuUtilizationLbl = new QLabel("CPU Utilization: ", utilizationWidget);
  cpuUtilizationVal = new QLabel("", utilizationWidget);
  memoryUtilizationLbl = new QLabel("Memory Utilization: ", utilizationWidget);
  memoryUtilizationVal = new QLabel("", utilizationWidget);
  networkDownloadBandwidthLbl = new QLabel("Download: ", utilizationWidget);
  networkDownloadBandwidthVal = new QLabel("", utilizationWidget);
  networkUploadBandwidthLbl = new QLabel("Upload: ", utilizationWidget);
  networkUploadBandwidthVal = new QLabel("", utilizationWidget);
  QHBoxLayout *hb2 = new QHBoxLayout(utilizationWidget);
  hb2->addWidget(cpuUtilizationLbl);
  hb2->addWidget(cpuUtilizationVal);
  hb2->addWidget(memoryUtilizationLbl);
  hb2->addWidget(memoryUtilizationVal);
  hb2->addWidget(networkDownloadBandwidthLbl);
  hb2->addWidget(networkDownloadBandwidthVal);
  hb2->addWidget(networkUploadBandwidthLbl);
  hb2->addWidget(networkUploadBandwidthVal);

  auto vb = new QVBoxLayout(this);
  vb->addWidget(lineEdit);
  vb->addWidget(processTable);
  vb->addWidget(utilizationWidget);

  connect(lineEdit, &QLineEdit::textEdited, this, &ProcessesTab::onLineEditTextEdited);
  connect(processTable, &ProcessTable::cpuUtilizationChanged, this, &ProcessesTab::onCpuUtilizationChanged);
  connect(processTable, &ProcessTable::memoryUtilizationChanged, this, &ProcessesTab::onMemoryUtilizationChanged);
  connect(processTable, &ProcessTable::networkDownloadBandwidthChanged, this, &ProcessesTab::onNetworkDownloadBandwidthChanged);
  connect(processTable, &ProcessTable::networkUploadBandwidthChanged, this, &ProcessesTab::onNetworkUploadBandwidthChanged);
}

ProcessesTab::
~ProcessesTab()
{
  delete processTable;
  delete networkUploadBandwidthVal;
  delete networkUploadBandwidthLbl;
  delete networkDownloadBandwidthVal;
  delete networkDownloadBandwidthLbl;
  delete memoryUtilizationVal;
  delete memoryUtilizationLbl;
  delete cpuUtilizationVal;
  delete cpuUtilizationLbl;
  delete lineEdit;
}

void ProcessesTab::
onLineEditTextEdited(const QString& text)
{
  QTimer *timer = processTable->getTimer();
  timer->stop();
  processTable->setFilter(text.toStdString());
  processTable->parseProcessesAndUpdateTable();
  timer->start(TIMEOUT_INTERVAL);
}

void ProcessesTab::
onCpuUtilizationChanged(const double& cpuPercent)
{
  cpuUtilizationVal->setText(QString::number(cpuPercent, 'd', 2) + " %");
}

void ProcessesTab::
onMemoryUtilizationChanged(const double& memoryPercent)
{
  memoryUtilizationVal->setText(QString::number(memoryPercent, 'd', 2) + " %");
}

void ProcessesTab::
onNetworkDownloadBandwidthChanged(const long long& value)
{
  QString unit;
  if(value >= BYTES_IN_MEGABYTE){
    unit = " MB/s";
    networkDownloadBandwidthVal->setText(QString::number(value/BYTES_IN_MEGABYTE, 'd', 2) + unit);
  }
  else if(value < BYTES_IN_MEGABYTE && value >= BYTES_IN_KILOBYTE){
    unit = " KB/s";
    networkDownloadBandwidthVal->setText(QString::number(value/BYTES_IN_KILOBYTE, 'd', 2) + unit);
  }
  else if (value < BYTES_IN_KILOBYTE){
    unit = " B/s";
    networkDownloadBandwidthVal->setText(QString::number(value) + unit);
  }
}


void ProcessesTab::
onNetworkUploadBandwidthChanged(const long long& value)
{
  QString unit;
  if(value >= BYTES_IN_MEGABYTE){
    unit = " MB/s";
    networkUploadBandwidthVal->setText(QString::number(value/BYTES_IN_MEGABYTE, 'd', 2) + unit);
  }
  else if(value < BYTES_IN_MEGABYTE && value >= BYTES_IN_KILOBYTE){
    unit = " KB/s";
    networkUploadBandwidthVal->setText(QString::number(value/BYTES_IN_KILOBYTE, 'd', 2) + unit);
  }
  else if (value < BYTES_IN_KILOBYTE){
    unit = " B/s";
    networkUploadBandwidthVal->setText(QString::number(value) + unit);
  }
}
