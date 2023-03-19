#ifndef CQChartsForceDirected_H
#define CQChartsForceDirected_H

#include <CQChartsTypes.h>
#include <CForceDirected.h>

class CQChartsForceDirectedEdge;

//---

enum class CQChartsForceDirectedEdgeValueType {
  NONE,
  SRC,
  DEST,
  SRC_DEST
};

//---

/*!
 * \brief Spring Node Customization
 * \ingroup Charts
 */
class CQChartsForceDirectedNode : public Springy::Node {
 public:
  using Edge          = CQChartsForceDirectedEdge;
  using EdgeValueType = CQChartsForceDirectedEdgeValueType;
  using Shape         = CQChartsNodeType;
  using Color         = CQChartsColor;
  using ColorInd      = CQChartsUtil::ColorInd;
  using OptReal       = CQChartsOptReal;
  using BBox          = CQChartsGeom::BBox;
  using Point         = CQChartsGeom::Point;
  using OptPoint      = std::optional<Point>;

  using EdgeSet = std::set<Edge *>;
  using Slots   = std::map<int, EdgeSet>;

 public:
  explicit CQChartsForceDirectedNode(int id) :
   Springy::Node(id) {
  }

  QString stringId() const { return QString("node:%1").arg(id()); }

  int group() const { return group_; }
  void setGroup(int i) { group_ = i; }

  const Shape &shape() const { return shape_; }
  void setShape(const Shape &s) { shape_ = s; }

  const Color &fillColor() const { return fillColor_; }
  void setFillColor(const Color &c) { fillColor_ = c; }

  const OptReal &nodeValue() const { return nodeValue_; }
  void setNodeValue(const OptReal &v) { nodeValue_ = v; }

  const EdgeValueType &totalValueType() const { return totalValueType_; }
  void setTotalValueType(const EdgeValueType &t) { totalValueType_ = t; }

  const OptReal &totalValue() const { return totalValue_; }
  void setTotalValue(const OptReal &r) { totalValue_ = r; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  const OptPoint &initPos() const { return initPos_; }
  void setInitPos(const OptPoint &p) { initPos_ = p; }

  const BBox &bbox() const { return bbox_; }
  void setBBox(const BBox &b) { bbox_ = b; }

  const ColorInd &colorInd() const { return colorInd_; }
  void setColorInd(const ColorInd &v) { colorInd_ = v; }

  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; }

  bool isInside() const { return inside_; }
  void setInside(bool b) { inside_ = b; }

  const Slots &occupiedSlots() { return occupiedSlots_; }
  void clearOccupiedSlots() { occupiedSlots_.clear(); }

  void addOccupiedSlot(int slot, Edge *edge) {
    occupiedSlots_[slot].insert(edge);
  }

  int numInEdges() const { return inEdges_.size(); }
  void addInEdge(Edge *edge) { inEdges_.insert(edge); }
  const EdgeSet &inEdges() const { return inEdges_; }

  int numOutEdges() const { return outEdges_.size(); }
  void addOutEdge(Edge *edge) { outEdges_.insert(edge); }
  const EdgeSet &outEdges() const { return outEdges_; }

 private:
  int         group_    { -1 };
  Shape       shape_    { Shape::NONE };
  Color       fillColor_;
  OptReal     nodeValue_;
  OptPoint    initPos_;
  QModelIndex ind_;
  BBox        bbox_;
  ColorInd    colorInd_;
  bool        selected_ { false };
  bool        inside_   { false };
  Slots       occupiedSlots_;

  EdgeValueType totalValueType_ { EdgeValueType::NONE };
  OptReal       totalValue_;

  EdgeSet inEdges_;
  EdgeSet outEdges_;
};

/*!
 * \brief Spring Edge Customization
 * \ingroup Charts
 */
class CQChartsForceDirectedEdge : public Springy::Edge {
 public:
  using Shape    = CQChartsEdgeType;
  using OptReal  = CQChartsOptReal;
  using Color    = CQChartsColor;
  using ColorInd = CQChartsUtil::ColorInd;
  using NodeP    = Springy::NodeP;
  using Point    = CQChartsGeom::Point;

 public:
  CQChartsForceDirectedEdge(int id, NodeP node1=NodeP(), NodeP node2=NodeP()) :
   Springy::Edge(id, node1, node2) {
  }

  QString stringId() const { return QString("edge:%1").arg(id()); }

  const Shape &shape() const { return shape_; }
  void setShape(const Shape &s) { shape_ = s; }

  const OptReal &edgeWidth() const { return edgeWidth_; }
  void setEdgeWidth(const OptReal &v) { edgeWidth_ = v; }

  const Color &fillColor() const { return fillColor_; }
  void setFillColor(const Color &c) { fillColor_ = c; }

  bool getIsLine() const { return isLine_; }
  void setIsLine(bool b) { isLine_ = b; }

  const Point &startPoint() const { return startPoint_; }
  void setStartPoint(const Point &p) { startPoint_ = p; }

  const Point &endPoint() const { return endPoint_; }
  void setEndPoint(const Point &p) { endPoint_ = p; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  const QPainterPath &edgePath() const { return edgePath_; }
  void setEdgePath(const QPainterPath &p) { edgePath_ = p; }

  const QPainterPath &curvePath() const { return curvePath_; }
  void setCurvePath(const QPainterPath &p) { curvePath_ = p; }

  const QPainterPath &selectPath() const { return selectPath_; }
  void setSelectPath(const QPainterPath &p) { selectPath_ = p; }

  const ColorInd &colorInd() const { return colorInd_; }
  void setColorInd(const ColorInd &v) { colorInd_ = v; }

  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; }

  bool isInside() const { return inside_; }
  void setInside(bool b) { inside_ = b; }

 private:
  Shape        shape_    { Shape::NONE };
  OptReal      edgeWidth_;
  Color        fillColor_;
  bool         isLine_   { true };
  Point        startPoint_;
  Point        endPoint_;
  QModelIndex  ind_;
  QPainterPath edgePath_;
  QPainterPath curvePath_;
  QPainterPath selectPath_;
  ColorInd     colorInd_;
  bool         selected_ { false };
  bool         inside_   { false };
};

/*!
 * \brief Spring Graph Customization
 * \ingroup Charts
 */
class CQChartsForceDirectedGraph : public Springy::Graph {
 public:
  explicit CQChartsForceDirectedGraph() :
   Springy::Graph() {
  }

  Springy::NodeP makeNode() const override {
    return Springy::NodeP(new CQChartsForceDirectedNode(++nextNodeId_));
  }

  Springy::EdgeP makeEdge() const override {
    return Springy::EdgeP(new CQChartsForceDirectedEdge(++nextEdgeId_));
  }
};

//---

class CQChartsForceDirected : public CForceDirected {
 public:
  CQChartsForceDirected() :
   CForceDirected() {
  }

  Springy::GraphP makeGraph() const override {
    return Springy::GraphP(new CQChartsForceDirectedGraph);
  }
};

#endif
