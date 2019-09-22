#ifndef CQChartsKey_H
#define CQChartsKey_H

#include <CQChartsBoxObj.h>
#include <CQChartsObjData.h>
#include <CQChartsKeyLocation.h>
#include <CQChartsOptLength.h>
#include <CQChartsGeom.h>
#include <CQChartsTypes.h>
#include <QSizeF>

#include <map>
#include <vector>

class CQChartsPlot;
class CQChartsKeyItem;
class CQChartsEditHandles;
class CQChartsPaintDevice;
class CQPropertyViewModel;
class QScrollBar;

//------

/*!
 * \brief Base Key class
 * \ingroup Charts
 */
class CQChartsKey : public CQChartsBoxObj,
 public CQChartsObjTextData      <CQChartsKey>,
 public CQChartsObjHeaderTextData<CQChartsKey> {
  Q_OBJECT

  Q_PROPERTY(bool                     horizontal    READ isHorizontal  WRITE setHorizontal   )
  Q_PROPERTY(bool                     autoHide      READ isAutoHide    WRITE setAutoHide     )
  Q_PROPERTY(bool                     clipped       READ isClipped     WRITE setClipped      )
  Q_PROPERTY(bool                     above         READ isAbove       WRITE setAbove        )
  Q_PROPERTY(CQChartsKeyLocation      location      READ location      WRITE setLocation     )
  Q_PROPERTY(QString                  header        READ headerStr     WRITE setHeaderStr    )
  Q_PROPERTY(double                   hiddenAlpha   READ hiddenAlpha   WRITE setHiddenAlpha  )
  Q_PROPERTY(int                      maxRows       READ maxRows       WRITE setMaxRows      )
  Q_PROPERTY(bool                     interactive   READ isInteractive WRITE setInteractive  )
  Q_PROPERTY(CQChartsKeyPressBehavior pressBehavior READ pressBehavior WRITE setPressBehavior)

  CQCHARTS_TEXT_DATA_PROPERTIES

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Header,header)

 public:
  CQChartsKey(CQChartsView *view);
  CQChartsKey(CQChartsPlot *plot);

  virtual ~CQChartsKey();

  QString calcId() const override;

  void setSelected(bool b) override;

  //---

  // horizontal
  bool isHorizontal() const { return horizontal_; }
  void setHorizontal(bool b) { horizontal_ = b; updateKeyItems(); }

  //---

  // auto hide
  bool isAutoHide() const { return autoHide_; }
  void setAutoHide(bool b) { autoHide_ = b; updatePosition(); }

  //---

  // clip
  bool isClipped() const { return clipped_; }
  void setClipped(bool b) { clipped_ = b; redraw(); }

  //---

  // above plot
  bool isAbove() const { return above_; }
  void setAbove(bool b) { above_ = b; redraw(); }

  //---

  // location
  const CQChartsKeyLocation &location() const { return location_; }
  void setLocation(const CQChartsKeyLocation &l);

  //---

  // header
  const QString &headerStr() const { return header_; }
  void setHeaderStr(const QString &s) { header_ = s; updateLayout(); }

  // TODO header text properties

  //---

  // alpha when associated object hidden
  double hiddenAlpha() const { return hiddenAlpha_; }
  void setHiddenAlpha(double r) { hiddenAlpha_ = r; }

  //---

  // max rows
  int maxRows() const { return maxRows_; }
  void setMaxRows(int i) { maxRows_ = i; }

  //---

  // interactive
  bool isInteractive() const { return interactive_; }
  void setInteractive(bool b) { interactive_ = b; }

  //---

  // press behavior
  const CQChartsKeyPressBehavior &pressBehavior() const { return pressBehavior_; }
  void setPressBehavior(const CQChartsKeyPressBehavior &v) { pressBehavior_ = v; }

  //---

  virtual void updatePosition(bool /*queued*/=true) { }

  virtual void updateLayout() { }

  virtual void updateKeyItems() { }

  //---

  virtual bool selectPress  (const CQChartsGeom::Point &, CQChartsSelMod) = 0;
  virtual bool selectMove   (const CQChartsGeom::Point &) = 0;
  virtual bool selectRelease(const CQChartsGeom::Point &) = 0;

  virtual bool editPress  (const CQChartsGeom::Point &) = 0;
  virtual bool editMove   (const CQChartsGeom::Point &) = 0;
  virtual bool editMotion (const CQChartsGeom::Point &) = 0;
  virtual bool editRelease(const CQChartsGeom::Point &) = 0;

  //---

  virtual bool contains(const CQChartsGeom::Point &p) const = 0;

  //---

  virtual void redraw(bool /*queued*/=true) = 0;

  virtual void draw(CQChartsPaintDevice *device) const = 0;

  //---

  CQChartsEditHandles *editHandles() { return editHandles_; }

 protected:
  void init();

 protected:
  struct ScrollData {
    bool              scrolled     { false };   //!< scrolled
    int               hpos         { 0 };       //!< horizontal scroll position
    int               vpos         { 0 };       //!< vertical scroll position
    double            pixelBarSize { 13 };      //!< scroll bar pixel size
    CQChartsOptLength width;                    //!< fixed width
    CQChartsOptLength height;                   //!< fixed height
    QScrollBar*       hbar         { nullptr }; //!< horizontal scroll bar
    QScrollBar*       vbar         { nullptr }; //!< vertical scroll bar
  };

  bool                     horizontal_     { false };   //!< is laid out horizontally
  bool                     above_          { true };    //!< draw above view/plot
  CQChartsKeyLocation      location_;                   //!< key location
  QString                  header_;                     //!< header
  bool                     autoHide_       { true };    //!< auto hide if too big
  bool                     clipped_        { true };    //!< clipped to parent
  bool                     interactive_    { true };    //!< is interactive
  double                   hiddenAlpha_    { 0.3 };     //!< alpha for hidden item
  int                      maxRows_        { 100 };     //!< max rows
  CQChartsKeyPressBehavior pressBehavior_;              //!< press behavior
  mutable ScrollData       scrollData_;                 //!< scrollbar data
  CQChartsEditHandles*     editHandles_    { nullptr }; //!< edit handles
};

