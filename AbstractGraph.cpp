#include "AbstractGraph.h"

#include <QTimer>
#include <QAction>
#include <QMenu>
#include <algorithm>


AbstractGraph::
AbstractGraph(QWidget *parent) : QWidget(parent)
{
  graph = new QCustomPlot;
  frameSize = 121;
  timeoutInterval = 1000;
  
  initializeVectors(frameSize);
  graph->addGraph();
  graph->xAxis->setRange(0, frameSize - 1);
  graph->graph(0)->setData(x, y);
  // define actions
  setFrameSize30 = new QAction("30s", graph);
  setFrameSize60 = new QAction("1min", graph);
  setFrameSize120 = new QAction("2min", graph);
  setFrameSize180 = new QAction("3min", graph);
  setFrameSize300 = new QAction("5min", graph);
  exportAsPdf = new QAction("pdf", graph);
  exportAsPng = new QAction("png", graph);
  exportAsJpg = new QAction("jpg", graph);
  exportAsBmp = new QAction("bmp", graph);
  
  timer = new QTimer(this);
  
  // connect actions and signals with corresponding slots
  connect(setFrameSize30, &QAction::triggered, this, &AbstractGraph::onSetFrameSize30);
  connect(setFrameSize60, &QAction::triggered, this, &AbstractGraph::onSetFrameSize60);
  connect(setFrameSize120, &QAction::triggered, this, &AbstractGraph::onSetFrameSize120);
  connect(setFrameSize180, &QAction::triggered, this, &AbstractGraph::onSetFrameSize180);
  connect(setFrameSize300, &QAction::triggered, this, &AbstractGraph::onSetFrameSize300);
  connect(exportAsPdf, &QAction::triggered, this, &AbstractGraph::onExportAsPdf);
  connect(exportAsPng, &QAction::triggered, this, &AbstractGraph::onExportAsPng);
  connect(exportAsJpg, &QAction::triggered, this, &AbstractGraph::onExportAsJpg);
  connect(exportAsBmp, &QAction::triggered, this, &AbstractGraph::onExportAsBmp);
  connect(graph, &QCustomPlot::mousePress, this, &AbstractGraph::mouseReleaseEvent);
}

AbstractGraph::
~AbstractGraph()
{
  delete timer;
  delete exportAsBmp;
  delete exportAsJpg;
  delete exportAsPng;
  delete exportAsPdf;
  delete setFrameSize300;
  delete setFrameSize180;
  delete setFrameSize120;
  delete setFrameSize60;
  delete setFrameSize30;
  delete graph;
}

void
AbstractGraph::
initializeVectors(int n)
{
  x.resize(n);
  y.resize(n);
  for(int i = 0; i < n; i++){
    x[i] = i;
    y[i] = 0;
  }
}

void 
AbstractGraph::
onSetFrameSize30()
{
  int oldFrameSize = frameSize;
  frameSize = 31;
  updateGraph(oldFrameSize, frameSize);
}

void 
AbstractGraph::
onSetFrameSize60()
{
  int oldFrameSize = frameSize;
  frameSize = 61;
  updateGraph(oldFrameSize, frameSize);
}

void 
AbstractGraph::
onSetFrameSize120()
{
  int oldFrameSize = frameSize;
  frameSize = 121;
  updateGraph(oldFrameSize, frameSize);
}

void 
AbstractGraph::
onSetFrameSize180()
{
  int oldFrameSize = frameSize;
  frameSize = 181;
  updateGraph(oldFrameSize, frameSize);
}

void 
AbstractGraph::
onSetFrameSize300()
{
  int oldFrameSize = frameSize;
  frameSize = 301;
  updateGraph(oldFrameSize, frameSize);
}

void 
AbstractGraph::
onExportAsPdf()
{
  QString filename = QFileDialog::getSaveFileName(this, tr("Save as PDF"));
  graph->savePdf(filename);
}

void 
AbstractGraph::
onExportAsPng()
{
  QString filename = QFileDialog::getSaveFileName(this, tr("Save as PNG"));
  graph->savePng(filename);
}

void 
AbstractGraph::
onExportAsJpg()
{
  QString filename = QFileDialog::getSaveFileName(this, tr("Save as JPG"));
  graph->saveJpg(filename);
}

void 
AbstractGraph::
onExportAsBmp()
{
  QString filename = QFileDialog::getSaveFileName(this, tr("Save as BMP"));
  graph->saveBmp(filename);
}

#ifndef QT_NO_CONTEXTMENU
void 
AbstractGraph::
mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::RightButton){
    QMenu *mainMenu = new QMenu(graph);
    QMenu* changeFrameSizeMenu = mainMenu->addMenu("Change frame size");
    changeFrameSizeMenu->addAction(setFrameSize30);
    changeFrameSizeMenu->addAction(setFrameSize60);
    changeFrameSizeMenu->addAction(setFrameSize120);
    changeFrameSizeMenu->addAction(setFrameSize180);
    changeFrameSizeMenu->addAction(setFrameSize300);
    QMenu *exportMenu = mainMenu->addMenu("Export graph as");
    exportMenu->addAction(exportAsPdf);
    exportMenu->addAction(exportAsPng);
    exportMenu->addAction(exportAsJpg);
    exportMenu->addAction(exportAsBmp);
    mainMenu->exec(event->globalPos());
  }
}
#endif

