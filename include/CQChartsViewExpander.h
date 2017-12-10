#ifndef CQChartsViewExpander_H
#define CQChartsViewExpander_H

#include <QFrame>
#include <QPointer>

class CQChartsWindow;

class CQChartsViewExpander : public QFrame {
  Q_OBJECT

  Q_PROPERTY(Side side     READ side       WRITE setSide    )
  Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded)

  Q_ENUMS(Side)

 public:
  enum class Side {
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
  };

 public:
  CQChartsViewExpander(CQChartsWindow *window, QWidget *w, const Side &side=Side::RIGHT);

  const Side &side() const { return side_; }
  void setSide(const Side &side);

  bool isExpanded() const { return expanded_; }
  void setExpanded(bool b);

  void setMargins(int l, int b, int r, int t);

  bool isVertical() const;

  void updateGeometry();

  void mousePressEvent  (QMouseEvent *) override;
  void mouseMoveEvent   (QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;

  void paintEvent(QPaintEvent *) override;

 private slots:
  void leftSlot();
  void rightSlot();
  void topSlot();
  void bottomSlot();

 private:
  using WidgetP = QPointer<QWidget>;

  CQChartsWindow *window_   { nullptr };
  WidgetP         w_;
  Side            side_     { Side::RIGHT };
  int             l_        { 0 };
  int             b_        { 0 };
  int             r_        { 0 };
  int             t_        { 0 };
  bool            expanded_ { false };
  bool            pressed_  { false };
  QPoint          pressPos_;
  QPoint          movePos_;
};

#endif
