#ifndef CQCOMMAND_H
#define CQCOMMAND_H

#include <QScrollArea>

class CQCommand;
class QPaintEvent;
class QKeyEvent;
class QResizeEvent;

class CQScrolledCommand : public QScrollArea {
  Q_OBJECT

 public:
  CQScrolledCommand(QWidget *parent=NULL);

  CQCommand *getCommand() const { return command_; }

  void resizeEvent(QResizeEvent *);

  void outputText(const std::string &str);

 public slots:
  void updateScroll();

 signals:
  void executeCommand(const QString &);

 private:
  CQCommand *command_ { nullptr };
};

//---

class CQCommand : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QString prompt   READ prompt   WRITE setPrompt  )
  Q_PROPERTY(int     minLines READ minLines WRITE setMinLines)

 private:
  class Line {
   public:
    Line(const std::string &text = "") :
     text_(text) {
    }

    const std::string &text() const { return text_; }
    void setText(const std::string &s) { text_ = s; }

   private:
    std::string text_;
  };

  class Entry {
   public:
    Entry(const std::string &text="") :
     text_(text) {
    }

    const std::string &getText() const { return text_; }
    void setText(const std::string &text) { text_ = text; pos_ = 0; }

    int getPos() const { return pos_; }

    void clear() { setText(""); }

    void cursorLeft() {
      if (pos_ > 0)
        --pos_;
    }

    void cursorRight() {
      if (pos_ < (int) text_.size())
        ++pos_;
    }

    void insert(const std::string &str) {
      std::string lhs = text_.substr(0, pos_);
      std::string rhs = text_.substr(pos_);

      text_ = lhs + str + rhs;

      pos_ += str.size();
    }

    void backSpace() {
      if (pos_ > 0) {
        std::string lhs = text_.substr(0, pos_);
        std::string rhs = text_.substr(pos_);

        text_ = lhs.substr(0, lhs.size() - 1) + rhs;

        --pos_;
      }
    }

    void deleteChar() {
      if (pos_ < (int) text_.size()) {
        std::string lhs = text_.substr(0, pos_);
        std::string rhs = text_.substr(pos_);

        text_ = lhs + rhs.substr(1);
      }
    }

   private:
    std::string text_;
    int         pos_ { 0 };
  };

 public:
  CQCommand(QWidget *parent=NULL);

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

  void outputText(const std::string &str);

 signals:
  void executeCommand(const QString &);

  void scrollEnd();

 private:
  using LineList = std::vector<Line *>;

  LineList    lines_;
  QString     prompt_;
  QStringList commands_;
  int         commandNum_    { -1 };
  int         minLines_      { 5 };
  Entry       entry_;
  int         parent_width_  { 0 };
  int         parent_height_ { 0 };
  int         pressLineNum_  { -1 };
  int         pressCharNum_  { -1 };
  int         moveLineNum_   { -1 };
  int         moveCharNum_   { -1 };
  bool        pressed_       { false };
};

#endif
