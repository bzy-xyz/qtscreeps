#ifndef SCREEPSCONSOLEWINDOW_H
#define SCREEPSCONSOLEWINDOW_H

#include <QtCore>
#include <QtWidgets>
#include <QtGui>

namespace Ui {
class ScreepsConsoleWindow;
}

class ScreepsConsoleWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ScreepsConsoleWindow(QWidget *parent = 0);
    ~ScreepsConsoleWindow();

    bool eventFilter(QObject *, QEvent *);

signals:
    void closed();

    void lineEntered(const QString text);

public slots:
    void addLine(QString line, QString prefix = "");
    void addLines(QList<QString> lines, QString prefix = "");

    void addIncomingLine(QString line);
    void addIncomingLines(QList<QString> lines);

    void addOutgoingLine(QString line);
    void addOutgoingLines(QList<QString> lines);

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void on_lineEdit_returnPressed();

private:
    Ui::ScreepsConsoleWindow *ui;

    QList<QString> commandHistory;
    int currentHistoryIndex;
};

#endif // SCREEPSCONSOLEWINDOW_H
