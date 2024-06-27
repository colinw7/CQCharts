#ifndef CQChartsTablePlot_H
#define CQChartsTablePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CQChartsColumnNum.h>
#include <CQSummaryModel.h>

#include <QPointer>

class QScrollBar;

//---

/*!
 * \brief Table plot type
 * \ingroup Charts
 */
class CQChartsTablePlotType : public CQChartsPlotType {
 public:
  CQChartsTablePlotType();

  QString name() const override { return "table"; }
  QString desc() const override { return "Table"; }

  void addParameters() override;

  bool hasObjs() const override { return true; }

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  QString description() const override;

  bool canProbe() const override { return false; }

  //---

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsTablePlot;
class CQChartsTable;
class CQChartsTableHHeaderObj;
class CQChartsTableVHeaderObj;
class CQChartsTableRowNumberObj;
class CQChartsTableCellObj;

class CQIntegerSpin;

//---

/*!
 * \brief Table Plot
 * \ingroup Charts
 */
class CQChartsTablePlot : public CQChartsPlot {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumns columns READ columns WRITE setColumns)

  // mode
  Q_PROPERTY(Mode mode    READ mode    WRITE setMode   )
  Q_PROPERTY(int  maxRows READ maxRows WRITE setMaxRows)

  // sort
  Q_PROPERTY(CQChartsColumnNum sortColumn READ sortColumn WRITE setSortColumn)
  Q_PROPERTY(int               sortRole   READ sortRole   WRITE setSortRole  )
  Q_PROPERTY(Qt::SortOrder     sortOrder  READ sortOrder  WRITE setSortOrder )

