#ifndef CirclePack_H
#define CirclePack_H

#include <set>
#include <map>
#include <vector>
#include <cmath>
#include <cassert>
#include <sys/types.h>

#include <CMathGeom2D.h>
#include <CMathMacros.h>
#include <CEnclosingCircle.h>

class CircleNode {
  public:
   CircleNode(double r=1.0, double x=0.0, double y=0.0) :
    r_(r), x_(x), y_(y) {
   }

   virtual ~CircleNode() { }

   virtual double radius() const { return r_; }

   virtual double x() const { return x_; }
   virtual double y() const { return y_; }

   virtual void setPosition(double x, double y) { x_ = x; y_ = y; }

  protected:
   double r_;
   double x_;
   double y_;
};

template<typename NODE>
class CirclePack {
 public:
  typedef std::vector<NODE *> Nodes;

 public:
  CirclePack() : ind1_(0), ind2_(1) { }

 ~CirclePack() {
    for (auto &n : nodes_)
      delete n;
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

  uint size() const { return nodes_.size(); }

  void boundingBox(double &xmin, double &ymin, double &xmax, double &ymax) const {
    xmin = 0.0;
    ymin = 0.0;
    xmax = 0.0;
    ymax = 0.0;

    int n = nodes_.size();

    for (int i = 0; i < n; ++i) {
      NODE *node = nodes_[i];

      xmin = std::min(xmin, node->x() - node->radius());
      ymin = std::min(ymin, node->y() - node->radius());
      xmax = std::max(xmax, node->x() + node->radius());
      ymax = std::max(ymax, node->y() + node->radius());
    }
  }

  void boundingCircle(double &xc, double &yc, double &r) const {
    CEnclosingCircle enclose;

    int n = nodes_.size();

    for (int i = 0; i < n; ++i) {
      NODE *node = nodes_[i];

      enclose.addCircle(node->x(), node->y(), node->radius());
    }

    enclose.calc(xc, yc, r);
  }

 private:
  bool findAddPos(double r, double &xc, double &yc) const {
    int n = nodes_.size();

    if (n == 0) {
      xc = 0.0;
      yc = 0.0;

      return true;
    }

    if (n == 1) {
      xc = nodes_[0]->radius() + r;
      yc = 0.0;

      return true;
    }

    while (ind2_ > ind1_) {
      if (testIndices(ind1_, ind2_, r, xc, yc)) {
        ind2_ = n;

        return true;
      }

      --ind2_;
    }

    return false;
  }

  bool testIndices(int ind1, int ind2, double r, double &xc, double &yc) const {
    while (ind1 < ind2) {
      NODE *node1 = nodes_[ind1];
      NODE *node2 = nodes_[ind2];

      //std::cerr << "Test: " << node1->id() << " and " << node2->id() << std::endl;

      double xi1, yi1, xi2, yi2;

      if (CMathGeom2D::CircleCircleIntersect(node1->x(), node1->y(), node1->radius() + r,
                                             node2->x(), node2->y(), node2->radius() + r,
                                             &xi1, &yi1, &xi2, &yi2)) {
        int orient1 = orientation(node1->x(), node1->y(), node2->x(), node2->y(), xi1, yi1);
        int orient2 = orientation(node1->x(), node1->y(), node2->x(), node2->y(), xi2, yi2);

        bool valid1 = (orient1 >= 0);
        bool valid2 = (orient2 >= 0);

        valid1 = (valid1 && ! isTouching(xi1, yi1, r));
        valid2 = (valid2 && ! isTouching(xi2, yi2, r));

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

          //std::cerr << "Place: " << xc << " " << yc << std::endl;

          return true;
        }
      }

      ++ind1;
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
    int n = nodes_.size();

    for (int i = 0; i < n; ++i) {
      NODE *node = nodes_[i];

      double dx = x - node->x();
      double dy = y - node->y();

      double d1 = sqrt(dx*dx + dy*dy);
      double d2 = node->radius() + r;

      if (d1 < d2 && ! REAL_EQ(d1, d2))
        return true;
    }

    return false;
  }

 private:
  Nodes       nodes_;
  mutable int ind1_;
  mutable int ind2_;
};

#endif
