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

  void analyzeModel(CQChartsModelData *modelData,
                    CQChartsAnalyzeModelData &analyzeModelData) override;

  //---

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
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

  Q_PROPERTY(Mode              mode          READ mode            WRITE setMode         )
  Q_PROPERTY(int               maxRows       READ maxRows         WRITE setMaxRows      )
  Q_PROPERTY(CQChartsColumnNum sortColumn    READ sortColumn      WRITE setSortColumn   )
  Q_PROPERTY(int               sortRole      READ sortRole        WRITE setSortRole     )
  Q_PROPERTY(Qt::SortOrder     sortOrder     READ sortOrder       WRITE setSortOrder    )
  Q_PROPERTY(int               pageSize      READ pageSize        WRITE setPageSize     )
  Q_PROPERTY(int               currentPage   READ currentPage     WRITE setCurrentPage  )
  Q_PROPERTY(QString           rowNums       READ rowNumsStr      WRITE setRowNumsStr   )
  Q_PROPERTY(bool              rowColumn     READ isRowColumn     WRITE setRowColumn    )
  Q_PROPERTY(bool              headerVisible READ isHeaderVisible WRITE setHeaderVisible)

  Q_PROPERTY(CQChartsColor headerColor     READ headerColor     WRITE setHeaderColor    )
  Q_PROPERTY(CQChartsColor gridColor       READ gridColor       WRITE setGridColor      )
  Q_PROPERTY(CQChartsColor textColor       READ textColor       WRITE setTextColor      )
  Q_PROPERTY(CQChartsColor cellColor       READ cellColor       WRITE setCellColor      )
  Q_PROPERTY(CQChartsColor insideColor     READ insideColor     WRITE setInsideColor    )
  Q_PROPERTY(CQChartsColor insideTextColor READ insideTextColor WRITE setInsideTextColor)

  Q_PROPERTY(double indent      READ indent       WRITE setIndent    )
  Q_PROPERTY(bool   followView  READ isFollowView WRITE setFollowView)

  Q_ENUMS(Mode)

 public:
  enum class Mode {
    NORMAL = (int) CQSummaryModel::Mode::NORMAL,
    RANDOM = (int) CQSummaryModel::Mode::RANDOM,
    SORTED = (int) CQSummaryModel::Mode::SORTED,
    PAGED  = (int) CQSummaryModel::Mode::PAGED,
    ROWS   = (int) CQSummaryModel::Mode::ROWS
  };

  using RowNums = CQSummaryModel::RowNums;

  struct HeaderObjData {
    CQChartsColumn c;
    BBox           rect;
    Qt::Alignment  align { Qt::AlignLeft | Qt::AlignVCenter };
    QString        str;

    HeaderObjData() = default;

    HeaderObjData(const CQChartsColumn &c) :
     c(c) {
    }
  };

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

  struct CellObjData {
    CQChartsModelIndex ind;
    BBox               rect;
    Qt::Alignment      align { Qt::AlignLeft | Qt::AlignVCenter };
    QString            str;

    CellObjData() = default;

    CellObjData(const CQChartsModelIndex &ind) :
     ind(ind) {
    }
  };

 public:
  CQChartsTablePlot(CQChartsView *view, const ModelP &model);

 ~CQChartsTablePlot();

  //---

  void setModel(const ModelP &model) override;

  //---

  // columns
  const CQChartsColumns &columns() const { return columns_; }
  void setColumns(const CQChartsColumns &c);

  //---

  CQSummaryModel *summaryModel() const { return summaryModel_; }

  //---

  void setFont(const CQChartsFont &f) override;

  //---

  bool allowZoomX() const override { return false; }
  bool allowZoomY() const override { return false; }

  bool allowPanX() const override { return false; }
  bool allowPanY() const override { return false; }

  //---

  // mode
  Mode mode() const;
  void setMode(const Mode &m);

  //---

  // max model rows
  int maxRows() const;
  void setMaxRows(int i);

  //---

  // sort data
  CQChartsColumnNum sortColumn() const;
  void setSortColumn(const CQChartsColumnNum &c);

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

  const CQChartsColor &headerColor() const { return headerData_.color; }
  void setHeaderColor(const CQChartsColor &c);

  //---

  const CQChartsColor &gridColor() const { return gridColor_; }
  void setGridColor(const CQChartsColor &c);

  const CQChartsColor &textColor() const { return textColor_; }
  void setTextColor(const CQChartsColor &c);

  const CQChartsColor &cellColor() const { return cellColor_; }
  void setCellColor(const CQChartsColor &c);

  const CQChartsColor &insideColor() const { return insideColor_; }
  void setInsideColor(const CQChartsColor &v);

  const CQChartsColor &insideTextColor() const { return insideTextColor_; }
  void setInsideTextColor(const CQChartsColor &v);

  //---

  double indent() const { return indent_; }
  void setIndent(double r);

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

  void postResize() override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(CQChartsPaintDevice *device) const override;

  //---

  void adjustPan() override;

  //---

  void modelViewExpansionChanged() override;

  //---

  const QFont &tableFont() const { return tableData_.font; }

  double scrollX() const;
  double scrollY() const;

  BBox calcTablePixelRect() const;

 protected:
  void drawTable(CQChartsPaintDevice *device) const;

  void initDrawData() const;

  void updateScrollBars() const;

  void updatePosition() const;

  void drawTableBackground(CQChartsPaintDevice *device) const;

  void createTableObjData() const;

  std::vector<Mode> modes() const { return
    {{ Mode::NORMAL, Mode::RANDOM, Mode::SORTED, Mode::PAGED, Mode::ROWS }};
  }

  QString modeName(const Mode &mode) const;

  virtual CQChartsTableHeaderObj *createHeaderObj(const HeaderObjData &headerObjData) const;
  virtual CQChartsTableRowObj    *createRowObj   (const RowObjData &rowObjData) const;
  virtual CQChartsTableCellObj   *createCellObj  (const CellObjData &cellObjData) const;

 private slots:
  void hscrollSlot(int);
  void vscrollSlot(int);

  void setModeSlot(bool b);

  void maxRowsSlot();
  void sortColumnSlot();
  void pageSizeSlot();
  void pageNumSlot();

 private:
  struct ColumnData {
    double pwidth    { 0.0 };
    double width     { 0.0 };
    double drawWidth { 0.0 };
    int    prefWidth { -1 };
    bool   numeric   { false };
  };

  using ColumnDataMap = std::map<CQChartsColumn,ColumnData>;

  using CQIntegerSpinP = QPointer<CQIntegerSpin>;

  struct TableData {
    QFont           font;
    QModelIndexList expandInds;
    int             nc       { 0 };   // number of columns
    int             nr       { 0 };   // number of rows
    int             nvr      { 0 };   // number of visible rows
    int             maxDepth { 0 };   // max model item depth
    double          prh      { 0.0 }; // pixel row height
    double          rh       { 0.0 }; // row height
    double          pcw      { 0.0 }; // pixel columns width
    double          sx       { 0.0 }; // scroll x
    double          sy       { 0.0 }; // scroll y
    double          dx       { 0.0 }; // overflow width
    double          dy       { 0.0 }; // overflow height
    double          xo       { 0.0 }; // x offset
    double          yo       { 0.0 }; // y offset
    int             pmargin  { 2 };   // pixel margin
    ColumnData      rowColumnData;
    ColumnDataMap   columnDataMap;
  };

  struct ScrollData {
    bool        scrolled     { false };   //!< scrolled
    int         hpos         { 0 };       //!< horizontal scroll position
    int         vpos         { 0 };       //!< vertical scroll position
    double      pixelBarSize { 13 };      //!< scroll bar pixel size
    QScrollBar* hbar         { nullptr }; //!< horizontal scroll bar
    QScrollBar* vbar         { nullptr }; //!< vertical scroll bar
  };

  struct HeaderData {
    bool          visible { true }; //!< header visible
    CQChartsColor color;            //!< header color
  };

  struct FitData {
    bool fitHorizontal { true };
    bool fitVertical   { false };
  };

  using HeaderObjMap = std::map<int,HeaderObjData>;
  using RowObjMap    = std::map<int,RowObjData>;
  using CellObjMap   = std::map<QModelIndex,CellObjData>;

 private:
  HeaderObjData& getHeaderObjData(const CQChartsColumn &c) const;
  RowObjData&    getRowObjData   (int row) const;
  CellObjData&   getCellObjData  (const CQChartsModelIndex &ind) const;

 private:
  TableData       tableData_;                //!< cached table data
  ScrollData      scrollData_;               //!< scroll bar data
  CQChartsColumns columns_;                  //!< columns
  CQSummaryModel* summaryModel_ { nullptr }; //!< summary model
  bool            rowColumn_    { false };   //!< draw row numbers column
  HeaderData      headerData_;               //!< header data
  FitData         fitData_;                  //!< fit data
  CQChartsColor   gridColor_;                //!< grid color
  CQChartsColor   textColor_;                //!< text color
  CQChartsColor   cellColor_;                //!< cell color
  CQChartsColor   insideColor_;              //!< cell inside fill color
  CQChartsColor   insideTextColor_;          //!< cell inside text color
  double          indent_       { 8.0 };     //!< hier indent
  double          fontScale_    { 1.0 };     //!< font scale
  bool            followView_   { false };   //!< follow view
  QMenu*          menu_         { nullptr }; //!< menu
  CQIntegerSpinP  maxRowsSpin_;              //!< max rows menu edit
  CQIntegerSpinP  sortColumnSpin_;           //!< sort column menu edit
  CQIntegerSpinP  pageSizeSpin_;             //!< page size menu edit
  CQIntegerSpinP  pageNumSpin_;              //!< page number menu edit

  HeaderObjMap headerObjMap_; //!< header object map
  RowObjMap    rowObjMap_;    //!< row object map
  CellObjMap   cellObjMap_;   //!< cell object map
};

