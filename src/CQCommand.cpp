#include <CQCommand.h>

#include <QPainter>
#include <QFontMetrics>
#include <QScrollBar>
#include <QKeyEvent>

CQScrolledCommand::
CQScrolledCommand(QWidget *parent) :
 QScrollArea(parent)
{
  setObjectName("scrolledCommand");

  setVerticalScrollBarPolicy  (Qt::ScrollBarAlwaysOn);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  command_ = new CQCommand(this);

  setWidget(command_);

  connect(command_, SIGNAL(executeCommand(const QString &)),
          this, SIGNAL(executeCommand(const QString &)));

  connect(command_, SIGNAL(scrollEnd()), this, SLOT(updateScroll()));
}

void
CQScrolledCommand::
resizeEvent(QResizeEvent *)
{
  int w = width();
  int h = height();

  int sh = horizontalScrollBar()->height();
  int sw = verticalScrollBar  ()->width ();

  command_->updateSize(w - sw, h - sh);
}

void
CQScrolledCommand::
outputText(const std::string &str)
{
  command_->outputText(str);
}

void
CQScrolledCommand::
updateScroll()
{
  ensureVisible(0, command_->height() - 1);
}

//------------

CQCommand::
CQCommand(QWidget *parent) :
 QWidget(parent)
{
  setObjectName("command");

  //---

  prompt_ = ">";

  parent_width_  = 0;
  parent_height_ = 0;

  setFocusPolicy(Qt::StrongFocus);

  updateSize(0, 0);
}

QSize
CQCommand::
sizeHint() const
{
  return QSize(std::max(width(), 100), std::max(height(), 100));
}

void
CQCommand::
updateSize(int w, int h)
{
  if (w > 0 && h > 0) {
    parent_width_  = w;
    parent_height_ = h;
  }

  int num_lines = std::max(int(lines_.size()) + 1, minLines());

  QFontMetrics fm(font());

  int ascent  = fm.ascent();
  int descent = fm.descent();

  int w1 = std::max(parent_width_, 100);
  int h1 = std::max(parent_height_, num_lines*(ascent + descent));

  resize(w1, h1);

  setMinimumSize(w1, h1);
  setMaximumSize(w1, h1);
}

void
CQCommand::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  //int w = width();
  int h = height();

  QFontMetrics fm(font());

  int ascent  = fm.ascent();
  int descent = fm.descent();

  int char_height = ascent + descent;

  int x = 0;
  int y = h - ascent;

  painter.setPen(QColor(0,0,0));

  painter.drawText(x, y, prompt());

  x += fm.width(prompt());

  const std::string &str = entry_.getText();
  int                pos = entry_.getPos();

  std::string lhs = str.substr(0, pos);
  std::string rhs = str.substr(pos);

  painter.drawText(x, y, lhs.c_str());

  x += fm.width(lhs.c_str());

  painter.setPen(QColor(0,255,0));

  painter.drawLine(x, y - ascent, x, y);

  painter.setPen(QColor(0,0,0));

  painter.drawText(x, y, rhs.c_str());

  x  = 0;
  y -= char_height;

  int num_lines = lines_.size();

  for (int i = num_lines - 1; i >= 0; --i) {
    painter.drawText(x, y, lines_[i]->text().c_str());

    y -= char_height;

    if (y + char_height < 0)
      break;
  }
}

void
CQCommand::
keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter: {
      const std::string &str = entry_.getText();

      QString qstr(str.c_str());

      outputText(prompt().toStdString() + str + "\n");

      entry_.clear();

      emit executeCommand(qstr);

      emit scrollEnd();

      commands_.push_back(qstr);

      commandNum_ = -1;

      break;
    }
    case Qt::Key_Left:
      entry_.cursorLeft();

      break;
    case Qt::Key_Right:
      entry_.cursorRight();

      break;
    case Qt::Key_Up: {
      QString command;

      int pos = commands_.size() + commandNum_;

      if (pos >= 0 && pos < commands_.size()) {
        command = commands_[pos];

        --commandNum_;
      }

      entry_.setText(command.toStdString());

      break;
    }
    case Qt::Key_Down: {
      ++commandNum_;

      QString command;

      int pos = commands_.size() + commandNum_;

      if (pos >= 0 && pos < commands_.size())
        command = commands_[pos];
      else
        commandNum_ = -1;

      entry_.setText(command.toStdString());

      break;
    }
    case Qt::Key_Backspace:
      entry_.backSpace();

      break;
    case Qt::Key_Delete:
      entry_.deleteChar();

      break;
    default:
      entry_.insert(event->text().toStdString());

      break;
  }

  update();
}

void
CQCommand::
outputText(const std::string &str)
{
  static std::string buffer;

  buffer += str;

  std::string::size_type pos = buffer.find('\n');

  while (pos != std::string::npos) {
    std::string lhs = buffer.substr(0, pos);

    lines_.push_back(new Line(lhs));

    buffer = buffer.substr(pos + 1);

    pos = buffer.find('\n');
  }

  updateSize(0, 0);
}
