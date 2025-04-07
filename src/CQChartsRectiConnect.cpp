#include <CQChartsRectiConnect.h>
#include <CAStarNode.h>

namespace CQChartsRectiConnect {

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

Router::
Router()
{
}

Router::
~Router()
{
  delete graph_;
}

bool
Router::
calcRoute(const Rect &rect1, const Rect &rect2, Points &route) const
{
  auto rc = calcRoute1(rect1, rect2, points_, route, true);

  if (! rc)
    rc = calcRoute1(rect1, rect2, points_, route, false);

  return rc;
}

bool
Router::
calcRoute1(const Rect &rect1, const Rect &rect2, Points &points,
           Points &route, bool overlap) const
{
  points.clear();

  auto p1 = rect1.calcConnectPoint();
  auto p2 = rect2.calcConnectPoint();

  auto w1 = offset()*rect1.getWidth ();
  auto h1 = offset()*rect1.getHeight();
  auto w2 = offset()*rect2.getWidth ();
  auto h2 = offset()*rect2.getHeight();

  auto p3 = rect1.offsetPoint(offset());
  auto p4 = rect2.offsetPoint(offset());

  auto xm = (p3.x + p4.x)/2.0;
  auto ym = (p3.y + p4.y)/2.0;

  auto p5 = Point(xm, p3.y);
  auto p6 = Point(xm, p4.y);

  auto xl1 = rect1.getXMin() - w1;
  auto yb1 = rect1.getYMin() - h1;
  auto xr1 = rect1.getXMax() + w1;
  auto yt1 = rect1.getYMax() + h1;

  auto xl2 = rect2.getXMin() - w2;
  auto yb2 = rect2.getYMin() - h2;
  auto xr2 = rect2.getXMax() + w2;
  auto yt2 = rect2.getYMax() + h2;

  auto eps = 0.01;

  auto orect1 = BBox(Point(xl1 + eps, yb1 + eps), Point(xr1 - eps, yt1 - eps));
  auto orect2 = BBox(Point(xl2 + eps, yb2 + eps), Point(xr2 - eps, yt2 - eps));

  //---

  std::set<double> xset, yset;

  auto addPoint = [&](const Point &p) {
    xset.insert(p.x);
    yset.insert(p.y);
  };

  addPoint(p1);
  addPoint(p2);
  addPoint(p3);
  addPoint(p4);
  addPoint(p5);
  addPoint(p6);

  addPoint(Point(xm, ym));

  addPoint(Point(xl1, yb1));
  addPoint(Point(xr1, yt1));
  addPoint(Point(xl2, yb2));
  addPoint(Point(xr2, yt2));

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

      Point p(x, y);

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

  auto addEdge = [&](Node *node1, Node *node2, bool check=true) {
    if (! node1 || ! node2) return;

    if (check) {
      const auto &p1 = node1->p();
      const auto &p2 = node2->p();

      if (orect1.inside(p1)) return;
      if (orect2.inside(p1)) return;
      if (orect1.inside(p2)) return;
      if (orect2.inside(p2)) return;
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

//---

Graph::
Graph()
{
}

Graph::
~Graph()
{
  for (auto *e : edges_)
    delete e;

  for (auto *n : nodes_)
    delete n;
}

void
Graph::
addNode(Node *node)
{
  nodes_.push_back(node);
}

void
Graph::
addEdge(Edge *edge)
{
  edges_.push_back(edge);
}

//---

Edge::
Edge(Node *node1, Node *node2) :
 node1_(node1), node2_(node2)
{
  node1->addEdge(this);
  node2->addEdge(this);
}

//---

Node::
Node(const Point &p) :
 p_(p)
{
}

void
Node::
addEdge(Edge *edge)
{
  edges_.push_back(edge);
}

//---

}
