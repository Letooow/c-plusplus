#include "mainwindow.h"

#include "menuwindow.h"
#include "mybutton.h"

MainWindow::MainWindow(int rows, int columns, int bombs, bool debug, QWidget *parent) :
	QMainWindow(parent), debugMode(debug)
{
	this->fixRows = rows;
	this->fixColumns = columns;
	this->bombsCount = bombs;

	setMinimumSize(350, 350);

	QString iniFilePath = QCoreApplication::applicationDirPath() + "/settings.ini";
	QSettings settings(iniFilePath, QSettings::IniFormat);
	QString language = settings.value("language", "en").toString();

	if (translator.load(":/translations/qt_" + language + ".qm"))
	{
		qApp->installTranslator(&translator);
	}

	createToolBar();

	scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	mainWidget = new QWidget();
	scrollArea->setWidget(mainWidget);
	setCentralWidget(scrollArea);

	centralWidget = new QWidget(mainWidget);
	centralWidget->setStyleSheet(
		"QWidget {"
		"   border-radius: 10px;"
		"   background-color: rgba(0, 0, 0, 200);"
		"}");
	gridLayout = new QGridLayout(centralWidget);

	createButtonField(fixRows, fixColumns, bombsCount);

	gridLayout->setSpacing(5);

	int fieldWidth = columns * 50 + (columns - 1) * 10;
	int fieldHeight = rows * 50 + (rows - 1) * 10;
	centralWidget->setFixedSize(fieldWidth, fieldHeight);

	QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
	mainLayout->addWidget(centralWidget);
	mainLayout->setAlignment(centralWidget, Qt::AlignCenter);
	mainWidget->setLayout(mainLayout);

	// QPixmap bkgnd(":/icons/grass-texture-background_64049-124-2716259149.png");
	QPixmap bkgnd(":/icons/a6685001691b30093ed052da4ee6460e-1806437242.jpg");
	bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
	QPalette palette;
	palette.setBrush(QPalette::Window, bkgnd);
	this->setPalette(palette);

	scrollArea->setStyleSheet("background-color: transparent");
}

void MainWindow::createToolBar()
{
	toolBar = addToolBar("Main Toolbar");
	toolBar->setStyleSheet(
		"QToolBar {"
		"   background-color: black;"
		"   color: white;"
		"}");

	addToolBar(Qt::BottomToolBarArea, toolBar);
	toolBar->setMovable(false);

	QAction *newGameAction = toolBar->addAction(tr("New Game"));
	QAction *settingsAction = toolBar->addAction(tr("Settings"));
	QAction *saveGameAction = toolBar->addAction(tr("Save Game"));
	QAction *loadGameAction = toolBar->addAction(tr("Load Game"));
	QAction *leftHandedAction = toolBar->addAction(tr("Left-Handed Mode"));

	if (debugMode)
	{
		QAction *toggleDebugAction = toolBar->addAction(tr("Debug"));
		connect(toggleDebugAction, &QAction::triggered, this, &MainWindow::toggleDebugMode);
	}

	connect(newGameAction, &QAction::triggered, this, &MainWindow::startNewGame);
	connect(settingsAction, &QAction::triggered, this, &MainWindow::openSettings);
	connect(saveGameAction, &QAction::triggered, this, &MainWindow::saveGame);
	connect(loadGameAction, &QAction::triggered, this, &MainWindow::loadGame);
	connect(leftHandedAction, &QAction::triggered, this, &MainWindow::toggleLeftHandedMode);

	mineCounterLabel = new QLabel(this);
	mineCounterLabel->setText(tr("Mines: %1").arg(bombsCount));
	mineCounterLabel->setStyleSheet("QLabel { color : red; font-size: 20px; }");
	toolBar->addWidget(mineCounterLabel);

	createMenuBar();
}

