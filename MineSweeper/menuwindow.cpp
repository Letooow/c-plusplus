#include "menuwindow.h"

MenuWindow::MenuWindow(QWidget *parent) : QDialog(parent), rows(10), columns(10), bombs(10)
{
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, Qt::gray);
	this->setPalette(palette);

	QString iniFilePath = QCoreApplication::applicationDirPath() + "/settings.ini";
	QSettings settings(iniFilePath, QSettings::IniFormat);
	QString language = settings.value("language", "en").toString();

	if (translator.load(":/translations/qt_" + language + ".qm"))
	{
		qApp->installTranslator(&translator);
	}

	rowsSpinBox = new QSpinBox(this);
	columnsSpinBox = new QSpinBox(this);
	bombsSpinBox = new QSpinBox(this);
	startButton = new QPushButton(tr("Start Game"), this);
	continueButton = new QPushButton(tr("Continue"), this);

	languageComboBox = new QComboBox(this);
	languageComboBox->addItem("English", "en");
	languageComboBox->addItem("Русский", "ru");

	rowsSpinBox->setRange(2, 30);
	columnsSpinBox->setRange(2, 30);
	rowsSpinBox->setValue(rows);
	columnsSpinBox->setValue(columns);

	bombsSpinBox->setRange(1, 99);
	bombsSpinBox->setValue(bombs);

	connect(startButton, &QPushButton::clicked, this, &MenuWindow::onStartButtonClicked);
	connect(continueButton, &QPushButton::clicked, this, &MenuWindow::onContinueButtonClicked);
	connect(languageComboBox, QOverload< int >::of(&QComboBox::currentIndexChanged), this, &MenuWindow::onLanguageChanged);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QHBoxLayout *rowsLayout = new QHBoxLayout();
	QHBoxLayout *columnsLayout = new QHBoxLayout();
	QHBoxLayout *bombsLayout = new QHBoxLayout();
	rowsLabel = new QLabel(tr("Rows:"), this);
	columnsLabel = new QLabel(tr("Columns:"), this);
	bombsLabel = new QLabel(tr("Bombs:"), this);

	rowsLayout->addWidget(rowsLabel);
	rowsLayout->addWidget(rowsSpinBox);
	columnsLayout->addWidget(columnsLabel);
	columnsLayout->addWidget(columnsSpinBox);
	bombsLayout->addWidget(bombsLabel);
	bombsLayout->addWidget(bombsSpinBox);

	mainLayout->addLayout(rowsLayout);
	mainLayout->addLayout(columnsLayout);
	mainLayout->addLayout(bombsLayout);
	mainLayout->addWidget(languageComboBox);
	mainLayout->addWidget(startButton);
	mainLayout->addWidget(continueButton);
}

void MenuWindow::onLanguageChanged(int index)
{
	QString language = languageComboBox->itemData(index).toString();
	selectedLanguage = language;

	if (translator.load(":/translations/qt_" + language + ".qm"))
	{
		qApp->installTranslator(&translator);
		retranslateUi();
	}

	QString iniFilePath = QCoreApplication::applicationDirPath() + "/settings.ini";
	QSettings settings(iniFilePath, QSettings::IniFormat);
	settings.setValue("language", selectedLanguage);
	settings.sync();
}

void MenuWindow::retranslateUi()
{
	rowsLabel->setText(tr("Rows:"));
	columnsLabel->setText(tr("Columns:"));
	bombsLabel->setText(tr("Bombs:"));
	startButton->setText(tr("Start Game"));
	continueButton->setText(tr("Continue"));
}

int MenuWindow::getRows() const
{
	return rows;
}

int MenuWindow::getColumns() const
{
	return columns;
}

int MenuWindow::getBombs() const
{
	return bombs;
}

bool MenuWindow::isContinue() const
{
	return continueGame;
}

void MenuWindow::onStartButtonClicked()
{
	rows = rowsSpinBox->value();
	columns = columnsSpinBox->value();
	bombs = bombsSpinBox->value();

	if (bombs >= rows * columns)
	{
		showError(tr("Number of bombs must be less than the number of cells."));
		return;
	}

	accept();
}

void MenuWindow::onContinueButtonClicked()
{
	continueGame = true;
	accept();
}

void MenuWindow::showError(const QString &message)
{
	QMessageBox::critical(this, tr("Error"), message);
}
