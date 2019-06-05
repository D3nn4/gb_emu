#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/log/trivial.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QStringList>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent, MyCanvas& canvas) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  _canvas(canvas)
{
  ui->setupUi(this);
  _fileIO.reset(nullptr);
  _romLoader.reset(nullptr);
  _cpu.reset(nullptr);

  _nextButton = this->findChild<QPushButton*>("nextButton");
  assert(_nextButton != nullptr);

  _loadedRom = this->findChild<QLabel*>("loadedRomText");
  assert(_loadedRom != nullptr);
  _loadedRom->setText("/home/denna/github/gb_emu/cpu_instrs/individual/01-special.gb");

  // _currentInstrText = this->findChild<QLabel*>("currentInstructionText");
  // assert(_currentInstrText != nullptr);
  // _currentInstrText->setText("");

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

  _historyList = this->findChild<QListWidget*>("historyList");
  assert(_historyList != nullptr);
  
  _fetchDataEntry = this->findChild<QLineEdit*>("fetchDataEntry");
  assert(_fetchDataEntry != nullptr);

  _screen = this->findChild<QLabel*>("screen");
  assert(_screen != nullptr);
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

void MainWindow::on_actionNormal_triggered()
{
    std::cout <<  "BOUM"  << std::endl;
    const std::string loadedRom = _loadedRom->text().toStdString();
    BOOST_LOG_TRIVIAL(debug) << "Starting " << loadedRom;

//     _fileIO.reset(new FileIO);
//     _romLoader.reset(new RomLoader(*_fileIO));
//     _cpu.reset(new Cpu(*_romLoader));
//     // connect(_cpu.get(), SIGNAL(screen_refresh()), _screen, SLOT(repaint()));
//     connect(_cpu.get(), SIGNAL(screen_refresh()), this, SLOT(renderScreen()));
//     if (!_cpu->launchGame(loadedRom)) {
//         BOOST_LOG_TRIVIAL(debug) << "Failed to load : " << loadedRom;
//         QMessageBox msgBox;
//         msgBox.setText("Failed to load rom !");
//         msgBox.exec();
//         return ;
//     }
// }
// void MainWindow::renderScreen() {
//     _canvas.renderScreen(_cpu->getScreen());
//     std::cout <<   "YEA" << std::endl;
// }
// void MainWindow::on_actionDebug_mode_triggered()
// {
//     const std::string loadedRom = _loadedRom->text().toStdString();
//     BOOST_LOG_TRIVIAL(debug) << "Starting " << loadedRom;

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
    // _currentInstrText->setText("");
    _historyList->clear();
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

    for (size_t i = 0; i < rom.size(); i++) {
        QString hexVal = QString("%1").arg(rom[i], 2, 16, QChar('0'));
        QTableWidgetItem *newItem = new QTableWidgetItem(hexVal);
        const int row = i / 16;
        const int col = i % 16;
        _memoryTable->setItem(row, col, newItem);
    }
}
void MainWindow::updateMemoryTable()
{
    auto rom = _cpu->getState().readOnlyMemory;
    for (size_t i = 0; i < rom.size(); i++) {
        QString hexVal = QString("%1").arg(rom[i], 2, 16, QChar('0'));

        const int row = i / 16;
        const int col = i % 16;
        QTableWidgetItem* item = _memoryTable->item(row, col);
        item->setText(hexVal);
    }
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
    updateMemoryTable();
    for(auto& cell : _dataFetchFocus) {
        _memoryTable->item(cell.row, cell.col)->setBackground(Qt::white);
    }
    _dataFetchFocus.clear();
    _memoryTable->item(_previousPcValueCell.row, _previousPcValueCell.col)->setBackground(Qt::white);
    auto state =_cpu->getState();

    QListWidgetItem* newItem = new QListWidgetItem(_cpu->getReadableInstruction().c_str());
    _historyList->addItem(newItem);
    _historyList->scrollToItem(newItem);

    // _currentInstrText->setText(_cpu->getReadableInstruction().c_str());

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
    std::vector<std::string> instructions;
    while (i < 100000) {
        _cpu->updateDebug();
        auto state =_cpu->getState();
        if (opCode == state.opCode) {
            break ;
        }
        instructions.push_back(_cpu->getReadableInstruction());
        i++;
    }
    if (i == 100000) {
        QMessageBox msgBox;
        msgBox.setText("Infinite loop detected in breakpoint !");
        msgBox.exec();

        auto it = instructions.rbegin();
        auto end = it + 100;
        std::vector<std::string> newVector(it.base(), end.base());
        instructions = newVector;
    }
    QListWidgetItem* last = nullptr;
    for(auto elem : instructions) {
        QListWidgetItem* newItem = new QListWidgetItem(elem.c_str());
        _historyList->addItem(newItem);
        last = newItem;
    }
    if (last) {
        _historyList->scrollToItem(last);
    }
    updateState();
}

void MainWindow::on_breakpointButton_clicked()
{
    QString text = _breakpointEntry->text();
    if (text.isEmpty()) {
        BOOST_LOG_TRIVIAL(debug) << "breakpoint empty !";
        return ;

    }
    BOOST_LOG_TRIVIAL(debug) << "breakpoint entry : " << text.toStdString();
    unsigned int opCodeBreakpoint = std::stoul(text.toStdString(), nullptr, 16);
    BOOST_LOG_TRIVIAL(debug) << std::hex << "breakpoint op code value : " << opCodeBreakpoint;

    _breakpointEntry->clear();
    continueUntilOpCodeBreakpoint(opCodeBreakpoint);
}

void MainWindow::on_focusAdressButton_clicked()
{

    QString text = _fetchDataEntry->text();
    BOOST_LOG_TRIVIAL(info) << "fetchDataEntry entry : " << text.toStdString();
    unsigned int adress = std::stoul(text.toStdString(), nullptr, 16);
    BOOST_LOG_TRIVIAL(info) << std::hex << "adress : " << adress;
    if (adress <= 0xffffu) {
        int col = adress % 16;
        int row = adress / 16;
        Cell newCell = {row, col};
        _dataFetchFocus.push_back(newCell);
        _memoryTable->item(row, col)->setBackground(Qt::red);
        _memoryTable->scrollToItem(_memoryTable->item(row, col));
    }
    else {
        QMessageBox msgBox;
        msgBox.setText("Adress doesn't exist");
        msgBox.exec();
    }
    _fetchDataEntry->clear();
}

void MainWindow::on_focusPcButton_clicked()
{
    auto state =_cpu->getState();
    const int row = state.pcValue / 16;
    const int col = state.pcValue % 16;
    _memoryTable->scrollToItem(_memoryTable->item(row, col));
}
