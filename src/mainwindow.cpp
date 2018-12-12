#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/log/trivial.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QStringList>
#include <QLineEdit>

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

  _registerTable = this->findChild<QTableWidget*>("registerTable");
  assert(_registerTable != nullptr);
  _registerTable->setRowCount(12);
  _registerTable->setColumnCount(2);


  _memoryTable = this->findChild<QTableWidget*>("memoryTable");
  assert(_memoryTable != nullptr);
  _memoryTable->setColumnCount(16);
  _memoryTable->setSelectionBehavior( QAbstractItemView::SelectItems );
  _memoryTable->setSelectionMode( QAbstractItemView::SingleSelection );

  _breakpointEntry = this->findChild<QLineEdit*>("breakpointEntry");
  assert(_breakpointEntry != nullptr);
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
    createMemoryTable();
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


void MainWindow::createMemoryTable()
{
    _previousPcValueCell = {0, 0};
    auto rom = _cpu->getState().readOnlyMemory;
    const int rowSize = rom.size() / 16;
    _memoryTable->setRowCount(rowSize);

    // generate headers 0 - 15
    QStringList headers;
    for (int i = 0; i < 16; i++) {
        headers += QString::number(i);
    }
    _memoryTable->setHorizontalHeaderLabels(headers);


    // generate row headers 0 - 4000 
    QStringList rowsHeader;
    for (int i = 0, currentVal; i < rowSize; i++, currentVal += 16) {
        QString hexVal = QString("%1").arg(currentVal, 4, 16, QChar('0'));
        rowsHeader += hexVal;
    }
    _memoryTable-> setVerticalHeaderLabels(rowsHeader);
    updateMemoryTable();
}

void MainWindow::updateMemoryTable()
{
    auto rom = _cpu->getState().readOnlyMemory;
    const int rowSize = rom.size() / 16;
    for (size_t i = 0; i < rom.size(); i++) {
        QString hexVal = QString("%1").arg(rom[i], 2, 16, QChar('0'));
        QTableWidgetItem *newItem = new QTableWidgetItem(hexVal);
        const int row = i / 16;
        const int col = i % 16;
        _memoryTable->setItem(row, col, newItem);
    }
    _memoryTable->setRowCount(rowSize);
}

void MainWindow::updateRegisterTable()
{
    auto state =_cpu->getState();

    int row = 0;

    BOOST_LOG_TRIVIAL(debug) << state.reg16Bit.size();
    for (auto & pair : state.reg16Bit) {
        BOOST_LOG_TRIVIAL(debug) << pair.first << " " << std::hex <<  static_cast<int>(pair.second);
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(pair.first.c_str());
            _registerTable->setItem(row, 0, newItem);
        }
        {
            QString hexVal = QString("%1").arg(pair.second, 4, 16, QChar('0'));
            QTableWidgetItem *newItem = new QTableWidgetItem(hexVal);
            _registerTable->setItem(row, 1, newItem);
        }
        row++;
    }
    for (auto & pair : state.flags) {
        BOOST_LOG_TRIVIAL(debug) << pair.first << " " << pair.second;
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(pair.first.c_str());
            _registerTable->setItem(row, 0, newItem);
        }
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(pair.second));
            _registerTable->setItem(row, 1, newItem);
        }
        row++;
    }
}

void MainWindow::updateState()
{
    _memoryTable->item(_previousPcValueCell.row, _previousPcValueCell.col)->setBackground(Qt::white);
    auto state =_cpu->getState();
    _currentInstrText->setText(_cpu->getReadableInstruction().c_str());

    const int row = state.pcValue / 16;
    const int col = state.pcValue % 16;
    _previousPcValueCell = {row, col};
    _memoryTable->item(row, col)->setBackground(Qt::cyan);
    _memoryTable->scrollToItem(_memoryTable->item(row, col));
    updateRegisterTable();
}

void MainWindow::on_nextButton_clicked()
{
    BOOST_LOG_TRIVIAL(debug) << "Next step";
    updateState();
    _cpu->updateDebug();
}


void MainWindow::continueUntilOpCodeBreakpoint(unsigned int opCode)
{
    int i = 0;
    while (i < 1000) {
        _cpu->updateDebug();
        auto state =_cpu->getState();
        if (opCode == state.opCode) {
            break ;
        }
        i++;
    }
    if (i == 1000) {
        QMessageBox msgBox;
        msgBox.setText("Infinite loop detected in breakpoint !");
        msgBox.exec();
    }
    updateState();
}

void MainWindow::on_breakpointButton_clicked()
{
    QString text = _breakpointEntry->text();
    BOOST_LOG_TRIVIAL(debug) << "breakpoint entry : " << text.toStdString();
    unsigned int opCodeBreakpoint = std::stoul(text.toStdString(), nullptr, 16);
    BOOST_LOG_TRIVIAL(debug) << std::hex << "breakpoint op code value : " << opCodeBreakpoint;

    continueUntilOpCodeBreakpoint(opCodeBreakpoint);
}
