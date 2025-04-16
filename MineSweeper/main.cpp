#include "mainwindow.h"
#include "menuwindow.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	bool debugMode = false;
	for (int i = 0; i < argc; ++i)
	{
		if (QString(argv[i]) == "dbg")
		{
			debugMode = true;
		}
	}

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
			return -1;
		}

		MainWindow *w;
		if (menuWindow.isContinue())
		{
			w = new MainWindow(10, 10, 10, debugMode);
			w->loadGame();
		}
		else
		{
			w = new MainWindow(rows, columns, bombs, debugMode);
			w->showMaximized();
		}
		return a.exec();
	}

	return 0;
}
