#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
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
    Ui::MainWindow *ui;

    std::unique_ptr<FileIO> _fileIO;
    std::unique_ptr<RomLoader> _romLoader;
    std::unique_ptr<Cpu> _cpu;

    QPushButton* _nextButton;
    QLabel* _loadedRom;
    QLabel* _currentInstrText;
};

#endif // MAINWINDOW_H
