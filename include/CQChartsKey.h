#ifndef CQChartsKey_H
#define CQChartsKey_H

#include <CQChartsBoxObj.h>
#include <CQChartsObjData.h>
#include <CQChartsKeyLocation.h>
#include <CQChartsPaletteName.h>
#include <CQChartsOptLength.h>
#include <CQChartsGeom.h>
#include <CQChartsTypes.h>
#include <CQChartsEditHandles.h>

#include <map>
#include <vector>

class CQChartsPlot;
class CQChartsKeyItem;
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

  Q_PROPERTY(Qt::Orientation          orientation   READ orientation   WRITE setOrientation  )
  Q_PROPERTY(bool                     autoHide      READ isAutoHide    WRITE setAutoHide     )
  Q_PROPERTY(bool                     clipped       READ isClipped     WRITE setClipped      )
  Q_PROPERTY(bool                     above         READ isAbove       WRITE setAbove        )
  Q_PROPERTY(CQChartsKeyLocation      location      READ location      WRITE setLocation     )
  Q_PROPERTY(QString                  header        READ headerStr     WRITE setHeaderStr    )
  Q_PROPERTY(CQChartsAlpha            hiddenAlpha   READ hiddenAlpha   WRITE setHiddenAlpha  )
  Q_PROPERTY(int                      columns       READ columns       WRITE setColumns      )
  Q_PROPERTY(int                      maxRows       READ maxRows       WRITE setMaxRows      )
  Q_PROPERTY(bool                     interactive   READ isInteractive WRITE setInteractive  )
  Q_PROPERTY(CQChartsKeyPressBehavior pressBehavior READ pressBehavior WRITE setPressBehavior)

  CQCHARTS_TEXT_DATA_PROPERTIES

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Header, header)

 public:
  using PropertyModel = CQPropertyViewModel;
  using SelMod        = CQChartsSelMod;
  using Location      = CQChartsKeyLocation;
  using KeyBehavior   = CQChartsKeyPressBehavior;
  using OptLength     = CQChartsOptLength;
  using Font          = CQChartsFont;
  using Alpha         = CQChartsAlpha;
  using ColorInd      = CQChartsUtil::ColorInd;

 public:
  CQChartsKey(View *view);
  CQChartsKey(Plot *plot);

  virtual ~CQChartsKey();

  QString calcId() const override;

  void setSelected(bool b) override;

  //---

  virtual const BBox &bbox() const = 0;

  //--

  //! get/set orientation
  const Qt::Orientation &orientation() const { return orientation_; }
  void setOrientation(const Qt::Orientation &orient);

  bool isHorizontal() const { return orientation() == Qt::Horizontal; }

  //---

  //! get/set auto hide
  bool isAutoHide() const { return autoHide_; }
  void setAutoHide(bool b);

  //---

  //! get/set clipped
  bool isClipped() const { return clipped_; }
  void setClipped(bool b);

  //---

  //! get/set above plot
  bool isAbove() const { return above_; }
  void setAbove(bool b);

  //---

  //! get/set location
  const Location &location() const { return location_; }
  void setLocation(const Location &l);

  //---

  //! get/set header text
  const QString &headerStr() const { return header_; }
  void setHeaderStr(const QString &s);

  //! TODO header text properties

  //---

  //! get/set alpha when associated object hidden
  const Alpha &hiddenAlpha() const { return hiddenAlpha_; }
  void setHiddenAlpha(const Alpha &a);

  //---

  //! get/set columns
  int columns() const { return columns_; }
  void setColumns(int i);

  //---

  //! get/set max rows
  int maxRows() const { return maxRows_; }
  void setMaxRows(int i);

  //---

  //! get/set interactive
  bool isInteractive() const { return interactive_; }
  void setInteractive(bool b);

  //---

  //! get/set press behavior
  const KeyBehavior &pressBehavior() const { return pressBehavior_; }
  void setPressBehavior(const KeyBehavior &b);

  //---

  virtual void updatePosition(bool /*queued*/=true) { }

  virtual void updateLayout() { }

  virtual void updateKeyItems() { }

  //---

  void nextRowCol(int &row, int &col) const;

  //---

  virtual void redraw(bool /*queued*/=true) = 0;

  virtual void draw(PaintDevice *device) const = 0;

 protected:
  void init();

 protected:
  struct ScrollData {
    bool        scrolled     { false };   //!< scrolled
    int         hpos         { 0 };       //!< horizontal scroll position
    int         vpos         { 0 };       //!< vertical scroll position
    double      pixelBarSize { 13 };      //!< scroll bar pixel size
    OptLength   width;                    //!< fixed width
    OptLength   height;                   //!< fixed height
    QScrollBar* hbar         { nullptr }; //!< horizontal scroll bar
    QScrollBar* vbar         { nullptr }; //!< vertical scroll bar
  };

  Qt::Orientation    orientation_    { Qt::Vertical }; //!< layout direction
  bool               above_          { true };         //!< draw above view/plot
  Location           location_;                        //!< key location
  QString            header_;                          //!< header
  bool               autoHide_       { true };         //!< auto hide if too big
  bool               clipped_        { true };         //!< clipped to parent
  bool               interactive_    { true };         //!< is interactive
  Alpha              hiddenAlpha_    { 0.3 };          //!< alpha for hidden item
  int                columns_        { 1 };            //!< columns
  int                maxRows_        { 100 };          //!< max rows
  KeyBehavior        pressBehavior_;                   //!< press behavior
  mutable ScrollData scrollData_;                      //!< scrollbar data
};

