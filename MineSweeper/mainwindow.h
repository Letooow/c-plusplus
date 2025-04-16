#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mybutton.h"

#include <QApplication>
#include <QCoreApplication>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSettings>
#include <QTimer>
#include <QToolBar>
#include <QTranslator>

class MainWindow : public QMainWindow
{
	Q_OBJECT

  public:
	MainWindow(int rows, int columns, int bombs, bool debugMode, QWidget *parent = nullptr);

	QTranslator getTranslator() const;
	void setTranslator(const QTranslator &newTranslator);

  protected:
	void resizeEvent(QResizeEvent *event) override;
	void closeEvent(QCloseEvent *event) override
	{
		if (!isWin && !isLose)
		{
			saveGame();
		}
		else
		{
			QString iniFilePath = QCoreApplication::applicationDirPath() + "/game_save.ini";
			QSettings settings(iniFilePath, QSettings::IniFormat);
			settings.clear();
			settings.sync();
		}
		event->accept();
	}

  public slots:
	void saveGame();
	void loadGame();
	void toggleLeftHandedMode();

  private slots:

	void buttonSetText(MyButton *button);
	void handleRightClick(MyButton *button);
	void handleMiddleClick(MyButton *button);
	void startNewGame();
	void openSettings();
	void toggleDebugMode();
	void changeLanguageToEnglish();
	void changeLanguageToRussian();
	void updateMineCounter();

  private:
	int bombsCount;
	int openCeils = 0;
	int fixRows, fixColumns;
	bool firstStep = false;
	bool debugMode;
	bool showAllCeils = false;
	bool isWin = false;
	bool isLose = false;
	bool isLeftHanded = false;

	void createButtonField(int rows, int columns, int bombs);
	void generateFieldValues(int x, int y, int rows, int columns);
	int checkMines(int value, int x, int y, int rows, int columns);
	void revealEmptyCells(int x, int y);
	void createToolBar();
	void createMenuBar();
	void updateButtonSize();

	QVector< MyButton * > buttons;
	QGridLayout *gridLayout;
	QWidget *centralWidget;
	QWidget *mainWidget;
	QToolBar *toolBar;
	QScrollArea *scrollArea;
	QMenuBar *menuBar;
	QTranslator translator;
	QLabel *mineCounterLabel;

	QVector< QVector< int > > fieldValues;

	void updateMineVisibility();
	void saveFieldState(QSettings &settings);
	void loadFieldState(QSettings &settings);
};

#endif	  // MAINWINDOW_H
