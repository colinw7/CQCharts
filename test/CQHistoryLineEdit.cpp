#include <CQHistoryLineEdit.h>
#include <CHistory.h>
#include <QKeyEvent>

CQHistoryLineEdit::
CQHistoryLineEdit(QWidget *parent) :
 QLineEdit(parent)
{
  history_ = new CHistory;

  connect(this, SIGNAL(returnPressed()), this, SLOT(execSlot()));
}

CQHistoryLineEdit::
~CQHistoryLineEdit()
{
}

void
CQHistoryLineEdit::
execSlot()
{
  QString str = text();

  emit exec(str);

  history_->addCommand(str.toStdString());

  setText("");
}

void
CQHistoryLineEdit::
keyPressEvent(QKeyEvent *event)
{
  int key = event->key();

  switch (key) {
    case Qt::Key_Up: {
      std::string cmd;

      if (history_->getPrevCommand(cmd))
        setText(cmd.c_str());

      break;
    }
    case Qt::Key_Down: {
      std::string cmd;

      if (history_->getNextCommand(cmd))
        setText(cmd.c_str());

      break;
    }
    default:
      QLineEdit::keyPressEvent(event);

      break;
  }
}
