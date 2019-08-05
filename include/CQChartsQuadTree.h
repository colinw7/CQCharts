#ifndef CQChartsQuadTree_H
#define CQChartsQuadTree_H

#include <cassert>
#include <list>
#include <map>
#include <vector>
#include <sys/types.h>

/*!
 * quad tree containing pointers to items of type DATA with an associated rect of type RECT
 *
 * the tree is split when the number of elements is greater than or equal to the auto
 * split limit
 *
 * tree does not take ownership of data. The application must ensure elements are not
 * deleted while in the tree and are deleted when required.
 *
 * DATA must support:
 *   const RECT &rect = data->rect();
 *
 * RECT must support:
 *   constructor RECT(l, b, r, t);
 *
 *   T l = rect.getXMin();
 *   T b = rect.getYMin();
 *   T r = rect.getXMax();
 *   T t = rect.getYMax();
 *
 */
template<typename DATA, typename RECT, typename T=double>
class CQChartsQuadTree {
 public:
  using DataList = std::list<DATA*>;

 public:
  explicit CQChartsQuadTree(const RECT &rect=RECT()) :
   rect_(rect) {
  }

 ~CQChartsQuadTree() {
    delete bl_tree_;
    delete br_tree_;
    delete tl_tree_;
    delete tr_tree_;
  }

 private:
  CQChartsQuadTree(CQChartsQuadTree *parent, const RECT &rect) :
   parent_(parent), rect_(rect) {
  }

 public:
  // reset quad tree
  void reset() {
    dataList_.clear();

    delete bl_tree_; bl_tree_ = 0;
    delete br_tree_; br_tree_ = 0;
    delete tl_tree_; tl_tree_ = 0;
    delete tr_tree_; tr_tree_ = 0;
  }

  bool isEmpty() const {
    return (dataList_.empty() && ! bl_tree_ && ! br_tree_ && ! tl_tree_ && ! tr_tree_);
  }

  // get bounding rect
  const RECT &rect() const { return rect_; }

  // get data list
  const DataList &dataList() const { return dataList_; }

  // get auto split limit
  static uint autoSplitLimit() {
    return *autoSplitLimitP();
  }

  // set auto split limit
  static void setAutoSplitLimit(uint limit) {
    *autoSplitLimitP() = limit;
  }

  // get auto split limit
  static uint minTreeSize() {
    return *minTreeSizeP();
  }

  // set auto split limit
  static void setTreeSize(uint limit) {
    *minTreeSizeP() = limit;
  }

  // get parent tree
  CQChartsQuadTree *parent() const { return parent_; }

  // get sub trees
  CQChartsQuadTree *blTree() const { return bl_tree_; }
  CQChartsQuadTree *brTree() const { return br_tree_; }
  CQChartsQuadTree *tlTree() const { return tl_tree_; }
  CQChartsQuadTree *trTree() const { return tr_tree_; }

  // has child trees
  bool hasChildren() const { return bl_tree_ != 0; }

  //----------

 public:
  // add data item to the tree
  void add(DATA *data) {
    const RECT &rect = data->rect();

    if (! rect_.isSet())
      rect_ = rect;

    if (! inside(rect))
      grow(rect);

    addData(data, rect);
  }

 private:
  void addData(DATA *data, const RECT &rect) {
    if (bl_tree_) {
      if        (rect.getXMax() <= br_tree_->rect_.getXMin()) {
        if        (rect.getYMax() <= tl_tree_->rect_.getYMin()) {
          bl_tree_->addData(data, rect); return;
        } else if (rect.getYMin() >= bl_tree_->rect_.getYMax()) {
          tl_tree_->addData(data, rect); return;
        }
      } else if (rect.getXMin() >= bl_tree_->rect_.getXMax()) {
        if        (rect.getYMax() <= tr_tree_->rect_.getYMin()) {
          br_tree_->addData(data, rect); return;
        } else if (rect.getYMin() >= br_tree_->rect_.getYMax()) {
          tr_tree_->addData(data, rect); return;
        }
      }
    }

    dataList_.push_back(data);

    if (! bl_tree_) {
      uint limit = autoSplitLimit();

      if (limit > 0 && dataList_.size() > limit)
        autoSplit();
    }
  }

