#ifndef SCREEPSLOGWINDOW_H
#define SCREEPSLOGWINDOW_H

#include <QtCore>
#include <QtWidgets>

namespace Ui {
class ScreepsLogWindow;
}

struct ScreepsLogWindowRow
{
    QDateTime ts = QDateTime::currentDateTimeUtc();
    QString msg;

    ScreepsLogWindowRow();
    ScreepsLogWindowRow(QString m);
};

class ScreepsLogWindowModel : public QAbstractTableModel
{

    QList<ScreepsLogWindowRow> __data;
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool appendLogMessage(QString message);

    void clampScrollback(int limit);
};

class ScreepsLogWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ScreepsLogWindow(QWidget *parent = 0);
    ~ScreepsLogWindow();

protected:
    void closeEvent(QCloseEvent *);
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);

private:
    Ui::ScreepsLogWindow *ui;
    ScreepsLogWindowModel m;

    bool autoscroll = false;
    unsigned int scrollbackLimit = 10000;

    void maybeSetAutoscroll();
    void maybeScrollToBottom();


signals:
    void closed();
    void windowShown();

public slots:
    void pushMessage(QString message);
    void pushMessages(QList<QString> messages);

    void postWindowShown();

};

#endif // SCREEPSLOGWINDOW_H
