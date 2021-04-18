#include "AbstractGraph.h"

class NetworkWidget: public AbstractGraph
{
public:
  NetworkWidget(QWidget *parent = nullptr);
  ~NetworkWidget();

  void updateGraph(int, int);
  void initializeVectors(int n);
  void scaleGraph(long long);
  void getNetworkBandwidth();
  void convertVectors(QString&, QString&);
private:
  int iteration;
  QFont *font;
  QLabel *downLbl,
         *downVal,
         *upLbl,
         *upVal;
  long long networkDownloadBandwidth, 
            networkUploadBandwidth,
            lastDownloadSample,
            lastUploadSample,
            biggerBandwidth,
            max;
  QVector<long long> bandsInBytes;
  QVector<double> y1;
};
