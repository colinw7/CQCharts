#ifndef CQChartsRectiConnect_H
#define CQChartsRectiConnect_H

#include <CQChartsGeom.h>

#include <vector>
#include <optional>

namespace CQChartsRectiConnect {

using Point = CQChartsGeom::Point;
using BBox  = CQChartsGeom::BBox;

class Node;
class Edge;

using Nodes = std::vector<Node *>;
using Edges = std::vector<Edge *>;

//---

class Node {
 public:
  Node(const Point &p);

  const Point &p() const { return p_; }

  const Edges &edges() const { return edges_; }

  void addEdge(Edge *edge);

 private:
  Point p_;
  Edges edges_;
};

//---

class Edge {
 public:
  Edge(Node *node1, Node *node2);

  Node *node1() const { return node1_; }
  Node *node2() const { return node2_; }

 private:
  Node* node1_ { nullptr };
  Node* node2_ { nullptr };
};

//---

class Graph {
 public:
  Graph();
 ~Graph();

  const Edges &edges() const { return edges_; }

  void addNode(Node *node);
  void addEdge(Edge *edge);

 private:
  Nodes nodes_;
  Edges edges_;
};

//---

class Rect {
 public:
  enum class Side {
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
  };

  using OptPoint = std::optional<Point>;

 public:
  static Side nextSide(const Side &side) {
    switch (side) {
      default:
      case Side::TOP   : return Side::RIGHT;
      case Side::BOTTOM: return Side::LEFT;
      case Side::LEFT  : return Side::TOP;
      case Side::RIGHT : return Side::BOTTOM;
    }
  };

  Rect() { }

  Rect(const BBox &bbox) :
   bbox_(bbox) {
  }

  const BBox &bbox() const { return bbox_; }

  const Side &side() const { return side_; }
  void setSide(const Side &s) { side_ = s; }

  void setConnectPoint(const Point &p) { connectPoint_ = p; }

  Point calcConnectPoint() const {
    if (connectPoint_) return connectPoint_.value();

    switch (side_) {
      default:
      case Side::TOP   : return Point(getXMid(), getYMax());
      case Side::BOTTOM: return Point(getXMid(), getYMin());
      case Side::LEFT  : return Point(getXMin(), getYMid());
      case Side::RIGHT : return Point(getXMax(), getYMid());
    }
  }

  Point offsetPoint(double offset) const {
    auto w = offset*bbox_.getWidth ();
    auto h = offset*bbox_.getHeight();

    switch (side_) {
      default:
      case Side::TOP   : return Point(getXMid()    , getYMax() + h);
      case Side::BOTTOM: return Point(getXMid()    , getYMin() - h);
      case Side::LEFT  : return Point(getXMin() - w, getYMid()    );
      case Side::RIGHT : return Point(getXMax() + w, getYMid()    );
    }
  }

  double getWidth () const { return bbox_.getWidth (); }
  double getHeight() const { return bbox_.getHeight(); }

  Point getMin() const { return bbox_.getMin(); }
  Point getMax() const { return bbox_.getMax(); }

  double getXMin() const { return bbox_.getXMin(); }
  double getXMid() const { return bbox_.getXMid(); }
  double getXMax() const { return bbox_.getXMax(); }

  double getYMin() const { return bbox_.getYMin(); }
  double getYMid() const { return bbox_.getYMid(); }
  double getYMax() const { return bbox_.getYMax(); }

  bool inside(const Point &p) const { return bbox_.inside(p); }

  void moveBy(const Point &d) { bbox_.moveBy(d); }

 public:
  BBox bbox_;
  Side side_ { Side::TOP };

  OptPoint connectPoint_;
};

//---

class Router {
 public:
  using Points = std::vector<Point>;

 public:
  Router();
 ~Router();

  double offset() const { return offset_; }
  void setOffset(double r) { offset_ = r; }

  bool calcRoute(const Rect &rect1, const Rect &rect2, Points &route) const;

  Graph *graph() const { return graph_; }

  const Points &points() const { return points_; }

 private:
  bool calcRoute1(const Rect &rect1, const Rect &rect2, Points &points,
                  Points &route, bool overlap) const;

 private:
  mutable Graph *graph_ { nullptr };

  double offset_ { 0.1 };

  mutable Points points_;
};

}

#endif
