#ifndef CDotParse_H
#define CDotParse_H

#include <CFileParse.h>
#include <memory>
#include <map>
#include <vector>

class CDotParse {
 public:
  CDotParse(const std::string &filename);

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  bool isPrint() const { return print_; }
  void setPrint(bool b) { print_ = b; }

  bool isCSV() const { return csv_; }
  void setCSV(bool b) { csv_ = b; }

  bool parse();

 private:
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

 private:
  class EnterLeave {
   public:
    EnterLeave(const CDotParse *parse, const std::string &proc) :
     parse_(parse), proc_(proc) {
      parse_->enter(proc_);
    }

   ~EnterLeave() {
      parse_->leave(proc_);
    }

   private:
    const CDotParse* parse_ {nullptr };
    std::string      proc_;
  };

  class Graph;
  class Edge;

  class Attributes {
   public:
    Attributes() { }

    bool empty() const { return nameValues_.empty(); }
    auto begin() const { return nameValues_.begin(); }
    auto end  () const { return nameValues_.end  (); }

    void setNameValue(const std::string &name, const std::string &value) {
      nameValues_[name] = value;
    }

   private:
    using NameValues = std::map<std::string, std::string>;

    NameValues nameValues_;
  };

  class Node {
   public:
    Node(const std::string &name);

    const Graph *graph() const { return graph_; }
    void setGraph(Graph *p) { graph_ = p; }

    const std::string &name() const { return name_; }

    const Attributes &attributes() const { return attributes_; }
    void setAttribute(const std::string &name, const std::string &value);

    Edge *addEdge(Node *node);

    void print() const;

    void outputCSV() const;

    std::string attributesCSVStr() const;

   private:
    using Edges = std::vector<Edge *>;

    Graph*      graph_ { nullptr };
    std::string name_;
    Edges       edges_;
    Attributes  attributes_;
  };

  using NodeMap = std::map<std::string, Node *>;

  class Edge {
   public:
    Edge(Node *fromNode, Node *toNode);

    Node *fromNode() const { return fromNode_; }
    Node *toNode  () const { return toNode_  ; }

    const Attributes &attributes() const { return attributes_; }
    void setAttribute(const std::string &name, const std::string &value);

    void print() const;

    void outputCSV() const;

    std::string attributesCSVStr() const;

   private:
    Node*      fromNode_ { nullptr };
    Node*      toNode_   { nullptr };
    Attributes attributes_;
  };

  class Graph {
   public:
    Graph(const std::string &name);

    const std::string &name() const { return name_; }

    Node *getNode(const std::string &name);

    const Attributes &attributes() const { return attributes_; }
    void setAttribute(const std::string &name, const std::string &value);

    const Attributes &nodeAttributes() const { return nodeAttributes_; }
    void setNodeAttribute(const std::string &name, const std::string &value);

    const Attributes &edgeAttributes() const { return edgeAttributes_; }
    void setEdgeAttribute(const std::string &name, const std::string &value);

    void print() const;

    void outputCSV() const;

   private:
    std::string name_;
    NodeMap     nodes_;
    Attributes  attributes_;
    Attributes  nodeAttributes_;
    Attributes  edgeAttributes_;
  };

  using GraphMap = std::map<std::string, Graph *>;

 private:
  Graph *getGraph(const std::string &name);

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

#endif
