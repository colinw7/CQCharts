#ifndef CQChartsTablePlot_H
#define CQChartsTablePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CQChartsColumnNum.h>
#include <CQSummaryModel.h>

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
class CQChartsTableHeaderObj;
class CQChartsTableRowObj;
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

  // row number column
  Q_PROPERTY(QString rowNums   READ rowNumsStr  WRITE setRowNumsStr)
  Q_PROPERTY(bool    rowColumn READ isRowColumn WRITE setRowColumn )

  // header
  Q_PROPERTY(bool          headerVisible READ isHeaderVisible WRITE setHeaderVisible)
  Q_PROPERTY(CQChartsColor headerColor   READ headerColor     WRITE setHeaderColor  )
  Q_PROPERTY(CQChartsFont  headerFont    READ headerFont      WRITE setHeaderFont   )

  // grid
  Q_PROPERTY(CQChartsColor gridColor READ gridColor WRITE setGridColor)

  // cells
  Q_PROPERTY(CQChartsColor cellColor     READ cellColor     WRITE setCellColor    )
  Q_PROPERTY(CQChartsColor insideColor   READ insideColor   WRITE setInsideColor  )
  Q_PROPERTY(CQChartsColor selectedColor READ selectedColor WRITE setSelectedColor)

  // options
  Q_PROPERTY(double indent      READ indent       WRITE setIndent    )
  Q_PROPERTY(int    cellMargin  READ cellMargin   WRITE setCellMargin)
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
  struct HeaderObjData {
    Column        c;
    BBox          rect;
    Qt::Alignment align { Qt::AlignLeft | Qt::AlignVCenter };
    QString       str;

    HeaderObjData() = default;

    HeaderObjData(const Column &c) :
     c(c) {
    }
  };

  //! row object data
  struct RowObjData {
    int           r { 0 };
    BBox          rect;
    Qt::Alignment align { Qt::AlignLeft | Qt::AlignVCenter };
    QString       str;

    RowObjData() = default;

    RowObjData(int r) :
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

  void setModel(const ModelP &model) override;

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

  bool isRowColumn() const { return rowColumn_; }
  void setRowColumn(bool b);

  //---

  bool isHeaderVisible() const { return headerData_.visible; }
  void setHeaderVisible(bool b);

  const Color &headerColor() const { return headerData_.color; }
  void setHeaderColor(const Color &c);

  const Font &headerFont() const { return headerFont_; }
  void setHeaderFont(const Font &f);

  //---

  const Color &gridColor() const { return gridColor_; }
  void setGridColor(const Color &c);

  const Color &cellColor() const { return cellColor_; }
  void setCellColor(const Color &c);

  const Color &insideColor() const { return insideColor_; }
  void setInsideColor(const Color &v);

  const Color &selectedColor() const { return selectedColor_; }
  void setSelectedColor(const Color &v);

  //---

  double indent() const { return indent_; }
  void setIndent(double r);

  int cellMargin() const { return cellMargin_; }
  void setCellMargin(int i);

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

  void autoFit() override;

  //---

  bool addMenuItems(QMenu *menu) override;

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

  const QFont &tableHeaderFont() const { return tableData_.headerFont; }

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

  using HeaderObj = CQChartsTableHeaderObj;
  using RowObj    = CQChartsTableRowObj;
  using CellObj   = CQChartsTableCellObj;

  virtual HeaderObj *createHeaderObj(const HeaderObjData &headerObjData) const;
  virtual RowObj    *createRowObj   (const RowObjData &rowObjData) const;
  virtual CellObj   *createCellObj  (const CellObjData &cellObjData) const;

 private slots:
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
    double drawWidth { 0.0 };
    int    prefWidth { -1 };
    bool   numeric   { false };
  };

  using ColumnDataMap = std::map<Column, ColumnData>;

  using CQIntegerSpinP = QPointer<CQIntegerSpin>;

  //! table data
  struct TableData {
    QFont           headerFont;
    QFont           font;
    QModelIndexList expandInds;
    int             nc         { 0 };   //!< number of columns
    int             nr         { 0 };   //!< number of rows
    int             nvr        { 0 };   //!< number of visible rows
    int             maxDepth   { 0 };   //!< max model item depth
    int             pSortWidth { 16 };  //!< sort pixel width
    double          phrh       { 0.0 }; //!< pixel header row height
    double          prh        { 0.0 }; //!< pixel row height
    double          hrh        { 0.0 }; //!< header row height
    double          rh         { 0.0 }; //!< row height
    double          pcw        { 0.0 }; //!< pixel columns width
    double          sx         { 0.0 }; //!< scroll x
    double          sy         { 0.0 }; //!< scroll y
    double          dx         { 0.0 }; //!< overflow width
    double          dy         { 0.0 }; //!< overflow height
    double          xo         { 0.0 }; //!< x offset
    double          yo         { 0.0 }; //!< y offset
    int             pmargin    { 2 };   //!< pixel margin
    ColumnData      rowColumnData;      //!< row column data
    ColumnDataMap   columnDataMap;      //!< column data map
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
    bool  visible { true }; //!< header visible
    Color color;            //!< header color
    Font  font;             //!< header font
  };

  //! fit data
  struct FitData {
    bool fitHorizontal { true };
    bool fitVertical   { false };
  };

  using HeaderObjMap = std::map<int, HeaderObjData>;
  using RowObjMap    = std::map<int, RowObjData>;
  using CellObjMap   = std::map<QModelIndex, CellObjData>;

 private:
  HeaderObjData& getHeaderObjData(const Column &c) const;
  RowObjData&    getRowObjData   (int row) const;
  CellObjData&   getCellObjData  (const ModelIndex &ind) const;

 private:
  TableData       tableData_;                     //!< cached table data
  ScrollData      scrollData_;                    //!< scroll bar data
  Columns         columns_;                       //!< columns
  Mode            mode_         { Mode::SORTED }; //!< summary model mode
  CQSummaryModel* summaryModel_ { nullptr };      //!< summary model
  bool            rowColumn_    { false };        //!< draw row numbers column
  HeaderData      headerData_;                    //!< header data
  FitData         fitData_;                       //!< fit data
  Font            headerFont_;                    //!< header font
  Color           gridColor_;                     //!< grid line color
  Color           cellColor_;                     //!< cell bg color
  Color           insideColor_;                   //!< cell inside bg color
  Color           selectedColor_;                 //!< cell selected bg color
  double          indent_       { 8.0 };          //!< hier indent
  double          fontScale_    { 1.0 };          //!< font scale
  int             cellMargin_   { 4 };            //!< cell margin
  bool            followView_   { false };        //!< follow view
  QMenu*          menu_         { nullptr };      //!< menu
  CQIntegerSpinP  maxRowsSpin_;                   //!< max rows menu edit
  CQIntegerSpinP  sortColumnSpin_;                //!< sort column menu edit
  CQIntegerSpinP  pageSizeSpin_;                  //!< page size menu edit
  CQIntegerSpinP  pageNumSpin_;                   //!< page number menu edit

  HeaderObjMap headerObjMap_; //!< header object map
  RowObjMap    rowObjMap_;    //!< row object map
  CellObjMap   cellObjMap_;   //!< cell object map
};

