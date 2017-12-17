#ifndef CQChartsKey_H
#define CQChartsKey_H

#include <CQChartsBoxObj.h>
#include <CQChartsGeom.h>
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

  Q_PROPERTY(bool          visible     READ isVisible    WRITE setVisible     )
  Q_PROPERTY(QString       location    READ locationStr  WRITE setLocationStr )
  Q_PROPERTY(QPointF       absPosition READ absPosition  WRITE setAbsPosition )
  Q_PROPERTY(bool          insideX     READ isInsideX    WRITE setInsideX     )
  Q_PROPERTY(bool          insideY     READ isInsideY    WRITE setInsideY     )
  Q_PROPERTY(int           spacing     READ spacing      WRITE setSpacing     )
  Q_PROPERTY(bool          horizontal  READ isHorizontal WRITE setHorizontal  )
  Q_PROPERTY(bool          above       READ isAbove      WRITE setAbove       )
  Q_PROPERTY(bool          flipped     READ isFlipped    WRITE setFlipped     )
  Q_PROPERTY(QString       textColor   READ textColorStr WRITE setTextColorStr)
  Q_PROPERTY(QFont         textFont    READ textFont     WRITE setTextFont    )
  Q_PROPERTY(Qt::Alignment textAlign   READ textAlign    WRITE setTextAlign   )

 public:
  enum LocationType {
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER_CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT,
    ABSOLUTE
  };

 public:
  CQChartsKey(CQChartsPlot *plot);
 ~CQChartsKey();

  CQChartsPlot *plot() const { return plot_; }

  //---

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; redraw(); }

  const LocationType &location() const { return location_.location; }
  void setLocation(const LocationType &l) { location_.location = l; updatePosition(); }

  QString locationStr() const;
  void setLocationStr(const QString &s);

  const QPointF &absPosition() const { return location_.absPosition; }
  void setAbsPosition(const QPointF &p) { location_.absPosition = p; updatePosition(); }

  bool isInsideX() const { return location_.insideX; }
  void setInsideX(bool b) { location_.insideX = b; updatePosition(); }

  bool isInsideY() const { return location_.insideY; }
  void setInsideY(bool b) { location_.insideY = b; updatePosition(); }

  int spacing() const { return spacing_; }
  void setSpacing(int i) { spacing_ = i; updateLayout(); }

  //---

  // text
  QString textColorStr() const;
  void setTextColorStr(const QString &str);

  QColor interpTextColor(int i, int n) const;

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

  const CQChartsGeom::BBox &bbox() const { return bbox_; }
  void setBBox(const CQChartsGeom::BBox &b) { bbox_ = b; }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void clearItems();

  void addItem(CQChartsKeyItem *item, int row, int col, int nrows=1, int ncols=1);

  int maxRow() const { return maxRow_; }
  int maxCol() const { return maxCol_; }

  void updatePosition();

  void updateLocation(const CQChartsGeom::BBox &bbox);

  void invalidateLayout();

  //---

  const QPointF &position() const { return position_; }
  void setPosition(const QPointF &p) { position_ = p; }

  //---

  QPointF absPlotPosition() const;
  void setAbsPlotPosition(const QPointF &p);

  //---

  QSizeF calcSize();

  //---

  void redraw();

  //---

  void updatePlotKey();

  void updateLayout();

  bool contains(const CQChartsGeom::Point &p) const;

  CQChartsKeyItem *getItemAt(const CQChartsGeom::Point &p) const;

  //---

  virtual bool mousePress  (const CQChartsGeom::Point &) { return false; }
  virtual bool mouseMove   (const CQChartsGeom::Point &);
  virtual void mouseRelease(const CQChartsGeom::Point &) { }

  virtual bool mouseDragPress  (const CQChartsGeom::Point &);
  virtual bool mouseDragMove   (const CQChartsGeom::Point &);
  virtual void mouseDragRelease(const CQChartsGeom::Point &);

  //---

  bool setInside(CQChartsKeyItem *item);

  //---

  void draw(QPainter *painter);

  //---

  QColor interpBgColor() const;

 private:
  void doLayout();

 private:
  struct Cell {
    double x      { 0 };
    double y      { 0 };
    double width  { 0 };
    double height { 0 };
  };

  struct Location {
    LocationType location    { LocationType::TOP_RIGHT };
    QPointF      absPosition;
    bool         insideX     { true };
    bool         insideY     { true };
  };

  using Items      = std::vector<CQChartsKeyItem*>;
  using ColCell    = std::map<int,Cell>;
  using RowColCell = std::map<int,ColCell>;

  bool                       visible_     { true };
  Location                   location_;
  int                        spacing_     { 2 };
  CQChartsPaletteColor       textColor_;
  QFont                      textFont_;
  Qt::Alignment              textAlign_   { Qt::AlignLeft | Qt::AlignVCenter };
  bool                       horizontal_  { false };
  bool                       above_       { true };
  bool                       flipped_     { false };
  Items                      items_;
  int                        maxRow_      { 0 };
  int                        maxCol_      { 0 };
  bool                       needsLayout_ { false };
  QPointF                    position_    { 0, 0 };
  QSizeF                     size_;
  int                        numRows_     { 0 };
  int                        numCols_     { 0 };
  RowColCell                 rowColCell_;
  mutable CQChartsGeom::BBox bbox_;
  CQChartsGeom::Point        dragPos_;
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

  const CQChartsGeom::BBox &bbox() const { return bbox_; }
  void setBBox(const CQChartsGeom::BBox &b) { bbox_ = b; }

  bool isInside() const { return inside_; }
  void setInside(bool b) { inside_ = b; }

  virtual bool mousePress(const CQChartsGeom::Point &) { return false; }
  virtual bool mouseMove (const CQChartsGeom::Point &) { return false; }

  virtual void draw(QPainter *painter, const CQChartsGeom::BBox &rect) = 0;

 protected:
  CQChartsKey*               key_     { nullptr };
  int                        row_     { 0 };
  int                        col_     { 0 };
  int                        rowSpan_ { 1 };
  int                        colSpan_ { 1 };
  bool                       inside_  { false };
  mutable CQChartsGeom::BBox bbox_;
};

