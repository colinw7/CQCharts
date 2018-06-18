#ifndef CQChartsModelVisitor_H
#define CQChartsModelVisitor_H

#include <QAbstractItemModel>
#include <QModelIndex>

class QAbstractItemModel;

class CQChartsModelVisitor {
 public:
  enum class State {
    OK,
    SKIP,
    TERMINATE
  };

 public:
  CQChartsModelVisitor() { }

  virtual ~CQChartsModelVisitor() { }

  int numCols() const { return numCols_; }

  int row() const { return row_; }

  int numRows() const { return numRows_; }
  void setNumRows(int nr) { numRows_ = nr; }

  int maxRows() const { return maxRows_; }
  void setMaxRows(int i) { maxRows_ = i; }

  bool isHierarchical() const { return hierarchical_; }

  //---

  void init(QAbstractItemModel *model);

  void step() { ++row_; }

  void term() { numRows_ = row_; }

  //---

  virtual State hierVisit(QAbstractItemModel *, const QModelIndex &, int) { return State::OK; }

  virtual State hierPostVisit(QAbstractItemModel *, const QModelIndex &, int) { return State::OK; }

  //---

  virtual State preVisit(QAbstractItemModel *, const QModelIndex &, int) { return State::OK; }

  virtual State visit(QAbstractItemModel *, const QModelIndex &, int) { return State::OK; }

  //virtual State postVisit(QAbstractItemModel *, const QModelIndex &, int) { return State::OK; }

 protected:
  int  numCols_      { 0 };
  int  row_          { 0 };
  int  numRows_      { 0 };
  int  maxRows_      { -1 };
  bool hierarchical_ { false };
};

#endif
