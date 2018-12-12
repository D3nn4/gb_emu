#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <memory>

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
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
    void on_actionLoad_rom_triggered();

    void on_actionDebug_mode_triggered();

    void on_actionStop_triggered();

    void on_nextButton_clicked();

private:
    void updateState();
    void updateRegisterTable();
    void updateMemoryTable();
    void createMemoryTable();

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
    QLabel*       _currentInstrText;
    QTableWidget* _registerTable;
    QTableWidget* _memoryTable;
    Cell          _previousPcValueCell;
};

#endif // MAINWINDOW_H
