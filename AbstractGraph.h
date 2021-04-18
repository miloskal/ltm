#ifndef AbstractGraph_H
#define AbstractGraph_H

#include <QWidget>
#include "qcustomplot.h"


class AbstractGraph: public QWidget
{
public:
  AbstractGraph(QWidget *parent = nullptr);
  virtual ~AbstractGraph();
  
protected:
  QCustomPlot *graph;
  int frameSize, timeoutInterval;
  QVector<double> x, y;
  QString xUnit, yUnit;
  QTimer *timer;
  QAction *setFrameSize30,
          *setFrameSize60,
          *setFrameSize120,
          *setFrameSize180,
          *setFrameSize300,
          *exportAsPdf,
          *exportAsPng,
          *exportAsJpg,
          *exportAsBmp;
    
  virtual void initializeVectors(int n);
  virtual void updateGraph(int oldFrameSize, int newFrameSize) = 0;

#ifndef QT_NO_CONTEXTMENU
void 
mouseReleaseEvent(QMouseEvent *event);
#endif

protected slots:
  void onSetFrameSize30();
  void onSetFrameSize60();
  void onSetFrameSize120();
  void onSetFrameSize180();
  void onSetFrameSize300();
  void onExportAsPdf();
  void onExportAsPng();
  void onExportAsJpg();
  void onExportAsBmp();

};

#endif
