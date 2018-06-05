#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "QMdiSubWindow"
#include "QMdiArea"
class QMdiArea;
class QMdiSubWindow;
class MyMdi;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    void UpdateMenus();
    void CreateMenus();
    void CreateAction();
    void FindChildSubWindow(QString filename);
    void ReadSetting();
    void WriteSetting();
    void showRecent();
    void closeEvent();
    void createStatusBar();
    QStringList ReadHistory();
    void UpdateHistory(QString fileName);
    MyMdi createSubWindow();
    MyMdi activeMdiWindow();

public slots:
    void NewFile();
    void OpenFile();
    void ReloadFile();
    void SaveCopyText();
    void Save();
    void SaveAs();
    void SaveAll();
    void RenameFile();
    void Close();
    void CloseAll();
    void CloseOthers();
    void DeleteFromDisk();

    void Print();
    void PrintNow();

    void OpenAllRecentFile();
    void ClearRecentHistory();

    void Exit();

    void Copy();
    void Paste();
    void Cut();
private:
    QMdiArea mdiArea;
};

#endif // MAINWINDOW_H
