#ifndef CQSummaryModel_H
#define CQSummaryModel_H

#include <QSortFilterProxyModel>
#include <vector>
#include <set>

class CQSummaryModel : public QAbstractProxyModel {
  Q_OBJECT

  Q_PROPERTY(int  maxRows     READ maxRows     WRITE setMaxRows    )
  Q_PROPERTY(bool random      READ isRandom    WRITE setRandom     )
  Q_PROPERTY(bool sorted      READ isSorted    WRITE setSorted     )
  Q_PROPERTY(bool paged       READ isPaged     WRITE setPaged      )
  Q_PROPERTY(int  pageSize    READ pageSize    WRITE setPageSize   )
  Q_PROPERTY(int  currentPage READ currentPage WRITE setCurrentPage)

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

  int maxRows() const { return maxRows_; }
  void setMaxRows(int i);

  bool isRandom() const { return mode_ == Mode::RANDOM; }
  void setRandom(bool b) { setMode(b ? Mode::RANDOM : Mode::NORMAL); }

  bool isSorted() const { return mode_ == Mode::SORTED; }
  void setSorted(bool b) { setMode(b ? Mode::SORTED : Mode::NORMAL); }

  bool isPaged() const { return mode_ == Mode::PAGED; }
  void setPaged(bool b) { setMode(b ? Mode::PAGED : Mode::NORMAL); }

  int sortColumn() const { return sortColumn_; }
  void setSortColumn(int i);

  int sortRole() const { return sortRole_; }
  void setSortRole(int r);

  int pageSize() const { return pageSize_; }
  void setPageSize(int i);

  int currentPage() const { return currentPage_; }
  void setCurrentPage(int i);

  //---

  // # Abstarct Model APIS

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

  // # Abstarct Proxy Model APIS

  // map source index to proxy index
  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
  // map proxy index to source index
  QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

 private:
  using RowSet = std::set<int>;

  void resetMapping();

  void initMapping();

  void randRows(RowSet &rowSet, int n, int nr) const;

 private:
  using RowInds = std::vector<int>;
  using RowMap  = std::map<int,int>;

  Mode    mode_        { Mode::NORMAL };
  int     maxRows_     { 1000 };
  int     sortColumn_  { 0 };
  int     sortRole_    { Qt::EditRole };
  int     pageSize_    { 100 };
  int     currentPage_ { 0 };
  RowInds rowInds_;
  RowMap  indRows_;
  bool    mapValid_    { false };
};

#endif
