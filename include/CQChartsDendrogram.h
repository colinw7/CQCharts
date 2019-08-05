#ifndef CQChartsDendrogram_H
#define CQChartsDendrogram_H

#include <QString>
#include <vector>
#include <algorithm>

/*!
 * \brief Dendrogram plot data
 * \ingroup Charts
 */
class CQChartsDendrogram {
 public:
  class RootNode;
  class HierNode;
  class Node;

  //! Node
  class Node {
   private:
    static uint nextId() {
      static int lastId = 0;

      return ++lastId;
    }

   public:
    Node(HierNode *parent, const QString &name="", double size=1.0);

    virtual ~Node() { }

    HierNode *parent() const { return parent_; }

    uint id() const { return id_; }

    const QString &name() const { return name_; }

    virtual double size() const { return size_; }

    int depth() const { return depth_; }
    void setDepth(int depth) { depth_ = depth; }

    double row() const { return row_; }
    void setRow(double row) { row_ = row; }

    double numRows() const { return nr_; }
    void setNumRows(double nr) { nr_ = nr; }

    double gap() const { return gap_; }

    void setGap(double gap);

    bool isPlaced() const { return placed_; }
    void setPlaced(bool placed) { placed_ = placed; }

    virtual void resetPlaced() { placed_ = false; }

    bool isOpen() const { return open_; }
    void setOpen(bool open) { open_ = open; }

    double x() const { return depth()*root()->dx(); }
    double y() const { return row  ()*root()->dy(); }

    double w() const { return root()->dx(); }
    double h() const { return numRows()*root()->dy(); }

    double xc() const { return x() + w()/2.0; }
    double yc() const { return y() + h()/2.0; }

    virtual int maxNodes() { return 1; }

    RootNode *root();
    const RootNode *root() const;

    virtual bool hasChildren() const { return false; }

    virtual void moveNode(double d);

    virtual void compressNode(double d);

    bool isNodeAtPoint(double x, double y, double tol) const;

   protected:
    HierNode* parent_ { nullptr };
    uint      id_;
    QString   name_;
    double    size_   { 0.0 };
    int       depth_  { 0 };
    double    row_    { 0.0 };
    double    nr_     { 0.0 };
    double    gap_    { 0.0 };
    bool      open_   { false };
    bool      placed_ { false };
  };

  //---

  typedef std::vector<Node *> Nodes;

  //---

  //! Hierarchical Node
  class HierNode : public Node {
   public:
    typedef std::vector<HierNode *> Children;

   public:
    HierNode(HierNode *parent=0, const QString &name="");

   ~HierNode();

    double size() const override;

    const Nodes    &getNodes   () const { return nodes_; }
    const Children &getChildren() const { return children_; }

    int depth() const;

    int numNodes() const;

    int maxNodes() override;

    bool hasChildren() const override;

    HierNode *findChild(const QString &name) const;

    void resetPlaced() override;

    void placeSubNodes(RootNode *root, int depth, double row);

    void addNode(Node *node);

    void compressNode(double d) override;

    Node *getNodeAtPoint(double x, double y, double tol);
    const Node *getNodeAtPoint(double x, double y, double tol) const;

   protected:
    Nodes    nodes_;
    Children children_;
  };

  //---

  //! Root Node
  class RootNode : public HierNode {
   public:
    RootNode(const QString &name="");

    void setDebug(bool debug) { debug_ = debug; }
    bool debug() const { return debug_; }

    void setSingleStep(bool singleStep) { singleStep_ = singleStep; }
    bool singleStep() const { return singleStep_; }

    double dx() const { return dx_; }
    double dy() const { return dy_; }

    // place child nodes in (1.0 by 1.0 rectangle)
    void placeNodes();

    // set gaps beneath each node
    void setGaps();

    // print node gaps
    void printGaps() const;

    // compress nodes by removing space below
    void compressNodes();

    // check if we can move node and children by delta
    bool canMoveNode(Node *node, double &move_gap, Nodes &lowestChildren);

    // get the child of node with lowest row
    Node *getLowestChild(HierNode *hierNode);

    // open all nodes down to depth and close all node below depth
    void setOpenDepth(int depth);

    // open node with name at depth
    void openNode(int depth, const QString &name);

    // compress node and children
    void compressNodeAndChildren(Node *node, const Nodes &lowestChildren, double d);

    //void moveChildNodes(HierNode *hierNode, double d);

    // recursively move node, higher nodes and parent
    void compressNodeUp(Node *node, double d);

    // move all nodes higher than specified node
    void moveHigherNodes(Node *node, double d);

    // place node (set depth, row and number of rows)
    void placeNode(Node *node, int depth, double row, double num_rows);

   protected:
    typedef std::vector<Nodes> DepthNodes;

    double     dx_         { 0.0 };
    double     dy_         { 0.0 };
    double     max_rows_   { 0.0 };
    bool       debug_      { false };
    bool       singleStep_ { false };
    DepthNodes depthNodes_;
  };

  //------

 public:
  CQChartsDendrogram();
 ~CQChartsDendrogram();

  bool debug() const;
  void setDebug(bool b);

  bool singleStep() const;
  void setSingleStep(bool b);

  HierNode *root() const { return root_; }

  HierNode *createRootNode(const QString &name);

  HierNode *createHierNode(HierNode *hier, const QString &name);

  Node *createNode(HierNode *hier, const QString &name, double size);

  void placeNodes();

  void compressNodes();

  void setOpenDepth(int depth);

  void openNode(int depth, const QString &name);

  Node *getNodeAtPoint(double x, double y, double tol=1E-3);
  const Node *getNodeAtPoint(double x, double y, double tol=1E-3) const;

  void printGaps();

 private:
  RootNode* root_ { nullptr };
};

#endif
