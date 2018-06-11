#ifndef CQSummaryModel_H
#define CQSummaryModel_H

#include <QSortFilterProxyModel>

class CQSummaryModel : public QAbstractProxyModel {
  Q_OBJECT

  Q_PROPERTY(int  maxRows READ maxRows  WRITE setMaxRows)
  Q_PROPERTY(bool sorted  READ isSorted WRITE setSorted )
  Q_PROPERTY(bool random  READ isRandom WRITE setRandom )

 public:
  CQSummaryModel(QAbstractItemModel *model, int maxRows=1000);

 ~CQSummaryModel();

  //---

  // get/set source model
  QAbstractItemModel *sourceModel() const;
  void setSourceModel(QAbstractItemModel *sourceModel) override;

  //---

  int maxRows() const { return maxRows_; }
  void setMaxRows(int i);

  bool isSorted() const { return sorted_; }
  void setSorted(bool b);

  bool isRandom() const { return random_; }
  void setRandom(bool b);

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
  int  maxRows_ { 1000 };
  bool sorted_  { false };
  bool random_  { false };
};

#endif
