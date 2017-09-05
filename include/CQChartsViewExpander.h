#ifndef CQChartsViewExpander_H
#define CQChartsViewExpander_H

#include <QFrame>

class CQChartsView;

class CQChartsViewExpander : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded)

 public:
  CQChartsViewExpander(CQChartsView *view);

  bool isExpanded() const { return expanded_; }
  void setExpanded(bool b) { expanded_ = b; update(); }

  void mousePressEvent  (QMouseEvent *);
  void mouseMoveEvent   (QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);

  void paintEvent(QPaintEvent *);

 private:
  CQChartsView *view_     { nullptr };
  bool          expanded_ { false };
  bool          pressed_  { false };
  QPoint        pressPos_;
  QPoint        movePos_;
};

#endif
