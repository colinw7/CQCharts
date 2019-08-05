#ifndef CQChartsCirclePack_H
#define CQChartsCirclePack_H

#include <CQChartsEnclosingCircle.h>
#include <COSNaN.h>
#include <cassert>

#include <set>
#include <map>
#include <vector>
#include <cmath>
#include <sys/types.h>

/*!
 * \brief Circle Pack Node
 * \ingroup Charts
 */
class CQChartsCircleNode {
  public:
   CQChartsCircleNode(double r=1.0, double x=0.0, double y=0.0) :
    r_(r), x_(x), y_(y) {
     assert(! COSNaN::is_nan(x_) && ! COSNaN::is_nan(y_));
   }

   virtual ~CQChartsCircleNode() { }

   virtual double radius() const { return r_; }

   virtual double x() const { return x_; }
   virtual void setX(double x) { assert(! COSNaN::is_nan(x)); x_ = x; }

   virtual double y() const { return y_; }
   virtual void setY(double y) { assert(! COSNaN::is_nan(y)); y_ = y; }

   virtual void setPosition(double x, double y) {
     assert(! COSNaN::is_nan(x) && ! COSNaN::is_nan(y));

     x_ = x; y_ = y;
   }

  protected:
   double r_ { 1.0 }; //!< circle radius
   double x_ { 0.0 }; //!< placed circle x
   double y_ { 0.0 }; //!< placed circle y
};

//---

/*!
 * \brief Pack circle nodes into smallest space
 * \ingroup Charts
 */
template<typename NODE>
class CQChartsCirclePack {
 public:
  using Nodes = std::vector<NODE*>;

 public:
  CQChartsCirclePack() : ind1_(0), ind2_(1) { }

 ~CQChartsCirclePack() { }

  void reset() {
    nodes_.clear();

    ind1_ = 0;
    ind2_ = 1;
  }

  bool addNode(NODE *node) {
    double r = node->radius();

    double xc = 0.0, yc = 0.0;

    if (! findAddPos(r, xc, yc))
      return false;

    node->setPosition(xc, yc);

    nodes_.push_back(node);

    return true;
  }

  const Nodes &nodes() const { return nodes_; }

  NODE *node(int i) const { assert(i >= 0 && i < int(nodes_.size())); return nodes_[i]; }

  uint size() const { return nodes_.size(); }

  void boundingBox(double &xmin, double &ymin, double &xmax, double &ymax) const {
    xmin = 0.0;
    ymin = 0.0;
    xmax = 0.0;
    ymax = 0.0;

    int n = size();

    for (int i = 0; i < n; ++i) {
      NODE *node = this->node(i);

      xmin = std::min(xmin, node->x() - node->radius());
      ymin = std::min(ymin, node->y() - node->radius());
      xmax = std::max(xmax, node->x() + node->radius());
      ymax = std::max(ymax, node->y() + node->radius());
    }
  }

  bool boundingCircle(double &xc, double &yc, double &r) const {
    xc = 0.0;
    yc = 0.0;
    r  = 0.0;

    CQChartsEnclosingCircle enclose;

    int n = size();

    if (n <= 0)
      return false;

    for (int i = 0; i < n; ++i) {
      NODE *node = this->node(i);

      enclose.addCircle(node->x(), node->y(), node->radius());
    }

    enclose.calc(xc, yc, r);

    return true;
  }

 private:
  bool findAddPos(double r, double &xc, double &yc) const {
    int n = size();

    if (n == 0) {
      xc = 0.0;
      yc = 0.0;

      return true;
    }

    if (n == 1) {
      xc = node(0)->radius() + r;
      yc = 0.0;

      return true;
    }

    // try to place next to node starting at last
    while (ind2_ > ind1_) {
      if (testIndices(ind1_, ind2_, r, xc, yc)) {
        ind2_ = n;

        return true;
      }

      --ind2_;
    }

    return testIndices(ind1_, ind2_, r, xc, yc, /*force*/true);
  }

