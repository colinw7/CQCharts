#ifndef CQChartsForceDirected_H
#define CQChartsForceDirected_H

#include <CQChartsTypes.h>
#include <CForceDirected.h>

//---

/*!
 * \brief Spring Node Customization
 * \ingroup Charts
 */
class CQChartsSpringyNode : public Springy::Node {
 public:
  using Shape   = CQChartsNodeType;
  using OptReal = CQChartsOptReal;
  using BBox    = CQChartsGeom::BBox;

  using Slots = std::set<int>;

 public:
  explicit CQChartsSpringyNode(int id) :
   Springy::Node(id) {
  }

  QString stringId() const { return QString("node:%1").arg(id()); }

  int group() const { return group_; }
  void setGroup(int i) { group_ = i; }

  const Shape &shape() const { return shape_; }
  void setShape(const Shape &v) { shape_ = v; }

  const OptReal &nodeValue() const { return nodeValue_; }
  void setNodeValue(const OptReal &v) { nodeValue_ = v; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  const BBox &bbox() const { return bbox_; }
  void setBBox(const BBox &v) { bbox_ = v; }

  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; }

  bool isInside() const { return inside_; }
  void setInside(bool b) { inside_ = b; }

  const Slots &occupiedSlots() { return occupiedSlots_; }
  void clearOccupiedSlots() { occupiedSlots_.clear(); }
  void addOccupiedSlot(int slot) { occupiedSlots_.insert(slot); }

 private:
  int         group_    { -1 };
  Shape       shape_    { Shape::NONE };
  OptReal     nodeValue_;
  QModelIndex ind_;
  BBox        bbox_;
  bool        selected_ { false };
  bool        inside_   { false };
  Slots       occupiedSlots_;
};

/*!
 * \brief Spring Edge Customization
 * \ingroup Charts
 */
class CQChartsSpringyEdge : public Springy::Edge {
 public:
  using Shape = CQChartsEdgeType;
  using NodeP = Springy::NodeP;
  using Point = CQChartsGeom::Point;

 public:
  CQChartsSpringyEdge(int id, NodeP node1=NodeP(), NodeP node2=NodeP()) :
   Springy::Edge(id, node1, node2) {
  }

  QString stringId() const { return QString("edge:%1").arg(id()); }

  const Shape &shape() const { return shape_; }
  void setShape(const Shape &v) { shape_ = v; }

  bool getIsLine() const { return isLine_; }
  void setIsLine(bool b) { isLine_ = b; }

  const Point &startPoint() const { return startPoint_; }
  void setStartPoint(const Point &v) { startPoint_ = v; }

  const Point &endPoint() const { return endPoint_; }
  void setEndPoint(const Point &v) { endPoint_ = v; }

  const QPainterPath &edgePath() const { return edgePath_; }
  void setEdgePath(const QPainterPath &v) { edgePath_ = v; }

  const QPainterPath &curvePath() const { return curvePath_; }
  void setCurvePath(const QPainterPath &v) { curvePath_ = v; }

  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; }

  bool isInside() const { return inside_; }
  void setInside(bool b) { inside_ = b; }

 private:
  Shape        shape_    { Shape::NONE };
  bool         isLine_   { true };
  Point        startPoint_;
  Point        endPoint_;
  QPainterPath edgePath_;
  QPainterPath curvePath_;
  bool         selected_ { false };
  bool         inside_   { false };
};

/*!
 * \brief Spring Graph Customization
 * \ingroup Charts
 */
class CQChartsSpringyGraph : public Springy::Graph {
 public:
  explicit CQChartsSpringyGraph() :
   Springy::Graph() {
  }

  Springy::NodeP makeNode() const override {
    return Springy::NodeP(new CQChartsSpringyNode(++nextNodeId_));
  }

  Springy::EdgeP makeEdge() const override {
    return Springy::EdgeP(new CQChartsSpringyEdge(++nextEdgeId_));
  }
};

//---

class CQChartsForceDirected : public CForceDirected {
 public:
  CQChartsForceDirected() :
   CForceDirected() {
  }

  Springy::GraphP makeGraph() const override {
    return Springy::GraphP(new CQChartsSpringyGraph);
  }
};

#endif
