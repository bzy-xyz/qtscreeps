#include "screepsconsolewindow.h"
#include "ui_screepsconsolewindow.h"

#include "screepsnetworkmanager.h"

static QTextCharFormat getConsoleDefaultFont()
{
    QTextCharFormat r;
    r.setFontFamily("monospace");

    return r;
}

ScreepsConsoleWindow::ScreepsConsoleWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScreepsConsoleWindow)
{
    ui->setupUi(this);

    commandHistory.push_back("");
    currentHistoryIndex = 0;

    ui->lineEdit->installEventFilter(this);
}

ScreepsConsoleWindow::~ScreepsConsoleWindow()
{
    delete ui;
}

void ScreepsConsoleWindow::on_lineEdit_returnPressed()
{

    emit lineEntered(ui->lineEdit->text());

    commandHistory[commandHistory.length() - 1] = ui->lineEdit->text();

    ui->lineEdit->clear();

    currentHistoryIndex = commandHistory.length();
    commandHistory.push_back("");

}

void ScreepsConsoleWindow::addLine(QString line, QString prefix)
{
    QTextCursor c = QTextCursor(ui->textEdit->document());
    c.movePosition(QTextCursor::End);
    c.setBlockCharFormat(getConsoleDefaultFont());
    c.insertText(prefix + line);
    c.insertBlock();

    ui->textEdit->ensureCursorVisible();
}

void ScreepsConsoleWindow::addLines(QList<QString> lines, QString prefix)
{
    QTextCursor c = QTextCursor(ui->textEdit->document());
    foreach(QString line, lines)
    {
        c.movePosition(QTextCursor::End);
        c.setBlockCharFormat(getConsoleDefaultFont());
        c.insertText(prefix + line);
        c.insertBlock();
    }

    if(lines.length())
    {
        ui->textEdit->ensureCursorVisible();
    }
}

void ScreepsConsoleWindow::addIncomingLine(QString line)
{
    addLine(line, ">>> ");
}

void ScreepsConsoleWindow::addIncomingLines(QList<QString> lines)
{
    addLines(lines, ">>> ");
}

void ScreepsConsoleWindow::addOutgoingLine(QString line)
{
    addLine(line, "<<< ");
}

void ScreepsConsoleWindow::addOutgoingLines(QList<QString> lines)
{
    addLines(lines, "<<< ");
}

void ScreepsConsoleWindow::closeEvent(QCloseEvent * ev)
{
    emit closed();
    ev->accept();
}

/**
 * @brief Filters special keypress events for various widgets in this window.
 * @param obj
 * @param ev
 */
bool ScreepsConsoleWindow::eventFilter(QObject * obj, QEvent * ev)
{
    // handle command history
    if(obj == ui->lineEdit)
    {
        if (ev->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(ev);
            if (keyEvent->key() == Qt::Key_Up && currentHistoryIndex > 0)
            {
                commandHistory[currentHistoryIndex--] = ui->lineEdit->text();
                ui->lineEdit->setText(commandHistory[currentHistoryIndex]);
                return true;
            }
            else if(keyEvent->key() == Qt::Key_Down && currentHistoryIndex < commandHistory.length() - 1)
            {
                commandHistory[currentHistoryIndex++] = ui->lineEdit->text();
                ui->lineEdit->setText(commandHistory[currentHistoryIndex]);
                return true;
            }
        }
        return false;
    }
    return false;
}