  //----------

 public:
  // increase size of bounding rect of tree (root)
  void grow(const RECT &rect) {
    T l, b, r, t;

    if (rect_.getXMin() > rect_.getXMax()) {
      l = rect.getXMin();
      b = rect.getYMin();
      r = rect.getXMax();
      t = rect.getYMax();
    }
    else {
      l = std::min(rect_.getXMin(), rect.getXMin());
      b = std::min(rect_.getYMin(), rect.getYMin());
      r = std::max(rect_.getXMax(), rect.getXMax());
      t = std::max(rect_.getYMax(), rect.getYMax());
    }

    if (bl_tree_) {
      const RECT &bl_rect = bl_tree_->rect();
      const RECT &br_rect = br_tree_->rect();
      const RECT &tl_rect = tl_tree_->rect();
      const RECT &tr_rect = tr_tree_->rect();

      bl_tree_->grow(RECT(l                , b                 ,
                          bl_rect.getXMax(), bl_rect.getYMax()));
      br_tree_->grow(RECT(br_rect.getXMin(), b                 ,
                          r                , br_rect.getYMax()));
      tl_tree_->grow(RECT(l                , tl_rect.getYMin(),
                          tl_rect.getXMax(), t                 ));
      tr_tree_->grow(RECT(tr_rect.getXMin(), tr_rect.getYMin(),
                          r                , t                 ));
    }

    rect_ = RECT(l, b, r, t);
  }

  //----------

 public:
  // remove data from tree
  void remove(DATA *data) {
    const RECT &rect = data->rect();

    assert(inside(rect));

    removeData(data, rect);
  }

 private:
  void removeData(DATA *data, const RECT &rect) {
    if (bl_tree_) {
      if      (bl_tree_->inside(rect)) return bl_tree_->removeData(data, rect);
      else if (br_tree_->inside(rect)) return br_tree_->removeData(data, rect);
      else if (tl_tree_->inside(rect)) return tl_tree_->removeData(data, rect);
      else if (tr_tree_->inside(rect)) return tr_tree_->removeData(data, rect);
    }

    dataList_.remove(data);
  }

  //----------

 public:
  // split tree at point (defining vertical and horizontal split)
  void split(T x, T y) {
    if (rect_.getXMin() == rect_.getXMax() && rect_.getYMin() == rect_.getXMax()) return;

    if (! bl_tree_) {
      if (x <= rect_.getXMin() || x >= rect_.getXMax() ||
          y <= rect_.getYMin() || y >= rect_.getYMax()) return;

      RECT blrect(rect_.getXMin(), rect_.getYMin(), x              , y              );
      RECT brrect(x              , rect_.getYMin(), rect_.getXMax(), y              );
      RECT tlrect(rect_.getXMin(), y              , x              , rect_.getYMax());
      RECT trrect(x              , y              , rect_.getXMax(), rect_.getYMax());

      bl_tree_ = new CQChartsQuadTree(this, blrect);
      br_tree_ = new CQChartsQuadTree(this, brrect);
      tl_tree_ = new CQChartsQuadTree(this, tlrect);
      tr_tree_ = new CQChartsQuadTree(this, trrect);

      auto p1 = dataList_.begin();

      while (p1 != dataList_.end()) {
        const RECT &rect = (*p1)->rect();

        if      (bl_tree_->inside(rect)) bl_tree_->add(*p1);
        else if (br_tree_->inside(rect)) br_tree_->add(*p1);
        else if (tl_tree_->inside(rect)) tl_tree_->add(*p1);
        else if (tr_tree_->inside(rect)) tr_tree_->add(*p1);
        else                             { ++p1; continue; }

        auto p = p1++;

        dataList_.erase(p);
      }
    }
    else {
      bl_tree_->split(x, y);
      br_tree_->split(x, y);
      tl_tree_->split(x, y);
      tr_tree_->split(x, y);
    }
  }