  // paging
  Q_PROPERTY(int pageSize    READ pageSize    WRITE setPageSize   )
  Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage)

  // row numbers (for ROWS mode)
  Q_PROPERTY(QString rowNums READ rowNumsStr WRITE setRowNumsStr)

  // horizontal header
  Q_PROPERTY(bool          hheaderVisible   READ isHHeaderVisible   WRITE setHHeaderVisible   )
  Q_PROPERTY(bool          hheaderFormatted READ isHHeaderFormatted WRITE setHHeaderFormatted)
  Q_PROPERTY(CQChartsColor hheaderColor     READ hheaderColor       WRITE setHHeaderColor    )
  Q_PROPERTY(CQChartsFont  hheaderFont      READ hheaderFont        WRITE setHHeaderFont     )

  // vertical header
  Q_PROPERTY(bool           vheaderVisible   READ isVHeaderVisible   WRITE setVHeaderVisible  )
  Q_PROPERTY(bool           vheaderFormatted READ isVHeaderFormatted WRITE setVHeaderFormatted)
  Q_PROPERTY(CQChartsColor  vheaderColor     READ vheaderColor       WRITE setVHeaderColor    )
  Q_PROPERTY(CQChartsFont   vheaderFont      READ vheaderFont        WRITE setVHeaderFont     )
  Q_PROPERTY(CQChartsLength vheaderWidth     READ vheaderWidth       WRITE setVHeaderWidth    )

  // rows number column
  Q_PROPERTY(bool          rowColumnVisible READ isRowColumnVisible WRITE setRowColumnVisible)
  Q_PROPERTY(CQChartsColor rowColumnColor   READ rowColumnColor     WRITE setRowColumnColor  )
  Q_PROPERTY(CQChartsFont  rowColumnFont    READ rowColumnFont      WRITE setRowColumnFont   )

  // grid
  Q_PROPERTY(CQChartsColor  gridColor READ gridColor WRITE setGridColor)
  Q_PROPERTY(CQChartsLength gridWidth READ gridWidth WRITE setGridWidth)

  // cells
  Q_PROPERTY(CQChartsColor cellColor  READ cellColor  WRITE setCellColor )
  Q_PROPERTY(int           cellMargin READ cellMargin WRITE setCellMargin)
  Q_PROPERTY(int           barXMargin READ barXMargin WRITE setBarXMargin)
  Q_PROPERTY(int           barYMargin READ barYMargin WRITE setBarYMargin)

  //options
  Q_PROPERTY(CQChartsColor insideColor   READ insideColor   WRITE setInsideColor  )
  Q_PROPERTY(CQChartsColor selectedColor READ selectedColor WRITE setSelectedColor)

  // options
  Q_PROPERTY(double indent      READ indent       WRITE setIndent    )
  Q_PROPERTY(bool   followView  READ isFollowView WRITE setFollowView)

  Q_ENUMS(Mode)

 public:
  enum class Mode {
    NORMAL = static_cast<int>(CQSummaryModel::Mode::NORMAL),
    RANDOM = static_cast<int>(CQSummaryModel::Mode::RANDOM),
    SORTED = static_cast<int>(CQSummaryModel::Mode::SORTED),
    PAGED  = static_cast<int>(CQSummaryModel::Mode::PAGED),
    ROWS   = static_cast<int>(CQSummaryModel::Mode::ROWS)
  };

  using RowNums = CQSummaryModel::RowNums;

  //! header object data
  struct HHeaderObjData {
    Column        c;
    int           ic { -999 };
    BBox          rect;
    Qt::Alignment align { Qt::AlignLeft | Qt::AlignVCenter };
    QString       str;

    HHeaderObjData() = default;

    HHeaderObjData(const Column &c_) :
     c(c_), ic(-999) {
    }

    HHeaderObjData(int ic_) :
     ic(ic_) {
    }

    bool isColumnHeader() const { return (ic == -999); }
  };

  //! row number object data
  struct RowNumberObjData {
    int           r { 0 };
    BBox          rect;
    Qt::Alignment align { Qt::AlignLeft | Qt::AlignVCenter };
    QString       str;

    RowNumberObjData() = default;

    RowNumberObjData(int r) :
     r(r) {
    }
  };

  //! vertical header object data
  struct VHeaderObjData {
    int           r { 0 };
    BBox          rect;
    Qt::Alignment align { Qt::AlignLeft | Qt::AlignVCenter };
    QString       str;

    VHeaderObjData() = default;

    VHeaderObjData(int r) :
     r(r) {
    }
  };

  //! cell object data
  struct CellObjData {
    ModelIndex    ind;
    BBox          rect;
    Qt::Alignment align { Qt::AlignLeft | Qt::AlignVCenter };
    QString       str;
    CQChartsColor bgColor;
    CQChartsColor fgColor;

    CellObjData() = default;

    CellObjData(const ModelIndex &ind) :
     ind(ind) {
    }
  };

 public:
  using ColumnNum = CQChartsColumnNum;

 public:
  CQChartsTablePlot(View *view, const ModelP &model);
 ~CQChartsTablePlot();

  //---

  void init() override;
  void term() override;

  //---

  void addModelI(const ModelP &model) override;

  //---

  // columns
  const Columns &columns() const { return columns_; }
  void setColumns(const Columns &c);

  //---

  Columns getNamedColumns(const QString &name) const override;
  void setNamedColumns(const QString &name, const Columns &c) override;

  //---

  CQSummaryModel *summaryModel() const { return summaryModel_; }

  //---

  void setFont(const Font &f) override;

  //---

  //! get/set mode
  Mode mode() const;
  void setMode(const Mode &mode);

  //---

  bool allowZoomX() const override { return false; }
  bool allowZoomY() const override { return false; }

  bool allowPanX() const override { return false; }
  bool allowPanY() const override { return false; }

  //---

  // max model rows
  int maxRows() const;
  void setMaxRows(int i);

  //---

  // sort data
  ColumnNum sortColumn() const;
  void setSortColumn(const ColumnNum &c);

  int sortRole() const;
  void setSortRole(int r);

  Qt::SortOrder sortOrder() const;
  void setSortOrder(Qt::SortOrder r);

  //---

  // page data
  int pageSize() const;
  void setPageSize(int i);

  int currentPage() const;
  void setCurrentPage(int i);

  //---

  // rows
  const RowNums &rowNums() const;
  void setRowNums(const RowNums &rowNums);

  QString rowNumsStr() const;
  void setRowNumsStr(const QString &str);

  //---

  bool isHHeaderVisible() const { return hheaderData_.visible; }
  void setHHeaderVisible(bool b);

  bool isHHeaderFormatted() const { return hheaderData_.formatted; }
  void setHHeaderFormatted(bool b);

  const Color &hheaderColor() const { return hheaderData_.color; }
  void setHHeaderColor(const Color &c);

  const Font &hheaderFont() const { return hheaderData_.font; }
  void setHHeaderFont(const Font &f);

  //---

  bool isVHeaderVisible() const { return vheaderData_.visible; }
  void setVHeaderVisible(bool b);

  bool isVHeaderFormatted() const { return vheaderData_.formatted; }
  void setVHeaderFormatted(bool b);

  const Color &vheaderColor() const { return vheaderData_.color; }
  void setVHeaderColor(const Color &c);

  const Font &vheaderFont() const { return vheaderData_.font; }
  void setVHeaderFont(const Font &f);

  const Length &vheaderWidth() const { return vheaderData_.size; }
  void setVHeaderWidth(const Length &f);

  //---

  bool isRowColumnVisible() const { return rowColumnData_.visible; }
  void setRowColumnVisible(bool b);

  const Color &rowColumnColor() const { return rowColumnData_.color; }
  void setRowColumnColor(const Color &c);

  const Font &rowColumnFont() const { return rowColumnData_.font; }
  void setRowColumnFont(const Font &f);

  //---

  const Color &cellColor() const { return cellColor_; }
  void setCellColor(const Color &c);

  //---

  const Color &gridColor() const { return gridData_.color; }
  void setGridColor(const Color &c);

  const Length &gridWidth() const { return gridData_.width; }
  void setGridWidth(const Length &l);

  //---

  const Color &insideColor() const { return insideColor_; }
  void setInsideColor(const Color &v);

  const Color &selectedColor() const { return selectedColor_; }
  void setSelectedColor(const Color &v);

  //---

  double indent() const { return indent_; }
  void setIndent(double r);

  int cellMargin() const { return cellMargin_; }
  void setCellMargin(int i);

  int barXMargin() const { return barXMargin_; }
  void setBarXMargin(int i);

  int barYMargin() const { return barYMargin_; }
  void setBarYMargin(int i);

  //---

  bool isFollowView() const { return followView_; }
  void setFollowView(bool b);

  //---

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  double getPanY(bool is_shift) const override;

  //---

  void calcTableSize() const;

  QString getHorizontalHeader(const Column &c, bool &ok) const;
  QString getVerticalHeader  (int r, bool &ok) const;

  void autoFit() override;

  //---

  bool addMenuItems(QMenu *menu, const Point &p) override;

  //---

  void wheelHScroll(int delta) override;
  void wheelVScroll(int delta) override;
  void wheelZoom(const Point &pp, int delta) override;

  //---

  void postResize() override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(PaintDevice *device) const override;

  //---

  void adjustPan() override;

  //---

  void modelViewExpansionChanged() override;

  //---

  const QFont &tableFont() const { return tableData_.font; }

  const QFont &tableHHeaderFont() const { return tableData_.hheaderFont; }
  const QFont &tableVHeaderFont() const { return tableData_.vheaderFont; }

  const QFont &tableRowColumnFont() const { return tableData_.rowColumnFont; }

  int sortPixelWidth() const { return tableData_.pSortWidth; }

  //---

  double scrollX() const;
  double scrollY() const;

  BBox calcTablePixelRect() const;

 protected:
  void drawTable(PaintDevice *device) const;

  void initDrawData() const;

  void updateScrollBars() const;

  void updatePosition() const;

  void drawTableBackground(PaintDevice *device) const;

  void createTableObjData() const;

  std::vector<Mode> modes() const { return
    {{ Mode::NORMAL, Mode::RANDOM, Mode::SORTED, Mode::PAGED, Mode::ROWS }};
  }

  QString modeName(const Mode &mode) const;

  using HHeaderObj   = CQChartsTableHHeaderObj;
  using VHeaderObj   = CQChartsTableVHeaderObj;
  using RowNumberObj = CQChartsTableRowNumberObj;
  using CellObj      = CQChartsTableCellObj;

  virtual HHeaderObj*   createHHeaderObj  (const HHeaderObjData &objData) const;
  virtual VHeaderObj*   createVHeaderObj  (const VHeaderObjData &objData) const;
  virtual RowNumberObj* createRowNumberObj(const RowNumberObjData &objData) const;
  virtual CellObj*      createCellObj     (const CellObjData &cellObjData) const;

 private Q_SLOTS:
  void modelTypeChangedSlot(int modelId);

  void hscrollSlot(int);
  void vscrollSlot(int);

  void setModeSlot(bool b);

  void maxRowsSlot();
  void sortColumnSlot();
  void pageSizeSlot();
  void pageNumSlot();

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  //! column data
  struct ColumnData {
    double pwidth    { 0.0 };
    double width     { 0.0 };
    double drawX     { 0.0 };
    double drawWidth { 0.0 };
    int    prefWidth { -1 };
    bool   numeric   { false };
  };

  using ColumnDataMap = std::map<Column, ColumnData>;

  using CQIntegerSpinP = QPointer<CQIntegerSpin>;

  //! table data
  struct TableData {
    QFont font;
    QFont hheaderFont;
    QFont vheaderFont;
    QFont rowColumnFont;

    QModelIndexList expandInds;

    int    nc         { 0 };   //!< number of columns
    int    nr         { 0 };   //!< number of rows
    int    nvr        { 0 };   //!< number of visible rows
    int    maxDepth   { 0 };   //!< max model item depth
    int    pSortWidth { 16 };  //!< sort pixel width
    double phrh       { 0.0 }; //!< pixel header row height
    double prh        { 0.0 }; //!< pixel row height
    double hrh        { 0.0 }; //!< header row height
    double rh         { 0.0 }; //!< row height
    double pcw        { 0.0 }; //!< pixel columns width
    double sx         { 0.0 }; //!< scroll x
    double sy         { 0.0 }; //!< scroll y
    double dx         { 0.0 }; //!< overflow width
    double dy         { 0.0 }; //!< overflow height
    double xo         { 0.0 }; //!< x offset
    double yo         { 0.0 }; //!< y offset
    int    pmargin    { 2 };   //!< pixel margin

    ColumnData    rowNumberColumnData;      //!< row number column data
    ColumnData    verticalHeaderColumnData; //!< vertical header column data
    ColumnDataMap columnDataMap;            //!< column data map
  };

  //! scroll data
  struct ScrollData {
    bool        scrolled     { false };   //!< scrolled
    int         hpos         { 0 };       //!< horizontal scroll position
    int         vpos         { 0 };       //!< vertical scroll position
    double      pixelBarSize { 13 };      //!< scroll bar pixel size
    QScrollBar* hbar         { nullptr }; //!< horizontal scroll bar
    QScrollBar* vbar         { nullptr }; //!< vertical scroll bar
  };

  //! header data
  struct HeaderData {
    bool   visible   { true }; //!< header visible
    bool   formatted { true }; //!< header formatted
    Color  color;              //!< header color
    Font   font;               //!< header font
    Length size;               //!< header size
  };

  //! row column data
  struct RowColumnData {
    bool   visible { false }; //!< column visible
    Color  color;             //!< column color
    Font   font;              //!< column font
    Length size;              //!< column size
  };

  //! fit data
  struct FitData {
    bool fitHorizontal { true };
    bool fitVertical   { false };
  };

  //! grid data
  struct GridData {
    Color  color; //! line color
    Length width; //! line width
  };

  using HHeaderObjMap   = std::map<int, HHeaderObjData>;
  using VHeaderObjMap   = std::map<int, VHeaderObjData>;
  using RowNumberObjMap = std::map<int, RowNumberObjData>;
  using CellObjMap      = std::map<QModelIndex, CellObjData>;

 private:
  HHeaderObjData& getHHeaderHeaderObjData  (const Column &c) const;
  HHeaderObjData& getVHeaderHeaderObjData  () const;
  HHeaderObjData& getRowColumnHeaderObjData() const;

  RowNumberObjData& getRowNumberObjData(int row) const;
  VHeaderObjData&   getVHeaderObjData  (int row) const;
  CellObjData&      getCellObjData     (const ModelIndex &ind) const;

 private:
  TableData       tableData_;                       //!< cached table data
  ScrollData      scrollData_;                      //!< scroll bar data
  Columns         columns_;                         //!< columns
  Mode            mode_           { Mode::SORTED }; //!< summary model mode
  CQSummaryModel* summaryModel_   { nullptr };      //!< summary model
  HeaderData      hheaderData_;                     //!< horizontal header data
  HeaderData      vheaderData_;                     //!< vertical header data
  RowColumnData   rowColumnData_;                   //!< row column data
  FitData         fitData_;                         //!< fit data
  GridData        gridData_;                        //!< grid data
  Color           cellColor_;                       //!< cell bg color
  Color           insideColor_;                     //!< cell inside bg color
  Color           selectedColor_;                   //!< cell selected bg color
  double          indent_         { 8.0 };          //!< hier indent
  double          fontScale_      { 1.0 };          //!< font scale
  int             cellMargin_     { 4 };            //!< cell margin
  int             barXMargin_     { 2 };            //!< bar x margin
  int             barYMargin_     { 1 };            //!< bar y margin
  bool            followView_     { false };        //!< follow view

  QMenu*         menu_         { nullptr }; //!< menu
  CQIntegerSpinP maxRowsSpin_;              //!< max rows menu edit
  CQIntegerSpinP sortColumnSpin_;           //!< sort column menu edit
  CQIntegerSpinP pageSizeSpin_;             //!< page size menu edit
  CQIntegerSpinP pageNumSpin_;              //!< page number menu edit

  HHeaderObjMap   hheaderObjMap_;   //!< header object map
  VHeaderObjMap   vheaderObjMap_;   //!< vertical header object map
  RowNumberObjMap rowNumberObjMap_; //!< row number object map
  CellObjMap      cellObjMap_;      //!< cell object map
};