/*!
 * \brief Table Header object
 * \ingroup Charts
 */
class CQChartsTableHeaderObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsTableHeaderObj(const CQChartsTablePlot *plot,
                         const CQChartsTablePlot::HeaderObjData &headerObjData);

  QString typeName() const override { return "header"; }

  QString calcId() const override;

  QString calcTipId() const override;

  void draw(CQChartsPaintDevice *device) override;

  void getObjSelectIndices(Indices &inds) const override;

  bool rectIntersect(const BBox &r, bool inside) const override;

 private:
  const CQChartsTablePlot*         plot_ { nullptr }; //!< parent plot
  CQChartsTablePlot::HeaderObjData headerObjData_;
};

/*!
 * \brief Table Row object
 * \ingroup Charts
 */
class CQChartsTableRowObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsTableRowObj(const CQChartsTablePlot *plot,
                      const CQChartsTablePlot::RowObjData &rowObjData);

  QString typeName() const override { return "row"; }

  QString calcId() const override;

  QString calcTipId() const override;

  void draw(CQChartsPaintDevice *device) override;

  bool rectIntersect(const BBox &r, bool inside) const override;

 private:
  const CQChartsTablePlot*      plot_ { nullptr }; //!< parent plot
  CQChartsTablePlot::RowObjData rowObjData_;
};

/*!
 * \brief Table Cell object
 * \ingroup Charts
 */
class CQChartsTableCellObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsTableCellObj(const CQChartsTablePlot *plot,
                       const CQChartsTablePlot::CellObjData &cellObjData);

  QString typeName() const override { return "cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  void draw(CQChartsPaintDevice *device) override;

  void getObjSelectIndices(Indices &inds) const override;

  bool inside(const Point &p) const override;

  bool rectIntersect(const BBox &r, bool inside) const override;

 private:
  const CQChartsTablePlot*       plot_ { nullptr }; //!< parent plot
  CQChartsTablePlot::CellObjData cellObjData_;
};

#endif
