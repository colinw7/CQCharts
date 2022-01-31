#ifndef CDotParse_H
#define CDotParse_H

#include <CFileParse.h>

#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <memory>

namespace CDotParse {

class Graph;
class Node;
class Edge;

using GraphP = std::shared_ptr<Graph>;
using NodeP  = std::shared_ptr<Node>;
using EdgeP  = std::shared_ptr<Edge>;

class Parse {
 public:
  Parse(const std::string &filename);

  virtual ~Parse();

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  bool isPrint() const { return print_; }
  void setPrint(bool b) { print_ = b; }

  bool isCSV() const { return csv_; }
  void setCSV(bool b) { csv_ = b; }

  bool parse();

 protected:
  bool parseGraph();
  bool parseStatementList();
  bool parseStatement();
  bool parseAttrList(const std::string &id);
  bool parseAList(const std::string &id);
  bool parseID(std::string &id);
  bool parseIdentifier(std::string &id);

  void skipSpace();

  void enter(const std::string &proc) const;
  void leave(const std::string &proc) const;

  void depthSpaces() const;

  bool errorMsg(const std::string &msg) const;

 public:
  using GraphMap = std::map<std::string, GraphP>;

 public:
  const GraphMap &graphs() const { return graphs_; }

  Graph *getGraph(const std::string &name);

  NodeP getNode(const std::string &name);

  Graph *currentGraph() const;

 protected:
  friend class Graph;
  friend class Node;

  Node *makeCurrentNode(const std::string &name) const;

  virtual Graph *makeGraph(const std::string &name) const;
  virtual Node  *makeNode (Graph *graph, const std::string &name) const;
  virtual Edge  *makeEdge (Node *node1, Node *node2) const;

 protected:
  class EnterLeave {
   public:
    EnterLeave(const Parse *parse, const std::string &proc) :
     parse_(parse), proc_(proc) {
      parse_->enter(proc_);
    }

   ~EnterLeave() {
      parse_->leave(proc_);
    }

   private:
    const Parse* parse_ {nullptr };
    std::string  proc_;
  };

 private:
  using ParseP = std::unique_ptr<CFileParse>;

  ParseP      parse_;
  mutable int depth_        { 0 };
  GraphMap    graphs_;
  Graph*      currentGraph_ { nullptr };
  Node*       currentNode_  { nullptr };
  Edge*       currentEdge_  { nullptr };
  bool        debug_        { false };
  bool        print_        { false };
  bool        csv_          { false };
};

//---

namespace Util {
  double stringToReal(const std::string &s, bool &ok);

  std::vector<double> stringToReals(const std::string &s, bool &ok);
}

//---

class Attributes {
 public:
  Attributes() { }

  bool empty() const { return nameValues_.empty(); }
  auto begin() const { return nameValues_.begin(); }
  auto end  () const { return nameValues_.end  (); }

  void setNameValue(const std::string &name, const std::string &value) {
    nameValues_[name] = value;
  }

  double getReal(const std::string &name, bool &ok) const {
    auto s = getString(name, ok);
    if (! ok) return 0.0;

    return Util::stringToReal(s, ok);
  }

  std::vector<double> getReals(const std::string &name, bool &ok) const {
    auto s = getString(name, ok);
    if (! ok) return std::vector<double>();

    return Util::stringToReals(strinpQuotes(s), ok);
  }

  std::string getString(const std::string &name, bool &ok) const {
    ok = true;
    auto p = nameValues_.find(name);
    if (p == nameValues_.end()) { ok = false; return ""; }
    return (*p).second;
  }

  std::string strinpQuotes(const std::string &s) const {
    int len = s.size();
    if (len > 1 && s[0] == '"' && s[len - 1] == '"')
      return s.substr(1, len - 2);
    return s;
  }

  void print(std::ostream &os) const {
    for (const auto &nv: nameValues_) {
      os << nv.first << "=" << nv.second << "\n";
    }
  }

 private:
  using NameValues = std::map<std::string, std::string>;

  NameValues nameValues_;
};

//---

class Graph {
 public:
  using NodeMap   = std::map<std::string, NodeP>;
  using EdgeSet   = std::set<EdgeP>;
  using NodeArray = std::vector<Node *>;
  using Graphs    = std::vector<GraphP>;

 public:
  Graph(Parse *parse, const std::string &name);

  virtual ~Graph();

  Parse *parse() const { return parse_; }