 public:
  // automatically split tree (1 or more times)
  // the split point is automatically determined from the data
  bool autoSplit(uint n=1) {
    if (n == 0) return false;

    if (! bl_tree_) {
      if (dataList_.size() <= minTreeSize()) return false;

      T x, y;

      if (splitPoint(x, y)) {
        split(x, y);

        if (bl_tree_) {
          bl_tree_->autoSplit(n - 1);
          br_tree_->autoSplit(n - 1);
          tl_tree_->autoSplit(n - 1);
          tr_tree_->autoSplit(n - 1);
        }
      }
    }
    else {
      bl_tree_->autoSplit(n);
      br_tree_->autoSplit(n);
      tl_tree_->autoSplit(n);
      tr_tree_->autoSplit(n);
    }

    return true;
  }

 private:
  bool splitPoint(T &x, T &y) {
    x = (rect_.getXMax() + rect_.getXMin())/2;
    y = (rect_.getYMax() + rect_.getYMin())/2;

    return (x != rect_.getXMin() && y != rect_.getYMin());
  }

  //-------

 public:
  // get data items inside the specified bounding rect
  void dataInsideRect(const RECT &rect, DataList &dataList) const {
    dataList.clear();

    addDataInsideRect(rect, dataList);
  }

  void addDataInsideRect(const RECT &rect, DataList &dataList) const {
    if (! overlaps(rect))
      return;

    // if tree completely inside, add all items
    if (inside(rect_, rect))
      addTreeDataToList(dataList);
    else {
      for (auto data : dataList_) {
        const RECT &rect1 = data->rect();

        if (inside(rect1, rect))
          dataList.push_back(data);
      }

      if (bl_tree_) {
        bl_tree_->addDataInsideRect(rect, dataList);
        br_tree_->addDataInsideRect(rect, dataList);
        tl_tree_->addDataInsideRect(rect, dataList);
        tr_tree_->addDataInsideRect(rect, dataList);
      }
    }
  }

  //-------

 public:
  // get data items touching the specified bounding rect
  void dataTouchingRect(const RECT &rect, DataList &dataList) const {
    dataList.clear();

    if (overlaps(rect))
      addDataTouchingRect(rect, dataList);
  }

  void addDataTouchingRect(const RECT &rect, DataList &dataList) const {
    // if tree completely inside, add all items
    if (inside(rect_, rect))
      addTreeDataToList(dataList);
    else {
      for (auto data : dataList_) {
        const RECT &rect1 = data->rect();

        if (overlaps(rect1, rect))
          dataList.push_back(data);
      }

      if (bl_tree_) {
        if (rect.getXMin() <= br_tree_->rect_.getXMin()) {
          if (rect.getYMin() <= tl_tree_->rect_.getYMin())
            bl_tree_->addDataTouchingRect(rect, dataList);
          if (rect.getYMax() >= bl_tree_->rect_.getYMax())
            tl_tree_->addDataTouchingRect(rect, dataList);
        }

        if (rect.getXMax() >= bl_tree_->rect_.getXMax()) {
          if (rect.getYMin() <= tr_tree_->rect_.getYMin())
            br_tree_->addDataTouchingRect(rect, dataList);
          if (rect.getYMax() >= br_tree_->rect_.getYMax())
            tr_tree_->addDataTouchingRect(rect, dataList);
        }
      }
    }
  }

  //-------

 public:
  // get data items which have the specified point inside them
  void dataAtPoint(T x, T y, DataList &dataList) const {
    dataList.clear();

    addDataAtPoint(x, y, dataList);
  }

  void addDataAtPoint(T x, T y, DataList &dataList) const {
    if (isEmpty())
      return;

    if (x < rect_.getXMin() || x > rect_.getXMax() ||
        y < rect_.getYMin() || y > rect_.getYMax())
      return;

    for (auto data : dataList_) {
      const RECT &rect = data->rect();

      if (x >= rect.getXMin() && x <= rect.getXMax() &&
          y >= rect.getYMin() && y <= rect.getYMax())
        dataList.push_back(data);
    }

    if (bl_tree_) {
      if (x <= br_tree_->rect_.getXMin()) {
        if (y <= tl_tree_->rect_.getYMin())
          bl_tree_->addDataAtPoint(x, y, dataList);
        if (y >= bl_tree_->rect_.getYMax())
          tl_tree_->addDataAtPoint(x, y, dataList);
      }

      if (x >= bl_tree_->rect_.getXMax()) {
        if (y <= tr_tree_->rect_.getYMin())
          br_tree_->addDataAtPoint(x, y, dataList);
        if (y >= br_tree_->rect_.getYMax())
          tr_tree_->addDataAtPoint(x, y, dataList);
      }
    }
  }

