#ifndef MDIGRAPHCHILD_H
#define MDIGRAPHCHILD_H
#include <QWidget>
#include "qwt-6.0/qwt_plot.h"
#include "qwt-6.0/qwt_plot_curve.h"

class MdiGraphChild : public QwtPlot
{
public:
    MdiGraphChild();
    MdiGraphChild(const QVector<QPointF> *data);
    void setCurFile(QString curFile);
    void setData(const QVector<QPointF> *data);
    QString currentFile() { return curFile; }

private:
    QString curFile;
    QwtPlotCurve* curve;
};

#endif // MDIGRAPHCHILD_H
