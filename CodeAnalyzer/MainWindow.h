#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QElapsedTimer>
#include "FolderProcessor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void openClicked();
    void indicateProcessing();
    void linkActivated(const QString& link);
    void updateFolderInfo(FolderInfo info);

protected:
    void dragEnterEvent(QDragEnterEvent*) override;
    void dragLeaveEvent(QDragLeaveEvent*) override;
    void dropEvent(QDropEvent*) override;

private:
    Ui::MainWindow *ui;
    FolderProcessor processor;
    QThread processingThread;
    FolderInfo folderInfo;
    QElapsedTimer processingTimer;

    void connectLinkHandler(const QObject *);
    QString getEncodingName(Encoding);
};

#endif // MAINWINDOW_H