  //-------

 public:
  // get tree which has the specified point inside it
  const CQChartsQuadTree *treeAtPoint(T x, T y) const {
    if (x < rect_.getXMin() || x > rect_.getXMax() ||
        y < rect_.getYMin() || y > rect_.getYMax())
      return 0;

    for (auto data : dataList_) {
      const RECT &rect = data->rect();

      if (x >= rect.getXMin() && x <= rect.getXMax() &&
          y >= rect.getYMin() && y <= rect.getYMax())
        return this;
    }

    if (bl_tree_) {
      const CQChartsQuadTree *tree = 0;

      if ((tree = bl_tree_->treeAtPoint(x, y)) != 0) return tree;
      if ((tree = br_tree_->treeAtPoint(x, y)) != 0) return tree;
      if ((tree = tl_tree_->treeAtPoint(x, y)) != 0) return tree;
      if ((tree = tr_tree_->treeAtPoint(x, y)) != 0) return tree;
    }

    return this;
  }

  //-------

 private:
  void addTreeDataToList(DataList &dataList) const {
    for (auto data : dataList_)
      dataList.push_back(data);

    if (bl_tree_) {
      bl_tree_->addTreeDataToList(dataList);
      br_tree_->addTreeDataToList(dataList);
      tl_tree_->addTreeDataToList(dataList);
      tr_tree_->addTreeDataToList(dataList);
    }
  }

  //-------

 private:
  // is rect inside tree
  bool inside(const RECT &rect) const {
    assert(rect.getXMin() <= rect.getXMax() && rect.getYMin() <= rect.getYMax());

    return inside(rect, rect_);
  }

  // does rect overlap tree
  bool overlaps(const RECT &rect) const {
    assert(rect.getXMin() <= rect.getXMax() && rect.getYMin() <= rect.getYMax());

    return overlaps(rect, rect_);
  }

  // is rect1 inside rect2
  static bool inside(const RECT &rect1, const RECT &rect2) {
    return ((rect1.getXMin() >= rect2.getXMin() && rect1.getXMax() <= rect2.getXMax()) &&
            (rect1.getYMin() >= rect2.getYMin() && rect1.getYMax() <= rect2.getYMax()));
  }

  // does rect1 overlap rect2
  static bool overlaps(const RECT &rect1, const RECT &rect2) {
    return ((rect1.getXMax() >= rect2.getXMin() && rect1.getXMin() <= rect2.getXMax()) &&
            (rect1.getYMax() >= rect2.getYMin() && rect1.getYMin() <= rect2.getYMax()));
  }

  //-------

 public:
  template<typename PROC>
  void process(PROC &proc) {
    for (auto data : dataList_) {
      proc(data);
    }

    if (bl_tree_) bl_tree_->process(proc);
    if (br_tree_) br_tree_->process(proc);
    if (tl_tree_) tl_tree_->process(proc);
    if (tr_tree_) tr_tree_->process(proc);
  }

  template<typename PROC>
  void processRect(PROC &proc) {
    int n = dataList_.size();

    proc(rect_, n);

    for (auto data : dataList_) {
      const RECT &rect1 = data->rect();

      proc(rect1, 0);
    }

    if (bl_tree_) bl_tree_->processRect(proc);
    if (br_tree_) br_tree_->processRect(proc);
    if (tl_tree_) tl_tree_->processRect(proc);
    if (tr_tree_) tr_tree_->processRect(proc);
  }

  //-------

 public:
  uint numElements() const {
    uint n = dataList_.size();

    if (bl_tree_)
      n = bl_tree_->numElements() + br_tree_->numElements() +
          tl_tree_->numElements() + tr_tree_->numElements();

    return n;
  }