//------

/*!
 * \brief View Key class
 * \ingroup Charts
 */
class CQChartsViewKey : public CQChartsKey {
  Q_OBJECT

 public:
  CQChartsViewKey(View *view);

 ~CQChartsViewKey();

  //---

  const BBox &bbox() const override { return wbbox_; }
  void setBBox(const BBox &b) { wbbox_ = b; }

  //---

  void updatePosition(bool queued=true) override;

  void updateLayout() override;

  //---

  void addProperties(PropertyModel *model, const QString &path, const QString &desc="") override;

  bool contains(const Point &p) const override;

  //---

  // implement select interface
  bool selectPress(const Point &w, SelMod selMod) override;

  //---

  // Implement edit interface
  bool editPress (const Point &w) override;
  bool editMove  (const Point &w) override;
  bool editMotion(const Point &w) override;

  //---

  virtual void doShow  (int i, SelMod selMod);
  virtual void doSelect(int i, SelMod selMod);

  //---

  void draw(PaintDevice *device) const override;

  //---

  void drawEditHandles(QPainter *painter) const override;

  void setEditHandlesBBox() const;

  //---

  void drawCheckBox(PaintDevice *device, double x, double y, int bs, bool checked) const;

  //---

  void redraw(bool queued=true) override;

 private:
  void doLayout();

 private:
  using Rects = std::vector<BBox>;

  int           numPlots_  { 0 };    //!< number of plots
  Point         pposition_ { 0, 0 }; //!< pixel position
  Point         wposition_ { 0, 0 }; //!< view position
  Size          size_;               //!< pixel size
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

  Q_PROPERTY(bool flipped READ isFlipped WRITE setFlipped)
  Q_PROPERTY(bool insideX READ isInsideX WRITE setInsideX)
  Q_PROPERTY(bool insideY READ isInsideY WRITE setInsideY)

  Q_PROPERTY(CQChartsGeom::Point absolutePosition
             READ absolutePosition  WRITE setAbsolutePosition )
  Q_PROPERTY(CQChartsGeom::BBox  absoluteRectangle
             READ absoluteRectangle WRITE setAbsoluteRectangle)

  Q_PROPERTY(double            spacing      READ spacing      WRITE setSpacing     )
  Q_PROPERTY(CQChartsOptLength scrollWidth  READ scrollWidth  WRITE setScrollWidth )
  Q_PROPERTY(CQChartsOptLength scrollHeight READ scrollHeight WRITE setScrollHeight)

 public:
  using KeyItem  = CQChartsKeyItem;
  using KeyItems = std::vector<KeyItem*>;

 public:
  CQChartsPlotKey(Plot *plot);
 ~CQChartsPlotKey();

  Plot *plot() const { return plot_; }

  Plot *drawPlot() const;

  //---

  bool isEmpty() const;

  bool isVisibleAndNonEmpty() const;

  bool isOverlayVisible() const;

  //---

  //! get/set is flipped (horizontally)
  bool isFlipped() const { return flipped_; }
  void setFlipped(bool b);

  //---

  //! get/set inside plot in x direction
  bool isInsideX() const { return locationData_.insideX; }
  void setInsideX(bool b) { locationData_.insideX = b; updatePosition(); }

  //---

  //! get/set inside plot in y direction
  bool isInsideY() const { return locationData_.insideY; }
  void setInsideY(bool b) { locationData_.insideY = b; updatePosition(); }

