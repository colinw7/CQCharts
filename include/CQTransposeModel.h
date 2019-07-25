#ifndef CQTransposeModel_H
#define CQTransposeModel_H

#include <QAbstractProxyModel>

class CQTransposeModel : public QAbstractProxyModel {
  Q_OBJECT

 public:
  CQTransposeModel(QAbstractItemModel *model);

 ~CQTransposeModel();

  //---

  // get/set source model
  QAbstractItemModel *sourceModel() const;
  void setSourceModel(QAbstractItemModel *sourceModel) override;

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

 private slots:
  void columnsAboutToBeInsertedSlot(const QModelIndex &parent, int start, int end);
  void columnsInsertedSlot(const QModelIndex &, int, int);
  void columnsAboutToBeMovedSlot(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                                 const QModelIndex &destinationParent, int destinationColumn);
  void columnsMovedSlot(const QModelIndex&, int, int, const QModelIndex&, int);
  void columnsAboutToBeRemovedSlot(const QModelIndex &parent, int start, int end);
  void columnsRemovedSlot(const QModelIndex&, int, int);

  void rowsAboutToBeInsertedSlot(const QModelIndex &parent, int start, int end);
  void rowsInsertedSlot(const QModelIndex&, int, int);
  void rowsAboutToBeMovedSlot(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                              const QModelIndex &destinationParent, int destinationColumn);
  void rowsMovedSlot(const QModelIndex&, int, int, const QModelIndex&, int);
  void rowsAboutToBeRemovedSlot(const QModelIndex &parent, int start, int end);
  void rowsRemovedSlot(const QModelIndex&, int, int);

  void dataChangedSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight);
  void headerDataChangedSlot(Qt::Orientation orientation, int first, int last);

  void modelAboutToBeResetSlot();
  void modelResetSlot();

 private:
  // connect/disconnect model slots
  void connectSlots();
  void disconnectSlots();

  void connectDisconnectSlots(bool b);
};

#endif
