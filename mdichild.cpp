#include <QtGui>
#include <QTextStream>
#include <log4qt/logger.h>
#include "mdichild.h"
#include "gfileexception.h"

using namespace Log4Qt;

MdiChild::MdiChild()
{
    setRowCount(3);
    setColumnCount(3);
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
    isGraph = true;
}

void MdiChild::newFile()
{
    static int sequenceNumber = 1;
    isUntitled = true;
    curFile = tr("matrix%1").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");
    isGraph = true;
    setColumnsTitlesPrefix(tr("Score %1"));
    setRowsTitlesPrefix(tr("Expert %1"));
}

bool MdiChild::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    isGraph = true;
    return true;
}

bool MdiChild::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MdiChild::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile);
    if (fileName.isEmpty())
        return false;
    return saveFile(fileName);
}

bool MdiChild::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    return true;
}

QString MdiChild::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void MdiChild::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MdiChild::documentWasModified()
{
    //    setWindowModified(document()->isModified());
}

bool MdiChild::maybeSave()
{
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("MDI"),
                                   tr("'%1' has been modified.\n"
                                      "Do you want to save your changes?")
                                   .arg(userFriendlyCurrentFile()),
                                   QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MdiChild::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString MdiChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MdiChild::toGraph()
{
    Logger::rootLogger()->info(tr("User select graph-file to open"));
    if (curGraph != NULL)
    {
    }
    Logger::rootLogger()->info(tr("Application try to create graph from file "));
    try
    {
        curGraph = new Graph(curFile);
    }
    catch (GFileException ex)
    {
        Logger::rootLogger()->error(ex.getMessage());
        return;
    }
    catch (...)
    {
        Logger::rootLogger()->error(tr("Unknown error!"));
        return;
    }
    Logger::rootLogger()->info(tr("Application create graph succefully"));
}

void MdiChild::setCurFile(QString curFile)
{
    this->curFile = curFile;
    setWindowTitle(curFile + "[*]");
}

void MdiChild::setColumnCount( int columns )
{
    ((QTableWidget*)this)->setColumnCount(columns);
    for (int i = 0; i<columnCount(); i++){
        QTableWidgetItem *newItem = new QTableWidgetItem(columnsTitlesPrefix.arg(i+1));
        setHorizontalHeaderItem(i, newItem);
    }
}

void MdiChild::setRowCount ( int rows )
{
    ((QTableWidget*)this)->setRowCount(rows);
    for (int i = 0; i<rowCount(); i++){
        QTableWidgetItem *newItem = new QTableWidgetItem(rowsTitlesPrefix.arg(i+1));
        setVerticalHeaderItem(i, newItem);
    }
}

void MdiChild::copy() {
    QList<QTableWidgetSelectionRange> list = selectedRanges();
    if (list.isEmpty()) return;
    QTableWidgetSelectionRange range = list.at(0);
    QString toClipboard = "";
    for (int i=range.topRow(); i<=range.bottomRow(); i++)
    {
        for (int j=range.leftColumn(); j<=range.rightColumn(); j++)
        {
            toClipboard.append(item(i, j)->text()).append("\t");
        }
        toClipboard.append("\n");
    }
    QApplication::clipboard()->setText(toClipboard);
}
void MdiChild::paste(){
    QString buf = QApplication::clipboard()->text();
    QList<QTableWidgetSelectionRange> list = selectedRanges();
    if (list.isEmpty() || buf.isEmpty()) return;
    QVector< QVector<int>* > *values = new QVector< QVector<int>* >();
    foreach(QString line, buf.split("\n")){
        QVector<int> *item = new QVector<int>();
        foreach(QString elem, line.split("\t")){
           bool ok = false;
            item->append(elem.toInt(&ok, 10));
        }
        values->append(item);
    }
    QTableWidgetSelectionRange range = list.at(0);
    int iLimit = qMin(rowCount(), qMin(range.bottomRow()-range.topRow()+1, values->count()));


    for (int i=0; i<iLimit; i++)
    {
        int jLimit = qMin(columnCount(), qMin(range.rightColumn()-range.leftColumn()+1, values->at(i)->count()));
        for (int j=0; j<jLimit; j++)
        {
            QString itemText("%1");
            itemText = itemText.arg(values->at(i)->at(j));
            QTableWidgetItem *item = new QTableWidgetItem(itemText);
            setItem(range.topRow()+i, range.leftColumn()+j, item);
        }
    }
    const QRect *rect = new QRect(range.leftColumn(), range.topRow(), range.leftColumn(), range.topRow());
    setSelection(*rect, QItemSelectionModel::Select);
}

void MdiChild::setColumnsTitlesPrefix(QString prefix){
    columnsTitlesPrefix = prefix;
}

void MdiChild::setRowsTitlesPrefix(QString prefix){
        rowsTitlesPrefix = prefix;
}