void MainWindow::updateMineCounter()
{
	int flaggedMines = 0;
	for (auto button : buttons)
	{
		if (button->isFlagged)
		{
			flaggedMines++;
		}
	}
	mineCounterLabel->setText(tr("Mines: %1").arg(bombsCount - flaggedMines));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	int w = width(), h = height();
	int toolBarHeight = toolBar->height();
	int menuHeight = menuBar->height();

	int availableWidth = w - 40;
	int availableHeight = h - toolBarHeight - menuHeight - 40;

	int spacing = 10;
	int maxButtonSize = qMax(
		qMin((availableWidth - (spacing * (fixColumns - 1))) / fixColumns, (availableHeight - (spacing * (fixRows - 1))) / fixRows),
		10);

	for (MyButton *button : buttons)
	{
		button->setFixedSize(maxButtonSize, maxButtonSize);
		button->setIconSize(QSize(maxButtonSize - 10, maxButtonSize - 10));
	}

	centralWidget->setFixedSize(
		qMax(maxButtonSize * fixColumns + spacing * (fixColumns - 1), 40),
		qMax(maxButtonSize * fixRows + spacing * (fixRows - 1), 40));

	gridLayout->setAlignment(Qt::AlignCenter);
	QMainWindow::resizeEvent(event);
}

void MainWindow::createMenuBar()
{
	menuBar = new QMenuBar(this);
	setMenuBar(menuBar);

	QMenu *gameMenu = menuBar->addMenu(tr("Game"));

	QAction *newGameAction = gameMenu->addAction(tr("New Game"));
	QAction *settingsAction = gameMenu->addAction(tr("Settings"));
	QAction *saveGameAction = gameMenu->addAction(tr("Save Game"));
	QAction *loadGameAction = gameMenu->addAction(tr("Load Game"));
	if (debugMode)
	{
		QAction *toggleDebugAction = gameMenu->addAction(tr("Debug"));
		connect(toggleDebugAction, &QAction::triggered, this, &MainWindow::toggleDebugMode);
	}

	connect(newGameAction, &QAction::triggered, this, &MainWindow::startNewGame);
	connect(settingsAction, &QAction::triggered, this, &MainWindow::openSettings);
	connect(saveGameAction, &QAction::triggered, this, &MainWindow::saveGame);
	connect(loadGameAction, &QAction::triggered, this, &MainWindow::loadGame);

	QMenu *languageMenu = menuBar->addMenu(tr("Language"));
	QAction *englishAction = languageMenu->addAction("English");
	QAction *russianAction = languageMenu->addAction("Русский");

	connect(englishAction, &QAction::triggered, this, &MainWindow::changeLanguageToEnglish);
	connect(russianAction, &QAction::triggered, this, &MainWindow::changeLanguageToRussian);
}

void MainWindow::changeLanguageToEnglish()
{
	if (translator.load(":/translations/qt_en.qm"))
	{
		qApp->installTranslator(&translator);
	}

	menuBar->clear();
	toolBar->clear();
	createToolBar();
}

void MainWindow::changeLanguageToRussian()
{
	if (translator.load(":/translations/qt_ru.qm"))
	{
		qApp->installTranslator(&translator);
	}

	menuBar->clear();
	toolBar->clear();
	createToolBar();
}

void MainWindow::updateButtonSize()
{
	int w = width(), h = height();
	int toolBarHeight = toolBar->height();
	int menuHeight = menuBar->height();

	int availableWidth = w - 40;
	int availableHeight = h - toolBarHeight - menuHeight - 40;

	int spacing = 10;
	int maxButtonSize = qMax(
		qMin((availableWidth - (spacing * (fixColumns - 1))) / fixColumns, (availableHeight - (spacing * (fixRows - 1))) / fixRows),
		10);

	for (MyButton *button : buttons)
	{
		button->setFixedSize(maxButtonSize, maxButtonSize);
		button->setIconSize(QSize(maxButtonSize - 10, maxButtonSize - 10));
	}
}