  //---

  //! get/set absolute position
  const Point &absolutePosition() const { return locationData_.absolutePosition; }
  void setAbsolutePosition(const Point &p) {
    locationData_.absolutePosition = p; updatePosition(); }

  //! get/set absolute rect
  const BBox &absoluteRectangle() const { return locationData_.absoluteRectangle; }
  void setAbsoluteRectangle(const BBox &bbox) {
    locationData_.absoluteRectangle = bbox; updatePosition(); }

  //---

  //! get/set item spacing (pixels)
  double spacing() const { return spacing_; }
  void setSpacing(double i) { spacing_ = i; updateLayout(); }

  //---

  //! get/set scroll width
  const OptLength &scrollWidth() const { return scrollData_.width; }
  void setScrollWidth(const OptLength &l) { scrollData_.width = l; }

  //! get/set scroll height
  const OptLength &scrollHeight() const { return scrollData_.height; }
  void setScrollHeight(const OptLength &l) { scrollData_.height = l; }

  //---

  //! get/set bbox
  const BBox &bbox() const override { return wbbox_; }
  void setBBox(const BBox &b) { wbbox_ = b; }

  //---

  void addProperties(PropertyModel *model, const QString &path, const QString &desc="") override;

  void clearItems();

  void addItem(KeyItem *item, int row, int col, int nrows=1, int ncols=1);

  int maxRow() const { return maxRow_; }
  int maxCol() const { return maxCol_; }

  const KeyItems &items() const { return items_; }

  //---

  void updatePosition(bool queued=true) override;

  void updatePlotLocation();
  void updateLocation(const BBox &bbox);

  void updatePositionAndLayout(bool queued=true);

  //---

  void invalidateLayout(bool reset=false);

  //---

  const Point &position() const { return position_; }
  void setPosition(const Point &p) { position_ = p; }

  //---

  Point absolutePlotPosition() const;
  void setAbsolutePlotPosition(const Point &p);

  BBox absolutePlotRectangle() const;
  void setAbsolutePlotRectangle(const BBox &bbox);

  //---

  int calcNumRows() const;

  Size calcSize();

  //---

  bool contains(const Point &p) const override;

  void boxObjInvalidate() override;

  void redraw(bool queued=true) override;

  //---

  void updateKeyItems() override;

  void updateLayout() override;

  KeyItem *getItemAt(const Point &p) const;

  //---

  bool tipText(const Point &p, QString &tip) const;

  //---

  bool selectMove(const Point &w) override;

  //---

  // Implement edit interface
  bool editPress (const Point &w) override;
  bool editMove  (const Point &w) override;
  bool editMotion(const Point &w) override;

  void editMoveBy(const Point &d) override;

  bool isEditResize() const override;

  //---

  bool setInsideItem(KeyItem *item);

  //---

  void draw(PaintDevice *device) const override;

  void drawEditHandles(QPainter *painter) const override;

  void setEditHandlesBBox() const;

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

  struct LocationData {
    Point absolutePosition;           //!< absolute position
    BBox  absoluteRectangle;          //!< absolute rectangle
    bool  insideX           { true }; //!< inside plot in x
    bool  insideY           { true }; //!< inside plot in y
  };

  // layout data (all data in window units)
  struct LayoutData {
    Size   size;                          //!< size (scrolled)
    Size   fullSize;                      //!< full size (not scrolled)
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

  using ColCell    = std::map<int, Cell>;
  using RowColCell = std::map<int, ColCell>;
  using RowHeights = std::map<int, double>;
  using ColWidths  = std::map<int, double>;

  struct Margin {
    double xl { 0.0 }; //!< x left margin (pixels)
    double xr { 0.0 }; //!< x right margin (pixels)
    double yt { 0.0 }; //!< y top margin (pixels)
    double yb { 0.0 }; //!< y bottom margin (pixels)
  };

