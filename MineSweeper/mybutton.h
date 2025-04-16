#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QMouseEvent>
#include <QPushButton>

class MyButton : public QPushButton
{
	Q_OBJECT

  public:
	int number;
	bool isFlagged;
	bool isQuestioned;
	bool isOpen;
	bool isBomb;
	bool isLeftHanded = false;
	MyButton(int x, int y, int number, QWidget* parent = nullptr);
	void empty();
	void bomb();
	void flag();
	void question();
	void value();
	void notPushed();
	void drawed();
	void showIcon();
	void hideIcon();
	int x, y;

  protected:
	void mousePressEvent(QMouseEvent* event) override;

  signals:
	void rightClicked(MyButton* button);
	void leftClicked(MyButton* button);
	void middleClicked(MyButton* button);
};

#endif	  // MYBUTTON_H