void MainWindow::startNewGame()
{
	isWin = false;
	isLose = false;
	firstStep = false;
	openCeils = 0;
	for (auto button : buttons)
	{
		button->deleteLater();
	}
	buttons.clear();
	createButtonField(fixRows, fixColumns, bombsCount);
	updateMineCounter();
	updateButtonSize();
}

void MainWindow::openSettings()
{
	MenuWindow menuWindow;
	menuWindow.setFixedSize(200, 250);
	if (menuWindow.exec() == QDialog::Accepted)
	{
		int rows = menuWindow.getRows();
		int columns = menuWindow.getColumns();
		int bombs = menuWindow.getBombs();

		if (bombs >= rows * columns)
		{
			QMessageBox::critical(nullptr, "Error", "Number of bombs must be less than the number of cells.");
			return;
		}

		MainWindow *w;
		if (menuWindow.isContinue())
		{
			w = new MainWindow(0, 0, 0, debugMode);
			w->loadGame();
		}
		else
		{
			w = new MainWindow(rows, columns, bombs, debugMode);
			w->showMaximized();
		}
		this->close();
	}
}

void MainWindow::toggleLeftHandedMode()
{
	isLeftHanded = !isLeftHanded;
	for (auto button : buttons)
	{
		button->isLeftHanded = isLeftHanded;
	}
}

void MainWindow::createButtonField(int rows, int columns, int bombs)
{
	this->fixRows = rows;
	this->fixColumns = columns;
	this->bombsCount = bombs;
	buttons.resize(rows * columns);
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			int index = i * columns + j;
			buttons[index] = new MyButton(j, i, 0, this);

			buttons[index]->setFixedSize(50, 50);
			buttons[index]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

			buttons[index]->notPushed();

			gridLayout->addWidget(buttons[index], i, j);
			connect(buttons[index], &MyButton::leftClicked, this, &MainWindow::buttonSetText);
			connect(buttons[index], &MyButton::rightClicked, this, &MainWindow::handleRightClick);
			connect(buttons[index], &MyButton::rightClicked, this, &MainWindow::updateMineCounter);
			connect(buttons[index], &MyButton::middleClicked, this, &MainWindow::handleMiddleClick);
		}
	}
}

void MainWindow::generateFieldValues(int x, int y, int rows, int columns)
{
	int remainingBombs = bombsCount;
	while (remainingBombs > 0)
	{
		int ind = QRandomGenerator::global()->bounded(rows * columns);
		if (buttons[ind]->number != -1 && (ind != (y * columns + x)))
		{
			buttons[ind]->number = -1;
			remainingBombs--;
		}
	}
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			if (buttons[i * columns + j]->number != -1)
			{
				int count = 0;
				for (int di = -1; di <= 1; ++di)
				{
					for (int dj = -1; dj <= 1; ++dj)
					{
						int ni = i + di;
						int nj = j + dj;
						if (ni >= 0 && ni < rows && nj >= 0 && nj < columns && !(di == 0 && dj == 0))
						{
							if (buttons[ni * columns + nj]->number == -1)
							{
								count++;
							}
						}
					}
				}
				buttons[i * columns + j]->number = count;
			}
		}
	}
}

void MainWindow::revealEmptyCells(int x, int y)
{
	if (buttons[y * fixColumns + x]->number != 0)
	{
		buttons[y * fixColumns + x]->value();
		return;
	}
	for (int di = -1; di <= 1; ++di)
	{
		for (int dj = -1; dj <= 1; ++dj)
		{
			int ni = y + di;
			int nj = x + dj;
			if (ni >= 0 && ni < fixRows && nj >= 0 && nj < fixColumns && !(di == 0 && dj == 0))
			{
				MyButton *button = buttons[ni * fixColumns + nj];
				if (!button->isOpen && !button->isFlagged && button->number != -1)
				{
					button->value();

					this->openCeils++;
					button->isOpen = true;
					if (button->number == 0)
					{
						button->setEnabled(false);
						button->setText("");
						revealEmptyCells(nj, ni);
					}
				}
			}
		}
	}
}

