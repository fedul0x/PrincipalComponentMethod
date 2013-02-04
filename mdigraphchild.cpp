#include "mdigraphchild.h"

MdiGraphChild::MdiGraphChild()
{
}

MdiGraphChild::MdiGraphChild(const QVector<QPointF> *data)
{
    QPen pen = QPen(Qt::red);
    curve = new QwtPlotCurve;
    curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve->setPen(pen);
    curve->attach(this);
    //    QwtSeriesData* series = new QwtSeriesData;
    //    series->
    //    curve->setData(x, y, n);
    curve->setSamples(*data);
    curve->setVisible(true);
}

void MdiGraphChild::setData(const QVector<QPointF> *data)
{
    //REMOVE MEMORY LEAK
    QPen pen = QPen(Qt::red);
    curve = new QwtPlotCurve;
    curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve->setPen(pen);
    curve->attach(this);
    curve->setVisible(false);
    curve->setSamples(*data);
    curve->setVisible(true);
}

void MdiGraphChild::setCurFile(QString curFile)
{
    this->curFile = curFile;
    setWindowTitle(curFile + "[*]");
}