  LocationData       locationData_;            //!< key location data
  double             spacing_       { 2 };     //!< key item spacing (pixels)
  bool               flipped_       { false }; //!< key order flipped
  KeyItems           items_;                   //!< key items
  int                maxRow_        { 0 };     //!< maximum key row
  int                maxCol_        { 0 };     //!< maximum key column
  bool               needsLayout_   { true };  //!< needs layout
  Point              position_      { 0, 0 };  //!< explicit position
  LayoutData         layoutData_;              //!< layout data
  int                numRows_       { 0 };     //!< number of rows
  int                numCols_       { 0 };     //!< number of columns
  mutable BBox       wbbox_;                   //!< window bounding box
  mutable RowHeights rowHeights_;              //!< row heights
  mutable ColWidths  colWidths_;               //!< column widths
  mutable RowColCell rowColCell_;              //!< cells (per row/col)
  mutable double     xs_            { 0.0 };   //!< x spacing (pixels)
  mutable double     ys_            { 0.0 };   //!< y spacing (pixels)
  mutable Margin     pmargin_;                 //!< margin (pixels)
  mutable Margin     ppadding_;                //!< padding (pixels)
  mutable double     sx_            { 0.0 };   //!< scroll x offset
  mutable double     sy_            { 0.0 };   //!< scroll y offset
};

//------

/*!
 * \brief Plot Key which shows unique values in column
 * \ingroup Charts
 */
class CQChartsColumnKey : public CQChartsPlotKey {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn column READ column WRITE setColumn)

 public:
  CQChartsColumnKey(Plot *plot);

  const CQChartsColumn &column() const { return column_; }
  void setColumn(const CQChartsColumn &c);

  void updatePosition(bool /*queued*/=true) override;

  void updateLayout() override;

  void updateItems();

  void addProperties(PropertyModel *model, const QString &path, const QString &desc="") override;

 private:
  CQChartsColumn column_;
};

//------

class CQChartsKeyItemGroup;

/*!
 * \brief Key Item base class
 * \ingroup Charts
 */
class CQChartsKeyItem : public QObject, public CQChartsSelectableIFace {
  Q_OBJECT

  Q_PROPERTY(int  rowSpan   READ rowSpan     WRITE setRowSpan)
  Q_PROPERTY(int  colSpan   READ colSpan     WRITE setColSpan)
  Q_PROPERTY(bool clickable READ isClickable WRITE setClickable)

 public:
  using Plot        = CQChartsPlot;
  using PlotKey     = CQChartsPlotKey;
  using ItemGroup   = CQChartsKeyItemGroup;
  using SelMod      = CQChartsSelMod;
  using PenBrush    = CQChartsPenBrush;
  using BrushData   = CQChartsBrushData;
  using PenData     = CQChartsPenData;
  using PaintDevice = CQChartsPaintDevice;
  using Alpha       = CQChartsAlpha;
  using ColorInd    = CQChartsUtil::ColorInd;

  using BBox  = CQChartsGeom::BBox;
  using Size  = CQChartsGeom::Size;
  using Point = CQChartsGeom::Point;

 public:
  CQChartsKeyItem(PlotKey *key, const ColorInd &ic);

  virtual ~CQChartsKeyItem() { }

  virtual Size size() const = 0;

  //! get/set item id
  virtual QString id() const { return id_; }
  virtual void setId(const QString &id) { id_ = id; }

  //! get/set parent plot key
  const PlotKey *key() const { return key_; }
  virtual void setKey(PlotKey *key) { key_ = key; }

  const ItemGroup *group() const { return group_; }
  void setGroup(ItemGroup *g) { group_ = g; }

  //---

  //! get/set associated color index
  const ColorInd &colorIndex() const { return ic_; }
  void setColorIndex(const ColorInd &i) { ic_ = i; }

  virtual ColorInd calcColorInd() const { return colorIndex(); }

  virtual ColorInd setIndex() const { return calcColorInd(); }

  //---

  //! get/set row
  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  //! get/set column
  int col() const { return col_; }
  void setCol(int i) { col_ = i; }

  //! get/set row span
  int rowSpan() const { return rowSpan_; }
  void setRowSpan(int i) { rowSpan_ = i; }

  //! get/set column span
  int colSpan() const { return colSpan_; }
  void setColSpan(int i) { colSpan_ = i; }

  //---

  //! get/set bounding box
  virtual const BBox &bbox() const { return bbox_; }
  void setBBox(const BBox &b) { bbox_ = b; }

  //---

  //! get/set is inside
  virtual bool isInside() const { return inside_; }
  void setInside(bool b) { inside_ = b; }

  //---

  // get/set clickable and is clicked
  virtual bool isClickable() const { return clickable_; }
  void setClickable(bool b) { clickable_ = b; }

  virtual bool isClicked() const;

  //---

  virtual bool tipText(const Point &p, QString &tip) const;

  //---

  // implement select interface
  bool selectPress(const Point &, SelMod) override;
  bool selectMove (const Point &) override;