//---

class CQChartsPlot;

//---

class CQChartsKeyText : public CQChartsKeyItem {
  Q_OBJECT

 public:
  CQChartsKeyText(CQChartsPlot *plot, const QString &text);

  QSizeF size() const override;

  virtual QColor interpTextColor(int i, int n) const;

  void draw(QPainter *painter, const CQChartsGeom::BBox &rect) override;

 protected:
  CQChartsPlot *plot_ { nullptr };
  QString       text_;
};

//---

class CQChartsKeyColorBox : public CQChartsKeyItem {
  Q_OBJECT

  Q_PROPERTY(double  cornerRadius READ cornerRadius   WRITE setCornerRadius  )
  Q_PROPERTY(QString borderColor  READ borderColorStr WRITE setBorderColorStr)

 public:
  CQChartsKeyColorBox(CQChartsPlot *plot, int i, int n);

  double cornerRadius() const { return cornerRadius_; }
  void setCornerRadius(double r) { cornerRadius_ = r; }

  QSizeF size() const override;

  void draw(QPainter *painter, const CQChartsGeom::BBox &rect) override;

  virtual QBrush fillBrush() const;

  virtual QString borderColorStr() const { return borderColor_.colorStr(); }
  virtual void setBorderColorStr(const QString &s) { borderColor_.setColorStr(s); }

  QColor interpBorderColor(int i, int n) const;

  bool isClickHide() const { return clickHide_; }
  void setClickHide(bool b) { clickHide_ = b; }

  bool mousePress(const CQChartsGeom::Point &) override;

 protected:
  CQChartsPlot*        plot_         { nullptr };
  int                  i_            { 0 };
  int                  n_            { 0 };
  double               cornerRadius_ { 0.0 };
  CQChartsPaletteColor borderColor_;
  bool                 clickHide_    { false };
};

#endif
