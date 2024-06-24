#ifndef CQFireworksModel_H
#define CQFireworksModel_H

#include <QAbstractItemModel>
#include <vector>

class CFireworks;
class QTimer;

class CQFireworksModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  CQFireworksModel();

 ~CQFireworksModel();

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

  // get header data for column/section
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  // get flags for index
  Qt::ItemFlags flags(const QModelIndex &index) const override;

 private:
  void init();

 private Q_SLOTS:
  void timerSlot();

 private:
  CFireworks *fireworks_   { nullptr };
  bool        initialized_ { false };
  QTimer*     timer_       { nullptr };
};

#endif