void MainWindow::buttonSetText(MyButton *buttonSender)
{
	if (buttonSender->isFlagged || buttonSender->isQuestioned)
		return;
	if (!firstStep)
	{
		generateFieldValues(buttonSender->x, buttonSender->y, this->fixRows, this->fixColumns);
		firstStep = true;
	}

	if (buttonSender->number == -1)
	{
		for (int i = 0; i < this->fixRows * this->fixColumns; i++)
		{
			buttons[i]->setEnabled(false);
			if (this->buttons[i]->number == -1)
			{
				buttons[i]->bomb();
			}
			else
			{
				buttons[i]->value();
			}
		}
		buttonSender->bomb();
		QMessageBox::information(this, tr("Game Over"), tr("YOU LOSE!"));
		isLose = true;
		return;
	}
	else
	{
		buttonSender->value();

		if (buttonSender->number == 0)
		{
			buttonSender->setText("");
			revealEmptyCells(buttonSender->x, buttonSender->y);
		}
		this->openCeils++;
		buttonSender->isOpen = true;
	}
	int count = 0;
	for (int k = 0; k < this->fixRows * this->fixColumns; k++)
	{
		if (buttons[k]->isOpen && !buttons[k]->isFlagged)
			count++;
	}
	if (count >= this->fixColumns * this->fixRows - this->bombsCount)
	{
		for (int i = 0; i < this->fixRows * this->fixColumns; i++)
		{
			buttons[i]->setEnabled(false);
			if (this->buttons[i]->number == -1)
			{
				buttons[i]->flag();
			}
		}
		QMessageBox::information(this, tr("WIN!"), tr("YOU'RE WINNER!"));
		isWin = true;
	}
	updateButtonSize();
}

void MainWindow::handleRightClick(MyButton *button)
{
	if (button->isOpen)
		return;
	else if (button->isQuestioned)
	{
		button->setIcon(QIcon());
		button->notPushed();
	}
	else if (!button->isFlagged)
	{
		button->flag();
	}
	else if (!button->isQuestioned)
	{
		button->question();
	}

	int w = width(), h = height();
	int toolBarHeight = toolBar->height();
	int menuHeight = menuBar->height();

	int availableWidth = w - 40;
	int availableHeight = h - toolBarHeight - menuHeight - 40;

	int spacing = 10;
	int maxButtonSize = qMax(
		qMin((availableWidth - (spacing * (fixColumns - 1))) / fixColumns, (availableHeight - (spacing * (fixRows - 1))) / fixRows),
		10);

	button->setFixedSize(maxButtonSize, maxButtonSize);
	button->setIconSize(QSize(maxButtonSize - 10, maxButtonSize - 10));
}

void MainWindow::handleMiddleClick(MyButton *button)
{
	if (!button->isOpen || button->number == 0)
		return;

	int row = button->y, col = button->x, flaggedCount = 0, mineCount = button->number;
	QVector< int > neighbors;

	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			int r = row + i, c = col + j;
			if (r >= 0 && r < fixRows && c >= 0 && c < fixColumns)
			{
				int neighborIndex = r * fixColumns + c;
				if (!buttons[neighborIndex]->isOpen)
				{
					neighbors.append(neighborIndex);
				}

				if (buttons[neighborIndex]->isFlagged)
				{
					flaggedCount++;
				}
			}
		}
	}

	if (flaggedCount == mineCount)
	{
		for (int neighborIndex : neighbors)
		{
			if (buttons[neighborIndex]->number == -1 && !buttons[neighborIndex]->isFlagged)
			{
				for (int i = 0; i < this->fixRows * this->fixColumns; i++)
				{
					buttons[i]->setEnabled(false);
					if (this->buttons[i]->number == -1)
					{
						buttons[i]->bomb();
					}
					else
					{
						buttons[i]->value();
					}
				}
				QMessageBox::information(this, tr("Game Over"), tr("YOU LOSE!"));
				return;
			}
			else if (!buttons[neighborIndex]->isFlagged && !buttons[neighborIndex]->isOpen && !buttons[neighborIndex]->isQuestioned)
			{
				buttonSetText(buttons[neighborIndex]);
			}
		}
	}
	else
	{
		for (int neighborIndex : neighbors)
		{
			if (!buttons[neighborIndex]->isFlagged)
				buttons[neighborIndex]->drawed();
		}

		QTimer::singleShot(
			500,
			this,
			[=]()
			{
				for (int neighborIndex : neighbors)
				{
					if (!buttons[neighborIndex]->isFlagged)
						buttons[neighborIndex]->notPushed();
				}
			});
	}
	updateButtonSize();
}

