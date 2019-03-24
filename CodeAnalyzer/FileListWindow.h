#ifndef FILELISTWINDOW_H
#define FILELISTWINDOW_H

#include <QWidget>

namespace Ui {
class FileListWindow;
}

class FileListWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FileListWindow(QWidget *parent = nullptr);
    ~FileListWindow();

private:
    Ui::FileListWindow *ui;
};

#endif // FILELISTWINDOW_H
