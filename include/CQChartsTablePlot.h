#ifndef CQChartsTablePlot_H
#define CQChartsTablePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CQSummaryModel.h>

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

  bool canProbe() const override { return false; }

  bool hasObjs() const override { return false; }

  QString description() const override;

  //---

  void analyzeModel(CQChartsModelData *modelData,
                    CQChartsAnalyzeModelData &analyzeModelData) override;

  //---

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsTablePlot;
class CQChartsTable;
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

  Q_PROPERTY(Mode          mode        READ mode         WRITE setMode       )
  Q_PROPERTY(int           maxRows     READ maxRows      WRITE setMaxRows    )
  Q_PROPERTY(int           sortColumn  READ sortColumn   WRITE setSortColumn )
  Q_PROPERTY(int           sortRole    READ sortRole     WRITE setSortRole   )
  Q_PROPERTY(Qt::SortOrder sortOrder   READ sortOrder    WRITE setSortOrder  )
  Q_PROPERTY(int           pageSize    READ pageSize     WRITE setPageSize   )
  Q_PROPERTY(int           currentPage READ currentPage  WRITE setCurrentPage)
  Q_PROPERTY(QString       rowNums     READ rowNumsStr   WRITE setRowNumsStr )
  Q_PROPERTY(bool          rowColumn   READ isRowColumn  WRITE setRowColumn  )
  Q_PROPERTY(CQChartsColor gridColor   READ gridColor    WRITE setGridColor  )
  Q_PROPERTY(CQChartsColor textColor   READ textColor    WRITE setTextColor  )
  Q_PROPERTY(CQChartsColor headerColor READ headerColor  WRITE setHeaderColor)
  Q_PROPERTY(CQChartsColor cellColor   READ cellColor    WRITE setCellColor  )
  Q_PROPERTY(double        indent      READ indent       WRITE setIndent     )
  Q_PROPERTY(bool          followView  READ isFollowView WRITE setFollowView )

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

 public:
  CQChartsTablePlot(CQChartsView *view, const ModelP &model);

 ~CQChartsTablePlot();

  //---

  // columns
  const CQChartsColumns &columns() const { return columns_; }
  void setColumns(const CQChartsColumns &c);

  //---

  CQSummaryModel *summaryModel() const { return summaryModel_; }

  //---

  void setFont(const CQChartsFont &f) override;

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
  int sortColumn() const;
  void setSortColumn(int i);

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

  const CQChartsColor &gridColor() const { return gridColor_; }
  void setGridColor(const CQChartsColor &c);

  const CQChartsColor &textColor() const { return textColor_; }
  void setTextColor(const CQChartsColor &c);

  const CQChartsColor &headerColor() const { return headerColor_; }
  void setHeaderColor(const CQChartsColor &c);

  const CQChartsColor &cellColor() const { return cellColor_; }
  void setCellColor(const CQChartsColor &c);

  //---

  double indent() const { return indent_; }
  void setIndent(double r);

  //---

  bool isFollowView() const { return followView_; }
  void setFollowView(bool b);

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  double getPanY(bool is_shift) const override;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(CQChartsPaintDevice *device) const override;

  //---

  void adjustPan() override;

  //---

  void modelViewExpansionChanged() override;

 private:
  void drawTable(CQChartsPaintDevice *device) const;

  std::vector<Mode> modes() const { return
    {{ Mode::NORMAL, Mode::RANDOM, Mode::SORTED, Mode::PAGED, Mode::ROWS }};
  }

  QString modeName(const Mode &mode) const;

 private slots:
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
    int             nc       { 0 };
    int             nr       { 0 };
    int             nvr      { 0 };
    int             maxDepth { 0 };
    double          prh      { 0.0 };
    double          rh       { 0.0 };
    double          pcw      { 0.0 };
    double          dx       { 0.0 };
    double          dy       { 0.0 };
    double          xo       { 0.0 };
    double          yo       { 0.0 };
    double          rcw      { 0.0 };
    ColumnData      rowColumnData;
    ColumnDataMap   columnDataMap;
  };

  TableData       tableData_;                //!< cached table data
  CQChartsColumns columns_;                  //!< columns
  CQSummaryModel* summaryModel_ { nullptr }; //!< summary model
  bool            rowColumn_    { false };   //!< draw row numbers column
  CQChartsColor   gridColor_;                //!< grid color
  CQChartsColor   textColor_;                //!< text color
  CQChartsColor   headerColor_;              //!< header color
  CQChartsColor   cellColor_;                //!< cell color
  double          indent_       { 8.0 };     //!< hier indent
  bool            followView_   { false };   //!< follow view
  QMenu*          menu_         { nullptr }; //!< menu
  CQIntegerSpinP  maxRowsSpin_;              //!< max rows menu edit
  CQIntegerSpinP  sortColumnSpin_;           //!< sort column menu edit
  CQIntegerSpinP  pageSizeSpin_;             //!< page size menu edit
  CQIntegerSpinP  pageNumSpin_;              //!< page number menu edit
};

#endif
