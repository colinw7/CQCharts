#include <CQUntangle.h>
#include <CMathRand.h>

#include <QApplication>
#include <QPainter>
#include <QKeyEvent>

#include <iostream>
#include <sys/time.h>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  bool debug = false;

  for (int i = 1; i < argc; ++i) {
    auto arg = std::string(argv[i]);

    if (arg == "-debug")
      debug = true;
  }

  auto *window = new Window;

  window->setDebug(debug);

  window->init();

  window->show();

  std::cout << "A : Auto Detangle\n";
  std::cout << "D : Detangle Groups\n";
  std::cout << "G : Move Groups\n";
  std::cout << "I : Init Groups\n";
  std::cout << "P : Print\n";
  std::cout << "R : Reload\n";
  std::cout << "S : Sort Nodes\n";
  std::cout << "U : Move Unconnected\n";
  std::cout << "V : Init All Group Overlaps\n";
  std::cout << "1 : Move Single\n";

  return app.exec();
}

Window::
Window()
{
  setFocusPolicy(Qt::StrongFocus);
}

void
Window::
init()
{
  auto seed = int(time(nullptr));

  if (debug_)
    std::cerr << seed << "\n";

  CMathRand::seedRand(seed);

  //---

  if (nodes_.empty())
    nodes_.resize(nc_);

  for (uint c = 0; c < nc_; ++c) {
    for (auto *node : nodes_[c])
      delete node;

    nodes_[c].clear();
  }

  for (auto *edge : edges_)
    delete edge;

  edges_.clear();

  //---

  lastSwap_.resize(nc_);

  for (uint c = 0; c < nc_; ++c)
    lastSwap_[c] = -1;

  //---

  if (! sizedGroups_.empty()) {
    for (uint c = 0; c < nc_ - 1; ++c) {
      for (auto &ps : sizedGroups_[c]) {
        auto &groups = ps.second;

        for (auto *group : groups)
          delete group;
      }
    }

    sizedGroups_.clear();
  }

  //---

  // create nodes for each column
  for (uint i = 0; i < nr_; ++i) {
    for (uint c = 0; c < nc_; ++c) {
      auto *node = new Node(i, QPoint(int(c), int(i)));

      nodes_[c].push_back(node);
    }
  }

  // create edges between nodes
  uint nr = uint(double(nr_)*1.5);

  for (uint i = 0; i < nr; ++i) {
    for (uint c = 0; c < nc_ - 1; ++c) {
      // try connect nodes without existing edge
      for (uint t = 0; t <= nr_; ++t) {
        int i1 = CMathRand::randInRange(0, int(nr_ - 1));
        int i2 = CMathRand::randInRange(0, int(nr_ - 1));

        auto *lnode = nodes_[c    ][uint(i1)];
        auto *rnode = nodes_[c + 1][uint(i2)];

        if (t == nr_ || lnode->edges().empty() || rnode->edges().empty()) {
          auto *edge = new Edge(lnode, rnode);

          edge->setInd(i);

          lnode->addEdge(edge);
          rnode->addEdge(edge);

          edges_.push_back(edge);

          break; // found
        }
      }
    }
  }
}

void
Window::
autoDetangle()
{
  initGroups();

  moveGroups();

  for (int i = 0; i < 20; ++i)
    detangleGroups();
}

void
Window::
detangleGroups()
{
  if (debug_)
    std::cerr << "detangleGroups\n";

  initGroups();

  // show groups overlaps (debug only)
  initAllGroupOverlaps(/*move*/true);

  for (uint c = 0; c < nc_ - 1; ++c) {
    for (auto &ps : sizedGroups_[c]) {
      auto &groups = ps.second;

      for (auto *group : groups) {
        if (group->fixed)
          continue;

        if (detangleGroup(group)) {
          if (debug_)
            printGroups();
        }
      }
    }
  }
}

