#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "FolderProcessor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openClicked();
    void indicateProcessing();
    void updateFolderInfo(FolderInfo info);

private:
    Ui::MainWindow *ui;
    FolderProcessor processor;
    QThread processingThread;
};

#endif // MAINWINDOW_H
