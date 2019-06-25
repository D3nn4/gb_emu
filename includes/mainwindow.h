#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QListWidget>
#include <QPlainTextEdit>
#include <memory>
#include <vector>

#include "mycanvas.hpp"

#include "fileio.hpp"
#include "romloader.hpp"
#include "cpu.hpp"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent , MyCanvas& canvas);
  ~MainWindow();

private slots:
    void on_focusPcButton_clicked();

private slots:
    void on_focusAdressButton_clicked();

    void on_actionLoad_rom_triggered();

    void on_actionDebug_mode_triggered();

    void on_actionNormal_triggered();

    void on_actionStop_triggered();

    void on_nextButton_clicked();

    void on_breakpointButton_clicked();

signals:
    void screen_refresh();

    public slots:
        void renderScreen();
private:
    void updateState();
    void updateRegisterTable();
    void updateMemoryTable();
    void createMemoryTable();
    void continueUntilOpCodeBreakpoint(unsigned int);

    Ui::MainWindow *ui;

    std::unique_ptr<FileIO>    _fileIO;
    std::unique_ptr<RomLoader> _romLoader;
    std::unique_ptr<Cpu>       _cpu;

    struct Cell {
        int row;
        int col;
    };

    QPushButton*  _nextButton;
    QLabel*       _loadedRom;
    /* QLabel*       _currentInstrText; */
    QTableWidget* _registerTable;
    QTableWidget* _memoryTable;
    QLineEdit*    _breakpointEntry;
    QLineEdit*    _fetchDataEntry;
    QListWidget*  _historyList;
    QLabel*       _screen;
    QImage* _image;
    MyCanvas& _canvas;


    Cell              _previousPcValueCell;
    std::vector<Cell> _dataFetchFocus;
};

#endif // MAINWINDOW_H