  //---

  virtual void doShow  (SelMod selMod);
  virtual void doSelect(SelMod selMod);

  //---

  virtual void adjustFillColor(QColor &c) const;

  //---

  virtual bool isSetHidden() const;
  virtual void setSetHidden(bool b);

  //---

  virtual QVariant drawValue() const = 0;

  virtual void draw(PaintDevice *device, const BBox &rect) const = 0;

 protected:
  PlotKey*     key_       { nullptr }; //!< parent key
  QString      id_;                    //!< id
  ItemGroup*   group_     { nullptr }; //!< associated group
  ColorInd     ic_;                    //!< color index
  int          row_       { 0 };       //!< row
  int          col_       { 0 };       //!< col
  int          rowSpan_   { 1 };       //!< row span
  int          colSpan_   { 1 };       //!< col span
  bool         inside_    { false };   //!< is inside
  mutable BBox bbox_;                  //!< bounding box
  bool         clickable_ { false };   //!< clickable
};

//---

class CQChartsPlot;

//---

/*!
 * \brief Key Item Group class
 * \ingroup Charts
 */
class CQChartsKeyItemGroup : public CQChartsKeyItem {
  Q_OBJECT

 public:
  using KeyItem  = CQChartsKeyItem;
  using KeyItems = std::vector<KeyItem *>;

 public:
  CQChartsKeyItemGroup(Plot *plot);
  CQChartsKeyItemGroup(PlotKey *key);

  virtual ~CQChartsKeyItemGroup();

  Plot *plot() const { return plot_; }

  void setKey(PlotKey *p) override;

  const KeyItems &items() const { return items_; }

  //---

  void addRowItems(KeyItem *litem, KeyItem *ritem);

  void addItem(KeyItem *item);

  void removeItem(KeyItem *item, bool keep=false);

  //---

  Size size() const override;

  //---

  bool tipText(const Point &p, QString &tip) const override;

  //---

  // implement select interface
  bool selectPress(const Point &, SelMod) override;
  bool selectMove (const Point &) override;

  //---

  void doShow  (SelMod selMod) override;
  void doSelect(SelMod selMod) override;

  //---

  QVariant drawValue() const override { return QVariant(); }

  void draw(PaintDevice *device, const BBox &rect) const override;

 protected:
  Plot*    plot_ { nullptr };
  KeyItems items_;
};

//---

/*!
 * \brief Key Text Item base class
 * \ingroup Charts
 */
class CQChartsTextKeyItem : public CQChartsKeyItem {
  Q_OBJECT

  Q_PROPERTY(QString text READ text WRITE setText)

 public:
  CQChartsTextKeyItem(Plot *plot, const QString &text, const ColorInd &ic);
  CQChartsTextKeyItem(PlotKey *key, const QString &text, const ColorInd &ic);

  Plot *plot() const { return plot_; }

  const QString &text() const { return text_; }
  void setText(const QString &s) { text_ = s; }

  Size size() const override;

  virtual QColor interpTextColor(const ColorInd &ind) const;

  QVariant drawValue() const override { return QVariant(text_); }

  void draw(PaintDevice *device, const BBox &rect) const override;

 protected:
  Plot*   plot_ { nullptr };
  QString text_;
};

//---

/*!
 * \brief Key Color Box Item base class
 * \ingroup Charts
 */
class CQChartsColorBoxKeyItem : public CQChartsKeyItem {
  Q_OBJECT

  Q_PROPERTY(CQChartsLength cornerRadius READ cornerRadius WRITE setCornerRadius)
  Q_PROPERTY(CQChartsColor  strokeColor  READ strokeColor  WRITE setStrokeColor )

 public:
  using Length     = CQChartsLength;
  using Color      = CQChartsColor;
  using Alpha      = CQChartsAlpha;
  using RangeValue = CQChartsGeom::RangeValue;

 public:
  CQChartsColorBoxKeyItem(Plot *plot, const ColorInd &is, const ColorInd &ig,
                          const ColorInd &iv, const RangeValue &xv=RangeValue(),
                          const RangeValue &yv=RangeValue());
  CQChartsColorBoxKeyItem(PlotKey *key, const ColorInd &is, const ColorInd &ig,
                          const ColorInd &iv, const RangeValue &xv=RangeValue(),
                          const RangeValue &yv=RangeValue());

  Plot *plot() const { return plot_; }

