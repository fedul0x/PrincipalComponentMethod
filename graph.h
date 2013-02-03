#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QTextStream>
#include "gnode.h"

class Graph : public QObject
{
    Q_OBJECT

private:

    int nodesCount;

    QList<GNode*> *createGNodesList(QMap<QString, QStringList> * nodes);
public:
    QList<GNode*> *gNodes;
    QList<QList<GNode*>*> *connectedComponentsList;
    QList<QList<GNode*>*> *findConnectedComponent(QList<GNode *> *);
    void SaveConnectedComponentsToFile(QString fileName);


    int GetLayerCount();
    QStringList *GetNodeNamesFromLayer(int layerIndex);

    QMap<QString, QStringList> *getNodesFromList(QStringList *list);
    int getNodesCount();

    Graph(QObject *parent = 0);
    Graph(QString fileName);
    Graph(QTextStream ts);
    ~Graph();
};

#endif // GRAPH_H
