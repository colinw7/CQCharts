#ifndef CQTabSplit_H
#define CQTabSplit_H

#include <QFrame>
#include <QSplitter>
#include <QTabWidget>
#include <QPointer>

class CQGroupBox;
class QVBoxLayout;

// TODO: auto switch depending on min size hint
class CQTabSplit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
  Q_PROPERTY(State           state       READ state       WRITE setState      )
  Q_PROPERTY(bool            grouped     READ isGrouped   WRITE setGrouped    )

  Q_ENUMS(State)

 public:
  enum class State {
    HSPLIT,
    VSPLIT,
    TAB,
    AUTO
  };

  using Sizes = QList<int>;

 public:
  CQTabSplit(Qt::Orientation orient, QWidget *parent=nullptr);
  CQTabSplit(QWidget *parent=nullptr);

  // get/set orientation
  Qt::Orientation orientation() const { return orient_; }
  void setOrientation(Qt::Orientation orient);

  State state() const { return state_; }
  void setState(State state);

  // get/set grouped
  bool isGrouped() const { return grouped_; }
  void setGrouped(bool b) { grouped_ = b; }

  QWidget *widget(int i) const;

  void addWidget(QWidget *w, const QString &name);

  int count() const;

  void setSizes(const Sizes &sizes);

  QSize sizeHint() const override;

 private:
  void init();

 private:
  using WidgetP = QPointer<QWidget>;

  struct WidgetData {
    WidgetP      w;
    QString      name;
    CQGroupBox*  group { nullptr };
    QVBoxLayout* layout;

    WidgetData(QWidget *w, const QString &name) :
     w(w), name(name) {
    }
  };

  using Widgets = std::vector<WidgetData>;

  Qt::Orientation orient_   { Qt::Horizontal };
  State           state_    { State::HSPLIT };
  bool            grouped_  { false };
  Widgets         widgets_;
  Sizes           hsizes_;
  Sizes           vsizes_;
  QTabWidget*     tabWidget_{ nullptr };
  QSplitter*      splitter_ { nullptr };
};

//---

class CQTabSplitSplitter : public QSplitter {
  Q_OBJECT

 public:
  CQTabSplitSplitter(CQTabSplit *split);

  CQTabSplit *split() const { return split_; }

  QSplitterHandle *createHandle() override;

 private:
  CQTabSplit *split_ { nullptr };
};

class CQTabSplitSplitterHandle : public QSplitterHandle {
  Q_OBJECT

  Q_PROPERTY(int barSize READ barSize WRITE setBarSize)

 public:
  CQTabSplitSplitterHandle(Qt::Orientation orient, CQTabSplitSplitter *splitter);

  CQTabSplitSplitter *splitter() const { return splitter_; }

  int barSize() const { return barSize_; }
  void setBarSize(int i) { barSize_ = i; }

  void contextMenuEvent(QContextMenuEvent *e) override;

  void paintEvent(QPaintEvent *) override;

  bool event(QEvent *event) override;

  QSize sizeHint() const override;

 private slots:
  void tabSlot();
  void splitSlot();

 private:
  CQTabSplitSplitter *splitter_ { nullptr };
  int                 barSize_  { 8 };
  bool                hover_    { false };
};

//---

class CQTabSplitTabWidget : public QTabWidget {
  Q_OBJECT

 public:
  CQTabSplitTabWidget(CQTabSplit *split);

  CQTabSplit *split() const { return split_; }

  void contextMenuEvent(QContextMenuEvent *e);

 private slots:
  void hsplitSlot();
  void vsplitSlot();

 private:
  CQTabSplit *split_ { nullptr };
};

#endif
