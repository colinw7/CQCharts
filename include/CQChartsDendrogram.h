#ifndef CQChartsDendrogram_H
#define CQChartsDendrogram_H

#include <CQChartsGeom.h>
#include <QString>
#include <vector>
#include <algorithm>
#include <optional>

/*!
 * \brief Dendrogram plot data
 * \ingroup Charts
 */
class CQChartsDendrogram {
 public:
  using BBox = CQChartsGeom::BBox;

  class Node;

  //---

  using OptReal = std::optional<double>;

  struct Edge {
    Node*   node { nullptr }; // dest node
    OptReal value;            // edge value

    Edge() = default;

    Edge(Node *node, const OptReal &value=OptReal()) :
     node(node), value(value) {
    }
  };

  using Nodes = std::vector<Node *>;
  using Edges = std::vector<Edge>;

  //---

  //! Node
  class Node {
   private:
    static uint nextId() {
      static uint lastId = 0;

      return ++lastId;
    }

   public:
    using Dendrogram = CQChartsDendrogram;

    Node(Node *parent, const QString &name="", const OptReal &size=OptReal());

    virtual ~Node();

    Node *parent() const { return parent_; }

    uint id() const { return id_; }

    const QString &name() const { return name_; }
    void setName(const QString &name) { name_ = name; }

    const OptReal &size() const { return size_; }
    void setSize(const OptReal &r) { size_ = r; }

    bool calcHierSize(double &size) const;

    //! get/set depth
    int depth() const { return depth_; }
    void setDepth(int depth) { depth_ = depth; }

    //! get/set row
    double row() const { return row_; }
    void setRow(double row) { row_ = row; }

    //! get/set number of rows
    double numRows() const { return nr_; }
    void setNumRows(double nr) { nr_ = nr; }

    double gap() const { return gap_; }
    void setGap(double gap) { gap_ = gap; }

    //! get/set is placed
    bool isPlaced() const { return placed_; }
    void setPlaced(bool placed) { placed_ = placed; }

    void resetPlaced();

    //! get/set is open
    bool isOpen() const { return open_; }
    void setOpen(bool open) { open_ = open; }

    //! get/set is root
    bool isRoot() const { return root_; }
    void setRoot(bool b) { root_ = b; };

    //! get/set is temp
    bool isTemp() const { return temp_; }
    void setTemp(bool b) { temp_ = b; }

    //---

    const Edges &getChildren() const { return children_; }

    bool isHier() const { return ! children_.empty(); }

    //---

    double x() const { return bbox_.getXMin(); }
    double y() const { return bbox_.getYMin(); }

    double w() const { return bbox_.getWidth(); }
    double h() const { return bbox_.getHeight(); }

    double xc() const { return x() + w()/2.0; }
    double yc() const { return y() + h()/2.0; }

    bool hasBBox() const { return bbox_.isValid(); }

    const BBox &bbox() const { return bbox_; }
    void setBBox(const BBox &b) { bbox_ = b; }

    //---

    Node *root();
    const Node *root() const;

    bool hasChildren() const;

    void clear();

    void addChild(Node *child, const OptReal &value=OptReal());
    void removeChild(Node *child);

    void removeAll();

    int numNodes() const;

    int maxNodes(bool ignoreOpen=false);
    int maxEdges(bool ignoreOpen=false);

    int calcDepth(bool ignoreOpen=false) const;

    //---

    bool hasChild(Node *child) const;
    Node *findChild(const QString &name) const;

    Node *findHierChild(const QString &name) const;

    //---

    OptReal childValue(const Node *child) const;
    void setChildValue(const Node *child, double value);

   protected:
    void setParent(Node *parent) { parent_ = parent; }

   protected:
    Node*   parent_ { nullptr }; //!< parent hier node
    uint    id_;                 //!< id
    QString name_;               //!< name
    OptReal size_;               //!< size
    int     depth_  { 0 };       //!< depth
    double  row_    { 0.0 };     //!< row
    double  nr_     { 0.0 };     //!< number of rows
    double  gap_    { 0.0 };     //!< gap
    bool    open_   { false };   //!< is open
    bool    root_   { false };   //!< is root
    bool    temp_   { false };   //!< is temp
    bool    placed_ { false };   //!< is placed
    BBox    bbox_;               //!< bbox

    //---

    Edges children_; //!< child hier node edges
  };

  //------

 public:
  CQChartsDendrogram();

  virtual ~CQChartsDendrogram();

  Node *root() const { return root_; }

  //! get/set debug
  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  //! get/set single setp
  bool isSingleStep() const { return singleStep_; }
  void setSingleStep(bool singleStep) { singleStep_ = singleStep; }

  double dx() const { return dx_; }
  double dy() const { return dy_; }

  Node *addRootNode(const QString &name);

  Node *addEdge(Node *hier, const QString &name, const OptReal &edgeValue=OptReal());
  Node *addNode(Node *hier, const QString &name, const OptReal &nodeValue=OptReal());

  Node *createRootNode(const QString &name) const;

  virtual Node *createNode(Node *hier, const QString &name, const OptReal &size=OptReal()) const;

  // place child nodes in (1.0 by 1.0 rectangle)
  void placeNodes();

  // compress nodes by removing space below
  void compressNodes();

  // set gaps beneath each node
  void setGaps();

  // check if we can move node and children by delta
  bool canMoveNode(Node *node, double &move_gap, Nodes &lowestChildren);

  // get the child of node with lowest row
  Node *getLowestChild(Node *hierNode);

  // open all nodes down to depth and close all node below depth
  void setOpenDepth(int depth);

  // open node with name at depth
  void openNode(int depth, const QString &name);

  // compress node and children
  void compressNodeAndChildren(Node *node, const Nodes &lowestChildren, double d);

  // recursively move node, higher nodes and parent
  void compressNodeUp(Node *node, double d);

  void compressNode(Node *node, double d);

  void setGap(Node *node, double gap);

  // move all nodes higher than specified node
  void moveHigherNodes(Node *node, double d);

  //void moveChildNodes(Node *hierNode, double d);

  void moveNode(Node *node, double d);

  // place node (set depth, row and number of rows)
  void placeNode(Node *node, int depth, double row, double num_rows);

  void placeSubNodes(Node *root, Node *node, int depth, double row);

  bool isNodeAtPoint(double x, double y, double tol) const;
  bool isNodeAtPoint(Node *node, double x, double y, double tol) const;

  Node *getNodeAtPoint(double x, double y, double tol=1E-3);
  const Node *getNodeAtPoint(double x, double y, double tol=1E-3) const;

  Node *getNodeAtPoint(Node *node, double x, double y, double tol=1E-3);

  // print node gaps
  void printGaps() const;

  //---

  double nodeX(Node *node) const;
  double nodeY(Node *node) const;
  double nodeW(Node *node) const;
  double nodeH(Node *node) const;

  double nodeXC(Node *node) const;
  double nodeYC(Node *node) const;

 private:
  using DepthNodes = std::vector<Nodes>;

  Node*      root_       { nullptr }; //!< root node
  bool       debug_      { false };   //!< is debug
  bool       singleStep_ { false };   //!< single step
  double     dx_         { 0.0 };     //!< dx
  double     dy_         { 0.0 };     //!< dy
  double     maxRows_    { 0.0 };     //!< max rows
  DepthNodes depthNodes_;             //!< nodes by depth
};

#endif