  Graph *parent() const { return parent_; }
  void setParent(Graph *p) { parent_ = p; }

  const std::string &name() const { return name_; }
  void setName(const std::string &name) { name_ = name; }

  const NodeMap &nodes() const { return nodes_; }
  const EdgeSet &edges() const { return edges_; }

  std::string hierName() const {
    if (parent_)
      return parent_->hierName() + "/" + name_;
    else
      return name_;
  }

  //---

  NodeP getNode(const std::string &name, bool create=false);

  NodeP addNode(const std::string &name);
  void  addNode(NodeP node);

  EdgeP addEdge(Node *fromNode, Node *toNode);
  void  addEdge(EdgeP edge);

  void removeEdge(EdgeP edge);

  //---

  const Attributes &attributes() const { return attributes_; }
  void setAttribute(const std::string &name, const std::string &value);

  const Attributes &nodeAttributes() const { return nodeAttributes_; }
  void setNodeAttribute(const std::string &name, const std::string &value);

  const Attributes &edgeAttributes() const { return edgeAttributes_; }
  void setEdgeAttribute(const std::string &name, const std::string &value);

  //---

  void addGraph(Graph *graph);

  //---

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const Graph &graph) {
    graph.print(os);

    return os;
  }

  void outputCSV(std::ostream &os) const;

  //---

  GraphP minimumSpaningTree() const;

  bool isCycle(Node *node) const;
  bool isCycle(Node *start_node, Node *node) const;

  void resetNodeVisited() const;
  void resetEdgeVisited() const;

  NodeArray shortestPath(NodeP fromNode, NodeP toNode) const;

  Graphs subGraphs() const;

 private:
  void addNodeToSubGraph(Node *startNode, GraphP graph) const;

 private:
  using SubGraphs = std::set<Graph *>;

  Parse*      parse_  { nullptr };
  Graph*      parent_ { nullptr };
  std::string name_;
  NodeMap     nodes_;
  EdgeSet     edges_;
  Attributes  attributes_;
  Attributes  nodeAttributes_;
  Attributes  edgeAttributes_;
  SubGraphs   graphs_;
};

//---

class Node {
 public:
  using Edges = std::vector<EdgeP>;

 public:
  Node(Graph *graph, const std::string &name);

  virtual ~Node() { }

  const Graph *graph() const { return graph_; }

  const std::string &name() const { return name_; }

  const Edges &edges() const { return edges_; }

  const Attributes &attributes() const { return attributes_; }
  void setAttribute(const std::string &name, const std::string &value);

  const std::string &color() const { return color_; }
  void setColor(const std::string &s) { color_ = s; }

  const std::string &label() const { return label_; }
  void setLabel(const std::string &s) { label_ = s; }

  EdgeP addNodeEdge(Node *node);

  void addEdge(EdgeP edge);

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const Node &node) {
    node.print(os);

    return os;
  }

  void outputCSV(std::ostream &os) const;

  std::string attributesCSVStr() const;

  bool isVisited() { return visited_; }
  void setVisited(bool visited) { visited_ = visited; }

 private:
  Graph*      graph_   { nullptr };
  std::string name_;
  std::string label_;
  Edges       edges_;
  Attributes  attributes_;
  std::string color_;
  bool        visited_ { false };
};

//---

class Edge {
 public:
  Edge(Node *fromNode, Node *toNode);

  virtual ~Edge() { }

  const Graph *graph() const {
    return (fromNode()->graph() == toNode()->graph() ? fromNode()->graph() : nullptr);
  }

  Node *fromNode() const { return fromNode_; }
  Node *toNode  () const { return toNode_  ; }

  //! get/set directed
  bool isDirected() const { return directed_; }
  void setDirected(bool b) { directed_ = b; }

  const Attributes &attributes() const { return attributes_; }
  void setAttribute(const std::string &name, const std::string &value);

  double cost() const { return cost_; }
  void setCost(double r) { cost_ = r; }

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const Edge &edge) {
    edge.print(os);

    return os;
  }

  void outputCSV(std::ostream &os) const;

  std::string attributesCSVStr() const;

  bool isVisited() { return visited_; }
  void setVisited(bool visited) { visited_ = visited; }

 private:
  Node*      fromNode_ { nullptr };
  Node*      toNode_   { nullptr };
  bool       directed_ { false };
  Attributes attributes_;
  double     cost_     { 1.0 };
  bool       visited_  { false };
};

//---

}

#endif
