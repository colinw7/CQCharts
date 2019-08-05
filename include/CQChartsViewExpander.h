#ifndef CQChartsViewExpander_H
#define CQChartsViewExpander_H

#include <QFrame>
#include <QIcon>
#include <QPointer>

class QTimer;

/*!
 * \brief view expander widget
 * \ingroup Charts
 */
class CQChartsViewExpander : public QFrame {
  Q_OBJECT

  Q_PROPERTY(Side    side     READ side       WRITE setSide    )
  Q_PROPERTY(bool    expanded READ isExpanded WRITE setExpanded)
  Q_PROPERTY(bool    detached READ isDetached WRITE setDetached)
  Q_PROPERTY(QString title    READ title      WRITE setTitle   )
  Q_PROPERTY(QIcon   icon     READ icon       WRITE setIcon    )

  Q_ENUMS(Side)

 public:
  enum class Side {
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
  };

 private:
  enum class PressSide {
    NONE,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
  };

 public:
  CQChartsViewExpander(QWidget *parent, QWidget *w, const Side &side=Side::RIGHT);

  const Side &side() const { return side_; }
  void setSide(const Side &side);

  bool isExpanded() const { return expanded_; }
  void setExpanded(bool b);

  bool isDetached() const { return detached_; }
  void setDetached(bool b);

  const QString &title() const { return title_; }
  void setTitle(const QString &s) { title_ = s; update(); }

  const QIcon &icon() const { return icon_; }
  void setIcon(const QIcon &i) { icon_ = i; update(); }

  void setMargins(int l, int b, int r, int t);

  bool isVertical() const;

  void updateGeometry();

  void mousePressEvent  (QMouseEvent *) override;
  void mouseMoveEvent   (QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

 private:
  void drawTitleLines(QPainter *p, const QRect &r);

  QRect handleRect() const;

  PressSide posToPressSide(const QPoint &pos);

  int maxSize() const;

 private slots:
  void leftSlot();
  void rightSlot();
  void topSlot();
  void bottomSlot();

  void detachSlot();
  void detachLaterSlot();

 private:
  using WidgetP = QPointer<QWidget>;

  QWidget*        parent_      { nullptr };
  WidgetP         w_;
  Side            side_        { Side::RIGHT };
  QString         title_;
  QIcon           icon_;
  int             l_           { 0 };
  int             b_           { 0 };
  int             r_           { 0 };
  int             t_           { 0 };
  Qt::WindowFlags windowFlags_ { 0 };
  QTimer*         detachTimer_ { nullptr };
  bool            expanded_    { false };
  bool            pressed_     { false };
  QPoint          pressPos_;
  PressSide       pressSide_   { PressSide::NONE };
  QPoint          movePos_;
  bool            detached_    { false };
  int             border_      { 4 };
  int             titleHeight_ { 12 };
};

#endif
