#ifndef CQRandomModel_H
#define CQRandomModel_H

#include <QAbstractItemModel>
#include <vector>

class CRandom;
class QTimer;

class CQRandomModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  struct CellData {
    double oldValue { 0.0 };
    double newValue { 0.0 };
    double value    { 0.0 };

    void initNewValue(double min, double r);

    void genNewValue(double r);

    void updateValue(double r);
  };

  using Cells = std::vector<CellData>;

  struct ColumnData {
    QString name;
    double  min   { 0.0 };
    double  max   { 1.0 };
    double  sum   { -1.0 };
    int     step  { 0 };
    int     steps { 10 };
    Cells   cells;

    void doStep();

    double randValue() const;
  };

 public:
  CQRandomModel();

 ~CQRandomModel();

  //---

  void resize(int numRows, int numColumns);

  //---

  void setColumnName(int c, const QString &name);

  void setColumnRange(int c, double min, double max);

  void setColumnSteps(int c, int n);

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
  using Columns = std::vector<ColumnData>;

  QTimer* timer_ { nullptr };
  Columns columns_;
  bool    initialized_ { false };
};

#endif
