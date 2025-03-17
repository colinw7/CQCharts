#include <CQRectiConnect.h>
#include <CAStarNode.h>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  auto *rapp = new CQRectiConnect::App;

  rapp->show();

  return app.exec();
}

namespace CQRectiConnect {

App::
App()
{
  rect1_ = CBBox2D(10, 10, 30, 30);
  rect2_ = CBBox2D(70, 70, 90, 90);

  displayRange_.setWindowRange(0, 0, 100, 100);
}

void
App::
resizeEvent(QResizeEvent *)
{
  displayRange_.setPixelRange(0, 0, width() - 1, height() - 1);
}

void
App::
paintEvent(QPaintEvent *)
{
  if (needsCalc_) {
    needsCalc_ = false;

    points_.clear();
    route_ .clear();

    (void) calcRoute(points_, route_);
  }

  //---

  QPainter painter(this);

  painter.fillRect(rect(), Qt::white);

  auto pointToPixel = [&](const CPoint2D &p) {
    CPoint2D p1;
    displayRange_.windowToPixel(p, p1);
    return p1;
  };

  auto bboxToPixel = [&](const Rect &bbox) {
    auto p1 = pointToPixel(bbox.getMin());
    auto p2 = pointToPixel(bbox.getMax());

    return CBBox2D(p1, p2);
  };

  auto rect1 = bboxToPixel(rect1_);
  auto rect2 = bboxToPixel(rect2_);

  auto drawRect = [&](const CBBox2D &bbox) {
    auto qrect = QRectF(bbox.getLeft(), bbox.getBottom(), bbox.getWidth(), bbox.getHeight());
    painter.drawRect(qrect);
  };

  auto drawPoint = [&](const CPoint2D &p) {
    auto p1 = pointToPixel(p);

    painter.drawLine(p1.x, p1.y - 2, p1.x, p1.y + 2);
    painter.drawLine(p1.x - 2, p1.y, p1.x + 2, p1.y);
  };

  auto drawEdge = [&](Edge *edge) {
    auto p1 = pointToPixel(edge->node1()->p());
    auto p2 = pointToPixel(edge->node2()->p());

    painter.drawLine(p1.x, p1.y, p2.x, p2.y);
  };

  painter.setPen(Qt::blue);

  drawRect(rect1);
  drawRect(rect2);

  if (debug_) {
    painter.setPen(Qt::green);

    for (auto *edge : graph_->edges())
      drawEdge(edge);

    painter.setPen(Qt::red);

    auto np = points_.size();

    for (size_t i = 0; i < np; ++i)
      drawPoint(points_[i]);
  }

  painter.setPen(Qt::black);

  auto npr = route_.size();

  for (size_t i = 1; i < npr; ++i) {
    auto p1 = pointToPixel(route_[i - 1]);
    auto p2 = pointToPixel(route_[i    ]);

    painter.drawLine(p1.x, p1.y, p2.x, p2.y);
  }
}

void
App::
mousePressEvent(QMouseEvent *e)
{
  pressed_ = true;

  double px, py;
  displayRange_.pixelToWindow(e->x(), e->y(), &px, &py);
  pressPoint_ = CPoint2D(px, py);

  inRect1_ = rect1_.inside(pressPoint_.x, pressPoint_.y);
  inRect2_ = rect2_.inside(pressPoint_.x, pressPoint_.y);
}

void
App::
mouseMoveEvent(QMouseEvent *e)
{
  if (! pressed_)
    return;

  double px, py;
  displayRange_.pixelToWindow(e->x(), e->y(), &px, &py);
  movePoint_ = CPoint2D(px, py);

  if      (inRect1_) {
    auto dx = movePoint_.x - pressPoint_.x;
    auto dy = movePoint_.y - pressPoint_.y;

    rect1_.moveBy(CPoint2D(dx, dy));

    needsCalc_ = true;

    update();
  }
  else if (inRect2_) {
    auto dx = movePoint_.x - pressPoint_.x;
    auto dy = movePoint_.y - pressPoint_.y;

    rect2_.moveBy(CPoint2D(dx, dy));

    needsCalc_ = true;

    update();
  }

  pressPoint_ = movePoint_;
}

void
App::
mouseReleaseEvent(QMouseEvent *e)
{
  mouseMoveEvent(e);

  pressed_ = false;
}

void
App::
keyPressEvent(QKeyEvent *e)
{
  if      (e->key() == Qt::Key_D) {
    debug_ = ! debug_;

    update();
  }
  else if (e->key() == Qt::Key_1) {
    rect1_.setSide(Rect::nextSide(rect1_.side()));

    needsCalc_ = true;
    update();
  }
  else if (e->key() == Qt::Key_2) {
    rect2_.setSide(Rect::nextSide(rect2_.side()));

    needsCalc_ = true;
    update();
  }
}

//---

class ShortestPath : public CAStar<Node> {
 public:
  ShortestPath(Graph *graph) :
   graph_(graph) {
  }