  const Length &cornerRadius() const { return boxData_.shape().stroke().cornerSize(); }
  void setCornerRadius(const Length &r) { boxData_.shape().stroke().setCornerSize(r); }

  Size size() const override;

  virtual QBrush fillBrush() const;
  virtual QPen strokePen() const;

  virtual const Color &strokeColor() const { return boxData_.shape().stroke().color(); }
  virtual void setStrokeColor(const Color &c) { boxData_.shape().stroke().setColor(c); }

  QColor interpStrokeColor(const ColorInd &ic) const;

  ColorInd calcColorInd() const override;

  virtual double xColorValue(bool relative=true) const;
  virtual double yColorValue(bool relative=true) const;

  const Color &color() const { return color_; }
  void setColor(const Color &color) { color_ = color; }

  const QVariant &value() const { return value_; }
  void setValue(const QVariant &v) { value_ = v; }

  //---

  // implement select interface
  bool selectPress(const Point &w, SelMod selMod) override;

  //---

  QVariant drawValue() const override;

  void draw(PaintDevice *device, const BBox &rect) const override;

 protected:
  using BoxData = CQChartsBoxData;

  Plot*      plot_     { nullptr }; //!< parent plot
  BoxData    boxData_;              //!< box data
  ColorInd   is_;                   //!< group index
  ColorInd   ig_;                   //!< group index
  ColorInd   iv_;                   //!< number of groups
  RangeValue xv_;                   //!< x value
  RangeValue yv_;                   //!< y value
  Color      color_;                //!< custom color
  QVariant   value_;                //!< associated value
};

//---

/*!
 * \brief Key Line Item base class
 * \ingroup Charts
 */
class CQChartsLineKeyItem : public CQChartsKeyItem {
  Q_OBJECT

 public:
  using SymbolData = CQChartsSymbolData;
  using RangeValue = CQChartsGeom::RangeValue;

 public:
  CQChartsLineKeyItem(Plot *plot, const ColorInd &is, const ColorInd &ig);
  CQChartsLineKeyItem(PlotKey *key, const ColorInd &is, const ColorInd &ig);

  Plot *plot() const { return plot_; }

  Size size() const override;

  //---

  const SymbolData &symbolData() const { return symbolData_; }
  void setSymbolData(const SymbolData &symbolData) { symbolData_ = symbolData; }

  //---

  const QVariant &value() const { return value_; }
  void setValue(const QVariant &v) { value_ = v; }

  //---

  // implement select interface
  bool selectPress(const Point &w, SelMod selMod) override;

  //---

  QVariant drawValue() const override { return value_; }

  void draw(PaintDevice *device, const BBox &rect) const override;

 protected:
  Plot*      plot_ { nullptr }; //!< parent plot
  ColorInd   is_   { 0 };       //!< set color index
  ColorInd   ig_   { 0 };       //!< group color index
  QVariant   value_;            //!< associated value
  SymbolData symbolData_;       //!< symbol data
};

//---

/*!
 * \brief Plot Gradient Key Item
 * \ingroup Charts
 */
class CQChartsGradientKeyItem : public CQChartsKeyItem {
  Q_OBJECT

  Q_PROPERTY(double              minValue READ minValue  WRITE setMinValue)
  Q_PROPERTY(double              maxValue READ maxValue  WRITE setMaxValue)
  Q_PROPERTY(bool                integer  READ isInteger WRITE setInteger )
  Q_PROPERTY(CQChartsPaletteName palette  READ palette   WRITE setPalette )

 public:
  using PaletteName = CQChartsPaletteName;

 public:
  CQChartsGradientKeyItem(Plot *plot);
  CQChartsGradientKeyItem(PlotKey *key);

  //---

  double minValue() const { return minValue_; }
  void setMinValue(double r) { minValue_ = r; }

  double maxValue() const { return maxValue_; }
  void setMaxValue(double r) { maxValue_ = r; }

  bool isInteger() const { return integer_; }
  void setInteger(bool b) { integer_ = b; }

  const PaletteName &palette() const { return palette_; }
  void setPalette(const PaletteName &n) { palette_ = n; }

  //---

  Size size() const override;

  QVariant drawValue() const override { return QVariant(); } // TODO

  void draw(PaintDevice *device, const BBox &rect) const override;

 private:
  void calcLabels(QStringList &labels) const;

 private:
  Plot*       plot_     { nullptr };
  double      minValue_ { 0 };
  double      maxValue_ { 100 };
  bool        integer_  { false };
  PaletteName palette_;
};

#endif
