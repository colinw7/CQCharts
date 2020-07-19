#ifndef CQModelVisitor_H
#define CQModelVisitor_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <future>

class QAbstractItemModel;

//---

/*!
 * \brief Visitor class for model data
 */
class CQModelVisitor {
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

    VisitData() = default;

    VisitData(const QModelIndex &parent, int row) :
     parent(parent), row(row) {
    }
  };

 public:
  CQModelVisitor() { }

  virtual ~CQModelVisitor() { }

  int numCols() const { return numCols_; }

  int numRows() const { return numRows_; }
  void setNumRows(int nr) { numRows_ = nr; }

  int row() const { return row_; }

  int numProcessedRows() const { return numProcessedRows_; }

  int maxRows() const { return maxRows_; }
  void setMaxRows(int i) { maxRows_ = i; }

  bool isHierarchical() const;

  //---

  virtual void initVisit() { }

  void step() { ++row_; }

  virtual void termVisit() { }

  void enter() { ++depth_; maxDepth_ = std::max(maxDepth_, depth_); }
  void leave() { --depth_; }

  //---

  int depth() const { return depth_; }

  int maxDepth() const { return maxDepth_; }

  //---

  // pre hier visit
  virtual State hierVisit(const QAbstractItemModel *, const VisitData &) { return State::OK; }

  // post hier visit
  virtual State hierPostVisit(const QAbstractItemModel *, const VisitData &) { return State::OK; }

  //---

  virtual State preVisit(const QAbstractItemModel *, const VisitData &) { return State::OK; }

  virtual State visit(const QAbstractItemModel *, const VisitData &) { return State::OK; }

  //virtual State postVisit(const QAbstractItemModel *, const VisitData &) { return State::OK; }

 public:
  // only called by CQModelVisit
  void init(const QAbstractItemModel *model);
  void term();

 protected:
  const QAbstractItemModel* model_            { nullptr }; //!< model to visit
  int                       numCols_          { 0 };       //!< number of columns
  int                       numRows_          { 0 };       //!< number of rows (current parent)
  int                       row_              { 0 };       //!< current row
  int                       depth_            { 0 };       //!< current depth
  int                       maxDepth_         { 0 };       //!< max depth
  int                       numProcessedRows_ { 0 };       //!< total number of rows processed
  int                       maxRows_          { -1 };      //!< maximum number of rows to process
  bool                      hierarchical_     { false };   //!< is hierarchical
  bool                      hierSet_          { false };   //!< is hierarchical set
  mutable std::mutex        mutex_;                        //!< mutex
};

//---

namespace CQModelVisit {

bool exec(const QAbstractItemModel *model, CQModelVisitor &visitor);

bool exec(const QAbstractItemModel *model, const QModelIndex &parent, int r,
          CQModelVisitor &visitor);

CQModelVisitor::State execIndex(const QAbstractItemModel *model, const QModelIndex &parent,
                                CQModelVisitor &visitor);

CQModelVisitor::State execRow(const QAbstractItemModel *model, const QModelIndex &parent,
                              int r, CQModelVisitor &visitor);

}

#endif
