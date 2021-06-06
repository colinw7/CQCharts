#ifndef CQChartsTreeMapPlace_H
#define CQChartsTreeMapPlace_H

#include <CQChartsGeom.h>
#include <vector>

class CQChartsTreeMapPlace {
 public:
  using BBox = CQChartsGeom::BBox;

 public:
  struct IArea {
    int    i    { 0 };
    double area { 0.0 };
    void*  data { nullptr };
    BBox   bbox;

    IArea(int i, double area, void *data=nullptr) :
     i(i), area(area), data(data) {
    }
  };

  using IAreas = std::vector<IArea>;

 private:
  class BBoxIAreas {
   public:
    BBoxIAreas() { }

    BBoxIAreas(const BBox &bbox, const IAreas &iareas) :
     bbox_(bbox), iareas_(iareas) {
    }

   ~BBoxIAreas() {
     delete bboxIAreas1_;
     delete bboxIAreas2_;
    }

    void placeAreas() {
      if (iareas_.size() < 2)
        return;

      // divide areas in two
      double a = bbox_.area();

      double midArea = a/2.0;

      IAreas iareas1, iareas2;

      int splitPos = 0;

      double a1 = 0.0;

      for (auto &iarea : iareas_) {
        if (a1 >= midArea)
          break;

        ++splitPos;

        a1 += iarea.area;
      }

      if      (splitPos == 0)
        splitPos = 1;
      else if (splitPos >= int(iareas_.size()))
        splitPos = iareas_.size() - 1;

      for (auto &iarea : iareas_) {
        if (splitPos > 0)
          iareas1.push_back(iarea);
        else
          iareas2.push_back(iarea);

        --splitPos;
      }

      assert(! iareas1.empty() && ! iareas2.empty());

      //---

      // determine bbox for each area
      a1 = 0.0;

      for (auto &iarea : iareas1)
        a1 += iarea.area;

      //double a2 = 0.0;

      //for (auto &iarea : iareas2)
      //  a2 += iarea.area;

      double f = a1/a;

      double w = bbox_.getWidth ();
      double h = bbox_.getHeight();

      BBox bbox1, bbox2;

      if (w >= h) {
        // divide vertically
        double w1 = w*f;

        bbox1 = BBox(bbox_.getXMin(), bbox_.getYMin(), bbox_.getXMin() + w1, bbox_.getYMax());
        bbox2 = BBox(bbox_.getXMin() + w1, bbox_.getYMin(), bbox_.getXMax(), bbox_.getYMax());
      }
      else {
        // divide horizontally
        double h1 = h*f;

        bbox1 = BBox(bbox_.getXMin(), bbox_.getYMin(), bbox_.getXMax(), bbox_.getYMin() + h1);
        bbox2 = BBox(bbox_.getXMin(), bbox_.getYMin() + h1, bbox_.getXMax(), bbox_.getYMax());
      }

      //---

      // create new placement set
      bboxIAreas1_ = new BBoxIAreas(bbox1, iareas1);
      bboxIAreas2_ = new BBoxIAreas(bbox2, iareas2);

      iareas_.clear();

      //---

      // iterate
      bboxIAreas1_->placeAreas();
      bboxIAreas2_->placeAreas();
    }

    void processAreas(const std::function<void (const BBox &bbox, const IArea &iarea)> &proc) {
      for (const auto &iarea : iareas_) {
        proc(bbox_, iarea);
      }

      if (bboxIAreas1_)
        bboxIAreas1_->processAreas(proc);

      if (bboxIAreas2_)
        bboxIAreas2_->processAreas(proc);
    }

   private:
    BBox        bbox_;
    IAreas      iareas_;
    BBoxIAreas* bboxIAreas1_ { nullptr };
    BBoxIAreas* bboxIAreas2_ { nullptr };
  };

 public:
  CQChartsTreeMapPlace(const BBox &bbox) :
   bbox_(bbox) {
    assert(bbox_.isValid());
  }

  void addValue(double v, void *data=nullptr) {
    values_.emplace_back(v, data);

    sum_ += v;
  }

  void placeValues() {
    int nv = values_.size();
    if (! nv) return;

    if (sum_ <= 0.0) return;

    double a  = bbox_.area();
    double da = a/sum_;

    for (int i = 0; i < nv; ++i) {
      double a1   = da*values_[i].value;
      void*  data = values_[i].data;

      iareas_.emplace_back(i, a1, data);
    }

    // reverse sort (largest to smallest)
    std::sort(iareas_.begin(), iareas_.end(),
      [](const IArea &lhs, const IArea &rhs) { return (lhs.area > rhs.area); });

    bboxIAreas_ = BBoxIAreas(bbox_, iareas_);

    bboxIAreas_.placeAreas();
  }

  void processAreas(const std::function<void (const BBox &bbox, const IArea &iarea)> &proc) {
    bboxIAreas_.processAreas(proc);
  }

 private:
  struct Value {
    double value { 0.0 };
    void*  data  { nullptr };

    Value(double value, void *data) :
     value(value), data(data) {
    }
  };

  using Values = std::vector<Value>;

  BBox       bbox_;
  Values     values_;
  double     sum_    { 0.0 };
  IAreas     iareas_;
  BBoxIAreas bboxIAreas_;
};

#endif