void
Window::
initAllGroupOverlaps(bool move)
{
  if (debug_)
    std::cerr << "initAllGroupOverlaps\n";

  for (uint c = 0; c < nc_ - 1; ++c) {
    for (auto &ps : sizedGroups_[c]) {
      auto &groups = ps.second;

      for (auto *group : groups)
        initGroupOverlaps(group, move);
    }
  }

  if (debug_)
    printGroups();
}

void
Window::
initGroupOverlaps(NodeGroup *nodeGroup, bool move)
{
  if (debug_)
    std::cerr << "initGroupOverlaps\n";

  // int all edges for group
  if (nodeGroup->edges.empty()) {
    EdgeSet edges;

    for (uint i = 0; i < 2; ++i) {
      for (auto *node : nodeGroup->nodes[i]) {
        for (auto *edge : node->edges()) {
          auto x1 = edge->fromNode()->x();
          auto x2 = edge->toNode  ()->x();

          if ((x1 != nodeGroup->x && x1 != nodeGroup->x + 1) ||
              (x2 != nodeGroup->x && x2 != nodeGroup->x + 1))
            continue;

          edges.insert(edge);
        }
      }
    }

    nodeGroup->edges.clear();

    for (auto *edge : edges)
      nodeGroup->edges.push_back(edge);
  }

  //---

  calcGroupWorstEdge(nodeGroup, move);
}

void
Window::
calcGroupWorstEdge(NodeGroup *nodeGroup, bool move)
{
  if (debug_)
    std::cerr << "calcGroupWorstEdge\n";

  // reset overlap data
  nodeGroup->overlapData.reset();

  for (uint ie1 = 0; ie1 < uint(nodeGroup->edges.size()); ++ie1)
    nodeGroup->edges[ie1]->resetOverlaps();

  //---

  // calc edge overlaps
  for (uint ie1 = 0; ie1 < uint(nodeGroup->edges.size()); ++ie1) {
    auto *edge1 = nodeGroup->edges[ie1];

    for (uint ie2 = ie1 + 1; ie2 < uint(nodeGroup->edges.size()); ++ie2) {
      auto *edge2 = nodeGroup->edges[ie2];

      auto overlap = edgeOverlap(edge1, edge2);

      if (overlap) {
        edge1->addOverlap(edge2);
        edge2->addOverlap(edge1);

        ++nodeGroup->overlapData.numOverlaps;
      }
    }
  }

  if (nodeGroup->overlapData.numOverlaps > 0 && nodeGroup->fixed) {
    std::cerr << "Invalid fixed group " << nodeGroup->name().toStdString() <<
                 " (" << nodeGroup->overlapData.numOverlaps << " overlaps)\n";
  }

  //---

  if (move) {
    // no overlap so fix and move to bottom
    if (! nodeGroup->overlapData.numOverlaps) {
      nodeGroup->fixed = true;

      moveGroup(nodeGroup, nodeGroup->fixed);

      return;
    }
  }

  //---

  // set group best/worse edges
  for (uint ie1 = 0; ie1 < uint(nodeGroup->edges.size()); ++ie1) {
    auto *edge = nodeGroup->edges[ie1];

    auto n = edge->numOverlaps();
    if (n == 0) continue;

    if (edge->isSkipOverlap()) {
      ++nodeGroup->overlapData.numSkip;
      continue;
    }

    if (! nodeGroup->overlapData.bestEdge || n < nodeGroup->overlapData.bestN) {
      nodeGroup->overlapData.bestEdge = edge;
      nodeGroup->overlapData.bestN    = n;
    }

    if (! nodeGroup->overlapData.worstEdge || n > nodeGroup->overlapData.worstN) {
      nodeGroup->overlapData.worstEdge = edge;
      nodeGroup->overlapData.worstN    = n;
    }

    //if (debug_)
    //  std::cerr << "Edge " << ie1 << " " << n << "\n";
  }

  if (debug_)
    printGroup(nodeGroup);

  if (debug_) {
    std::cerr << "Num: " << nodeGroup->overlapData.numOverlaps;

    if (nodeGroup->overlapData.bestEdge)
      std::cerr << ", Best: " << nodeGroup->overlapData.bestEdge->name().toStdString() <<
                   " #" << nodeGroup->overlapData.bestN;

    if (nodeGroup->overlapData.worstEdge)
      std::cerr << ", Worst: " << nodeGroup->overlapData.worstEdge->name().toStdString() <<
                   " #" << nodeGroup->overlapData.worstN;

    std::cerr << "\n";
  }
}