void MainWindow::toggleDebugMode()
{
	updateMineVisibility();
	updateButtonSize();
	showAllCeils = !showAllCeils;
}

void MainWindow::updateMineVisibility()
{
	for (auto button : buttons)
	{
		if (!button->isOpen && !button->isFlagged && !button->isQuestioned)
		{
			if (!showAllCeils)
			{
				button->showIcon();
			}
			else
			{
				button->hideIcon();
			}
		}
	}
}

void MainWindow::saveGame()
{
	QString iniFilePath = QCoreApplication::applicationDirPath() + "/game_save.ini";
	QSettings settings(iniFilePath, QSettings::IniFormat);

	settings.clear();

	if (isWin || isLose)
	{
		QMessageBox::critical(this, tr("Error"), tr("You can not save game if the game is lost or won"));
		return;
	}

	settings.setValue("rows", fixRows);
	settings.setValue("columns", fixColumns);
	settings.setValue("bombs", bombsCount);
	settings.setValue("openCeils", openCeils);
	settings.setValue("firstStep", firstStep);

	settings.beginWriteArray("Buttons", buttons.size());
	for (int i = 0; i < buttons.size(); ++i)
	{
		settings.setArrayIndex(i);
		settings.setValue("number", buttons[i]->number);
		settings.setValue("isFlagged", buttons[i]->isFlagged);
		settings.setValue("isOpen", buttons[i]->isOpen);
		settings.setValue("isQuestioned", buttons[i]->isQuestioned);
	}
	settings.endArray();
	settings.sync();
}

void MainWindow::loadGame()
{
	QString iniFilePath = QCoreApplication::applicationDirPath() + "/game_save.ini";
	QSettings settings(iniFilePath, QSettings::IniFormat);

	fixRows = settings.value("rows", 10).toInt();
	fixColumns = settings.value("columns", 10).toInt();
	bombsCount = settings.value("bombs", 10).toInt();
	openCeils = settings.value("openCeils", 0).toInt();
	firstStep = settings.value("firstStep", false).toBool();

	this->hide();
	MainWindow *newWindow = new MainWindow(fixRows, fixColumns, bombsCount, debugMode);
	newWindow->showMaximized();

	newWindow->firstStep = firstStep;
	int size = settings.beginReadArray("Buttons");
	for (int i = 0; i < size; ++i)
	{
		settings.setArrayIndex(i);
		newWindow->buttons[i]->number = settings.value("number", 0).toInt();
		newWindow->buttons[i]->isFlagged = settings.value("isFlagged", false).toBool();
		newWindow->buttons[i]->isOpen = settings.value("isOpen", false).toBool();
		newWindow->buttons[i]->isQuestioned = settings.value("isQuestioned", false).toBool();
		if (newWindow->buttons[i]->isOpen)
		{
			newWindow->buttons[i]->value();
		}
		else if (newWindow->buttons[i]->isFlagged)
		{
			newWindow->buttons[i]->flag();
		}
		else if (newWindow->buttons[i]->isQuestioned)
		{
			newWindow->buttons[i]->question();
		}
		else
		{
			newWindow->buttons[i]->notPushed();
		}
	}

	newWindow->updateMineCounter();
	settings.endArray();
	newWindow->saveGame();
	this->close();
}