//---

/*!
 * \brief Table Header object
 * \ingroup Charts
 */
class CQChartsTableHHeaderObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using TablePlot = CQChartsTablePlot;
  using ObjData   = TablePlot::HHeaderObjData;

 public:
  CQChartsTableHHeaderObj(const TablePlot *plot, const ObjData &objData);

  QString typeName() const override { return "hheader"; }

  QString calcId() const override;

  QString calcTipId() const override;

  // select interface
  bool selectPress(const Point &p, SelData &selData) override;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  void getObjSelectIndices(Indices &inds) const override;

  bool rectIntersect(const BBox &r, bool inside) const override;

 private:
  const TablePlot* tablePlot_ { nullptr }; //!< parent plot
  ObjData          objData_;
};

//---

/*!
 * \brief Vertical Header object
 * \ingroup Charts
 */
class CQChartsTableVHeaderObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using TablePlot = CQChartsTablePlot;
  using ObjData   = TablePlot::VHeaderObjData;

 public:
  CQChartsTableVHeaderObj(const TablePlot *plot, const ObjData &objData);

  QString typeName() const override { return "vheader"; }

  QString calcId() const override;

  QString calcTipId() const override;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  bool rectIntersect(const BBox &r, bool inside) const override;

 private:
  const TablePlot* tablePlot_ { nullptr }; //!< parent plot
  ObjData          objData_;
};

