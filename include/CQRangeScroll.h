#ifndef CQRangeScroll_H
#define CQRangeScroll_H

#include <QFrame>

class CQRangeScrollCanvas;
class CQRangeScrollHandle;
class QScrollBar;

class CQRangeScroll : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QColor rangeColor READ rangeColor WRITE setRangeColor)
  Q_PROPERTY(double rangeAlpha READ rangeAlpha WRITE setRangeAlpha)

 public:
  CQRangeScroll(QWidget *parent=nullptr, Qt::Orientation orient=Qt::Horizontal);
  CQRangeScroll(Qt::Orientation orient, QWidget *parent=nullptr);

 ~CQRangeScroll() { }

  Qt::Orientation orientation() const { return orientation_; }

  const QColor &rangeColor() const { return rangeColor_; }
  void setRangeColor(const QColor &c) { rangeColor_ = c; }

  double rangeAlpha() const { return rangeAlpha_; }
  void setRangeAlpha(double a) { rangeAlpha_ = a; }

  CQRangeScrollCanvas *canvas() const { return canvas_; }

  void setRange(double min, double max);

  double min() const { return min_; }
  void setMin(double r) { min_ = r; }

  double max() const { return max_; }
  void setMax(double r) { max_ = r; }

  double pos() const { return pos_; }
  void setPos(double r, bool update=true);

  double len() const { return len_; }
  void setLen(double r, bool update=true);

  int margin() const { return margin_; }

  virtual void drawBackground(QPainter *);

  void updateScroll();

  void emitWindowChanged();

  int posToPixel(double pos) const;

  double pixelToPos(int p) const;

  void paintEvent(QPaintEvent *) override;

  void resizeEvent(QResizeEvent *) override;

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 private:
  void init();

 signals:
  void windowChanged();

 private slots:
  void scrollSlot(int);

 private:
  Qt::Orientation      orientation_ { Qt::Horizontal };
  QColor               rangeColor_  { 100,100,200 };
  double               rangeAlpha_  { 0.5 };
  CQRangeScrollCanvas *canvas_      { nullptr };
  QScrollBar          *scroll_      { nullptr };
  double               min_         { 0 };
  double               max_         { 1 };
  double               pos_         { 0 };
  double               len_         { 1 };
  int                  margin_      { 12 };
};

//------

class CQRangeScrollCanvas : public QFrame {
  Q_OBJECT

 public:
  CQRangeScrollCanvas(CQRangeScroll *scroll);

  void updateHandles();

  void paintEvent(QPaintEvent *) override;

  void resizeEvent(QResizeEvent *) override;

 public slots:
  void startHandleSlot(double);
  void endHandleSlot(double);

 private:
  CQRangeScroll       *scroll_      { nullptr };
  CQRangeScrollHandle *startHandle_ { nullptr };
  CQRangeScrollHandle *endHandle_   { nullptr };
};

//------

class CQRangeScrollHandle : public QFrame {
  Q_OBJECT

 public:
  CQRangeScrollHandle(CQRangeScroll *scroll);

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

 signals:
  void valueChanged(double);

 private:
  CQRangeScroll *scroll_ { nullptr };
  bool           pressed_ { false };
  QPoint         pressPos_;
  QPoint         mousePos_;
};

#endif
