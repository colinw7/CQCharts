#ifndef CQScrollArea_H
#define CQScrollArea_H

#include <QFrame>

class QScrollBar;
class QPainter;

/*!
 * Area to contain a virtual scrollable widget.
 *
 * The actual widget is the same size as the scroll area
 * (unlike QScrollArea which uses a widget of any size).
 *
 * The widget needs to tell the scroll area the virtual widget size and the scroll bars
 * update the virtual widget offsets which are used in the widget's paint event to draw
 * the contents at the specified offset.
 *
 * This is more efficient for large widgets or widgets that can clip themselves.
 *
 * API:
 *   Use setXSize and setYSize to set virutal size
 *
 *   Use getXOffset and getYOffset when drawing into widget
 *
 *   Use setXSingleStep and setYSingleStep to set scroll step
 *
 *   Call handleWheelEvent in event override for QEvent::Wheel to scroll on wheel
 */
class CQScrollArea : public QFrame {
  Q_OBJECT

 public:
  CQScrollArea(QWidget *parent, QWidget *widget);
  CQScrollArea(QWidget *widget);

  virtual ~CQScrollArea() { }

  //! get/set child widget (can only be set once)
  QWidget *getWidget() const { return widget_; }
  void setWidget(QWidget *widget);

  //! show/hide horizontal/vertical scroll bar
  void showHBar(bool flag);
  void showVBar(bool flag);

  //! set corner widget
  void setCornerWidget(QWidget *w);

  //! get/set virtual widget size
  int getXSize() const { return x_size_; }
  void setXSize(int x_size);

  //! get/set virtual widget size
  int getYSize() const { return y_size_; }
  void setYSize(int y_size);

  //! get/set virtual widget x offset
  int getXOffset() const { return x_offset_; }
  void setXOffset(int x_offset);

  //! get/set virtual widget y offset
  void setYOffset(int y_offset);
  int getYOffset() const { return y_offset_; }

  //! set x/y scrollbar single step
  void setXSingleStep(int x);
  void setYSingleStep(int y);

  //! handle scroll directions
  void scrollUp   (bool page=false);
  void scrollDown (bool page=false);
  void scrollLeft (bool page=false);
  void scrollRight(bool page=false);

  //! ensure pixel position widget
  void ensureVisible(int x, int y, int xmargin=50, int ymargin=50);

  //! update scrollbars (called when virtual widget changes)
  void updateScrollbars();

  //! update contents
  virtual void updateContents() { }

  //! handle wheel event
  void handleWheelEvent(QWheelEvent *e);

 private slots:
  void hscrollSlot(int value);
  void vscrollSlot(int value);

 signals:
  void updateArea();

 private:
  void init();

 private:
  QWidget*    widget_   { nullptr };
  QScrollBar* hbar_     { nullptr };
  QScrollBar* vbar_     { nullptr };
  int         x_size_   { -1 };
  int         y_size_   { -1 };
  int         x_offset_ { 0 };
  int         y_offset_ { 0 };
};

#endif