//------

/*!
 * \brief View Key class
 * \ingroup Charts
 */
class CQChartsViewKey : public CQChartsKey {
  Q_OBJECT

 public:
  CQChartsViewKey(CQChartsView *view);

 ~CQChartsViewKey();

  //---

  const CQChartsGeom::BBox &bbox() const { return wbbox_; }
  void setBBox(const CQChartsGeom::BBox &b) { wbbox_ = b; }

  //---

  void updatePosition(bool queued=true) override;

  void updateLayout() override;

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc="") override;

  bool contains(const CQChartsGeom::Point &p) const override;

  bool selectPress  (const CQChartsGeom::Point &w, CQChartsSelMod selMod) override;
  bool selectMove   (const CQChartsGeom::Point &w) override;
  bool selectRelease(const CQChartsGeom::Point &w) override;

  bool editPress  (const CQChartsGeom::Point &w) override;
  bool editMove   (const CQChartsGeom::Point &w) override;
  bool editMotion (const CQChartsGeom::Point &w) override;
  bool editRelease(const CQChartsGeom::Point &w) override;

  virtual void doShow  (int i, CQChartsSelMod selMod);
  virtual void doSelect(int i, CQChartsSelMod selMod);

  //---

  void draw(CQChartsPaintDevice *device) const override;

  void drawEditHandles(QPainter *painter) const;

  void drawCheckBox(CQChartsPaintDevice *device, double x, double y, int bs, bool checked) const;

  //---

  void redraw(bool queued=true) override;

 private:
  void doLayout();

 private:
  using BBox  = CQChartsGeom::BBox;
  using Rects = std::vector<BBox>;

  QPointF       pposition_ { 0, 0 }; //!< pixel position
  QPointF       wposition_ { 0, 0 }; //!< view position
  QSizeF        size_;               //!< pixel size
  mutable BBox  wbbox_;              //!< view bounding box
  mutable BBox  pbbox_;              //!< view pixel bounding box
  mutable Rects prects_;             //!< plot key item rects
};