  // intersect last node with each previous node until find place to add with no overlap
  bool testIndices(int ind1, int ind2, double r, double &xc, double &yc, bool force=false) const {
    NODE *node2 = node(ind2);

    int ind = ind2 - 1;

    while (ind >= ind1) {
      NODE *node1 = node(ind);

      //std::cerr << "Test: " << node1->id() << " and " << node2->id() << "\n";

      double xi1, yi1, xi2, yi2;

      // get overlap of extended circle at last node and previous node
      if (CircleCircleIntersect(node1->x(), node1->y(), node1->radius() + r,
                                node2->x(), node2->y(), node2->radius() + r,
                                &xi1, &yi1, &xi2, &yi2)) {
        int orient1 = orientation(node1->x(), node1->y(), node2->x(), node2->y(), xi1, yi1);
        int orient2 = orientation(node1->x(), node1->y(), node2->x(), node2->y(), xi2, yi2);

        bool valid1 = (orient1 >= 0);
        bool valid2 = (orient2 >= 0);

        if (! force) {
          valid1 = (valid1 && ! isTouching(xi1, yi1, r));
          valid2 = (valid2 && ! isTouching(xi2, yi2, r));
        }

        if (valid1 || valid2) {
          double a1 = (valid1 ? calcAtan(yi1, xi1) : 1E50);
          double a2 = (valid2 ? calcAtan(yi2, xi2) : 1E50);

          if (a1 < a2) {
            xc = xi1;
            yc = yi1;
          }
          else {
            xc = xi2;
            yc = yi2;
          }

          //std::cerr << "Place: " << xc << " " << yc << "\n";

          return true;
        }
      }

      --ind;
    }

    return false;
  }

  double calcAtan(double dy, double dx) const {
    double a = atan2(dy, dx);

    if (a < 0) a = 2*M_PI - a;

    return a;
  }

  int orientation(double x1, double y1, double x2, double y2, double x3, double y3) const {
    double dx1 = x2 - x1;
    double dy1 = y2 - y1;

    double dx2 = x3 - x2;
    double dy2 = y3 - y2;

    double product = dx1*dy2 - dy1*dx2;

    if      (product > 0.0) return  1;
    else if (product < 0.0) return -1;
    else                    return  0;
  }

  bool isTouching(double x, double y, double r) const {
    int n = size();

    for (int i = n - 1; i >= 0; --i) {
      NODE *node = this->node(i);

      double dx = x - node->x();
      double dy = y - node->y();

      double d1 = std::hypot(dx, dy);
      double d2 = node->radius() + r;

      if (d1 < d2 && ! realEq(d1, d2))
        return true;
    }

    return false;
  }

 private:
  static bool CircleCircleIntersect(double x1, double y1, double r1,
                                    double x2, double y2, double r2,
                                    double *xi1, double *yi1, double *xi2, double *yi2) {
    // distance between circle centers
    double dx = x2 - x1;
    double dy = y2 - y1;

    double d = std::hypot(dx, dy);

    double sr12 = r1 + r2;
    double dr12 = fabs(r1 - r2);

    if (d > sr12) return false; // separate
    if (d < dr12) return false; // contained

    if (d == 0.0 && r1 == r2) return false; // coincident

    // (x3,y3) is the point where the line through the circle intersection points
    // crosses the line between the circle centers.

    // calc distance from point 1 to point 3.
    double a = (r1*r1 - r2*r2 + d*d)/(2.0*d);

    // calc distance from point 3 to either intersection points
    double h = sqrt(std::max(r1*r1 - a*a, 0.0));

    // calc the coordinates of point 3
    double x3 = x1 + a*dx/d;
    double y3 = y1 + a*dy/d;

    // calc the offsets of the intersection points from point 3
    double rx = -h*dy/d;
    double ry =  h*dx/d;

    // calc the absolute intersection points
    *xi1 = x3 + rx;
    *xi2 = x3 - rx;
    *yi1 = y3 + ry;
    *yi2 = y3 - ry;

    assert(! COSNaN::is_nan(*xi1) && ! COSNaN::is_nan(*yi1) &&
           ! COSNaN::is_nan(*xi2) && ! COSNaN::is_nan(*yi2));

    return true;
  }

  static bool realEq(double r1, double r2) {
    return (fabs((r1) - (r2)) < 1E-5);
  }

 private:
  Nodes       nodes_; //!< circle nodes
  mutable int ind1_;  //!< last but one placed circle node index
  mutable int ind2_;  //!< last placed circle node index
};

#endif
