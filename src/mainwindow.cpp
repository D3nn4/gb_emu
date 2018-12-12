#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/log/trivial.hpp>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  _fileIO.reset(nullptr);
  _romLoader.reset(nullptr);
  _cpu.reset(nullptr);

  _nextButton = this->findChild<QPushButton*>("nextButton");
  assert(_nextButton != nullptr);

  _loadedRom = this->findChild<QLabel*>("loadedRomText");
  assert(_loadedRom != nullptr);
  _loadedRom->setText("/home/sgregory/cpu_instrs/individual/01-special.gb");

  _currentInstrText = this->findChild<QLabel*>("currentInstructionText");
  assert(_loadedRom != nullptr);
  _currentInstrText->setText("");
}

MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::on_actionLoad_rom_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this,
                                                  tr("Load Rom"), "",
                                                  tr("Gameboy rom (*.gb);;All Files (*)"));
  _loadedRom->setText(filename);
  BOOST_LOG_TRIVIAL(debug) << "set rom " << _loadedRom->text().toStdString();
}

void MainWindow::on_actionDebug_mode_triggered()
{
    const std::string loadedRom = _loadedRom->text().toStdString();
    BOOST_LOG_TRIVIAL(debug) << "Starting " << loadedRom;

    _fileIO.reset(new FileIO);
    _romLoader.reset(new RomLoader(*_fileIO));
    _cpu.reset(new Cpu(*_romLoader));
    if (!_cpu->launchGameDebug(loadedRom)) {
        BOOST_LOG_TRIVIAL(debug) << "Failed to load : " << loadedRom;
        QMessageBox msgBox;
        msgBox.setText("Failed to load rom !");
        msgBox.exec();
        return ;
    }
    _nextButton->setEnabled(true);
}

void MainWindow::on_actionStop_triggered()
{
    BOOST_LOG_TRIVIAL(debug) << "Stopping Gameboy";
    _cpu.reset(nullptr);
    _romLoader.reset(nullptr);
    _fileIO.reset(nullptr);
    _nextButton->setDisabled(true);
    _currentInstrText->setText("");
}

void MainWindow::on_nextButton_clicked()
{
    BOOST_LOG_TRIVIAL(debug) << "Next step";
    _cpu->updateDebug();
    _currentInstrText->setText(_cpu->getReadableInstruction().c_str());
}
