#ifndef CQChartsKey_H
#define CQChartsKey_H

#include <CQChartsBoxObj.h>
#include <CBBox2D.h>
#include <QFont>
#include <QPointF>
#include <QSizeF>

#include <map>
#include <vector>

class CQChartsPlot;
class CQChartsKeyItem;
class CQPropertyTree;
class QPainter;

class CQChartsKey : public CQChartsBoxObj {
  Q_OBJECT

  Q_PROPERTY(bool    visible   READ isVisible   WRITE setVisible    )
  Q_PROPERTY(QString location  READ locationStr WRITE setLocationStr)
  Q_PROPERTY(bool    insideX   READ isInsideX   WRITE setInsideX    )
  Q_PROPERTY(bool    insideY   READ isInsideY   WRITE setInsideY    )
  Q_PROPERTY(int     spacing   READ spacing     WRITE setSpacing    )
  Q_PROPERTY(QColor  textColor READ textColor   WRITE setTextColor  )
  Q_PROPERTY(QFont   textFont  READ textFont    WRITE setTextFont   )

 public:
  enum Location {
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER_CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT
  };

 public:
  CQChartsKey(CQChartsPlot *plot);

  CQChartsPlot *plot() const { return plot_; }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; redraw(); }

  const Location &location() const { return location_; }
  void setLocation(const Location &l) { location_ = l; updatePosition(); }

  bool isInsideX() const { return insideX_; }
  void setInsideX(bool b) { insideX_ = b; updatePosition(); }

  bool isInsideY() const { return insideY_; }
  void setInsideY(bool b) { insideY_ = b; updatePosition(); }

  int spacing() const { return spacing_; }
  void setSpacing(int i) { spacing_ = i; updateLayout(); }

  const QColor &textColor() const { return textColor_; }
  void setTextColor(const QColor &v) { textColor_ = v; updateLayout(); }

  const QFont &textFont() const { return textFont_; }
  void setTextFont(const QFont &v) { textFont_ = v; updateLayout(); }

  QString locationStr() const;
  void setLocationStr(const QString &s);

  const CBBox2D &bbox() const { return bbox_; }
  void setBBox(const CBBox2D &v) { bbox_ = v; }

  void addProperties(CQPropertyTree *tree, const QString &path);

  void clearItems();

  void addItem(CQChartsKeyItem *item, int row, int col, int nrows=1, int ncols=1);

  int maxRow() const { return maxRow_; }
  int maxCol() const { return maxCol_; }

  void updatePosition();

  void invalidateLayout();

  const QPointF &position() const { return position_; }
  void setPosition(const QPointF &v) { position_ = v; }

  QSizeF calcSize();

  void redraw() override;

  void updateLayout();

  bool contains(const CPoint2D &p) const;

  CQChartsKeyItem *getItemAt(const CPoint2D &p) const;

  virtual bool mousePress(const CPoint2D &) { return false; }

  void draw(QPainter *p);

 private:
  void doLayout();

 private:
  struct Cell {
    double x      { 0 };
    double y      { 0 };
    double width  { 0 };
    double height { 0 };
  };

  typedef std::vector<CQChartsKeyItem *> Items;
  typedef std::map<int,Cell>             ColCell;
  typedef std::map<int,ColCell>          RowColCell;

  CQChartsPlot*   plot_        { nullptr };
  bool            visible_     { true };
  Location        location_    { Location::TOP_RIGHT };
  bool            insideX_     { true };
  bool            insideY_     { true };
  int             spacing_     { 2 };
  QColor          textColor_;
  QFont           textFont_;
  Items           items_;
  int             maxRow_      { 0 };
  int             maxCol_      { 0 };
  bool            needsLayout_ { false };
  QPointF         position_    { 0, 0 };
  QSizeF          size_;
  int             numRows_     { 0 };
  int             numCols_     { 0 };
  RowColCell      rowColCell_;
  mutable CBBox2D bbox_;
};

//------

class CQChartsKeyItem : public QObject {
  Q_OBJECT

 public:
  CQChartsKeyItem(CQChartsKey *key);

  virtual ~CQChartsKeyItem() { }

  virtual QSizeF size() const = 0;

  const CQChartsKey *key() const { return key_; }
  void setKey(CQChartsKey *p) { key_ = p; }

  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  int col() const { return col_; }
  void setCol(int i) { col_ = i; }

  int rowSpan() const { return rowSpan_; }
  void setRowSpan(int i) { rowSpan_ = i; }

  int colSpan() const { return colSpan_; }
  void setColSpan(int i) { colSpan_ = i; }

  const CBBox2D &bbox() const { return bbox_; }
  void setBBox(const CBBox2D &v) { bbox_ = v; }

  virtual bool mousePress(const CPoint2D &) { return false; }

  virtual void draw(QPainter *p, const CBBox2D &rect) = 0;

 protected:
  CQChartsKey*    key_     { nullptr };
  int             row_     { 0 };
  int             col_     { 0 };
  int             rowSpan_ { 1 };
  int             colSpan_ { 1 };
  mutable CBBox2D bbox_;
};

//---

class CQChartsPlot;

//---

class CQChartsKeyText : public CQChartsKeyItem {
  Q_OBJECT

 public:
  CQChartsKeyText(CQChartsPlot *plot, const QString &text);

  QSizeF size() const override;

  void draw(QPainter *p, const CBBox2D &rect) override;

  virtual QColor textColor() const;

 protected:
  CQChartsPlot *plot_ { nullptr };
  QString       text_;
};

//---

class CQChartsKeyColorBox : public CQChartsKeyItem {
  Q_OBJECT

 public:
  CQChartsKeyColorBox(CQChartsPlot *plot, int i, int n);

  QSizeF size() const override;

  void draw(QPainter *p, const CBBox2D &rect) override;

  virtual QColor fillColor  () const;
  virtual QColor borderColor() const;

 protected:
  CQChartsPlot *plot_ { nullptr };
  int           i_    { 0 };
  int           n_    { 0 };
};

#endif