//---

/*!
 * \brief Table Row object
 * \ingroup Charts
 */
class CQChartsTableRowNumberObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using TablePlot = CQChartsTablePlot;
  using ObjData   = TablePlot::RowNumberObjData;

 public:
  CQChartsTableRowNumberObj(const TablePlot *plot, const ObjData &objData);

  QString typeName() const override { return "row"; }

  QString calcId() const override;

  QString calcTipId() const override;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  bool rectIntersect(const BBox &r, bool inside) const override;

 private:
  const TablePlot* tablePlot_ { nullptr }; //!< parent plot
  ObjData          objData_;
};

//---

/*!
 * \brief Table Cell object
 * \ingroup Charts
 */
class CQChartsTableCellObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using TablePlot = CQChartsTablePlot;

 public:
  CQChartsTableCellObj(const TablePlot *plot, const TablePlot::CellObjData &cellObjData);

  QString typeName() const override { return "cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  void getObjSelectIndices(Indices &inds) const override;

  bool inside(const Point &p) const override;

  bool rectIntersect(const BBox &r, bool inside) const override;

 private:
  const TablePlot*       tablePlot_ { nullptr }; //!< parent plot
  TablePlot::CellObjData cellObjData_;
};

//---

#include <CQChartsPlotCustomControls.h>

/*!
 * \brief Table Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsTablePlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsTablePlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(Plot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void addWidgets() override;

  void addColumnWidgets() override;

  void connectSlots(bool b) override;

 protected:
  CQChartsTablePlot* tablePlot_ { nullptr };
};

#endif
