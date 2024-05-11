#include "Process.h"
#include <QTableView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QHeaderView>
#include <vector>
#include "RowHoverDelegate.h"
#include <QScrollBar>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QTimer>
#include <dirent.h>
#include <iterator>
#include <sys/types.h>
#include <signal.h>

#include "ShellCommands.h"
#include "Constants.h"



class ProcessTable : public QTableView
{
  Q_OBJECT
  
public:
  ProcessTable(QWidget *parent = nullptr);
  ~ProcessTable();
  std::vector<Process> *processes;
  QVector<QList<QStandardItem*>*> *lists;
  QVector<Process*> *toFree;

  QTimer* getTimer();
  double cpuUtilization, memoryUtilization;
  long long networkDownloadBandwidth, 
            networkUploadBandwidth,
            lastDownloadSample,
            lastUploadSample;

  void setFilter(std::string filter);
  void parseProcessesAndUpdateTable();
  void getNetworkBandwidth();
  void getCpuUtilization();
private:
  QStandardItemModel *dataModel;
  QVector<QStandardItem*> *rowsVec;
  QStandardItem *pidItem, *nameItem, *userItem, *cpuItem, *memItem;
  QStringList *columnNames;
  std::string filter;
  RowHoverDelegate *rowHoverDelegate;
  int verticalValue, horizontalValue, sortCriterion, vSliderValue, cpuCores;
  QScrollBar *vSlider;
  QModelIndexList selected;
  QTimer *timer;
  long long lastCpuUtilizationSample;
  double cpuCorrectionFactor;
  QMenu *mainMenu;
  QAction *killSelectedProcesses;

 // void errorFatal(std::string msg);
  std::string getRidOfSlashes(const std::string &s);
  void sortProcesses();
  void addProcessToVector(std::string line);
  void updateTable();
  void parseProcesses();
  void mouseMoveEvent(QMouseEvent*) override;
  void leaveEvent(QEvent*) override;
  void sendSignalToProcess(const Process& p);
  void cleanUp(); // remove residuals spawned in previous timer iteration

  // action handlers
  void onKillSelectedProcesses();

private slots:
  void onClickedColumn(int col);
  void restartTimer();

signals:
  void hoverIndexChanged(QModelIndex index);
  void leaveTableEvent();
  void cpuUtilizationChanged(double);
  void memoryUtilizationChanged(double);
  void networkDownloadBandwidthChanged(long long);
  void networkUploadBandwidthChanged(long long);
protected:
  #ifndef QT_NO_CONTEXTMENU
  void mouseReleaseEvent(QMouseEvent *event) override;
  #endif

};
