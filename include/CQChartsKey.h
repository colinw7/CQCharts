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
class CQPropertyViewModel;
class QPainter;

class CQChartsKey : public CQChartsBoxObj {
  Q_OBJECT

  Q_PROPERTY(bool          visible    READ isVisible    WRITE setVisible    )
  Q_PROPERTY(QString       location   READ locationStr  WRITE setLocationStr)
  Q_PROPERTY(bool          insideX    READ isInsideX    WRITE setInsideX    )
  Q_PROPERTY(bool          insideY    READ isInsideY    WRITE setInsideY    )
  Q_PROPERTY(int           spacing    READ spacing      WRITE setSpacing    )
  Q_PROPERTY(QColor        textColor  READ textColor    WRITE setTextColor  )
  Q_PROPERTY(QFont         textFont   READ textFont     WRITE setTextFont   )
  Q_PROPERTY(Qt::Alignment textAlign  READ textAlign    WRITE setTextAlign  )
  Q_PROPERTY(bool          horizontal READ isHorizontal WRITE setHorizontal )
  Q_PROPERTY(bool          above      READ isAbove      WRITE setAbove      )
  Q_PROPERTY(bool          flipped    READ isFlipped    WRITE setFlipped    )

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
 ~CQChartsKey();

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

  //---

  // text
  const QColor &textColor() const { return textColor_; }
  void setTextColor(const QColor &c) { textColor_ = c; updateLayout(); }

  const QFont &textFont() const { return textFont_; }
  void setTextFont(const QFont &f) { textFont_ = f; updateLayout(); }

  Qt::Alignment textAlign() const { return textAlign_; }
  void setTextAlign(const Qt::Alignment &a) { textAlign_ = a; }

  //---

  bool isHorizontal() const { return horizontal_; }
  void setHorizontal(bool b) { horizontal_ = b; updatePlotKey(); }

  bool isAbove() const { return above_; }
  void setAbove(bool b) { above_ = b; redraw(); }

  bool isFlipped() const { return flipped_; }
  void setFlipped(bool b);

  QString locationStr() const;
  void setLocationStr(const QString &s);

  const CBBox2D &bbox() const { return bbox_; }
  void setBBox(const CBBox2D &b) { bbox_ = b; }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void clearItems();

  void addItem(CQChartsKeyItem *item, int row, int col, int nrows=1, int ncols=1);

  int maxRow() const { return maxRow_; }
  int maxCol() const { return maxCol_; }

  void updatePosition();

  void invalidateLayout();

  //---

  const QPointF &position() const { return position_; }
  void setPosition(const QPointF &p) { position_ = p; }

  //---

  QSizeF calcSize();

  //---

  void redrawBoxObj() override;

  void redraw();

  //---

  void updatePlotKey();

  void updateLayout();

  bool contains(const CPoint2D &p) const;

  CQChartsKeyItem *getItemAt(const CPoint2D &p) const;

  //---

  virtual bool mousePress(const CPoint2D &) { return false; }
  virtual bool mouseMove (const CPoint2D &) { return true; }

  //---

  bool setInside(CQChartsKeyItem *item);

  //---

  void draw(QPainter *p);

  //---

  QColor bgColor() const;

 private:
  void doLayout();

 private:
  struct Cell {
    double x      { 0 };
    double y      { 0 };
    double width  { 0 };
    double height { 0 };
  };

  using Items      = std::vector<CQChartsKeyItem*>;
  using ColCell    = std::map<int,Cell>;
  using RowColCell = std::map<int,ColCell>;

  CQChartsPlot*   plot_        { nullptr };
  bool            visible_     { true };
  Location        location_    { Location::TOP_RIGHT };
  bool            insideX_     { true };
  bool            insideY_     { true };
  int             spacing_     { 2 };
  QColor          textColor_;
  QFont           textFont_;
  Qt::Alignment   textAlign_   { Qt::AlignLeft | Qt::AlignVCenter };
  bool            horizontal_  { false };
  bool            above_       { true };
  bool            flipped_     { false };
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
  void setBBox(const CBBox2D &b) { bbox_ = b; }

  bool isInside() const { return inside_; }
  void setInside(bool b) { inside_ = b; }

  virtual bool mousePress(const CPoint2D &) { return false; }
  virtual bool mouseMove (const CPoint2D &) { return false; }

  virtual void draw(QPainter *p, const CBBox2D &rect) = 0;

 protected:
  CQChartsKey*    key_     { nullptr };
  int             row_     { 0 };
  int             col_     { 0 };
  int             rowSpan_ { 1 };
  int             colSpan_ { 1 };
  bool            inside_  { false };
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

  Q_PROPERTY(double cornerRadius READ cornerRadius WRITE setCornerRadius)
  Q_PROPERTY(QColor borderColor  READ borderColor  WRITE setBorderColor )

 public:
  CQChartsKeyColorBox(CQChartsPlot *plot, int i, int n);

  double cornerRadius() const { return cornerRadius_; }
  void setCornerRadius(double r) { cornerRadius_ = r; }

  QSizeF size() const override;

  void draw(QPainter *p, const CBBox2D &rect) override;

  virtual QBrush fillBrush() const;

  virtual QColor borderColor() const { return borderColor_; }
  virtual void setBorderColor(const QColor &c) { borderColor_ = c; }

 protected:
  CQChartsPlot *plot_         { nullptr };
  int           i_            { 0 };
  int           n_            { 0 };
  double        cornerRadius_ { 0.0 };
  QColor        borderColor_  { Qt::black };
};

#endif
