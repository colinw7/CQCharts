#ifndef CQCOMMAND_H
#define CQCOMMAND_H

#include <QScrollArea>

class QPaintEvent;
class QKeyEvent;
class QResizeEvent;

namespace CQCommand {

class CommandWidget;

enum class LineType {
  NONE,
  COMMAND,
  OUTPUT
};

class ScrollArea : public QScrollArea {
  Q_OBJECT

 public:
  ScrollArea(QWidget *parent=NULL);

  void init();

  virtual CommandWidget *createCommandWidget() const;

  CommandWidget *getCommand() const;

  void resizeEvent(QResizeEvent *);

  void outputText(const QString &str);

 public slots:
  void updateScroll();

 signals:
  void executeCommand(const QString &);

 private:
  CommandWidget *command_     { nullptr };
  bool           initialized_ { false };
};

//---

class CommandWidget : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QString prompt   READ prompt   WRITE setPrompt  )
  Q_PROPERTY(int     minLines READ minLines WRITE setMinLines)

  class Line {
   public:
    Line() = default;

    Line(const QString &text, LineType type, int ind=-1) :
     text_(text), type_(type), ind_(ind) {
    }

    const QString &text() const { return text_; }
    void setText(const QString &s) { text_ = s; }

    const LineType &type() const { return type_; }
    void setType(const LineType &t) { type_ = t; }

    int ind() const { return ind_; }
    void setInd(int i) { ind_ = i; }

    int x() const { return x_; }
    void setX(int x) { x_ = x; }

    int y() const { return y_; }
    void setY(int y) { y_ = y; }

   private:
    QString  text_;
    LineType type_ { LineType::NONE };
    int      ind_  { -1 };
    int      x_    { 0 };
    int      y_    { 0 };
  };

  class Entry {
   public:
    Entry(const QString &text="") :
     text_(text) {
    }

    const QString &getText() const { return text_; }
    void setText(const QString &text) { text_ = text; pos_ = 0; }

    int getPos() const { return pos_; }

    void clear() { setText(""); }

    void cursorStart() { pos_ = 0; }
    void cursorEnd  () { pos_ = text_.length(); }

    void cursorLeft() {
      if (pos_ > 0)
        --pos_;
    }

    void cursorRight() {
      if (pos_ < text_.length())
        ++pos_;
    }

    void insert(const QString &str) {
      auto lhs = text_.mid(0, pos_);
      auto rhs = text_.mid(pos_);

      text_ = lhs + str + rhs;

      pos_ += str.length();
    }

    void backSpace() {
      if (pos_ > 0) {
         auto lhs = text_.mid(0, pos_);
         auto rhs = text_.mid(pos_);

        text_ = lhs.mid(0, lhs.length() - 1) + rhs;

        --pos_;
      }
    }

    void deleteChar() {
      if (pos_ < text_.length()) {
        auto lhs = text_.mid(0, pos_);
        auto rhs = text_.mid(pos_);

        text_ = lhs + rhs.mid(1);
      }
    }

   private:
    QString text_;
    int     pos_ { 0 };
  };

 public:
  CommandWidget(QWidget *parent=nullptr);

  virtual ~CommandWidget();

  const QString &prompt() const { return prompt_; }
  void setPrompt(const QString &s) { prompt_ = s; }

  int minLines() const { return minLines_; }
  void setMinLines(int i) { minLines_ = i; }

  QSize sizeHint() const;

  void updateSize(int w, int h);

  void paintEvent(QPaintEvent *e);

  void mousePressEvent  (QMouseEvent *e);
  void mouseMoveEvent   (QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

  void pixelToText(const QPoint &p, int &lineNum, int &charNum);

  void keyPressEvent(QKeyEvent *e);

  void contextMenuEvent(QContextMenuEvent *e);

  void outputText(const QString &str);

  virtual void complete() { }

 private:
  void outputTypeText(const QString &str, const LineType &type, int ind);

  void drawCursor(QPainter *painter, int x, int y, const QChar &c);

  void drawLine(QPainter *painter, Line *line, int y);

  void drawPrompt(QPainter *painter, Line *line, int y);

  void drawSelectedChars(QPainter *painter, int lineNum1, int charNum1,
                         int lineNum2, int charNum2);

  void copy(const QString &text) const;
  void paste();

  QString selectedText() const;

 signals:
  void executeCommand(const QString &);

  void scrollEnd();

 private slots:
  void copySlot();
  void pasteSlot();

 private:
  using LineList = std::vector<Line *>;

  LineList    lines_;
  QString     prompt_;
  QStringList commands_;
  int         commandNum_   { -1 };
  int         minLines_     { 5 };
  Entry       entry_;
  int         parentWidth_  { 0 };
  int         parentHeight_ { 0 };
  int         pressLineNum_ { -1 };
  int         pressCharNum_ { -1 };
  int         moveLineNum_  { -1 };
  int         moveCharNum_  { -1 };
  int         charWidth_    { 8 };
  int         charHeight_   { 12 };
  int         charAscent_   { 8 };
  int         charDescent_  { 0 };
  int         promptY_      { 0 };
  int         promptWidth_  { 0 };
  bool        pressed_      { false };
  int         lastInd_      { 0 };
  int         indMargin_    { 0 };
#if 0
  /* light */
  QColor      bgColor_      { 255, 255, 255 };
  QColor      indColor_     { 0, 0, 0 };
  QColor      commandColor_ { 80, 100, 80 };
  QColor      outputColor_  { 50, 50, 240 };
  QColor      promptColor_  { 0, 0, 0 };
  QColor      cursorColor_  { 0, 255, 0 };
#else
  /* dark */
  QColor      bgColor_      { 40, 40, 40 };
  QColor      indColor_     { 240, 240, 240 };
  QColor      commandColor_ { 200, 255, 200 };
  QColor      outputColor_  { 200, 200, 255 };
  QColor      promptColor_  { 240, 240, 240 };
  QColor      cursorColor_  { 255, 255, 0 };
#endif
};

}

#endif