bool
Window::
detangleGroup(NodeGroup *nodeGroup)
{
  if (debug_)
    std::cerr << "detangleGroup " << nodeGroup->name().toStdString() << "\n";

  if (! nodeGroup->overlapData.bestEdge)
    return false;

  for (auto *edge : nodeGroup->edges)
    edge->setSkipOverlap(false);

  auto *saveBestEdge = nodeGroup->overlapData.bestEdge;

  auto *edge1 = *nodeGroup->overlapData.bestEdge->overlaps().begin();

  auto *toNode1 = nodeGroup->overlapData.bestEdge->toNode();
  auto *toNode2 = edge1                          ->toNode();

  if (debug_)
    std::cerr << "Swap right nodes\n";

  swapNodes(nodes_[toNode1->x()], toNode1->y(), toNode2->y());

  uint saveNumOverlaps = nodeGroup->overlapData.numOverlaps;

  calcGroupWorstEdge(nodeGroup, /*move*/true);

  if (nodeGroup->fixed)
    return true;

  if (nodeGroup->overlapData.numOverlaps >= saveNumOverlaps) {
    if (debug_)
      std::cerr << "Not better, swap left nodes\n";

    swapNodes(nodes_[toNode1->x()], toNode1->y(), toNode2->y());

    calcGroupWorstEdge(nodeGroup, /*move*/false);

    assert(! nodeGroup->fixed);

    //---

    assert(nodeGroup->overlapData.bestEdge);

    edge1 = *nodeGroup->overlapData.bestEdge->overlaps().begin();

    auto *fromNode1 = nodeGroup->overlapData.bestEdge->fromNode();
    auto *fromNode2 = edge1                          ->fromNode();

    swapNodes(nodes_[fromNode1->x()], fromNode1->y(), fromNode2->y());

    saveNumOverlaps = nodeGroup->overlapData.numOverlaps;

    calcGroupWorstEdge(nodeGroup, /*move*/true);

    if (nodeGroup->fixed)
      return true;

    if (nodeGroup->overlapData.numOverlaps >= saveNumOverlaps) {
      if (debug_)
        std::cerr << "Not better, skip\n";

      swapNodes(nodes_[fromNode1->x()], fromNode1->y(), fromNode2->y());

      saveBestEdge->setSkipOverlap(true);

      calcGroupWorstEdge(nodeGroup, /*move*/false);

      assert(! nodeGroup->fixed);

      return false;
    }
  }

  return true;
}

void
Window::
sortNodes()
{
  resetFixed();

  //---

  using IndNodes = std::map<int, Nodes>;

  for (uint i = 0; i < nc_ - 1; ++i) {
    NodeSet added;

    IndNodes indNodes;

    // add right nodes for left nodes at left node pos
    for (auto *lnode : nodes_[i]) {
      for (auto *edge : lnode->edges()) {
        auto *rnode = edge->toNode();

        if (rnode->x() != i + 1) continue; // only right edges

        if (added.find(rnode) == added.end()) {
          indNodes[int(lnode->y())].push_back(rnode);

          added.insert(rnode);
        }
      }
    }

    // add right nodes not connected to lnode
    for (auto *rnode : nodes_[i + 1]) {
      if (added.find(rnode) == added.end()) {
        indNodes[-1].push_back(rnode);

        added.insert(rnode);
      }
    }

    uint j = 0;

    for (const auto &pi : indNodes) {
      for (auto *node : pi.second) {
        assert(node->x() == i + 1);

        nodes_[i + 1][j] = node;

        //node->setInd(j);

        node->setY(j);

        ++j;
      }
    }
  }
}

