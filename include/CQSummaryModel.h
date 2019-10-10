#ifndef CQSummaryModel_H
#define CQSummaryModel_H

#include <QSortFilterProxyModel>
#include <vector>
#include <set>

class CQSummaryModel : public QAbstractProxyModel {
  Q_OBJECT

  // mode
  Q_PROPERTY(Mode          mode        READ mode         WRITE setMode       )

  // max rows
  Q_PROPERTY(int           maxRows     READ maxRows      WRITE setMaxRows    )

  // random
  Q_PROPERTY(bool          randomMode  READ isRandomMode WRITE setRandomMode )

  // sort
  Q_PROPERTY(bool          sortMode    READ isSortMode   WRITE setSortMode   )
  Q_PROPERTY(int           sortColumn  READ sortColumn   WRITE setSortColumn )
  Q_PROPERTY(int           sortRole    READ sortRole     WRITE setSortRole   )
  Q_PROPERTY(Qt::SortOrder sortOrder   READ sortOrder    WRITE setSortOrder  )

  // paged
  Q_PROPERTY(bool          pagedMode   READ isPagedMode  WRITE setPagedMode  )
  Q_PROPERTY(int           pageSize    READ pageSize     WRITE setPageSize   )
  Q_PROPERTY(int           currentPage READ currentPage  WRITE setCurrentPage)

  // rows mode
  Q_PROPERTY(bool          rowsMode    READ isRowsMode   WRITE setRowsMode   )

  Q_ENUMS(Mode)

 public:
  enum class Mode {
    NORMAL,
    RANDOM,
    SORTED,
    PAGED,
    ROWS
  };

  using RowNums = std::vector<int>;

 public:
  CQSummaryModel(QAbstractItemModel *model, int maxRows=1000);

 ~CQSummaryModel();

  //---

  // get/set source model
  QAbstractItemModel *sourceModel() const;
  void setSourceModel(QAbstractItemModel *sourceModel) override;

  //---

  // mode
  const Mode &mode() const { return mode_; }
  void setMode(const Mode &m);

  //---

  // max model rows
  int maxRows() const { return maxRows_; }
  void setMaxRows(int i);

  //---

  // random
  bool isRandomMode() const { return mode_ == Mode::RANDOM; }
  void setRandomMode(bool b) { setMode(b ? Mode::RANDOM : Mode::NORMAL); }

  //---

  // sort data
  bool isSortMode() const { return mode_ == Mode::SORTED; }
  void setSortMode(bool b) { setMode(b ? Mode::SORTED : Mode::NORMAL); }

  int sortColumn() const { return sortColumn_; }
  void setSortColumn(int i);

  int sortRole() const { return sortRole_; }
  void setSortRole(int r);

  Qt::SortOrder sortOrder() const { return sortOrder_; }
  void setSortOrder(Qt::SortOrder r);

  //---

  // page data
  bool isPagedMode() const { return mode_ == Mode::PAGED; }
  void setPagedMode(bool b) { setMode(b ? Mode::PAGED : Mode::NORMAL); }

  int pageSize() const { return pageSize_; }
  void setPageSize(int i);

  int currentPage() const { return currentPage_; }
  void setCurrentPage(int i);

  //---

  // rows
  bool isRowsMode() const { return mode_ == Mode::ROWS; }
  void setRowsMode(bool b) { setMode(b ? Mode::ROWS : Mode::NORMAL); }

  const RowNums &rowNums() const { return rowNums_; }
  void setRowNums(const RowNums &rowNums);

  //---

  // # Abstract Model APIS

  // get column count
  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  // get child row count of index
  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  // get child of parent at row/column
  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  // get parent of child
  QModelIndex parent(const QModelIndex &child) const override;

  // does parent have children
  bool hasChildren(const QModelIndex &parent=QModelIndex()) const override;

  // get role data for index
  QVariant data(const QModelIndex &index, int role) const override;

  // set role data for index
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

  // get header data for column/section
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  // set header data for column/section
  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role) override;

  // get flags for index
  Qt::ItemFlags flags(const QModelIndex &index) const override;

  //---

  // # Abstract Proxy Model APIS

  // map source index to proxy index
  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
  // map proxy index to source index
  QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

 private:
  using RowSet = std::set<int>;

  void resetMapping();

  void initMapping();

  void randRows(RowSet &rowSet, int n, int nr) const;

 private:
  using RowInds = std::vector<int>;
  using RowMap  = std::map<int,int>;

  // mode
  Mode          mode_        { Mode::NORMAL };       //!< summary mode

  // max rows
  int           maxRows_     { 1000 };               //!< max rows

  // sort
  int           sortColumn_  { 0 };                  //!< sort column
  int           sortRole_    { Qt::EditRole };       //!< sort role
  Qt::SortOrder sortOrder_   { Qt::AscendingOrder }; //!< sort order

  // paged
  int           pageSize_    { 100 };                //!< page size
  int           currentPage_ { 0 };                  //!< current page

  // rows
  RowNums       rowNums_;                            //!< specific rows numbers

  // cache
  RowInds       rowInds_;                            //!< row indices
  RowMap        indRows_;                            //!< index rows
  bool          mapValid_    { false };              //!< is mapping valid
  bool          mapNone_     { false };              //!< map not needed
};

#endif
