#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QTranslator>
#include <QVBoxLayout>

class MenuWindow : public QDialog
{
	Q_OBJECT

  public:
	explicit MenuWindow(QWidget *parent = nullptr);

	int getRows() const;
	int getColumns() const;
	int getBombs() const;
	bool isContinue() const;
	QString getSelectedLanguage() const;

  private slots:
	void onLanguageChanged(int index);
	void onStartButtonClicked();
	void onContinueButtonClicked();

  private:
	QSpinBox *rowsSpinBox;
	QSpinBox *columnsSpinBox;
	QSpinBox *bombsSpinBox;
	QPushButton *startButton;
	QPushButton *continueButton;
	QComboBox *languageComboBox;
	QLabel *rowsLabel;
	QLabel *columnsLabel;
	QLabel *bombsLabel;

	QString selectedLanguage = "en";
	QTranslator translator;

	int rows;
	int columns;
	int bombs;
	bool continueGame = false;

	void showError(const QString &message);
	void retranslateUi();
};

#endif	  // MENUWINDOW_H
