#ifndef CQFloatTip_H
#define CQFloatTip_H

#include <QFrame>
#include <QPointer>

class CQIconButton;

class QScrollArea;
class QLabel;
class QEvent;
class QTimerEvent;

class CQFloatTip : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString       text        READ text          WRITE setText       )
  Q_PROPERTY(bool          enabled     READ isEnabled     WRITE setEnabled    )
  Q_PROPERTY(Qt::Alignment align       READ align         WRITE setAlign      )
  Q_PROPERTY(bool          locked      READ isLocked      WRITE setLocked     )
  Q_PROPERTY(bool          followMouse READ isFollowMouse WRITE setFollowMouse)
  Q_PROPERTY(int           border      READ border        WRITE setBorder     )
  Q_PROPERTY(int           margin      READ margin        WRITE setMargin     )
  Q_PROPERTY(BarStyle      barStyle    READ barStyle      WRITE setBarStyle   )
  Q_PROPERTY(double        hideSecs    READ hideSecs      WRITE setHideSecs   )

  Q_ENUMS(BarStyle)

 public:
  enum BarStyle {
    BAR_NONE,
    BAR_LINES,
    BAR_GRADIENT
  };

 public:
  CQFloatTip(QWidget *widget=nullptr);

  virtual ~CQFloatTip();

  const QWidget *widget() const { return widget_.data(); }
  void setWidget(QWidget *w);

  const QString &text() const { return text_; }
  void setText(const QString &s);

  bool isEnabled() const { return enabled_; }
  void setEnabled(bool b);

  const Qt::Alignment &align() const { return align_; }
  void setAlign(const Qt::Alignment &v) { align_ = v; }

  bool isLocked() const { return locked_; }
  void setLocked(bool b);

  bool isFollowMouse() const { return followMouse_; }
  void setFollowMouse(bool b) { followMouse_ = b; }

  int border() const { return border_; }
  void setBorder(int i);

  int margin() const { return margin_; }
  void setMargin(int i);

  //! get/set bar style
  const BarStyle &barStyle() const { return barStyle_; }
  void setBarStyle(const BarStyle &v);

  double hideSecs() const { return hideSecs_; }
  void setHideSecs(double r) { hideSecs_ = r; }

  //---

  virtual void showTip(const QPoint &pos);
  virtual void hideTip();

  virtual void showQuery(const QPoint &pos);

  virtual bool isIgnoreKey(Qt::Key key, Qt::KeyboardModifiers modifiers) const;

  void place();

  void paintEvent(QPaintEvent *) override;

  void drawTitleBar(QPainter *painter);

  QSize sizeHint() const override;

  bool eventFilter(QObject *o, QEvent *e) override;

  void timerEvent(QTimerEvent *event) override;

 protected:
  void init();

  void startHideTimer();

  void stopTimer();

  void hideLater();

  void updateWidgetPalette();

  void setWidgetPalette(QWidget *w);

  void resizeFit();

 private slots:
  void hideSlot();

  void lockSlot(bool);

  void querySlot();

  void fontSlot();

  void focusChangedSlot(QWidget *, QWidget *newW);

 private:
  using QWidgetP = QPointer<QWidget>;

  // widget and text
  QWidgetP widget_; //!< parent widget for tip
  QString  text_;   //!< tip text

  bool enabled_ { true }; //!< is enabled

  // placement
  Qt::Alignment align_  { Qt::AlignRight | Qt::AlignBottom }; //!< align in parent widget
  int           x_      { -1 };                               //!< explicit x position
  int           y_      { -1 };                               //!< explicit y position
  bool          locked_ { false };                            //!< is locked visible

  bool followMouse_ { false }; //!< update on mouse move when locked

  // appearance
  int      border_   { 2 };         //!< inner border
  int      margin_   { 2 };         //!< outer margin
  BarStyle barStyle_ { BAR_LINES }; //!< bar style
  bool     isDark_   { false };     //!< is dark

  double hideSecs_ { 3.0 };

  // widgets
  QScrollArea*  scroll_      { nullptr }; //!< scroll area
  QLabel*       label_       { nullptr }; //!< label widget
  CQIconButton* lockButton_  { nullptr }; //!< lock widget
  CQIconButton* queryButton_ { nullptr }; //!< query widget

  // hide timer
  int hideTimer_ { 0 }; //!< hide delay timer

  // tip state
  QPoint gpos_; //!< last tip pos

  // drag state
  bool   dragging_   { false }; //!< is dragging
  QPoint dragOffset_;           //!< drag offset at press
  QPoint dragPos_;              //!< current drag position

  // inside state
  QPoint mousePos_;            //!< current mouse position
  bool   inside_    { false }; //!< is inside
};

#endif
