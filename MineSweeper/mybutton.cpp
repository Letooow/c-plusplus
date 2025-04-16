#include "mybutton.h"

MyButton::MyButton(int x, int y, int number, QWidget *parent) :
	QPushButton(parent), number(number), isFlagged(false), isOpen(false)
{
	this->x = x;
	this->y = y;
}

void MyButton::mousePressEvent(QMouseEvent *e)
{
	if (isLeftHanded)
	{
		if (e->button() == Qt::RightButton)
		{
			emit leftClicked(this);
		}
		else if (e->button() == Qt::LeftButton)
		{
			emit rightClicked(this);
		}
		else if (e->button() == Qt::MiddleButton)
		{
			emit middleClicked(this);
		}
	}
	else
	{
		if (e->button() == Qt::LeftButton)
		{
			emit leftClicked(this);
		}
		else if (e->button() == Qt::RightButton)
		{
			emit rightClicked(this);
		}
		else if (e->button() == Qt::MiddleButton)
		{
			emit middleClicked(this);
		}
	}
}

void MyButton::bomb()
{
	this->setStyleSheet(
		"QPushButton {"
		"   border: 2px solid #8f8f91;"
		"   border-radius: 5px;"
		"   background-color: #FF0000;"
		"   padding: 5px;"
		"}");
	QIcon buttonIcon(":/icons/bomb2.png");
	this->setIcon(buttonIcon);
	this->setIconSize(QSize(53, 53));
	this->isOpen = false;
	this->isFlagged = false;
	this->isQuestioned = false;
	this->isBomb = true;
}

void MyButton::empty()
{
	this->setStyleSheet(
		"QPushButton {"
		"   border: 2px solid #8f8f91;"
		"   border-radius: 5px;"
		"   background-color: #BCBCDC;"
		"   padding: 5px;"
		"   color: black;"
		"}");
	this->isFlagged = false;
	this->isQuestioned = false;
	this->isOpen = true;
}

void MyButton::flag()
{
	this->setStyleSheet(
		"QPushButton {"
		"   border: 2px solid #8f8f91;"
		"   border-radius: 5px;"
		"   background-color: #E5F5C6;"
		"   padding: 5px;"
		"}");
	QIcon buttonIcon(":/icons/flag1.png");
	this->setIcon(buttonIcon);
	this->setIconSize(QSize(43, 43));
	this->isFlagged = true;
	this->isQuestioned = false;
	this->isOpen = false;
}

void MyButton::question()
{
	this->setStyleSheet(
		"QPushButton {"
		"   border: 2px solid #8f8f91;"
		"   border-radius: 5px;"
		"   background-color: #E5F5C6;"
		"   padding: 5px;"
		"   color: black;"
		"}");
	QIcon buttonIcon(":/icons/quesion_mark.png");
	this->setIcon(buttonIcon);
	this->setIconSize(QSize(43, 43));
	this->isFlagged = false;
	this->isQuestioned = true;
	this->isOpen = false;
}

void MyButton::notPushed()
{
	this->setStyleSheet(
		"QPushButton {"
		"   border: 2px solid #8f8f91;"
		"   border-radius: 5px;"
		"   background-color: #BCBCDC;"
		"   padding: 5px;"
		"}");
	this->isFlagged = false;
	this->isQuestioned = false;
	this->isOpen = false;
}

void MyButton::drawed()
{
	this->setStyleSheet(
		"QPushButton {"
		"   border: 2px solid #8f8f91;"
		"   border-radius: 5px;"
		"   background-color: #EFFD72;"
		"   padding: 5px;"
		"}");
	this->isFlagged = false;
	this->isQuestioned = false;
	this->isOpen = false;
}

void MyButton::value()
{
	this->setStyleSheet(
		"QPushButton {"
		"   border: 2px solid #8f8f91;"
		"   border-radius: 5px;"
		"   background-color: #f0f0f0;"
		"   padding: 5px;"
		"   color: black;"
		"}");

	QString iconPath;
	switch (this->number)
	{
	case 1:
		iconPath = ":/icons/1.png";
		break;
	case 2:
		iconPath = ":/icons/2.png";
		break;
	case 3:
		iconPath = ":/icons/3.png";
		break;
	case 4:
		iconPath = ":/icons/4.png";
		break;
	case 5:
		iconPath = ":/icons/5.png";
		break;
	case 6:
		iconPath = ":/icons/6.png";
		break;
	case 7:
		iconPath = ":/icons/7.png";
		break;
	case 8:
		iconPath = ":/icons/8.png";
		break;
	default:
		this->setText("");
		break;
	}

	if (!iconPath.isEmpty())
	{
		QIcon icon(iconPath);
		this->setIcon(icon);
		this->setIconSize(QSize(43, 43));
	}
	this->isOpen = true;
}

void MyButton::showIcon()
{
	if (number == -1)
	{
		this->setStyleSheet(
			"QPushButton {"
			"   border: 2px solid #8f8f91;"
			"   border-radius: 5px;"
			"   background-color: #CD0074;"
			"   padding: 5px;"
			"}");
		QIcon buttonIcon(":/icons/bomb2.png");
		this->setIcon(buttonIcon);
		this->setIconSize(QSize(57, 57));
	}
	else
	{
		this->setStyleSheet(
			"QPushButton {"
			"   border: 2px solid #8f8f91;"
			"   border-radius: 5px;"
			"   background-color: #BCBCDC;"
			"   padding: 5px;"
			"   color: black;"
			"}");
		QString iconPath;
		switch (this->number)
		{
		case 1:
			iconPath = ":/icons/1.png";
			break;
		case 2:
			iconPath = ":/icons/2.png";
			break;
		case 3:
			iconPath = ":/icons/3.png";
			break;
		case 4:
			iconPath = ":/icons/4.png";
			break;
		case 5:
			iconPath = ":/icons/5.png";
			break;
		case 6:
			iconPath = ":/icons/6.png";
			break;
		case 7:
			iconPath = ":/icons/7.png";
			break;
		case 8:
			iconPath = ":/icons/8.png";
			break;
		default:
			this->setText("");
			break;
		}

		if (!iconPath.isEmpty())
		{
			QIcon icon(iconPath);
			this->setIcon(icon);
			this->setIconSize(QSize(60, 60));
		}
	}
}

void MyButton::hideIcon()
{
	this->setStyleSheet(
		"QPushButton {"
		"   border: 2px solid #8f8f91;"
		"   border-radius: 5px;"
		"   background-color: #BCBCDC;"
		"   padding: 5px;"
		"   color: black;"
		"}");
	this->setIcon(QIcon());
}