//---

/*!
 * \brief Table Header object
 * \ingroup Charts
 */
class CQChartsTableHeaderObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot = CQChartsTablePlot;

 public:
  CQChartsTableHeaderObj(const Plot *plot, const Plot::HeaderObjData &headerObjData);

  QString typeName() const override { return "header"; }

  QString calcId() const override;

  QString calcTipId() const override;

  // select interface
  bool selectPress(const Point &p, SelMod selMod) override;

  void draw(PaintDevice *device) const override;

  void getObjSelectIndices(Indices &inds) const override;

  bool rectIntersect(const BBox &r, bool inside) const override;

 private:
  const Plot*         plot_ { nullptr }; //!< parent plot
  Plot::HeaderObjData headerObjData_;
};

//---

/*!
 * \brief Table Row object
 * \ingroup Charts
 */
class CQChartsTableRowObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot = CQChartsTablePlot;

 public:
  CQChartsTableRowObj(const Plot *plot, const Plot::RowObjData &rowObjData);

  QString typeName() const override { return "row"; }

  QString calcId() const override;

  QString calcTipId() const override;

  void draw(PaintDevice *device) const override;

  bool rectIntersect(const BBox &r, bool inside) const override;

 private:
  const Plot*      plot_ { nullptr }; //!< parent plot
  Plot::RowObjData rowObjData_;
};

//---

/*!
 * \brief Table Cell object
 * \ingroup Charts
 */
class CQChartsTableCellObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot = CQChartsTablePlot;

 public:
  CQChartsTableCellObj(const Plot *plot, const Plot::CellObjData &cellObjData);

  QString typeName() const override { return "cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  void draw(PaintDevice *device) const override;

  void getObjSelectIndices(Indices &inds) const override;

  bool inside(const Point &p) const override;

  bool rectIntersect(const BBox &r, bool inside) const override;

 private:
  const Plot*       plot_ { nullptr }; //!< parent plot
  Plot::CellObjData cellObjData_;
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

  void addWidgets() override;

  void setPlot(CQChartsPlot *plot) override;

 protected:
  void connectSlots(bool b) override;

 public slots:
  void updateWidgets() override;

 private:
  CQChartsTablePlot* plot_ { nullptr };
};

#endif