void
Window::
moveUnconnected()
{
  resetFixed();

  //---

  // move all unconnected to bottom
  auto moveUnconnected = [&](Nodes &nodes, int &lastSwap) {
    for (uint i1 = 0; i1 < uint(nodes.size()); ++i1) {
      bool unconnected = nodes[i1]->edges().empty();

      if (! unconnected)
        lastSwap = int(i1);
    }

    for (uint i1 = uint(lastSwap + 1); i1 < uint(nodes.size()); ++i1)
      nodes[i1]->setFixed(true);

    bool changed = false;

    // move all unconnected nodes to end of list
    for (uint i1 = 0; i1 <= uint(lastSwap); ++i1) {
      bool unconnected = nodes[i1]->edges().empty();

      if (unconnected) {
        nodes[i1]->setFixed(true);

        if (i1 != uint(lastSwap)) {
          for (uint j = i1; j < uint(lastSwap); ++j)
            swapNodes(nodes, j, j + 1);

          changed = true;
        }

        --lastSwap;
      }
    }

    return changed;
  };

  calcLastSwap();

  for (uint i = 0; i < nc_; ++i)
    (void) moveUnconnected(nodes_[i], lastSwap_[i]);
}

void
Window::
moveSingle()
{
  calcLastSwap();

  // move single connected to bottom
  bool changed = false;

  for (auto *edge : edges_) {
    if (edge->fromNode()->isFixed())
      continue;

    //if (edge->numOverlaps() == 0)
    //  continue;

    bool singleConnected = (edge->fromNode()->edges().size() == 1 &
                            edge->toNode  ()->edges().size() == 1);

    if (singleConnected) {
      auto *fromNode = edge->fromNode();
      auto *toNode   = edge->toNode  ();

      if (moveBottom(fromNode->x(), fromNode->y(), /*fixed*/true))
        changed = true;

      if (moveBottom(toNode->x(), toNode->y(), /*fixed*/true))
        changed = true;
    }
  }
}

void
Window::
moveGroups()
{
  initGroups();

  //---

  // move fixed groups
  for (uint c = 0; c < nc_ - 1; ++c) {
    for (auto &ps : sizedGroups_[c]) {
      auto &groups = ps.second;

      for (auto *group : groups) {
        bool fixed = false;

        for (uint i = 0; i < 2; ++i) {
          if (group->nodes[i].size() <= 1) {
            fixed = true;
            break;
          }
        }

        group->fixed = fixed;

        if (group->fixed)
          moveGroup(group, group->fixed);
      }
    }
  }

  // move unfixed groups
  for (uint c = 0; c < nc_ - 1; ++c) {
    for (const auto &ps : sizedGroups_[c]) {
      const auto &groups = ps.second;

      for (auto *group : groups) {
        if (! group->fixed)
          moveGroup(group, group->fixed);
      }
    }
  }

  //---

  initAllGroupOverlaps(/*move*/true);
}

void
Window::
moveGroup(NodeGroup *group, bool fixed)
{
  if (debug_)
    std::cerr << "Move Group " << group->name().toStdString() << " (fixed " << fixed << ")\n";

  calcLastSwap();

  using PosNode = std::map<uint, Node *>;

  for (uint i = 0; i < 2; ++i) {
    if (! group->nodes[i].empty()) {
      PosNode posNode;

      for (auto *node : group->nodes[i])
        posNode[node->y()] = node;

      assert(posNode.size() == group->nodes[i].size());

      for (auto pn : posNode) {
        auto *node = pn.second;

        moveBottom(node->x(), node->y(), fixed);
      }
    }
  }
}

void
Window::
initGroups()
{
  if (sizedGroups_.empty()) {
    sizedGroups_.resize(nc_ - 1);

    uint ind = 0;

    for (uint c = 0; c < nc_ - 1; ++c) {
      NodeSet visited;

      for (auto *node : nodes_[c]) {
        if (node->isFixed())
          continue;

        if (visited.find(node) != visited.end())
          continue;

        auto *nodeGroup = new NodeGroup(c);

        nodeGroup->nodes.resize(2); // left and right

        nodeGroup->ind = ind++;

        followNode(c, node, nodeGroup, visited);

        uint n = 0;

        for (uint i = 0; i < 2; ++i)
          n += nodeGroup->nodes[i].size();

        assert(n > 0);

        sizedGroups_[c][n].push_back(nodeGroup);
      }
    }
  }

  //---

  if (debug_)
    printGroups();
}