//------

/*!
 * \brief Plot Key class
 * \ingroup Charts
 */
class CQChartsPlotKey : public CQChartsKey {
  Q_OBJECT

  Q_PROPERTY(bool              flipped           READ isFlipped         WRITE setFlipped          )
  Q_PROPERTY(bool              insideX           READ isInsideX         WRITE setInsideX          )
  Q_PROPERTY(bool              insideY           READ isInsideY         WRITE setInsideY          )
  Q_PROPERTY(QPointF           absolutePosition  READ absolutePosition  WRITE setAbsolutePosition )
  Q_PROPERTY(QRectF            absoluteRectangle READ absoluteRectangle WRITE setAbsoluteRectangle)
  Q_PROPERTY(int               spacing           READ spacing           WRITE setSpacing          )
  Q_PROPERTY(CQChartsOptLength scrollWidth       READ scrollWidth       WRITE setScrollWidth      )
  Q_PROPERTY(CQChartsOptLength scrollHeight      READ scrollHeight      WRITE setScrollHeight     )

 public:
  CQChartsPlotKey(CQChartsPlot *plot);
 ~CQChartsPlotKey();

  CQChartsPlot *plot() const { return plot_; }

  //---

  bool isEmpty() const;

  bool isVisibleAndNonEmpty() const;

  bool isOverlayVisible() const;

  //---

  // flipped
  bool isFlipped() const { return flipped_; }
  void setFlipped(bool b);

  //---

  // inside plot in x direction
  bool isInsideX() const { return locationData_.insideX; }
  void setInsideX(bool b) { locationData_.insideX = b; updatePosition(); }

  //---

  // inside plot in y direction
  bool isInsideY() const { return locationData_.insideY; }
  void setInsideY(bool b) { locationData_.insideY = b; updatePosition(); }

  //---

  // absolute position
  const QPointF &absolutePosition() const { return locationData_.absolutePosition; }
  void setAbsolutePosition(const QPointF &p) {
    locationData_.absolutePosition = p; updatePosition(); }

  // absolute rect
  const QRectF &absoluteRectangle() const { return locationData_.absoluteRectangle; }
  void setAbsoluteRectangle(const QRectF &r) {
    locationData_.absoluteRectangle = r; updatePosition(); }

  //---

  // item spacing
  int spacing() const { return spacing_; }
  void setSpacing(int i) { spacing_ = i; updateLayout(); }

  //---

  // scroll width
  const CQChartsOptLength &scrollWidth() const { return scrollData_.width; }
  void setScrollWidth(const CQChartsOptLength &l) { scrollData_.width = l; }

  // scroll height
  const CQChartsOptLength &scrollHeight() const { return scrollData_.height; }
  void setScrollHeight(const CQChartsOptLength &l) { scrollData_.height = l; }

  //---

