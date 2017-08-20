#ifndef CQChartsKey_H
#define CQChartsKey_H

#include <CBBox2D.h>
#include <QObject>
#include <QColor>
#include <QPointF>
#include <QSizeF>
#include <vector>

class CQChartsPlot;
class CQChartsKeyItem;
class CQPropertyTree;
class QPainter;

class CQChartsKey : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool    displayed   READ isDisplayed WRITE setDisplayed  )
  Q_PROPERTY(bool    border      READ isBorder    WRITE setBorder     )
  Q_PROPERTY(QColor  background  READ background  WRITE setBackground )
  Q_PROPERTY(QColor  borderColor READ borderColor WRITE setBorderColor)
  Q_PROPERTY(QString location    READ locationStr WRITE setLocationStr)
  Q_PROPERTY(int     margin      READ margin      WRITE setMargin     )
  Q_PROPERTY(int     spacing     READ spacing     WRITE setSpacing    )

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

  bool isDisplayed() const { return displayed_; }
  void setDisplayed(bool b) { displayed_ = b; redraw(); }

  bool isBorder() const { return border_; }
  void setBorder(bool b) { border_ = b; redraw(); }

  const QColor &background() const { return background_; }
  void setBackground(const QColor &v) { background_ = v; redraw(); }

  const QColor &borderColor() const { return borderColor_; }
  void setBorderColor(const QColor &v) { borderColor_ = v; redraw(); }

  const Location &location() const { return location_; }
  void setLocation(const Location &l) { location_ = l; redraw(); }

  int margin() const { return margin_; }
  void setMargin(int i) { margin_ = i; redraw(true); }

  int spacing() const { return spacing_; }
  void setSpacing(int i) { spacing_ = i; redraw(true); }

  QString locationStr() const;
  void setLocationStr(const QString &s);

  const CBBox2D &bbox() const { return bbox_; }
  void setBBox(const CBBox2D &v) { bbox_ = v; }

  void addProperties(CQPropertyTree *tree, const QString &path);

  void clearItems();

  void addItem(CQChartsKeyItem *item, int row, int col, int nrows=1, int ncols=1);

  void invalidateLayout();

  const QPointF &position() const { return position_; }
  void setPosition(const QPointF &v) { position_ = v; }

  QSizeF calcSize();

  void redraw(bool layout=false);

  bool contains(const CPoint2D &p) const;

  CQChartsKeyItem *getItemAt(const CPoint2D &p) const;

  virtual void mousePress(const CPoint2D &) { }

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
  bool            displayed_   { true };
  bool            border_      { true };
  QColor          background_  { 255, 255, 255 };
  QColor          borderColor_ { 0, 0, 0 };
  Location        location_    { Location::TOP_RIGHT };
  int             margin_      { 2 };
  int             spacing_     { 2 };
  Items           items_;
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

  virtual void mousePress(const CPoint2D &) { }

  virtual void draw(QPainter *p, const CBBox2D &rect) = 0;

 private:
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

 protected:
  CQChartsPlot *plot_ { nullptr };
  int           i_    { 0 };
  int           n_    { 0 };
};

#endif
