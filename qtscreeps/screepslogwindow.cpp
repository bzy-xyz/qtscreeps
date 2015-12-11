#include "screepslogwindow.h"
#include "ui_screepslogwindow.h"

ScreepsLogWindowRow::ScreepsLogWindowRow() {}

ScreepsLogWindowRow::ScreepsLogWindowRow(QString m)
    : msg(m)
{}

int ScreepsLogWindowModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : __data.length();
}

int ScreepsLogWindowModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 2;
}

QVariant ScreepsLogWindowModel::data(const QModelIndex &index, int role) const
{
    ScreepsLogWindowRow r = __data.at(index.row());

    if(role == Qt::DisplayRole)
    {
        return index.column() == 0 ? QVariant(r.ts.toString(Qt::ISODate)) : QVariant(r.msg);
    }
    else
    {
        return QVariant();
    }
}

QVariant ScreepsLogWindowModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole)
    {
        if(orientation == Qt::Horizontal)
        {
            switch(section)
            {
                case 0:
                    return "Time";
                case 1:
                    return "Text";
                default:
                    return "";
            }
        }
        else if(orientation == Qt::Vertical)
        {
            return section;
        }
        else
        {
            return QVariant();
        }
    }
    else
    {
        return QVariant();
    }
}

bool ScreepsLogWindowModel::appendLogMessage(QString message)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    __data.push_back(ScreepsLogWindowRow(message));
    endInsertRows();
    return true;
}

void ScreepsLogWindowModel::clampScrollback(int limit)
{
    int delta = rowCount() - limit;
    if(delta > 0)
    {
        beginRemoveRows(QModelIndex(), 0, delta - 1);
        while(delta > 0)
        {
            __data.pop_front();
            delta--;
        }
        endRemoveRows();
    }
}

ScreepsLogWindow::ScreepsLogWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScreepsLogWindow)
{
    ui->setupUi(this);
    ui->tableView->setModel(&m);

    connect(this, SIGNAL(windowShown()), this, SLOT(postWindowShown()), Qt::QueuedConnection);
}

ScreepsLogWindow::~ScreepsLogWindow()
{
    delete ui;
}

void ScreepsLogWindow::pushMessage(QString message)
{
    maybeSetAutoscroll();
    m.appendLogMessage(message);
    ui->tableView->resizeRowToContents(m.rowCount()-1);
    m.clampScrollback(scrollbackLimit);
    ui->tableView->resizeColumnToContents(0);
    maybeScrollToBottom();
}

void ScreepsLogWindow::pushMessages(QList<QString> messages)
{
    maybeSetAutoscroll();
    foreach(QString msg, messages)
    {
        m.appendLogMessage(msg);
        ui->tableView->resizeRowToContents(m.rowCount()-1);
    }
    m.clampScrollback(scrollbackLimit);
    ui->tableView->resizeColumnToContents(0);
    maybeScrollToBottom();
}

void ScreepsLogWindow::maybeSetAutoscroll()
{
    autoscroll = ui->tableView->rowViewportPosition(m.rowCount()-1) < ui->tableView->height();
}

void ScreepsLogWindow::maybeScrollToBottom()
{
    if(autoscroll)
    {
        ui->tableView->scrollToBottom();
    }
}


void ScreepsLogWindow::showEvent(QShowEvent * ev)
{
    QWidget::showEvent(ev);
    ev->accept();

    if(ev->spontaneous())
    {
        emit windowShown();
    }
}

void ScreepsLogWindow::closeEvent(QCloseEvent * ev)
{
    emit closed();
    ev->accept();
}

void ScreepsLogWindow::postWindowShown()
{
    qDebug() << "postShown";
    ui->tableView->resizeColumnToContents(0);
    ui->tableView->resizeRowsToContents();
}

void ScreepsLogWindow::resizeEvent(QResizeEvent * ev)
{
    ui->tableView->resizeColumnToContents(0);
    ui->tableView->resizeRowsToContents();
    ev->accept();
}

