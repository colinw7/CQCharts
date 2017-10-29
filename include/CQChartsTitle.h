#ifndef CQChartsTitle_H
#define CQChartsTitle_H

#include <CQChartsTextBoxObj.h>
#include <CBBox2D.h>
#include <QPointF>
#include <QSizeF>
#include <vector>

class CQChartsPlot;
class CQPropertyViewTree;
class QPainter;

class CQChartsTitle : public CQChartsTextBoxObj {
  Q_OBJECT

  Q_PROPERTY(bool    visible  READ isVisible   WRITE setVisible    )
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

  //---

  void setMargin (int i) override { CQChartsBoxObj::setMargin (i); updatePosition(); }
  void setPadding(int i) override { CQChartsBoxObj::setPadding(i); updatePosition(); }

  void setText(const QString &s) override { CQChartsTextBoxObj::setText(s); updatePosition(); }
  void setFont(const QFont &f) override { CQChartsTextBoxObj::setFont(f); updatePosition(); }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; redraw(); }

  QString locationStr() const;
  void setLocationStr(const QString &s);

  const Location &location() const { return location_; }
  void setLocation(const Location &l) { location_ = l; updatePosition(); }

  bool isInside() const { return inside_; }
  void setInside(bool b) { inside_ = b; updatePosition(); }

  //---

  const CBBox2D &bbox() const { return bbox_; }
  void setBBox(const CBBox2D &b) { bbox_ = b; }

  //---

  void addProperties(CQPropertyViewTree *tree, const QString &path) override;

  //---

  const QPointF &position() const { return position_; }
  void setPosition(const QPointF &p) { position_ = p; }

  //---

  QSizeF calcSize();

  void updatePosition();

  //---

  void redrawBoxObj() override;

  void redraw();

  //---

  bool contains(const CPoint2D &p) const;

  virtual bool mousePress(const CPoint2D &) { return false; }

  //---

  void draw(QPainter *p);

 private:
  CQChartsPlot*   plot_     { nullptr };
  bool            visible_  { true };
  Location        location_ { Location::TOP};
  bool            inside_   { false };
  QPointF         position_ { 0, 0 };
  QSizeF          size_;
  mutable CBBox2D bbox_;
};

#endif
