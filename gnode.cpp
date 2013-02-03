#include "gnode.h"

GNode::GNode(QObject *parent) :
    QObject(parent)
{

}

GNode::GNode(QString name)
{
    this->name = name;
    incNodes = new QList<GNode*>(); // осв
}

void GNode::smartAddingConnection(GNode *incNode)
{
    if (!incNodes->contains(incNode))
    {
        incNodes->insert(0, incNode);
    }
    if (!incNode->incNodes->contains(this))
    {
        incNode->smartAddingConnection(this);
    }
}

QString GNode::getName()
{
    return name;
}

void GNode::setName(QString name)
{
    this->name = name;
}

QList<GNode *> *GNode::getIncNodes()
{
    return incNodes;
}

GNode::~GNode()
{
    delete incNodes;
}


