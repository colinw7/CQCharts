#ifndef CQSummaryModel_H
#define CQSummaryModel_H

#include <QSortFilterProxyModel>
#include <vector>
#include <set>

class CQSummaryModel : public QAbstractProxyModel {
  Q_OBJECT

  Q_PROPERTY(int           maxRows     READ maxRows     WRITE setMaxRows    )
  Q_PROPERTY(bool          random      READ isRandom    WRITE setRandom     )
  Q_PROPERTY(bool          sorted      READ isSorted    WRITE setSorted     )
  Q_PROPERTY(int           sortColumn  READ sortColumn  WRITE setSortColumn )
  Q_PROPERTY(int           sortRole    READ sortRole    WRITE setSortRole   )
  Q_PROPERTY(Qt::SortOrder sortOrder   READ sortOrder   WRITE setSortOrder  )
  Q_PROPERTY(bool          paged       READ isPaged     WRITE setPaged      )
  Q_PROPERTY(int           pageSize    READ pageSize    WRITE setPageSize   )
  Q_PROPERTY(int           currentPage READ currentPage WRITE setCurrentPage)

 public:
  enum class Mode {
    NORMAL,
    RANDOM,
    SORTED,
    PAGED
  };

 public:
  CQSummaryModel(QAbstractItemModel *model, int maxRows=1000);

 ~CQSummaryModel();

  //---

  // get/set source model
  QAbstractItemModel *sourceModel() const;
  void setSourceModel(QAbstractItemModel *sourceModel) override;

  //---

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &m);

  //---

  int maxRows() const { return maxRows_; }
  void setMaxRows(int i);

  //---

  // random
  bool isRandom() const { return mode_ == Mode::RANDOM; }
  void setRandom(bool b) { setMode(b ? Mode::RANDOM : Mode::NORMAL); }

  //---

  // sorted
  bool isSorted() const { return mode_ == Mode::SORTED; }
  void setSorted(bool b) { setMode(b ? Mode::SORTED : Mode::NORMAL); }

  int sortColumn() const { return sortColumn_; }
  void setSortColumn(int i);

  int sortRole() const { return sortRole_; }
  void setSortRole(int r);

  Qt::SortOrder sortOrder() const { return sortOrder_; }
  void setSortOrder(Qt::SortOrder r);

  //---

  // paged
  bool isPaged() const { return mode_ == Mode::PAGED; }
  void setPaged(bool b) { setMode(b ? Mode::PAGED : Mode::NORMAL); }

  int pageSize() const { return pageSize_; }
  void setPageSize(int i);

  int currentPage() const { return currentPage_; }
  void setCurrentPage(int i);

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

  Mode          mode_        { Mode::NORMAL };       // summary mode
  int           maxRows_     { 1000 };               // max rows
  int           sortColumn_  { 0 };                  // sort column
  int           sortRole_    { Qt::EditRole };       // sort role
  Qt::SortOrder sortOrder_   { Qt::AscendingOrder }; // sort order
  int           pageSize_    { 100 };                // page size
  int           currentPage_ { 0 };                  // current page
  RowInds       rowInds_;                            // row indices
  RowMap        indRows_;                            // index rows
  bool          mapValid_    { false };              // is mapping valid
  bool          mapNone_     { false };              // map not needed
};

#endif