  const CQChartsGeom::BBox &bbox() const { return wbbox_; }
  void setBBox(const CQChartsGeom::BBox &b) { wbbox_ = b; }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc="") override;

  void clearItems();

  void addItem(CQChartsKeyItem *item, int row, int col, int nrows=1, int ncols=1);

  int maxRow() const { return maxRow_; }
  int maxCol() const { return maxCol_; }

  void updatePosition(bool queued=true) override;

  void updateLocation(const CQChartsGeom::BBox &bbox);

  void invalidateLayout();

  //---

  const QPointF &position() const { return position_; }
  void setPosition(const QPointF &p) { position_ = p; }

  //---

  QPointF absolutePlotPosition() const;
  void setAbsolutePlotPosition(const QPointF &p);

  QRectF absolutePlotRect() const;
  void setAbsolutePlotRect(const QRectF &p);

  //---

  int calcNumRows() const;

  QSizeF calcSize();

  //---

  bool contains(const CQChartsGeom::Point &p) const override;

  void boxDataInvalidate() override { redraw(); }

  void redraw(bool queued=true) override;

  //---

  void updateKeyItems() override;

  void updateLayout() override;

  CQChartsKeyItem *getItemAt(const CQChartsGeom::Point &p) const;

  //---

  bool tipText(const CQChartsGeom::Point &p, QString &tip) const;

  //---

  bool selectPress  (const CQChartsGeom::Point &w, CQChartsSelMod selMod) override;
  bool selectMove   (const CQChartsGeom::Point &w) override;
  bool selectRelease(const CQChartsGeom::Point &w) override;

  bool editPress  (const CQChartsGeom::Point &w) override;
  bool editMove   (const CQChartsGeom::Point &w) override;
  bool editMotion (const CQChartsGeom::Point &w) override;
  bool editRelease(const CQChartsGeom::Point &w) override;

  virtual void editMoveBy(const QPointF &d);

  //---

  bool setInsideItem(CQChartsKeyItem *item);

  //---

  void draw(CQChartsPaintDevice *device) const override;

  void drawEditHandles(QPainter *painter) const;

  //---

  bool isPixelWidthExceeded () const { return layoutData_.pixelWidthExceeded ; }
  bool isPixelHeightExceeded() const { return layoutData_.pixelHeightExceeded; }

  //---

  QColor interpBgColor() const;

 private slots:
  void hscrollSlot(int);
  void vscrollSlot(int);

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
    QPointF absolutePosition;           //!< absolute position
    QRectF  absoluteRectangle;          //!< absolute rectangle
    bool    insideX           { true }; //!< inside plot in x
    bool    insideY           { true }; //!< inside plot in y
  };

  // layout data (all data in window units)
  struct LayoutData {
    QSizeF size;                          //!< size (scrolled)
    QSizeF fullSize;                      //!< full size (not scrolled)
    double headerWidth         { 0.0 };   //!< header width
    double headerHeight        { 0.0 };   //!< header height
    bool   hscrolled           { false }; //!< has horizontal scroll
    bool   vscrolled           { false }; //!< has vertical scroll
    double scrollAreaWidth     { 0.0 };   //!< scrolled area width
    double scrollAreaHeight    { 0.0 };   //!< scrolled area height
    double hbarHeight          { 0.0 };   //!< horizontal scroll height (if displayed);
    double vbarWidth           { 0.0 };   //!< vertical scroll width (if displayed)
    bool   pixelWidthExceeded  { false }; //!< pixel max width exceeded
    bool   pixelHeightExceeded { false }; //!< pixel max height exceeded
  };

  using Items      = std::vector<CQChartsKeyItem*>;
  using ColCell    = std::map<int,Cell>;
  using RowColCell = std::map<int,ColCell>;
  using RowHeights = std::map<int,double>;
  using ColWidths  = std::map<int,double>;

  Location                   locationData_;              //!< key location data
  int                        spacing_       { 2 };       //!< key item spacing
  bool                       flipped_       { false };   //!< key order flipped
  Items                      items_;                     //!< key items
  int                        maxRow_        { 0 };       //!< maximum key row
  int                        maxCol_        { 0 };       //!< maximum key column
  bool                       needsLayout_   { true };    //!< needs layout
  QPointF                    position_      { 0, 0 };    //!< explicit position
  LayoutData                 layoutData_;                //!< layout data
  int                        numRows_       { 0 };       //!< number of rows
  int                        numCols_       { 0 };       //!< number of columns
  mutable CQChartsGeom::BBox wbbox_;                     //!< window bounding box
  mutable RowHeights         rowHeights_;                //!< row heights
  mutable ColWidths          colWidths_;                 //!< column widths
  mutable RowColCell         rowColCell_;                //!< cells (per row/col)
  mutable double             xs_            { 0.0 };     //!< x spacing (pixels)
  mutable double             ys_            { 0.0 };     //!< y spacing (pixels)
  mutable double             xm_            { 0.0 };     //!< x margin (pixels)
  mutable double             ym_            { 0.0 };     //!< y margin (pixels)
  mutable double             sx_            { 0.0 };     //!< scroll x offset
  mutable double             sy_            { 0.0 };     //!< scroll y offset
};

//------

/*!
 * \brief Key Item base class
 * \ingroup Charts
 */