  virtual ~ShortestPath() { }

  Graph *graph() const { return graph_; }

  // smallest/optimal cost to goal
  double pathCostEstimate(Node * /*startNode*/, Node * /*goalNode*/) override {
    return 1;
  }

  double traverseCost(Node *node, Node *newNode) override {
    NodeArray nodes;
    getPathNodes(node, nodes);
    auto n = nodes.size();
    if (n > 1) {
      auto p1 = nodes[n - 2]->p();
      auto p2 = nodes[n - 1]->p();
      auto p3 = newNode     ->p();
      auto dx1 = (p2.x - p1.x);
      auto dy1 = (p2.y - p1.y);
      auto dx2 = (p3.x - p2.x);
      auto dy2 = (p3.y - p2.y);
      if (dx1 == 0 && dx2 == 0) return 1;
      if (dy1 == 0 && dy2 == 0) return 1;
    }
    return 2;
  }

  NodeList getNextNodes(Node *node) const override {
    NodeList nodes;

    for (const auto &edge : node->edges())
      nodes.push_back(node == edge->node1() ? edge->node2() : edge->node1());

    return nodes;
  }

 private:
  Graph *graph_ { nullptr };
};

//---

bool
App::
calcRoute(std::vector<CPoint2D> &points, std::vector<CPoint2D> &route) const
{
  auto rc = calcRoute1(points, route, true);

  if (! rc)
    rc = calcRoute1(points, route, false);

  return rc;
}

bool
App::
calcRoute1(std::vector<CPoint2D> &points, std::vector<CPoint2D> &route, bool overlap) const
{
  auto p1 = rect1_.connectPoint();
  auto p2 = rect2_.connectPoint();

  auto w1 = 0.1*rect1_.getWidth ();
  auto h1 = 0.1*rect1_.getHeight();
  auto w2 = 0.1*rect2_.getWidth ();
  auto h2 = 0.1*rect2_.getHeight();

  auto p3 = rect1_.offsetPoint();
  auto p4 = rect2_.offsetPoint();

  auto xm = (p3.x + p4.x)/2.0;
  auto ym = (p3.y + p4.y)/2.0;

  auto p5 = CPoint2D(xm, p3.y);
  auto p6 = CPoint2D(xm, p4.y);

  auto xl1 = rect1_.getXMin() - w1;
  auto yb1 = rect1_.getYMin() - h1;
  auto xr1 = rect1_.getXMax() + w1;
  auto yt1 = rect1_.getYMax() + h1;

  auto xl2 = rect2_.getXMin() - w2;
  auto yb2 = rect2_.getYMin() - h2;
  auto xr2 = rect2_.getXMax() + w2;
  auto yt2 = rect2_.getYMax() + h2;

  auto eps = 0.01;

  auto orect1 = CBBox2D(CPoint2D(xl1 + eps, yb1 + eps), CPoint2D(xr1 - eps, yt1 - eps));
  auto orect2 = CBBox2D(CPoint2D(xl2 + eps, yb2 + eps), CPoint2D(xr2 - eps, yt2 - eps));

  //---

  std::set<double> xset, yset;

  auto addPoint = [&](const CPoint2D &p) {
    xset.insert(p.x);
    yset.insert(p.y);
  };

  addPoint(p1);
  addPoint(p2);
  addPoint(p3);
  addPoint(p4);
  addPoint(p5);
  addPoint(p6);

  addPoint(CPoint2D(xm, ym));

  addPoint(CPoint2D(xl1, yb1));
  addPoint(CPoint2D(xr1, yt1));
  addPoint(CPoint2D(xl2, yb2));
  addPoint(CPoint2D(xr2, yt2));

  auto setVector = [](const std::set<double> &s) {
    std::vector<double> v;
    for (const auto &r : s)
      v.push_back(r);
    return v;
  };

  auto xvals = setVector(xset);
  auto yvals = setVector(yset);

  //---

  delete graph_;

  graph_ = new Graph;

  //---

  Node *startNode  { nullptr };
  Node *startNode1 { nullptr };
  Node *endNode    { nullptr };
  Node *endNode1   { nullptr };

  auto nx = xvals.size();
  auto ny = yvals.size();

  using NodeMap     = std::map<size_t, Node *>;
  using NodeNodeMap = std::map<size_t, NodeMap>;

  NodeNodeMap nodeNodeMap;

  for (size_t ix = 0; ix < nx; ++ix) {
    auto x = xvals[ix];

    for (size_t iy = 0; iy < ny; ++iy) {
      auto y = yvals[iy];

      CPoint2D p(x, y);

      auto *node = new Node(p);

      graph_->addNode(node);

      nodeNodeMap[ix][iy] = node;

      if      (p == p1) startNode  = node;
      else if (p == p2) endNode    = node;
      else if (p == p3) startNode1 = node;
      else if (p == p4) endNode1   = node;

      points.push_back(p);
    }
  }

#if 0
  auto isCheckNode = [&](Node *node) {
    if (node == startNode ) return false;
    if (node == endNode   ) return false;
    if (node == startNode1) return false;
    if (node == endNode1  ) return false;
    return true;
  };
#endif

  auto addEdge = [&](Node *node1, Node *node2, bool check=true) {
#if 0
    if (isCheckNode(node1) && isCheckNode(node2)) {
#else
    if (check) {
#endif
      const auto &p1 = node1->p();
      const auto &p2 = node2->p();

      if (orect1.inside(p1.x, p1.y)) return;
      if (orect2.inside(p1.x, p1.y)) return;
      if (orect1.inside(p2.x, p2.y)) return;
      if (orect2.inside(p2.x, p2.y)) return;
    }

    auto *edge = new Edge(node1, node2);
    graph_->addEdge(edge);
  };

  for (size_t ix = 0; ix < nx; ++ix) {
    for (size_t iy = 0; iy < ny; ++iy) {
      auto *node = nodeNodeMap[ix][iy];

      Node *lnode { nullptr };
      Node *rnode { nullptr };
      Node *tnode { nullptr };
      Node *bnode { nullptr };

      if (ix > 0     ) lnode = nodeNodeMap[ix - 1][iy];
      if (ix < nx - 1) rnode = nodeNodeMap[ix + 1][iy];
      if (iy > 0     ) bnode = nodeNodeMap[ix][iy - 1];
      if (iy < ny - 1) tnode = nodeNodeMap[ix][iy + 1];

      if (lnode) addEdge(lnode, node, overlap);
      if (rnode) addEdge(node, rnode, overlap);
      if (bnode) addEdge(bnode, node, overlap);
      if (tnode) addEdge(node, tnode, overlap);
    }
  }

  addEdge(startNode, startNode1, /*check*/false);
  addEdge(endNode1 , endNode   , /*check*/false);

  //---

  ShortestPath shortestPath(graph_);

  ShortestPath::NodeList nodes;

  if (! shortestPath.search(startNode, endNode, nodes))
    return false;

  for (auto *node : nodes)
    route.push_back(node->p());

  return true;
}

}
