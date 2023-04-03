#ifndef CQCOMMAND_H
#define CQCOMMAND_H

#include <CQScrollArea.h>
#include <QListWidget>

class QPaintEvent;
class QKeyEvent;
class QResizeEvent;
class QEventLoop;

namespace CQCommand {

class CommandWidget;
class CompletionList;

enum class LineType {
  NONE,
  COMMAND,
  OUTPUT,
  COMMAND_CONT
};

//---

// Scroll Area containing command widget
//
// Override createCommandWidget API to provide own dervied class for command widget
//
// Call init after construction to setup
//
// Connected to executeCommand signal to execute entered command
class ScrollArea : public CQScrollArea {
  Q_OBJECT

 public:
  ScrollArea(QWidget *parent=nullptr);

  virtual ~ScrollArea() { }

  void init();

  virtual CommandWidget *createCommandWidget() const;

  CommandWidget *getCommand() const;

  //void resizeEvent(QResizeEvent *) override;

  void outputText(const QString &str);

  void updateContents() override;

 public Q_SLOTS:
  void updateScroll();

 Q_SIGNALS:
  void executeCommand(const QString &);
  void keyPress(const QString &);

 private:
  CommandWidget *command_     { nullptr };
  bool           initialized_ { false };
};

//---

// Command Widget containing terminal prompt and output
//
// Override textChanged to handle text changes (before enter)
//
// Override posColor to provide syntax highlighting
//
// Override complete to provide command completion
class CommandWidget : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString prompt     READ prompt       WRITE setPrompt    )
  Q_PROPERTY(int     minLines   READ minLines     WRITE setMinLines  )
  Q_PROPERTY(bool    showPrompt READ isShowPrompt WRITE setShowPrompt)

 public:
  enum class CompleteMode {
    None,
    Longest,
    Interactive
  };

 private:
  struct Part {
    Part() = default;

    QString text;
    QColor  color;
  };

  using Parts = std::vector<Part>;

  class Line {
   public:
    Line() = default;

    Line(const QString &text, LineType type, int ind=-1);

    const QString &text() const { return text_; }
    void setText(const QString &s);

    const LineType &type() const { return type_; }
    void setType(const LineType &t);

    int ind() const { return ind_; }
    void setInd(int i) { ind_ = i; }

    int x() const { return x_; }
    void setX(int x) { x_ = x; }

    int y() const { return y_; }
    void setY(int y) { y_ = y; }

    void initParts();

    const Parts &parts() const { return parts_; }

    int numLines() const { return 1; }

   private:
    QString  text_;
    LineType type_ { LineType::NONE };
    int      ind_  { -1 };
    int      x_    { 0 };
    int      y_    { 0 };
    Parts    parts_;
  };

  class Entry {
   public:
    Entry(const QString &text="") :
     text_(text) {
    }

    const QString &getText() const { return text_; }
    void setText(const QString &text) { text_ = text; pos_ = 0; }

    int getPos() const { return pos_; }

    void clear() { setText(""); numLines_ = 1; }

    void addNewLine() { insert("\n"); ++numLines_; }

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

    int numLines() const { return numLines_; }

   private:
    QString text_;
    int     pos_ { 0 };
    int     numLines_ { 1 };
  };

 public:
  CommandWidget(ScrollArea *area);

  virtual ~CommandWidget();

  //---

  //! get/set area
  ScrollArea *area() const { return area_; }
  void setArea(ScrollArea *area) { area_ = area; }

  //! get/set prompt string
  const QString &prompt() const { return prompt_; }
  void setPrompt(const QString &s) { prompt_ = s; }

  //! get/set show prompt
  bool isShowPrompt() const { return showPrompt_; }
  void setShowPrompt(bool b) { showPrompt_ = b; }

  //! get/set min lines
  int minLines() const { return minLines_; }
  void setMinLines(int i) { minLines_ = i; }

  //---

  QSize sizeHint() const override;

  void updateSize();

  void paintEvent(QPaintEvent *e) override;

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

  void pixelToText(const QPoint &p, int &lineNum, int &charNum);

  bool event(QEvent *event) override;

  void keyPressEvent(QKeyEvent *e) override;

  void contextMenuEvent(QContextMenuEvent *e) override;

  void outputText(const QString &str);

  //---

  // override to handle text changes (before enter)
  virtual void textChanged() { }

  //---

  // override to provide syntax highlighting
  virtual QColor posColor(int /*pos*/) const { return QColor(); }

  //---

  // overrides to provide command completion

  virtual bool isCompleteLine(const QString &str) const;

  virtual int getNextLineIndent(const QString &) const { return 0; }

  virtual bool complete(const QString & /*text*/, int /*pos*/,
                        QString & /*newText*/, CompleteMode /*completeMode*/) const {
    return false;
  }

  QString showCompletionChooser(const QStringList &strs, bool modal=true);

  //---

  void clearEntry();

 protected:
  const QString &getText() const;

  void setText(const QString &text);

  //---

 private:
  void outputTypeText(const QString &str, const LineType &type, int ind);

  void drawCursor(QPainter *painter, int x, int y, const QChar &c);

  void drawLine(QPainter *painter, Line *line, int y);

  void drawPrompt(QPainter *painter, Line *line, int y);

  void drawSelectedChars(QPainter *painter, int lineNum1, int charNum1,
                         int lineNum2, int charNum2);

  void drawPosText(QPainter *painter, int &x, int &y, const QString &text, int &pos);

  void drawText(QPainter *painter, int x, int y, const QString &text);

  //---

  void copy(const QString &text) const;
  void paste();

  QString selectedText() const;

 Q_SIGNALS:
  void executeCommand(const QString &);
  void keyPress(const QString &);

  void scrollEnd();

 private Q_SLOTS:
  void copySlot();
  void pasteSlot();

  void completionSelectedSlot(const QString &str);
  void completionCancelledSlot();

 private:
  using LineList = std::vector<Line *>;

  ScrollArea* area_         { nullptr };
  LineList    lines_;

  QString prompt_;
  bool    showPrompt_ { true };

  QStringList commands_;
  int         commandNum_   { -1 };
  int         minLines_     { 5 };
  Entry       entry_;
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
  int         xo_           { 0 };
  int         yo_           { 0 };

#if 0
  /* light */
  QColor bgColor_      { 255, 255, 255 };
  QColor indColor_     { 0, 0, 0 };
  QColor commandColor_ { 80, 100, 80 };
  QColor outputColor_  { 50, 50, 240 };
  QColor promptColor_  { 0, 0, 0 };
  QColor cursorColor_  { 0, 255, 0 };
#else
  /* dark */
  QColor bgColor_      { 40, 40, 40 };
  QColor indColor_     { 240, 255, 255 };
  QColor commandColor_ { 200, 255, 200 };
  QColor outputColor_  { 240, 240, 240 };
  QColor promptColor_  { 240, 240, 240 };
  QColor cursorColor_  { 255, 255, 0 };
#endif

  CompletionList *completionList_ { nullptr };
  QEventLoop*     eventLoop_      { nullptr };
  QString         completionItem_;
};

//---

class CompletionList : public QListWidget {
  Q_OBJECT

 public:
  CompletionList(CommandWidget *w);

  bool event(QEvent *event) override;

 Q_SIGNALS:
  void itemSelected(const QString &);
  void itemCancelled();

 private:
  CommandWidget* w_ { nullptr };
};

}

#endif