void
Window::
followNode(uint c, Node *node, NodeGroup *nodeGroup, NodeSet &visited) const
{
  if (visited.find(node) != visited.end())
    return;

  if (node->x() != c && node->x() != c + 1)
    return;

  uint i = node->x() - c;

  nodeGroup->nodes[i].push_back(node);

  visited.insert(node);

  for (auto *edge : node->edges()) {
    if (node == edge->fromNode())
      followNode(c, edge->toNode  (), nodeGroup, visited);
    else
      followNode(c, edge->fromNode(), nodeGroup, visited);
  }
}

void
Window::
printGroups() const
{
  if (sizedGroups_.empty())
    return;

  for (uint c = 0; c < nc_ - 1; ++c) {
    for (const auto &ps : sizedGroups_[c]) {
      const auto &groups = ps.second;

      for (auto *group : groups)
        printGroup(group);
    }
  }
}

void
Window::
printGroup(NodeGroup *group) const
{
  std::cerr << group->name().toStdString() << ")";

  for (uint i = 0; i < 2; ++i) {
    for (auto *node : group->nodes[i])
      std::cerr << " " << node->name().toStdString();
  }

  std::cerr << " [";

  for (auto *edge : group->edges)
    std::cerr << " " << edge->nodeNames().toStdString();

  std::cerr << " ]";

  if (group->fixed)
    std::cerr << " fixed";

  std::cerr << " (#" << group->overlapData.numOverlaps << ")";

  std::cerr << "\n";
}

bool
Window::
edgeOverlap(Edge *edge1, Edge *edge2) const
{
  const auto &p11 = edge1->fromNode()->p();
  const auto &p12 = edge1->toNode  ()->p();
  const auto &p21 = edge2->fromNode()->p();
  const auto &p22 = edge2->toNode  ()->p();

  if ((p11.y() <= p21.y() && p12.y() <= p22.y()) ||
      (p11.y() >= p21.y() && p12.y() >= p22.y()))
    return false;

  return true;
}

void
Window::
resetFixed()
{
  for (uint i = 0; i < nc_; ++i)
    for (auto *node : nodes_[i])
      node->setFixed(false);

  // depends on fixed
  if (! sizedGroups_.empty()) {
    for (uint c = 0; c < nc_ - 1; ++c) {
      for (auto &ps : sizedGroups_[c]) {
        auto &groups = ps.second;

        for (auto *group : groups)
          delete group;
      }
    }

    sizedGroups_.clear();
  }
}

void
Window::
calcLastSwap()
{
  // update last unfixed pos on left/right
  for (uint i = 0; i < nc_; ++i)
    lastSwap_[i] = -1;

  for (uint i = 0; i < nc_; ++i) {
    for (uint j = uint(nodes_[i].size()); j >= 1; --j) {
      if (! nodes_[i][j - 1]->isFixed()) {
        lastSwap_[i] = int(j - 1);
        break; // found unfixed
      }
    }
  }

  if (debug_) {
    std::cerr << "Last Swap:";

    for (uint i = 0; i < nc_; ++i)
      std::cerr << " " << lastSwap_[i];

    std::cerr << "\n";
  }
}

bool
Window::
moveBottom(uint ind, uint i, bool fixed)
{
  if (lastSwap_[ind] < 0)
    return false;

  if (fixed)
    nodes_[ind][i]->setFixed(true);

  bool changed = false;

  if (i != uint(lastSwap_[ind])) {
    for (uint j = i; j < uint(lastSwap_[ind]); ++j)
      swapNodes(nodes_[ind], j, j + 1);

    changed = true;
  }

  --lastSwap_[ind];

  return changed;
}

