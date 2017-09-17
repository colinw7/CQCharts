#ifndef CQChartsTitle_H
#define CQChartsTitle_H

#include <CQChartsBoxObj.h>
#include <CBBox2D.h>
#include <QFont>
#include <QPointF>
#include <QSizeF>
#include <vector>

class CQChartsPlot;
class CQPropertyView;
class QPainter;

class CQChartsTitle : public CQChartsBoxObj {
  Q_OBJECT

  Q_PROPERTY(bool    visible  READ isVisible   WRITE setVisible    )
  Q_PROPERTY(QString text     READ text        WRITE setText       )
  Q_PROPERTY(QFont   font     READ font        WRITE setFont       )
  Q_PROPERTY(QColor  color    READ color       WRITE setColor      )
  Q_PROPERTY(QString location READ locationStr WRITE setLocationStr)
  Q_PROPERTY(bool    inside   READ isInside    WRITE setInside     )

 public:
  enum Location {
    TOP,
    CENTER,
    BOTTOM
  };

 public:
  CQChartsTitle(CQChartsPlot *plot);

  CQChartsPlot *plot() const { return plot_; }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; redraw(); }

  void setMargin(int i) override { CQChartsBoxObj::setMargin(i); updatePosition(); }

  void setPadding(int i) override { CQChartsBoxObj::setPadding(i); updatePosition(); }

  const QString &text() const { return text_; }
  void setText(const QString &s) { text_ = s; updatePosition(); }

  const QFont &font() const { return font_; }
  void setFont(const QFont &f) { font_ = f; updatePosition(); }

  const QColor &color() const { return color_; }
  void setColor(const QColor &c) { color_ = c; redraw(); }

  QString locationStr() const;
  void setLocationStr(const QString &s);

  const Location &location() const { return location_; }
  void setLocation(const Location &l) { location_ = l; updatePosition(); }

  bool isInside() const { return inside_; }
  void setInside(bool b) { inside_ = b; updatePosition(); }

  const CBBox2D &bbox() const { return bbox_; }
  void setBBox(const CBBox2D &b) { bbox_ = b; }

  void addProperties(CQPropertyView *tree, const QString &path);

  const QPointF &position() const { return position_; }
  void setPosition(const QPointF &p) { position_ = p; }

  QSizeF calcSize();

  void updatePosition();

  void redraw() override;

  bool contains(const CPoint2D &p) const;

  virtual bool mousePress(const CPoint2D &) { return false; }

  void draw(QPainter *p);

 private:
  CQChartsPlot*   plot_     { nullptr };
  bool            visible_  { true };
  QString         text_;
  QFont           font_;
  QColor          color_    { 0, 0, 0 };
  Location        location_ { Location::TOP};
  bool            inside_   { false };
  QPointF         position_ { 0, 0 };
  QSizeF          size_;
  mutable CBBox2D bbox_;
};

#endif
