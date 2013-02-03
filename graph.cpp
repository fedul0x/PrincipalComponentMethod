#include "graph.h"
#include "gexception.h"
#include "gfileexception.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QMap>
#include "log4qt/logger.h"

Graph::Graph(QObject *parent) :
    QObject(parent)
{
}

Graph::Graph(QString fileName)
{
    QFile file(fileName);

    if (fileName.isEmpty())
    {
        Log4Qt::Logger::rootLogger()->error("Application can't open in untitled file");
        throw GFileException("FileName is empty");
    }

    if (!file.size()>0)
    {
        Log4Qt::Logger::rootLogger()->error("Application can't open empty file "+file.fileName());
        throw GFileException("File is empty");
    }

    if (!QFile::exists(fileName))
    {
        Log4Qt::Logger::rootLogger()->error("Application can't open not existing file");
        throw GFileException("File does not exist");
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Log4Qt::Logger::rootLogger()->error("Application can't open not existing file "+file.fileName());
        throw GFileException("File cannot be openned!");
    }

    QTextStream in(&file);
    QStringList *list = new QStringList(); // осв
    while (!in.atEnd())
    {
        *list << in.readLine();
    }
    file.close();

    QMap<QString, QStringList> *nodes = getNodesFromList(list);
    delete list;

    gNodes = createGNodesList(nodes);
    delete nodes;

    connectedComponentsList = findConnectedComponent(gNodes);
}

Graph::Graph(QTextStream in)
{
    QStringList *list = new QStringList();
    while (!in.atEnd())
    {
        *list << in.readLine();
    }
    QMap<QString, QStringList> *nodes = getNodesFromList(list);
    delete list;
    gNodes = createGNodesList(nodes);
    delete nodes;
    connectedComponentsList = findConnectedComponent(gNodes);
}

Graph::~Graph()
{
    delete connectedComponentsList;
    delete gNodes;
}

int Graph::getNodesCount()
{
    return nodesCount;
}

QMap<QString, QStringList> *Graph::getNodesFromList(QStringList *list)
{
    QMap<QString, QStringList> *nodes = new QMap<QString, QStringList>(); //воз осв
    foreach (QString s, *list)
    {
        QStringList l = s.split(" ", QString::SkipEmptyParts);
        QString name = l[0];
        if (nodes->contains(name))
        {
            throw GFileException("Invalid file format");
        }
        l.removeAt(0);
        (*nodes)[name] = l;
    }
    return nodes;
}

QList<QList<GNode *> *> *Graph::findConnectedComponent(QList<GNode*> *gNodes)
{
    ///////// АЛГОРИТМ ////////
    //    §         Присвоим всем вершинам статус 1.

    //    §         Присвоим ИСХОДНОЙ вершине статус 2.

    //    §         Пока есть вершины имеющие статус = 2 выполнить

    //        o       Выберем любую вершину имеющую статус 2 назовем её ТЕКУЩЕЙ

    //        o       Для всех вершин инцидентных ТЕКУЩЕЙ выполнить

    //            §         Если вершина имеет статус 1 то её статус = 2

    //        o       Статус ТЕКУЩЕЙ вершины = 3


    QMap<QString , int> *statusesMap = new QMap<QString, int>(); // осв
    foreach (GNode *n, *gNodes)
    {
        statusesMap->insert(n->getName(), 1);
    }

    QList<QList<GNode *> *> * result = new QList<QList<GNode *> *>(); // воз !!! connComp
    while(statusesMap->values().contains(1))
    {
        (*statusesMap)[statusesMap->key(1)] = 2;

        QList<GNode *> * component = new QList<GNode *> (); //инсерт в result
        //QMapIterator<QString, int> iterStatuses(*statusesMap);
        while (statusesMap->values().contains(2))
        {
            int indexTwo = statusesMap->values().indexOf(2, 0);
            GNode *curNode = gNodes->at(indexTwo);
            if (!component->contains(curNode))
                component->insert(component->size(), curNode);

            foreach (GNode *node, (*curNode->getIncNodes()))
            {
                if (statusesMap->value(node->getName()) == 1)
                {
                    (*statusesMap)[node->getName()] = 2;
                    component->insert(component->size(), node);
                }
            }
            (*statusesMap)[curNode->getName()] = 3;
        }

        // добавляем компоненту в список компонент
        result->insert(result->size(), component);
    }

    delete statusesMap;

    return result;
}

void Graph::SaveConnectedComponentsToFile(QString fileName)
{
        QFile file(fileName);

        if (fileName.isEmpty())
        {
            throw GFileException("Application can't save connected components in untitled file");
        }

        if (file.exists())
        {
            if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
            {
                throw GFileException("Application can't save connected components in "+fileName);
            }
        }
        else
        {
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                throw GFileException("Application can't save connected components in "+fileName);
            }
        }
        QTextStream resultStream(&file);
        resultStream << "Graph contains " << connectedComponentsList->count()
                     << " connected components" << "\n";

        foreach (QList<GNode*>* q, *connectedComponentsList)
        {
            QString *record = new QString(); // осв
            foreach (GNode* n, *q)
            {
                record->append(n->getName()+ " ");
            }
            resultStream << *record << "\n";
            delete record;
        }
        file.close();
        Log4Qt::Logger::rootLogger()->info("Application saved connectedComponents in "+fileName);
}

QList<GNode *> *Graph::createGNodesList(QMap<QString, QStringList> * nodes)
{
    QList<GNode *> *res = new QList<GNode *>(); // пока не осв

    QList<QString> nodeNames = nodes->keys();
    for (int i = 0; i < nodeNames.count(); i++)
    {
        res->insert(i, new GNode(nodeNames.at(i))); // инс в рез потом освоб
    }

    for (int i = 0; i < nodeNames.count(); i++)
    {
        foreach (QString s, nodes->value(nodeNames.at(i)))
        {
            res->at(i)->smartAddingConnection(res->at(nodeNames.indexOf(s)));
        }
    }
    return res;
}
