#ifndef GNODE_H
#define GNODE_H

#include <QObject>

class GNode : public QObject
{
    Q_OBJECT

private:
    QList<GNode*> *incNodes;
    QString name;
public:
    explicit GNode(QObject *parent = 0);

    GNode(QString name);
    void smartAddingConnection(GNode *incNode);
    QString getName();
    void setName(QString name);
    QList<GNode*> *getIncNodes();
    ~GNode();

};

#endif // GNODE_H
