#ifndef CQToolTip_H
#define CQToolTip_H

#include <QWidget>
#include <QPointer>
#include <QMap>

class QVBoxLayout;

class CQToolTipIFace {
 public:
  CQToolTipIFace() { }

  virtual ~CQToolTipIFace() { }

  virtual bool canTip(const QPoint &) const { return true; }

  virtual QWidget *showWidget(const QPoint &pos) = 0;

  virtual void hideWidget() { }

  virtual bool updateWidget(const QPoint &) { return true; }

  virtual bool trackMouse() const { return false; }

  virtual double hideSecs() const { return -1; }

  virtual int    margin () const { return -1; }
  virtual double opacity() const { return -1; }

  virtual bool isTransparent() const { return false; }

  virtual Qt::Alignment alignment() const { return Qt::AlignLeft | Qt::AlignTop; }

  virtual bool outside() const { return false; }

  virtual bool isHideKey(int, Qt::KeyboardModifiers) { return true; }

  virtual QSize sizeHint() const { return QSize(); }
};

//---

class CQToolTipWidgetIFace : public CQToolTipIFace {
 public:
  CQToolTipWidgetIFace(QWidget *w) :
    w_(w) {
  }

  virtual QWidget *showWidget(const QPoint &) { return w_; }

 private:
  typedef QPointer<QWidget> QWidgetP;

  QWidgetP w_;
};

//---

#define CQToolTipInst CQToolTip::instance()

class CQToolTip : public QWidget {
  Q_OBJECT

 public:
  static CQToolTipIFace *getToolTip(QWidget *w);

  static void setToolTip(QWidget *parent, QWidget *tooltip);
  static void setToolTip(QWidget *parent, CQToolTipIFace *tooltip);

  static void unsetToolTip(QWidget *parent);

  static void release();

 protected:
  static CQToolTip *instance();

  CQToolTip();
 ~CQToolTip();

  void show(const QPoint &pos, CQToolTipIFace *tooltip, QWidget *parent);

 private:
  void showAtPos(const QPoint &pos);

  void enterEvent(QEvent *e);

  void paintEvent(QPaintEvent *);

  bool eventFilter(QObject *o, QEvent *e);

  void timerEvent(QTimerEvent *event);

  void hideLater();

  void startHideTimer();

  void updateSize();

  void updateOpacity(CQToolTipIFace *tooltip);

  QSize sizeHint() const;

  int calcMargin() const;

  QSize calcSize() const;

  QRect desktopRect(const QPoint &pos) const;

 private slots:
  void hideSlot();

 private:
  typedef QPointer<QWidget> QWidgetP;

  typedef QMap<QWidgetP,CQToolTipIFace *> WidgetMap;

  WidgetMap tooltips_;
  QWidgetP  tooltip_;
  QWidgetP  parent_;
  double    hideSecs_  { 3 };
  int       hideTimer_ { 0 };
  int       margin_    { 1 };
  double    opacity_   { 0.8 };
};

#endif
