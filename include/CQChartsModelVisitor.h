#ifndef CQChartsModelVisitor_H
#define CQChartsModelVisitor_H

#include <QAbstractItemModel>
#include <QModelIndex>

class CQCharts;
class QAbstractItemModel;

//---

class CQChartsModelVisitor {
 public:
  enum class State {
    OK,
    SKIP,
    TERMINATE
  };

  struct VisitData {
    QModelIndex parent;
    int         row  { -1 };
    int         vrow { -1 };

    VisitData(const QModelIndex &parent, int row) :
     parent(parent), row(row) {
    }
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

  bool isHierarchical() const;

  //---

  void init(QAbstractItemModel *model);

  void step() { ++row_; }

  void term() { numRows_ = row_; }

  //---

  virtual State hierVisit(QAbstractItemModel *, const VisitData &) { return State::OK; }

  virtual State hierPostVisit(QAbstractItemModel *, const VisitData &) { return State::OK; }

  //---

  virtual State preVisit(QAbstractItemModel *, const VisitData &) { return State::OK; }

  virtual State visit(QAbstractItemModel *, const VisitData &) { return State::OK; }

  //virtual State postVisit(QAbstractItemModel *, const VisitData &) { return State::OK; }

 protected:
  QAbstractItemModel* model_        { nullptr };
  int                 numCols_      { 0 };
  int                 row_          { 0 };
  int                 numRows_      { 0 };
  int                 maxRows_      { -1 };
  bool                hierarchical_ { false };
  bool                hierSet_      { false };
};

//---

namespace CQChartsModelVisit {

bool exec(CQCharts *charts, QAbstractItemModel *model, CQChartsModelVisitor &visitor);

bool exec(QAbstractItemModel *model, const QModelIndex &parent, int r,
          CQChartsModelVisitor &visitor);

CQChartsModelVisitor::State execIndex(QAbstractItemModel *model, const QModelIndex &parent,
                                            CQChartsModelVisitor &visitor);

CQChartsModelVisitor::State execRow(QAbstractItemModel *model, const QModelIndex &parent,
                                    int r, CQChartsModelVisitor &visitor);

}

#endif
