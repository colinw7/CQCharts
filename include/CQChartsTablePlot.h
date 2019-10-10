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

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsTablePlot;
class CQChartsTable;

//---

/*!
 * \brief Table Plot
 * \ingroup Charts
 */
class CQChartsTablePlot : public CQChartsPlot {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumns columns READ columns WRITE setColumns)

  Q_PROPERTY(Mode          mode        READ mode        WRITE setMode       )
  Q_PROPERTY(int           maxRows     READ maxRows     WRITE setMaxRows    )
  Q_PROPERTY(int           sortColumn  READ sortColumn  WRITE setSortColumn )
  Q_PROPERTY(int           sortRole    READ sortRole    WRITE setSortRole   )
  Q_PROPERTY(Qt::SortOrder sortOrder   READ sortOrder   WRITE setSortOrder  )
  Q_PROPERTY(int           pageSize    READ pageSize    WRITE setPageSize   )
  Q_PROPERTY(int           currentPage READ currentPage WRITE setCurrentPage)
  Q_PROPERTY(QString       rowNums     READ rowNumsStr  WRITE setRowNumsStr )

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

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(CQChartsPaintDevice *device) const override;

 private:
  void drawTable(CQChartsPaintDevice *device) const;

 private:
  struct ColumnData {
    double pwidth    { 0.0 };
    double width     { 0.0 };
    double drawWidth { 0.0 };
    int    prefWidth { -1 };
    bool   numeric   { false };
  };

  using ColumnDataMap = std::map<CQChartsColumn,ColumnData>;

  struct TableData {
    QFont         font;
    int           nc  { 0 };
    int           nr  { 0 };
    double        prh { 0.0 };
    double        rh  { 0.0 };
    double        pcw { 0.0 };
    double        xo  { 0.0 };
    double        yo  { 0.0 };
    ColumnDataMap columnDataMap;
  };

  TableData       tableData_;                //!< cached table data
  CQChartsColumns columns_;                  //!< columns
  CQSummaryModel* summaryModel_ { nullptr }; //!< summary model
};

#endif
