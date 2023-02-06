#include <CQCommand.h>
#include <CQUtil.h>

#include <QApplication>
#include <QPainter>
#include <QFontMetrics>
#include <QScrollBar>
#include <QContextMenuEvent>
#include <QMenu>
#include <QKeyEvent>
#include <QClipboard>
#include <QEventLoop>

#include <iostream>

namespace CQCommand {

ScrollArea::
ScrollArea(QWidget *parent) :
 CQScrollArea(parent, nullptr)
{
  setObjectName("scrolledCommand");

  //setVerticalScrollBarPolicy  (Qt::ScrollBarAlwaysOn);
  //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void
ScrollArea::
init()
{
  command_ = createCommandWidget();

  command_->setArea(this);

  setWidget(command_);

  connect(command_, SIGNAL(executeCommand(const QString &)),
          this, SIGNAL(executeCommand(const QString &)));
  connect(command_, SIGNAL(keyPress(const QString &)),
          this, SIGNAL(keyPress(const QString &)));

  connect(command_, SIGNAL(scrollEnd()), this, SLOT(updateScroll()));

  command_->updateSize();

  initialized_ = true;
}

CommandWidget *
ScrollArea::
createCommandWidget() const
{
  return new CommandWidget(const_cast<ScrollArea *>(this));
}

CommandWidget *
ScrollArea::
getCommand() const
{
  if (! initialized_)
    const_cast<ScrollArea *>(this)->init();

  return command_;
}

void
ScrollArea::
outputText(const QString &str)
{
  getCommand()->outputText(str);
}

void
ScrollArea::
updateScroll()
{
  ensureVisible(0, getYSize() - 1);
}

void
ScrollArea::
updateContents()
{
  getWidget()->update();
}

//------------

CommandWidget::
CommandWidget(ScrollArea *area) :
 QFrame(area), area_(area)
{
  setObjectName("command");

  //---

  prompt_ = "> ";

  setFocusPolicy(Qt::StrongFocus);

  //---

  auto fixedFont = CQUtil::getMonospaceFont();

  setFont(fixedFont);

  //---

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

CommandWidget::
~CommandWidget()
{
}

QSize
CommandWidget::
sizeHint() const
{
  return QSize(std::max(width(), 100), std::max(height(), 100));
}

void
CommandWidget::
updateSize()
{
  int numLines = 0;

  for (auto *line : lines_) {
    numLines += line->numLines();
  }

  numLines += entry_.numLines();

  numLines = std::max(numLines, minLines());

  QFontMetrics fm(font());

  charWidth_  = fm.horizontalAdvance("X");
  charHeight_ = fm.height();

  int maxLen = 0;

  for (const auto &line : lines_)
    maxLen = std::max(maxLen, line->text().length());

  if (maxLen < 100)
    maxLen = 100;

  int w1 = maxLen*charWidth_;
  int h1 = (numLines + 1)*charHeight_;

  area_->setXSize(w1);
  area_->setYSize(h1);

  area_->setXSingleStep(charWidth_);
  area_->setYSingleStep(charHeight_);
}

void
CommandWidget::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.fillRect(rect(), bgColor_);

  //---

  xo_ = area_->getXOffset();
  yo_ = area_->getYOffset();

  //---

//int w = area_->getXSize();
  int h = std::max(height(), area_->getYSize());

  QFontMetrics fm(font());

  charWidth_   = fm.horizontalAdvance("X");
  charHeight_  = fm.height();
  charAscent_  = fm.ascent();
  charDescent_ = fm.descent();

  //---

  int indLen = 1;
  int indVal = lastInd_;

  while (indVal > 9) {
    indVal /= 10;

    ++indLen;
  }

  indMargin_ = charWidth_*indLen;

  //--

  int x = xo_ + indMargin_;
  int y = yo_ + h - 1; // bottom

  //---

  // draw prompt
  y -= (entry_.numLines() - 1)*charHeight_;

  promptY_ = y;

  if (isShowPrompt()) {
    promptWidth_ = prompt().length()*charWidth_;

    drawPrompt(&painter, nullptr, y);

    x += promptWidth_;
  }
  else
    promptWidth_ = 0;

  //---

  // get entry text before/after cursor
  const auto &str = getText();
  int         pos = entry_.getPos();

  auto lhs = str.mid(0, pos);
  auto rhs = str.mid(pos);

  //---

  int tpos = 0;

  // draw entry text before and after cursor
  painter.setPen(commandColor_);

  drawPosText(&painter, x, y, lhs, tpos);

  int cx = x;

  drawPosText(&painter, x, y, rhs, tpos);

  //---

  // draw cursor
  drawCursor(&painter, cx, y, (rhs.length() ? rhs[0] : QChar()));

  //---

  // draw lines (bottom to top)
  y -= entry_.numLines()*charHeight_;

  int numLines = int(lines_.size());

  for (int i = numLines - 1; i >= 0; --i) {
    const auto &line = lines_[size_t(i)];

    drawLine(&painter, line, y);

    y -= charHeight_;
  }

  //---

  // draw selection
  int lineNum1 = pressLineNum_;
  int charNum1 = pressCharNum_;
  int lineNum2 = moveLineNum_;
  int charNum2 = moveCharNum_;

  if (lineNum1 > lineNum2 || (lineNum1 == lineNum2 && charNum1 > charNum2)) {
    std::swap(lineNum1, lineNum2);
    std::swap(charNum1, charNum2);
  }

  if (lineNum1 != lineNum2 || charNum1 != charNum2)
    drawSelectedChars(&painter, lineNum1, charNum1, lineNum2, charNum2);
}

void
CommandWidget::
drawCursor(QPainter *painter, int x, int y, const QChar &c)
{
  QRect r(x, y - charHeight_, charWidth_, charHeight_);

  if (hasFocus()) {
    painter->fillRect(r, cursorColor_);

    if (! c.isNull()) {
      painter->setPen(bgColor_);

      drawText(painter, x, y, QString(c));
    }
  }
  else {
    painter->setPen(cursorColor_);

    painter->drawRect(r);
  }
}

void
CommandWidget::
drawLine(QPainter *painter, Line *line, int y)
{
  int x = xo_;

  line->setY(y);

  if      (line->type() == LineType::OUTPUT)
    line->setX(x);
  else if (line->type() == LineType::COMMAND)
    line->setX(x + indMargin_ + promptWidth_);
  else
    line->setX(x);

  if (y + charHeight_ < 0)
    return;

  if (line->type() == LineType::COMMAND) {
    if (isShowPrompt())
      drawPrompt(painter, line, y);
  }

  if      (line->type() == LineType::OUTPUT)
    painter->setPen(outputColor_);
  else if (line->type() == LineType::COMMAND || line->type() == LineType::COMMAND_CONT)
    painter->setPen(commandColor_);
  else
    painter->setPen(Qt::red);

  x = line->x();

  const auto &parts = line->parts();

  if (parts.empty()) {
    const auto &text = line->text();

    drawText(painter, x, y, text);
  }
  else {
    for (const auto &part : parts) {
      if (part.color.isValid())
        painter->setPen(part.color);
      else
        painter->setPen(outputColor_);

      const auto &text = part.text;

      drawText(painter, x, y, text);
    }
  }
}

void
CommandWidget::
drawPrompt(QPainter *painter, Line *line, int y)
{
  int ind = (line ? line->ind() : lastInd_ + 1);

  painter->setPen(indColor_);

  int x = xo_;

  drawText(painter, x, y, QString("%1").arg(ind));

  x += indMargin_;

  painter->setPen(promptColor_);

  drawText(painter, x, y, prompt());
}

void
CommandWidget::
drawSelectedChars(QPainter *painter, int lineNum1, int charNum1, int lineNum2, int charNum2)
{
  int numLines = int(lines_.size());

  painter->setPen(bgColor_);

  for (int i = lineNum1; i <= lineNum2; ++i) {
    if (i < 0 || i >= numLines) continue;

    const auto &line = lines_[size_t(i)];

    int ty = line->y();
    int tx = line->x();

    const auto &text = line->text();

    int len = text.size();

    for (int j = 0; j < len; ++j) {
      if      (i == lineNum1 && i == lineNum2) {
        if (j < charNum1 || j > charNum2)
          continue;
      }
      else if (i == lineNum1) {
        if (j < charNum1)
          continue;
      }
      else if (i == lineNum2) {
        if (j > charNum2)
          continue;
      }

      int tx1 = tx + j*charWidth_;

      painter->fillRect(QRect(tx1, ty - charHeight_, charWidth_, charHeight_), outputColor_);

      drawText(painter, tx1, ty, text.mid(j, 1));
    }
  }
}

void
CommandWidget::
drawPosText(QPainter *painter, int &x, int &y, const QString &text, int &pos)
{
  auto c = painter->pen().color();

  auto c1 = c;

  for (int i = 0; i < text.length(); ++i) {
    auto c2 = posColor(pos);

    if (c2.isValid() && c2 != c1) {
      painter->setPen(c2);

      c1 = c2;
    }

    if (text[i] == '\n') {
      y += charHeight_;
      x  = 0;
    }
    else {
      painter->drawText(x, y - charDescent_, text[i]);

      x += charWidth_;
    }

    ++pos;
  }

  if (c != c1)
    painter->setPen(c);
}

void
CommandWidget::
drawText(QPainter *painter, int x, int y, const QString &text)
{
  for (int i = 0; i < text.length(); ++i) {
    painter->drawText(x, y - charDescent_, text[i]);

    x += charWidth_;
  }
}

void
CommandWidget::
mousePressEvent(QMouseEvent *e)
{
  if      (e->button() == Qt::LeftButton) {
    pixelToText(e->pos(), pressLineNum_, pressCharNum_);

    moveLineNum_ = pressLineNum_;
    moveCharNum_ = pressCharNum_;
    pressed_     = true;
  }
  else if (e->button() == Qt::MiddleButton) {
    paste();

    update();
  }
}

void
CommandWidget::
mouseMoveEvent(QMouseEvent *e)
{
  if (pressed_) {
    pixelToText(e->pos(), moveLineNum_, moveCharNum_);

    update();
  }
}

void
CommandWidget::
mouseReleaseEvent(QMouseEvent *e)
{
  if (pressed_)
    pixelToText(e->pos(), moveLineNum_, moveCharNum_);

  pressed_ = false;
}

void
CommandWidget::
pixelToText(const QPoint &p, int &lineNum, int &charNum)
{
  int numLines = int(lines_.size());

  lineNum = -1;
  charNum = -1;

  for (int i = 0; i < numLines; ++i) {
    const auto &line = lines_[size_t(i)];

    int y2 = line->y(); // bottom
    int y1 = y2 - charHeight_ + 1;

    if (p.y() >= y1 && p.y() <= y2) {
      lineNum = i;
      charNum = (p.x() - line->x())/charWidth_;
      break;
    }
  }
}

bool
CommandWidget::
event(QEvent *e)
{
  if (e->type() == QEvent::KeyPress) {
    QKeyEvent *ke = static_cast<QKeyEvent *>(e);

    if (ke->key() == Qt::Key_Tab) {
      CompleteMode completeMode = CompleteMode::Longest;

      if (ke->modifiers() & Qt::ControlModifier)
        completeMode = CompleteMode::Interactive;

      QString text;

      if (complete(getText(), entry_.getPos(), text, completeMode)) {
        setText(text);

        update();
      }

      return true;
    }
  }
  else if (e->type() == QEvent::Wheel) {
    area_->handleWheelEvent(static_cast<QWheelEvent *>(e));
  }

  return QFrame::event(e);
}

void
CommandWidget::
keyPressEvent(QKeyEvent *event)
{
  auto key = event->key();
  auto mod = event->modifiers();

  QKeySequence ks(event->key() | event->modifiers());

  if (key == Qt::Key_Return || key == Qt::Key_Enter) {
    auto str = getText(); // copy as entry is cleared

    if (isCompleteLine(str)) {
      outputTypeText(str + "\n", LineType::COMMAND, ++lastInd_);

      entry_.clear();

      Q_EMIT executeCommand(str);

      Q_EMIT scrollEnd();

      commands_.push_back(str);

      commandNum_ = -1;

      textChanged();
    }
    else {
      entry_.addNewLine();

      int n = getNextLineIndent(str);

      for (int i = 0; i < n; ++i)
        entry_.insert(" ");

      textChanged();
    }
  }
  else if (key == Qt::Key_Left) {
    entry_.cursorLeft();
  }
  else if (key == Qt::Key_Right) {
    entry_.cursorRight();
  }
  else if (key == Qt::Key_Up) {
    QString command;

    int pos = commands_.size() + commandNum_;

    if (pos >= 0 && pos < commands_.size()) {
      command = commands_[pos];

      --commandNum_;
    }

    setText(command);
  }
  else if (key == Qt::Key_Down) {
    ++commandNum_;

    QString command;

    int pos = commands_.size() + commandNum_;

    if (pos >= 0 && pos < commands_.size())
      command = commands_[pos];
    else
      commandNum_ = -1;

    setText(command);
  }
  else if (key == Qt::Key_Backspace) {
    entry_.backSpace();

    textChanged();
  }
  else if (key == Qt::Key_Delete) {
    entry_.deleteChar();

    textChanged();
  }
  else if (key == Qt::Key_Insert) {
    paste();
  }
  else if (key == Qt::Key_Tab) {
    // handled by event
  }
  else if (key == Qt::Key_Home) {
    // TODO
  }
  else if (key == Qt::Key_Escape) {
    // TODO
  }
  else if ((key >= Qt::Key_A && key <= Qt::Key_Z) && (mod & Qt::ControlModifier)) {
    if      (key == Qt::Key_A) // beginning
      entry_.cursorStart();
    else if (key == Qt::Key_E) // end
      entry_.cursorEnd();
    else if (key == Qt::Key_U) { // delete all before
      //entry_.clearBefore();
      //textChanged();
    }
    else if (key == Qt::Key_H) {
      entry_.backSpace();
      textChanged();
    }
    else if (key == Qt::Key_W) { // delete word before
      //entry_.clearWordBefore();
      //textChanged();
    }
    else if (key == Qt::Key_K) { // delete all after
      //entry_.clearAfter();
      //textChanged();
    }
    else if (key == Qt::Key_T) { // swap chars before
      //entry_.swapBefore();
      //textChanged();
    }
    else if (key == Qt::Key_C) {
      copy(selectedText());
    }
    else if (key == Qt::Key_V) {
      paste();
    }
  }
  else {
    entry_.insert(event->text());

    textChanged();
  }

  update();

  Q_EMIT keyPress(ks.toString());
}

void
CommandWidget::
contextMenuEvent(QContextMenuEvent *e)
{
  auto *menu = new QMenu;

  //---

  auto *copyAction  = menu->addAction("&Copy\tCtrl+C");
  auto *pasteAction = menu->addAction("&Paste\tCtrl+V");

  connect(copyAction, SIGNAL(triggered()), this, SLOT(copySlot()));
  connect(pasteAction, SIGNAL(triggered()), this, SLOT(pasteSlot()));

  //---

  (void) menu->exec(e->globalPos());

  delete menu;
}

QString
CommandWidget::
selectedText() const
{
  int lineNum1 = pressLineNum_;
  int charNum1 = pressCharNum_;
  int lineNum2 = moveLineNum_;
  int charNum2 = moveCharNum_;

  if (lineNum1 > lineNum2 || (lineNum1 == lineNum2 && charNum1 > charNum2)) {
    std::swap(lineNum1, lineNum2);
    std::swap(charNum1, charNum2);
  }

  if (lineNum1 == lineNum2 && charNum1 == charNum2)
    return "";

  int numLines = int(lines_.size());

  QString str;

  for (int i = lineNum1; i <= lineNum2; ++i) {
    if (i < 0 || i >= numLines) continue;

    const auto &line = lines_[size_t(i)];

    const auto &text = line->text();

    if (str.length() > 0)
      str += "\n";

    int len = text.size();

    for (int j = 0; j < len; ++j) {
      if      (i == lineNum1 && i == lineNum2) {
        if (j < charNum1 || j > charNum2)
          continue;
      }
      else if (i == lineNum1) {
        if (j < charNum1)
          continue;
      }
      else if (i == lineNum2) {
        if (j > charNum2)
          continue;
      }

      str += text.mid(j, 1);
    }
  }

  return str;
}

const QString &
CommandWidget::
getText() const
{
  return entry_.getText();
}

void
CommandWidget::
setText(const QString &text)
{
  entry_.setText(text);

  entry_.cursorEnd();

  textChanged();
}

//---

void
CommandWidget::
copySlot()
{
  copy(selectedText());
}

void
CommandWidget::
pasteSlot()
{
  paste();

  update();
}

void
CommandWidget::
copy(const QString &text) const
{
  auto *clipboard = QApplication::clipboard();

  clipboard->setText(text, QClipboard::Selection);
}

void
CommandWidget::
paste()
{
  auto *clipboard = QApplication::clipboard();

  auto text = clipboard->text(QClipboard::Selection);

  entry_.insert(text);

  textChanged();
}

//---

void
CommandWidget::
outputText(const QString &str)
{
  outputTypeText(str, CQCommand::LineType::OUTPUT, -1);
}

void
CommandWidget::
outputTypeText(const QString &str, const LineType &type, int ind)
{
  static QString buffer;

  buffer += str;

  auto pos = buffer.indexOf('\n');

  auto type1 = type;

  while (pos != -1) {
    auto lhs = buffer.mid(0, pos);

    auto *line = new Line(lhs, type1, ind);

    if (type1 == CQCommand::LineType::OUTPUT)
      line->initParts();

    lines_.push_back(line);

    buffer = buffer.mid(pos + 1);

    pos = buffer.indexOf('\n');

    if (type1 == LineType::COMMAND)
      type1 = LineType::COMMAND_CONT;
  }

  updateSize();
}

bool
CommandWidget::
isCompleteLine(const QString &str) const
{
  auto len = str.length();

  if (len && str[len - 1] == '\\')
    return false;

  return true;
}

QString
CommandWidget::
showCompletionChooser(const QStringList &strs, bool modal)
{
  if (! completionList_)
    completionList_ = new CompletionList(this);

  completionList_->move(0, height() - charHeight_);

  int margin = 4;
  int border = 10;

  int w = 0;

  for (const auto &str : strs)
    w = std::max(w, str.length()*charWidth_);

  if (w + 2*border >= width() - 2*margin)
    w = width() - 2*margin - 2*border;

  int h = strs.size()*charHeight_;

  if (h + 2*border >= height() - charHeight_ - 2*margin)
    h = height() - charHeight_ - 2*margin - 2*border;

  completionList_->move(margin, height() - h - 2*border - charHeight_ - margin);
  completionList_->resize(w + 2*border, h + 2*border);

  completionList_->clear();

  for (const auto &str : strs)
    completionList_->addItem(str);

  completionList_->setCurrentItem(completionList_->item(0));

  completionList_->show();

  completionList_->setFocus();

  completionItem_ = "";

  if (modal) {
    connect(completionList_, SIGNAL(itemSelected(const QString &)),
            this, SLOT(completionSelectedSlot(const QString &)));
    connect(completionList_, SIGNAL(itemCancelled()), this, SLOT(completionCancelledSlot()));

    assert(! eventLoop_);

    eventLoop_ = new QEventLoop;

    (void) eventLoop_->exec();

    disconnect(completionList_, SIGNAL(itemSelected(const QString &)),
               this, SLOT(completionSelectedSlot(const QString &)));
    disconnect(completionList_, SIGNAL(itemCancelled()), this, SLOT(completionCancelledSlot()));

    delete eventLoop_;

    eventLoop_ = nullptr;
  }

  return completionItem_;
}

void
CommandWidget::
completionSelectedSlot(const QString &str)
{
  completionItem_ = str;

  if (eventLoop_)
    eventLoop_->exit();
}

void
CommandWidget::
completionCancelledSlot()
{
  completionItem_ = "";

  if (eventLoop_)
    eventLoop_->exit();
}

void
CommandWidget::
clearEntry()
{
  entry_.clear();
}

//------

CommandWidget::Line::
Line(const QString &text, LineType type, int ind) :
 text_(text), type_(type), ind_(ind)
{
}

void
CommandWidget::Line::
setText(const QString &s)
{
  text_ = s;

  parts_.clear();
}

void
CommandWidget::Line::
setType(const LineType &t)
{
  type_ = t;
}

void
CommandWidget::Line::
initParts()
{
  auto getEscapeColor = [](int n) {
    if      (n ==  0) return QColor();
    else if (n == 31) return QColor(Qt::red);
    else if (n == 32) return QColor(Qt::green);
    else if (n == 33) return QColor(Qt::yellow);
    else if (n == 34) return QColor(Qt::blue);
    else if (n == 35) return QColor(Qt::magenta);
    else if (n == 36) return QColor(Qt::cyan);
    else if (n == 37) return QColor(Qt::white);
    else              return QColor();
  };

  parts_.clear();

  int len = text_.length();

  Part part;

  for (int i = 0; i < len; ++i) {
    char c = text_[i].toLatin1();

    if (c == '') {
      if (part.text.length()) {
        parts_.push_back(part);

        part = Part();
      }

      c = text_[++i].toLatin1();

      if (i < len && c == '[') {
        c = text_[++i].toLatin1();

        int n = 0;

        while (i < len && isdigit(c)) {
          n = n*10 + (c - '0');

          c = text_[++i].toLatin1();
        }

        if (i < len && c == 'm') {
          part.color = getEscapeColor(n);
        }
      }
    }
    else {
      part.text += text_[i];
    }
  }

  if (part.text.length())
    parts_.push_back(part);
}

//------

CompletionList::
CompletionList(CommandWidget *w) :
 QListWidget(w), w_(w)
{
  setFocusPolicy(Qt::StrongFocus);
}

bool
CompletionList::
event(QEvent *e)
{
  if (e->type() == QEvent::KeyPress) {
    QKeyEvent *ke = static_cast<QKeyEvent *>(e);

    if      (ke->key() == Qt::Key_Escape) {
      Q_EMIT itemCancelled();

      hide();

      e->setAccepted(true);

      return true;
    }
    else if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
      if (currentItem())
        Q_EMIT itemSelected(currentItem()->text());

      hide();

      e->setAccepted(true);

      return true;
    }
    else if (ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down) {
      QListWidget::event(e);

      e->setAccepted(true);

      return true;
    }
  }

  return QListWidget::event(e);
}

}
