#include "ProcessTable.h"
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include "Constants.h"

class ProcessesTab: public QWidget
{
public:
  ProcessesTab(QWidget *parent = nullptr);
  ~ProcessesTab();

private:
  QLineEdit *lineEdit;        // filter prompt
  ProcessTable* processTable; // table
  QLabel *cpuUtilizationLbl,  // labels beneath table
         *cpuUtilizationVal,
         *memoryUtilizationLbl, 
         *memoryUtilizationVal,
         *networkDownloadBandwidthLbl,
         *networkUploadBandwidthLbl,
         *networkDownloadBandwidthVal,
         *networkUploadBandwidthVal;

private slots:
  void onLineEditTextEdited(const QString&);
  void onCpuUtilizationChanged(const double&);
  void onMemoryUtilizationChanged(const double&);
  void onNetworkDownloadBandwidthChanged(const long long&);
  void onNetworkUploadBandwidthChanged(const long long&);
};
