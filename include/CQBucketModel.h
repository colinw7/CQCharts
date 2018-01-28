#ifndef CQBucketModel_H
#define CQBucketModel_H

#include <CQBucketer.h>
#include <QAbstractProxyModel>
#include <set>
#include <cassert>

class CQBucketModel : public QAbstractProxyModel {
  Q_OBJECT

  Q_PROPERTY(int bucketColumn READ bucketColumn WRITE setBucketColumn)

 public:
  CQBucketModel(QAbstractItemModel *model, const CQBucketer &bucketer=CQBucketer());

 ~CQBucketModel();

  //---

  // get/set source model
  QAbstractItemModel *sourceModel() const;
  void setSourceModel(QAbstractItemModel *sourceModel) override;

  //---

  // get/set bucket data
  const CQBucketer &bucketer() const { return bucketer_; }
  void setBucketer(const CQBucketer &bucketer);

  // get/set bucket column
  int bucketColumn() const { return bucketColumn_; }
  void setBucketColumn(int i);

  // get/set bucket column
  int bucketRole() const { return bucketRole_; }
  void setBucketRole(int i) { bucketRole_ = i; }

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

 private slots:
  void bucket();

 private:
  int bucketPos() const { return bucketPos_; }

  // clear model
  void clear();

  // connect/disconnect model slots
  void connectSlots();
  void disconnectSlots();

 private:
  int        bucketColumn_ { 0 };               // bucket source model column
  int        bucketRole_   { Qt::DisplayRole }; // bucket role
  int        bucketPos_    { -1 };              // bucket proxy model column
  CQBucketer bucketer_;                         // bucketer
  bool       bucketed_     { false };           // is bucketer
};

#endif