class CQChartsKeyItem : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool clickable READ isClickable WRITE setClickable)

 public:
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsKeyItem(CQChartsPlotKey *key, const ColorInd &ic);

  virtual ~CQChartsKeyItem() { }

  virtual QSizeF size() const = 0;

  virtual QString id() const { return ""; }

  const CQChartsPlotKey *key() const { return key_; }
  void setKey(CQChartsPlotKey *p) { key_ = p; }

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

  bool isClickable() const { return clickable_; }
  void setClickable(bool b) { clickable_ = b; }

  //---

  virtual bool tipText(const CQChartsGeom::Point &p, QString &tip) const;

  //---

  virtual bool selectPress(const CQChartsGeom::Point &, CQChartsSelMod);
  virtual bool selectMove (const CQChartsGeom::Point &);

  virtual void doShow  (CQChartsSelMod selMod);
  virtual void doSelect(CQChartsSelMod selMod);

  //---

  virtual void draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect) const = 0;

 protected:
  CQChartsPlotKey*           key_       { nullptr }; //!< parent key
  ColorInd                   ic_;                    //!< color index
  int                        row_       { 0 };       //!< row
  int                        col_       { 0 };       //!< col
  int                        rowSpan_   { 1 };       //!< row span
  int                        colSpan_   { 1 };       //!< col span
  bool                       inside_    { false };   //!< is inside
  mutable CQChartsGeom::BBox bbox_;                  //!< bounding box
  bool                       clickable_ { false };   //!< clickable
};

//---

class CQChartsPlot;

//---

/*!
 * \brief Key Text Item base class
 * \ingroup Charts
 */
class CQChartsKeyText : public CQChartsKeyItem {
  Q_OBJECT

 public:
  CQChartsKeyText(CQChartsPlot *plot, const QString &text, const ColorInd &ic);

  CQChartsPlot *plot() const { return plot_; }

  const QString &text() const { return text_; }
  void setText(const QString &s) { text_ = s; }

  QSizeF size() const override;

  virtual QColor interpTextColor(const ColorInd &ind) const;

  void draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect) const override;

 protected:
  CQChartsPlot* plot_ { nullptr };
  QString       text_;
};

//---

/*!
 * \brief Key Color Box Item base class
 * \ingroup Charts
 */
class CQChartsKeyColorBox : public CQChartsKeyItem {
  Q_OBJECT

  Q_PROPERTY(CQChartsLength cornerRadius READ cornerRadius WRITE setCornerRadius)
  Q_PROPERTY(CQChartsColor  strokeColor  READ strokeColor  WRITE setStrokeColor )

 public:
  using RangeValue = CQChartsGeom::RangeValue;

 public:
  CQChartsKeyColorBox(CQChartsPlot *plot, const ColorInd &is, const ColorInd &ig,
                      const ColorInd &iv, const RangeValue &xv=RangeValue(),
                      const RangeValue &yv=RangeValue());

  CQChartsPlot *plot() const { return plot_; }

  const CQChartsLength &cornerRadius() const { return boxData_.shape().stroke().cornerSize(); }
  void setCornerRadius(const CQChartsLength &r) { boxData_.shape().stroke().setCornerSize(r); }

  QSizeF size() const override;

  void draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect) const override;

  virtual QBrush fillBrush() const;

  virtual const CQChartsColor &strokeColor() const { return boxData_.shape().stroke().color(); }
  virtual void setStrokeColor(const CQChartsColor &c) { boxData_.shape().stroke().setColor(c); }

  QColor interpStrokeColor(const ColorInd &ic) const;

  ColorInd calcColorInd() const;

  virtual double xColorValue(bool relative=true) const;
  virtual double yColorValue(bool relative=true) const;

 protected:
  CQChartsPlot*   plot_     { nullptr }; //!< parent plot
  CQChartsBoxData boxData_;              //!< box data
  ColorInd        is_;                   //!< group index
  ColorInd        ig_;                   //!< group index
  ColorInd        iv_;                   //!< number of groups
  RangeValue      xv_;                   //!< x value
  RangeValue      yv_;                   //!< y value
};

#endif