  uint minElements() const {
    uint n = dataList_.size();

    if (bl_tree_)
      n = std::min(std::min(bl_tree_->minElements(), br_tree_->minElements()),
                   std::min(tl_tree_->minElements(), tr_tree_->minElements()));

    return n;
  }

  uint maxElements() const {
    uint n = dataList_.size();

    if (bl_tree_)
      n = std::max(std::max(bl_tree_->maxElements(), br_tree_->maxElements()),
                   std::max(tl_tree_->maxElements(), tr_tree_->maxElements()));

    return n;
  }

  uint depth() const {
    return (parent_ ? parent_->depth() + 1 : 1);
  }

  uint maxBorder() const {
    if (bl_tree_) {
      uint nd = dataList_.size();
      uint nl = bl_tree_->maxBorder();
      uint nr = br_tree_->maxBorder();
      uint nb = tl_tree_->maxBorder();
      uint nt = tr_tree_->maxBorder();

      return std::max(nd, std::max(nl, std::max(nr, std::max(nb, nt))));
    }
    else
      return 0;
  }

 public:
  using Trees    = std::vector<const CQChartsQuadTree *>;
  using FitTrees = std::map<int,Trees>;

  RECT fitRect(double w, double h) const {
    FitTrees fitTrees;

    addFitTrees(w, h, fitTrees);

    if (fitTrees.empty())
      return rect_;

    const Trees &trees = (*fitTrees.begin()).second;

    assert(! trees.empty());

    if (trees.size() == 1)
      return trees[0]->rect_;

    const CQChartsQuadTree *minTree { nullptr };
    double                  minArea { 0.0 };

    for (const auto &tree : trees) {
      const RECT &r = tree->rect_;

      double xmid = (r.getXMin() + r.getXMax())/2.0;
      double ymid = (r.getYMin() + r.getYMax())/2.0;

      RECT r1(xmid - w/2, ymid - h/2, xmid + w/2, ymid + h/2);

      double area = 0.0;

      if (tree->parent_) {
        for (auto data : tree->parent_->dataList_) {
          const RECT &r2 = data->rect();

          double xo = std::max(0.0,
            std::min(r1.getXMax(), r2.getXMax()) - std::max(r1.getXMin(), r2.getXMin()));
          double yo = std::max(0.0,
            std::min(r1.getYMax(), r2.getYMax()) - std::max(r1.getYMin(), r2.getYMin()));

          area += xo*yo;
        }
      }

      if (! minTree || area < minArea) {
        minTree = tree;
        minArea = area;
      }
    }

    assert(minTree);

    return minTree->rect_;
  }

 private:
  void addFitTrees(double w, double h, FitTrees &fitTrees) const {
    double w1 = rect_.getXMax() - rect_.getXMin();
    double h1 = rect_.getYMax() - rect_.getYMin();

    if (w > w1 || h > h1)
      return;

    int n = numElements();

    fitTrees[n].push_back(this);

    if (bl_tree_) bl_tree_->addFitTrees(w, h, fitTrees);
    if (br_tree_) br_tree_->addFitTrees(w, h, fitTrees);
    if (tl_tree_) tl_tree_->addFitTrees(w, h, fitTrees);
    if (tr_tree_) tr_tree_->addFitTrees(w, h, fitTrees);
  }

 private:
  static uint *autoSplitLimitP() {
    static uint autoSplitLimit = 16;

    return &autoSplitLimit;
  }

  // if tree contains less than or equal to this number of elements don't bother splitting
  static uint *minTreeSizeP() {
    static uint minTreeSize = 16;

    return &minTreeSize;
  }

 private:
  CQChartsQuadTree* parent_  { nullptr }; //!< parent tree (0 if root)
  RECT              rect_;                //!< bounding rect of tree
  DataList          dataList_;            //!< data list
  CQChartsQuadTree* bl_tree_ { nullptr }; //!< bottom left sub tree
  CQChartsQuadTree* br_tree_ { nullptr }; //!< bottom right sub tree
  CQChartsQuadTree* tl_tree_ { nullptr }; //!< top left sub tree
  CQChartsQuadTree* tr_tree_ { nullptr }; //!< top right sub tree
};

#endif
