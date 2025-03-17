#ifndef CQRectiConnect_H
#define CQRectiConnect_H

#include <CDisplayRange2D.h>
#include <CBBox2D.h>
#include <CPoint2D.h>

#include <QFrame>
#include <vector>

namespace CQRectiConnect {

struct Node;
struct Edge;

using Nodes = std::vector<Node *>;
using Edges = std::vector<Edge *>;

class Node {
 public:
  Node(const CPoint2D &p) :
   p_(p) {
  }

  const CPoint2D &p() const { return p_; }

  const Edges &edges() const { return edges_; }

  void addEdge(Edge *edge) {
    edges_.push_back(edge);
  }

 private:
  CPoint2D p_;
  Edges    edges_;

};

class Edge {
 public:
  Edge(Node *node1, Node *node2) :
   node1_(node1), node2_(node2) {
    node1->addEdge(this);
    node2->addEdge(this);
  }

  Node *node1() const { return node1_; }
  Node *node2() const { return node2_; }

 private:
  Node* node1_ { nullptr };
  Node* node2_ { nullptr };
};

class Graph {
 public:
  Graph() { }

 ~Graph() {
    for (auto *e : edges_)
      delete e;
  }

  const Edges &edges() const { return edges_; }

  void addNode(Node *node) {
    nodes_.push_back(node);
  }

  void addEdge(Edge *edge) {
    edges_.push_back(edge);
  }

 private:
  Nodes nodes_;
  Edges edges_;
};

class Rect {
 public:
  enum class Side {
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
  };

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

  Rect(const CBBox2D &bbox) :
   bbox_(bbox) {
  }

  const CBBox2D &bbox() const { return bbox_; }

  const Side &side() const { return side_; }
  void setSide(const Side &s) { side_ = s; }

  CPoint2D connectPoint() const {
    switch (side_) {
      default:
      case Side::TOP   : return CPoint2D(getXMid(), getYMax());
      case Side::BOTTOM: return CPoint2D(getXMid(), getYMin());
      case Side::LEFT  : return CPoint2D(getXMin(), getYMid());
      case Side::RIGHT : return CPoint2D(getXMax(), getYMid());
    }
  }

  CPoint2D offsetPoint() const {
    auto w = 0.1*bbox_.getWidth ();
    auto h = 0.1*bbox_.getHeight();

    switch (side_) {
      default:
      case Side::TOP   : return CPoint2D(getXMid()    , getYMax() + h);
      case Side::BOTTOM: return CPoint2D(getXMid()    , getYMin() - h);
      case Side::LEFT  : return CPoint2D(getXMin() - w, getYMid()    );
      case Side::RIGHT : return CPoint2D(getXMax() + w, getYMid()    );
    }
  }

  double getWidth () const { return bbox_.getWidth (); }
  double getHeight() const { return bbox_.getHeight(); }

  CPoint2D getMin() const { return bbox_.getMin(); }
  CPoint2D getMax() const { return bbox_.getMax(); }

  double getXMin() const { return bbox_.getXMin(); }
  double getXMid() const { return bbox_.getXMid(); }
  double getXMax() const { return bbox_.getXMax(); }

  double getYMin() const { return bbox_.getYMin(); }
  double getYMid() const { return bbox_.getYMid(); }
  double getYMax() const { return bbox_.getYMax(); }

  bool inside(double x, double y) const { return bbox_.inside(x, y); }

  void moveBy(const CPoint2D &d) { bbox_.moveBy(d); }

 public:
  CBBox2D bbox_;
  Side    side_ { Side::TOP };
};

class App : public QFrame {
 public:
  App();

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

  void keyPressEvent(QKeyEvent *e) override;

  bool calcRoute(std::vector<CPoint2D> &points, std::vector<CPoint2D> &route) const;

 private:
  bool calcRoute1(std::vector<CPoint2D> &points, std::vector<CPoint2D> &route, bool overlap) const;

 private:
  CDisplayRange2D displayRange_;

  Rect rect1_;
  Rect rect2_;

  bool needsCalc_ { true };

  std::vector<CPoint2D> points_;
  std::vector<CPoint2D> route_;

  mutable Graph *graph_ { nullptr };

  bool pressed_;

  CPoint2D pressPoint_;
  CPoint2D movePoint_;

  bool inRect1_ { false };
  bool inRect2_ { false };

  bool debug_ { false };
};

}

#endif