void
Window::
swapNodes(Nodes &nodes, uint i1, uint i2) const
{
  assert(i1 != i2);

  auto p1 = nodes[i1]->p();
  auto p2 = nodes[i2]->p();

  nodes[i1]->setP(p2);
  nodes[i2]->setP(p1);

  std::swap(nodes[i1], nodes[i2]);
}

void
Window::
paintEvent(QPaintEvent *)
{
  QFontMetrics fm(font());

  //---

  std::vector<uint> offset;

  offset.resize(nc_);

  for (uint i = 0; i < nc_; ++i)
    offset[i] = 0;

#if 0
  if (! sizedGroups_.empty()) {
    using Groups = std::vector<NodeGroup *>;
    using YGroup = std::map<int, Groups>;

    for (uint c = 0; c < nc_ - 1; ++c) {
      // get sorted groups
      YGroup sortedGroups;

      for (auto &ps : sizedGroups_[c]) {
        auto &groups = ps.second;

        for (auto *group : groups) {
          for (uint i = 0; i < 2; ++i) {
            if (! group->nodes[i].empty()) {
              int y = int(group->nodes[i].front()->y());
              //assert(sortedGroups.find(y) == sortedGroups.end());

              sortedGroups[y].push_back(group);

              break;
            }
          }
        }
      }

      for (const auto &pg : sortedGroups) {
        for (auto *group : pg.second) {
          for (uint i = 0; i < 2; ++i) {
            for (auto *node : group->nodes[i])
              node->setOffset(offset[i]);
          }

          uint offset1 = 0;

          if (group->nodes[0].size() >= group->nodes[1].size()) {
            offset1 = uint(group->nodes[0].size() - group->nodes[1].size());

            offset[i] += offset1;
          }
        }
      }
    }
  }
  else {
    for (uint c = 0; c < nc_; ++c) {
      for (auto *node : nodes_[c])
        node->setOffset(offset[c]);
    }
  }
#endif

  //---

  uint maxOffset = 0;

  for (uint i = 0; i < nc_ - 1; ++i)
    maxOffset = std::max(maxOffset, offset[i]);

  auto node_height = nr_ + maxOffset;

  double r  = std::max(double(height())/(2.0*double(node_height)), 4.0);
  double dx = double(width())/double(nc_);
  double d1 = dx/2.0;
  double dy = 2*r;

  //---

  QPainter painter(this);

  auto mapPoint = [&](const QPoint &p, uint offset=0) {
    double x = p.x()*dx + d1;
    double y = ((double(p.y()) + offset)*dy) + r;

    return QPointF(int(x), int(y));
  };

  auto mapNodePoint = [&](Node *node) {
    return mapPoint(node->p(), node->offset());
  };

  auto drawLine = [&](const QPointF &p1, const QPointF &p2) {
    painter.drawLine(int(p1.x()), int(p1.y()), int(p2.x()), int(p2.y()));
  };

  //---

  EdgeSet overlapEdges;

  if (! sizedGroups_.empty()) {
    for (uint c = 0; c < nc_ - 1; ++c) {
      for (auto &ps : sizedGroups_[c]) {
        auto &groups = ps.second;

        for (auto *group : groups) {
          // collect overlapping edges
          for (auto *edge : group->edges) {
            if (edge->numOverlaps() > 0)
              overlapEdges.insert(edge);
          }

          //---

          // get y range
          auto calcYRange = [](const Nodes &nodes, uint &ymin, uint &ymax) {
            ymin = 9999; ymax = 0;

            for (auto *node : nodes) {
              ymin = std::min(ymin, uint(node->y() + node->offset()));
              ymax = std::max(ymax, uint(node->y() + node->offset()));
            }
          };

          std::vector<uint> ymin, ymax;

          ymin.resize(2);
          ymax.resize(2);

          for (uint i = 0; i < 2; ++i) {
            ymin[i] = 9999;
            ymax[i] = 0;
          }

          for (uint i = 0; i < 2; ++i)
            calcYRange(group->nodes[i], ymin[i], ymax[i]);

          //---

          auto drawText = [&](double x, double y, const QString &text) {
            int dy = (fm.ascent() - fm.descent())/2;

            painter.drawText(int(x), int(y) + dy, text);
          };

          //---

          std::vector<QPointF> p1, p2;

          p1.resize(2);
          p2.resize(2);

          for (uint i = 0; i < 2; ++i) {
            p1[i] = mapPoint(QPoint(int(c + i), int(ymin[i])));
            p2[i] = mapPoint(QPoint(int(c + i), int(ymax[i])));
          }

          if      (! group->nodes[0].empty() && ! group->nodes[1].empty()) {
            double y1 = std::min(p1[0].y(), p1[1].y()) - r + 8.0;
            double y2 = std::max(p2[0].y(), p2[1].y()) + r - 8.0;

            double x1 = p1[0].x() - 8.0;
            double x2 = p1[1].x() + 8.0;

            QRect rect(int(x1), int(y1), int(x2 - x1), int(y2 - y1));

          //painter.drawRect(rect);
            painter.fillRect(rect, QBrush(QColor(180, 180, 200, 200)));

            drawText(p1[0].x() - r - 32, (y1 + y2)/2, group->name());
            drawText(p1[1].x() + r + 16, (y1 + y2)/2, group->name());
          }
          else if (! group->nodes[0].empty()) {
            double x1 = p1[0].x() - 8.0;

            painter.drawLine(QPointF(x1, p1[0].y()), QPointF(x1, p2[0].y()));

            drawText(p1[0].x() - r - 32, (p1[0].y() + p2[0].y())/2, group->name());
          }
          else if (! group->nodes[1].empty()) {
            double x2 = p1[1].x() + 8.0;

            painter.drawLine(QPointF(x2, p1[1].y()), QPointF(x2, p2[1].y()));

            drawText(p1[1].x() + r + 16, (p1[1].y() + p2[1].y())/2, group->name());
          }
        }
      }
    }
  }

  //---

  auto drawEdge = [&](Edge *edge) {
    auto p1 = mapNodePoint(edge->fromNode());
    auto p2 = mapNodePoint(edge->toNode  ());

    auto p = overlapEdges.find(edge);

    if (p != overlapEdges.end())
      painter.setPen(Qt::red);

    drawLine(p1, p2);

    if (p != overlapEdges.end())
      painter.setPen(Qt::black);
  };

  auto drawNode = [&](uint i, Node *node) {
    assert(i == node->x());

    auto p = mapNodePoint(node);

    auto rect = QRect(int(p.x() - r), int(p.y() - r), int(2*r), int(2*r));

    auto bg = (node->isFixed() ? QColor(200, 100, 100) : QColor(100, 200, 100));
    painter.setBrush(bg);

    painter.drawEllipse(rect);

    for (auto *edge : node->edges())
      drawEdge(edge);

    painter.drawText(rect, Qt::AlignCenter, node->name());
  };

  auto drawNodes = [&](uint i, const Nodes &nodes) {
    for (auto *node : nodes)
      drawNode(i, node);
  };

  for (uint i = 0; i < nc_; ++i)
    drawNodes(i, nodes_[i]);
}

void
Window::
keyPressEvent(QKeyEvent *e)
{
  if      (e->key() == Qt::Key_A) {
    autoDetangle();

    update();
  }
  else if (e->key() == Qt::Key_D) {
    //bool debug = true;

    //std::swap(debug_, debug);

    detangleGroups();

    //std::swap(debug_, debug);

    update();
  }
  else if (e->key() == Qt::Key_G) {
    moveGroups();

    update();
  }
  else if (e->key() == Qt::Key_I) {
    initGroups();

    update();
  }
  else if (e->key() == Qt::Key_P) {
    printGroups();
  }
  else if (e->key() == Qt::Key_R) {
    init();

    update();
  }
  else if (e->key() == Qt::Key_S) {
    sortNodes();

    update();
  }
  else if (e->key() == Qt::Key_U) {
    moveUnconnected();

    update();
  }
  else if (e->key() == Qt::Key_V) {
    initAllGroupOverlaps(/*move*/true);

    update();
  }
  else if (e->key() == Qt::Key_1) {
    moveSingle();

    update();
  }
}
