#ifndef CQLorenzModel_H
#define CQLorenzModel_H

#include <QAbstractItemModel>
#include <vector>

class CLorenzCalc;

class CQLorenzModel : public QAbstractItemModel {
  Q_OBJECT

  Q_PROPERTY(double a READ getA WRITE setA)
  Q_PROPERTY(double b READ getB WRITE setB)
  Q_PROPERTY(double c READ getC WRITE setC)

 public:
  CQLorenzModel(int start=0, int end=80000);

 ~CQLorenzModel();

  //---

  double getA() const;
  void setA(double r);

  double getB() const;
  void setB(double r);

  double getC() const;
  void setC(double r);

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

 private:
  struct Point {
    Point() { }

    Point(double x, double y, double z) :
     x(x), y(y), z(z) {
    }

    double x { 0.0 };
    double y { 0.0 };
    double z { 0.0 };
  };

  using Points = std::vector<Point>;

  CLorenzCalc *calc_        { nullptr };
  bool         initialized_ { false };
  Points       points_;
};

#endif
