#include <CDotParse.h>
#include <CFileParse.h>
#include <CAStarNode.h>

#include <list>
#include <cassert>

namespace CDotParse {

Parse::
Parse(const std::string &filename)
{
  parse_ = std::make_unique<CFileParse>(filename);

  parse_->setStream(true);
}

Parse::
~Parse()
{
}

bool
Parse::
parse()
{
  EnterLeave el(this, "parse");

  if (parse_->isChar('#')) {
    while (! parse_->eof()) {
      if (parse_->isChar('\n'))
        break;

      parse_->skipChar();
    }
  }

  //---

  while (! parse_->eof()) {
    skipSpace();

    if (parse_->eof())
      break;

    std::string identifier;

    if (! parseIdentifier(identifier))
      return errorMsg("expected identfier");

    if (identifier == "strict") {
      skipSpace();

      if (! parseIdentifier(identifier))
        return errorMsg("expected identfier");
    }

    if (identifier == "graph" || identifier == "digraph") {
      if (! parseGraph())
        return errorMsg("parseGraph failed");
    }
    else
      return errorMsg("Invalid identifier '" + identifier + "'");
  }

  if (isPrint()) {
    for (const auto &pg : graphs_)
      pg.second->print(std::cout);
  }

  if (isCSV()) {
    std::cout << "From,To,Attributes,Graph\n";

    for (const auto &pg : graphs_)
      pg.second->outputCSV(std::cout);
  }

  return true;
}

bool
Parse::
parseGraph()
{
  EnterLeave el(this, "parseGraph");

  while (true) {
    skipSpace();

    if (parse_->eof())
      break;

    if      (parse_->isChar('[')) {
      parseAttrList("");
    }
    else if (parse_->isChar('{')) {
      parse_->skipChar();

      parseStatementList();

      skipSpace();

      if (! parse_->isChar('}'))
        return errorMsg("expected }");

      parse_->skipChar();
    }
    else {
      // graph id
      std::string id;

      if (! parseID(id))
        return errorMsg("expected identfier");

      currentGraph_ = getGraph(id);
    }
  }

  currentGraph_ = nullptr;
  currentNode_  = nullptr;

  return true;
}

bool
Parse::
parseStatementList()
{
  EnterLeave el(this, "parseStatementList");

  while (true) {
    skipSpace();

    if (parse_->eof())
      break;

    if (! parseStatement())
      return errorMsg("parseStatement failed");

    skipSpace();

    if (parse_->isChar(';')) {
      parse_->skipChar();

      skipSpace();
    }

    if (parse_->isChar('}'))
      return true;
  }

  return errorMsg("parseStatementList failed");
}

bool
Parse::
parseStatement()
{
  EnterLeave el(this, "parseStatement");

  skipSpace();

  //---

  if (parse_->isChar('{')) {
    parse_->skipChar();

    parseStatementList();

    skipSpace();

    if (! parse_->isChar('}'))
      return errorMsg("expected }");

    parse_->skipChar();

    skipSpace();

    return true;
  }

  //---

  std::string id;

  if (! parseID(id))
    return errorMsg("expected identifier");

  skipSpace();

  // attr_stmt
  if      (id == "graph" || id == "node" || id == "edge") {
    parseAttrList(id);
  }
  else if (id == "subgraph") {
    std::string id1;

    skipSpace();

    if (! parseID(id1))
      return errorMsg("expected identfier");

    //---

    auto *subGraph = getGraph(id1);

    currentGraph_->addGraph(subGraph);

    currentGraph_ = subGraph;

    //---

    skipSpace();

    if (parse_->isChar('{')) {
      parse_->skipChar();

      if (! parseStatementList())
        return errorMsg("parseStatementList failed");

      skipSpace();

      if (! parse_->isChar('}'))
        return errorMsg("expected }");

      parse_->skipChar();

      skipSpace();
    }

    currentGraph_ = subGraph->parent();
  }
  // name = value
  else if (parse_->isChar('=')) {
    parse_->skipChar();

    skipSpace();

    std::string id1;

    if (! parseID(id1))
      return errorMsg("expected identifier");
  }
  // node [ <attributes> ]
  else if (parse_->isChar('[')) {
    currentNode_ = getNode(id).get();

    parseAttrList("");
  }
  // edge
  else if (parse_->isString("->") || parse_->isString("--")) {
    bool directed = parse_->isString("->");

    parse_->skipChar(2);

    skipSpace();

    std::vector<Node *> nodes1;

    Node *node1 = getNode(id).get();

    nodes1.push_back(node1);

    while (true) {
      std::vector<Node *> nodes2;

      if (parse_->isChar('{')) {
        parse_->skipChar();

        skipSpace();

        while (! parse_->eof() && ! parse_->isChar('}')) {
          std::string id1;

          if (! parseID(id1))
            return errorMsg("expected identfier");

          Node *node2 = getNode(id1).get();

          nodes2.push_back(node2);

          for (const auto &n1 : nodes1) {
            currentEdge_ = n1->addNodeEdge(node2).get();

            currentEdge_->setDirected(directed);
          }

          skipSpace();

          if (parse_->isChar('[')) {
            parseAttrList("");

            skipSpace();
          }

          if (parse_->isChar(',')) {
            parse_->skipChar();

            skipSpace();
          }
        }

        if (parse_->isChar('}')) {
          parse_->skipChar();

          skipSpace();
        }
      }
      else {
        std::string id1;

        if (! parseID(id1))
          return errorMsg("expected identfier");

        Node *node2 = getNode(id1).get();

        nodes2.push_back(node2);

        for (const auto &n1 : nodes1) {
          currentEdge_ = n1->addNodeEdge(node2).get();

          currentEdge_->setDirected(directed);
        }

        skipSpace();

        if (parse_->isChar('[')) {
          parseAttrList("");

          skipSpace();
        }
      }

      //---

      if (! parse_->isString("->") && ! parse_->isString("--"))
        break;

      parse_->skipChar(2);

      skipSpace();

      //---

      nodes1 = nodes2;
    }

    currentEdge_ = nullptr;
  }
  else if (parse_->isChar('}')) {
    currentNode_ = getNode(id).get();
  }
  else {
    currentNode_ = getNode(id).get();
  }

  return true;
}

bool
Parse::
parseAttrList(const std::string &id)
{
  EnterLeave el(this, "parseAttrList " + id);

  while (true) {
    skipSpace();

    if (parse_->eof())
      break;

    if (! parse_->isChar('['))
      return errorMsg("expected [");

    parse_->skipChar();

    parse_->skipSpace();

    if (parse_->isChar(']')) {
      parse_->skipChar();
      break;
    }

    if (! parseAList(id))
      return errorMsg("parseAList failed");

    skipSpace();

    if (! parse_->isChar(']'))
      return errorMsg("expected ]");

    parse_->skipChar();

    skipSpace();

    if (! parse_->isChar('['))
      break;
  }

  return true;
}

bool
Parse::
parseAList(const std::string &id)
{
  EnterLeave el(this, "parseAList " + id);

  while (true) {
    skipSpace();

    if (parse_->eof())
      break;

    std::string id1;

    if (! parseID(id1))
      return errorMsg("expected identifier");

    skipSpace();

    if (! parse_->isChar('='))
      return errorMsg("expected =");

    parse_->skipChar();

    skipSpace();

    if (parse_->isDigit() || parse_->isChar('.') || parse_->isChar('-')) {
      double r;

      if (! parse_->readReal(&r))
        return errorMsg("expected real");

      if      (currentEdge_)
        currentEdge_->setAttribute(id1, std::to_string(r));
      else if (currentNode_)
        currentNode_->setAttribute(id1, std::to_string(r));
    }
    else {
      std::string id2;

      if (! parseID(id2))
        return errorMsg("expected identifier");

      if      (id == "graph") {
        currentGraph_->setAttribute(id1, id2);
      }
      else if (id == "node") {
        currentGraph_->setNodeAttribute(id1, id2);
      }
      else if (id == "edge") {
        currentGraph_->setEdgeAttribute(id1, id2);
      }
      else {
        if      (currentEdge_)
          currentEdge_->setAttribute(id1, id2);
        else if (currentNode_)
          currentNode_->setAttribute(id1, id2);
      }
    }

    //---

    skipSpace();

    if (parse_->isChar(']'))
      break;

    if (parse_->isChar(';') || parse_->isChar(',')) {
      parse_->skipChar();

      skipSpace();
    }
  }

  return true;
}

bool
Parse::
parseID(std::string &id)
{
  EnterLeave el(this, "parseID");

  // TODO: single quote not allowed ?
  if      (parse_->isChar('\'')) {
    id += '\'';

    parse_->skipChar();

    while (! parse_->eof()) {
      char c = parse_->readChar();

      id += c;

      if (c == '\'')
        break;
    }
  }
  else if (parse_->isChar('\"')) {
    id += '\"';

    parse_->skipChar();

    while (! parse_->eof()) {
      char c = parse_->readChar();

      id += c;

      if (c == '\"')
        break;
    }
  }
  else if (parse_->isChar('<')) {
    id += '<';

    parse_->skipChar();

    int num_html   = 1;
    int num_squote = 0;
    int num_dquote = 0;

    while (! parse_->eof()) {
      char c = parse_->readChar();

      id += c;

      if      (num_squote > 0) {
        if      (c == '\\') {
          if (! parse_->eof()) {
            char c1 = parse_->readChar();

            id += c1;
          }
        }
        else if (c == '\'') {
          --num_squote;
        }
      }
      else if (num_dquote > 0) {
        if      (c == '\\') {
          if (! parse_->eof()) {
            char c1 = parse_->readChar();

            id += c1;
          }
        }
        else if (c == '\"') {
          --num_dquote;
        }
      }
      else {
        if      (c == '<')
          ++num_html;
        else if (c == '>') {
          --num_html;

          if (num_html == 0)
            break;
        }
        else if (c == '\'')
          ++num_squote;
        else if (c == '\"')
          ++num_dquote;
      }
    }
  }
  else if (parse_->isDigit()) {
    while (! parse_->eof() && parse_->isDigit())
      id += parse_->readChar();

    if (parse_->isChar('.')) {
      id += parse_->readChar();

      while (! parse_->eof() && parse_->isDigit())
        id += parse_->readChar();
    }
  }
  else {
    if (! parse_->readIdentifier(id))
      return false;
  }

  if (isDebug()) {
    depthSpaces(); std::cerr << " " << id << "\n";
  }

  return true;
}

bool
Parse::
parseIdentifier(std::string &id)
{
  EnterLeave el(this, "parseIdentifier");

  if (! parse_->readIdentifier(id))
    return false;

  if (isDebug()) {
    depthSpaces(); std::cerr << " " << id << "\n";
  }

  return true;
}

void
Parse::
skipSpace()
{
  while (true) {
    // skip space
    while (! parse_->eof() && parse_->isSpace()) {
      std::string sstr;

      while (! parse_->eof() && parse_->isSpace())
        sstr += parse_->readChar();

      if (parse_->isChar('#')) {
        auto p = sstr.find('\n');

        if (p != std::string::npos) {
          while (! parse_->eof()) {
            if (parse_->isChar('\n'))
              break;

            parse_->skipChar();
          }
        }
      }
    }

    // skip comments
    if      (parse_->isString("//")) { // single line
      while (! parse_->eof() && parse_->isString("//")) {
        parse_->skipChar(2);

        while (! parse_->eof() && ! parse_->isChar('\n'))
          parse_->skipChar();

        if (parse_->isChar('\n'))
          parse_->skipChar();

        parse_->skipSpace();
      }
    }
    else if (parse_->isString("/*")) { // multi line
      parse_->skipChar(2);

      while (! parse_->eof() && ! parse_->isString("*/")) {
        parse_->skipChar();
      }

      if (parse_->isString("*/"))
        parse_->skipChar(2);
    }
    else
      break;
  }
}

void
Parse::
enter(const std::string &proc) const
{
  if (isDebug()) {
    depthSpaces(); std::cerr << "> " << proc << "\n";
  }

  ++depth_;
}

void
Parse::
leave(const std::string &proc) const
{
  --depth_;

  if (isDebug()) {
    depthSpaces(); std::cerr << "< " << proc << "\n";
  }
}

void
Parse::
depthSpaces() const
{
  for (int i = 0; i < depth_; ++i)
    std::cerr << " ";
}

bool
Parse::
errorMsg(const std::string &msg) const
{
  std::cerr << "Error: " << parse_->fileName() << "@" <<
               parse_->lineNum() << ":" << parse_->charNum() << " ";
  std::cerr << msg << "\n";
  return false;
}

Graph *
Parse::
getGraph(const std::string &name)
{
  auto p = graphs_.find(name);

  if (p == graphs_.end()) {
    auto graph = GraphP(makeGraph(name));

    p = graphs_.insert(p, GraphMap::value_type(name, graph));
  }

  return (*p).second.get();
}

NodeP
Parse::
getNode(const std::string &name)
{
  for (auto &pg : graphs_) {
    auto graph = pg.second;

    auto node = graph->getNode(name, /*create*/false);
    if (node) return node;
  }

  return currentGraph()->getNode(name, /*create*/true);
}

Graph *
Parse::
makeGraph(const std::string &name) const
{
  return new Graph(const_cast<Parse *>(this), name);
}

Graph *
Parse::
currentGraph() const
{
  if (currentGraph_)
    return currentGraph_;

  assert(! graphs_.empty());

  return graphs_.begin()->second.get();
}

Node *
Parse::
makeCurrentNode(const std::string &name) const
{
  assert(currentGraph());

  return makeNode(currentGraph(), name);
}

Node *
Parse::
makeNode(Graph *graph, const std::string &name) const
{
  return new Node(graph, name);
}

Edge *
Parse::
makeEdge(Node *node1, Node *node2) const
{
  //assert(node1->graph() == this && node2->graph() == this);

  return new Edge(node1, node2);
}

//---

Graph::
Graph(Parse *parse, const std::string &name) :
 parse_(parse), name_(name)
{
}

Graph::
~Graph()
{
}

NodeP
Graph::
getNode(const std::string &name, bool create)
{
  auto p = nodes_.find(name);

  if (p != nodes_.end())
    return (*p).second;

  if (! create)
    return NodeP();

  //---

  return addNode(name);
}

NodeP
Graph::
addNode(const std::string &name)
{
  auto node = NodeP(parse()->makeNode(this, name));

  for (const auto &pn : nodeAttributes())
    node->setAttribute(pn.first, pn.second);

  addNode(node);

  return node;
}

void
Graph::
addNode(NodeP node)
{
  auto p = nodes_.find(node->name());
  assert(p == nodes_.end());

  nodes_[node->name()] = node;
}

EdgeP
Graph::
addEdge(Node *fromNode, Node *toNode)
{
  return fromNode->addNodeEdge(toNode);
}

void
Graph::
addEdge(EdgeP edge)
{
  edges_.insert(edge);
}

void
Graph::
removeEdge(EdgeP edge)
{
  auto p = edges_.find(edge);

  if (p != edges_.end())
    edges_.erase(p);
}

void
Graph::
setAttribute(const std::string &name, const std::string &value)
{
  attributes_.setNameValue(name, value);
}

void
Graph::
setNodeAttribute(const std::string &name, const std::string &value)
{
  nodeAttributes_.setNameValue(name, value);
}

void
Graph::
setEdgeAttribute(const std::string &name, const std::string &value)
{
  edgeAttributes_.setNameValue(name, value);
}

void
Graph::
addGraph(Graph *graph)
{
  graph->setParent(this);

  graphs_.insert(graph);
}

void
Graph::
print(std::ostream &os) const
{
  os << "Graph " << name_ << "\n";

  for (auto &pn : nodes_) {
    os << " "; pn.second->print(os); os << "\n";
  }
}

void
Graph::
outputCSV(std::ostream &os) const
{
  for (auto &pn : nodes_)
    pn.second->outputCSV(os);
}

GraphP
Graph::
minimumSpaningTree() const
{
  auto newGraph = GraphP(parse_->makeGraph(""));

  int num_nodes = nodes_.size();
  int num_edges = edges_.size();

  if (num_nodes == 0 || num_edges == 0)
    return newGraph;

  std::list<EdgeP> in_edges;

  for (auto &edge : edges_)
    in_edges.push_back(edge);

  int num_in_edges = in_edges.size();

  while (num_in_edges > 0) {
    EdgeP  min_edge;
    double min_cost { };

    for (auto &iedge : in_edges) {
      auto cost = iedge->cost();

      if (min_edge == nullptr || cost < min_cost) {
        min_edge = iedge;
        min_cost = cost;
      }
    }

    if (! min_edge)
      break;

    in_edges.remove(min_edge);

    --num_in_edges;

    auto *minFromNode = min_edge->fromNode();
    auto *minToNode   = min_edge->toNode  ();

    const std::string &name1 = minFromNode->name();
    const std::string &name2 = minToNode  ->name();

    minFromNode = newGraph->getNode(name1).get();
    minToNode   = newGraph->getNode(name2).get();

    if (minFromNode == nullptr)
      minFromNode = newGraph->addNode(name1).get();

    if (minToNode == nullptr)
      minToNode = newGraph->addNode(name2).get();

    min_edge = newGraph->addEdge(minFromNode, minToNode);

    min_edge->setCost(min_cost);

    if (newGraph->isCycle(minFromNode) || newGraph->isCycle(minToNode)) {
      newGraph->removeEdge(min_edge);
    }
  }

  if (parse_->isDebug()) {
    std::cout << "Kruskal" << std::endl;

    std::cout << *newGraph << std::endl;
  }

  return newGraph;
}

Graph::NodeArray
Graph::
shortestPath(NodeP fromNode, NodeP toNode) const
{
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

    double traverseCost(Node * /*node*/, Node * /*newNode*/) override {
      return 1;
    }

    NodeList getNextNodes(Node *node) const override {
      NodeList nodes;

      for (const auto &edge : node->edges())
        nodes.push_back(edge->toNode());

      return nodes;
    }

   private:
    Graph *graph_ { nullptr };
  };

  ShortestPath shortestPath(const_cast<Graph *>(this));

  ShortestPath::NodeList nodes;

  NodeArray pnodes;

  if (! shortestPath.search(fromNode.get(), toNode.get(), nodes))
    return pnodes;

  for (auto *node : nodes)
    pnodes.push_back(node);

  return pnodes;
}

bool
Graph::
isCycle(Node *node) const
{
  resetEdgeVisited();

  return isCycle(node, node);
}

bool
Graph::
isCycle(Node *node, Node *startNode) const
{
  auto edges = startNode->edges();

  for (auto &edge : edges) {
    if (edge->isVisited())
      continue;

    edge->setVisited(true);

    auto *fromNode = edge->fromNode();
    auto *toNode   = edge->toNode  ();

    if (fromNode == startNode) {
      if (node == toNode)
        return true;

      if (isCycle(node, toNode))
        return true;
    }
    else {
      if (node == fromNode)
        return true;

      if (isCycle(node, fromNode))
        return true;
    }
  }

  return false;
}

void
Graph::
resetNodeVisited() const
{
  for (const auto &node : nodes())
    node.second->setVisited(false);
}

void
Graph::
resetEdgeVisited() const
{
  for (const auto &edge : edges())
    edge->setVisited(false);
}

Graph::Graphs
Graph::
subGraphs() const
{
  Graphs graphs;

  resetNodeVisited();
  resetEdgeVisited();

  int ind = 0;

  // find next start node
  auto nextStartNode = [&]() {
    NodeP startNode;

    for (const auto &node : nodes()) {
      if (! node.second->isVisited())
        return node.second;
    }

    return NodeP();
  };

  // get start node
  auto startNode = nextStartNode();

  while (startNode) {
    // create sub graph for node
    auto subName = "subgraph_" + std::to_string(ind++);

    auto subGraph = GraphP(parse_->makeGraph(subName));

    graphs.push_back(subGraph);

    // add node edges
    addNodeToSubGraph(startNode.get(), subGraph);

    // next start node
    startNode = nextStartNode();
  }

  return graphs;
}

void
Graph::
addNodeToSubGraph(Node *startNode, GraphP subGraph) const
{
  startNode->setVisited(true);

  auto edges = startNode->edges();

  for (auto &edge : edges) {
    if (edge->isVisited())
      continue;

    edge->setVisited(true);

    auto subEdge = EdgeP(parse_->makeEdge(edge->fromNode(), edge->toNode()));

    subGraph->addEdge(subEdge);

    addNodeToSubGraph(edge->toNode(), subGraph);
  }
}

#if 0
Graph::Edges
Graph::
getEdges() const
{
  std::set<EdgeP> edges;

  for (const auto &pn : nodes_) {
    for (auto &edge : pn.second->edges())
      edges.insert(edge);
  }

  return edges;
}
#endif

//---

Edge::
Edge(Node *fromNode, Node *toNode) :
 fromNode_(fromNode), toNode_(toNode)
{
  setAttribute("shape", "arrow");
}

void
Edge::
setAttribute(const std::string &name, const std::string &value)
{
  attributes_.setNameValue(name, value);
}

void
Edge::
print(std::ostream &os) const
{
  os << fromNode_->name() << " -> " << toNode_->name();

  if (! attributes().empty()) {
    os << " [";

    bool first = true;

    for (const auto &pn : attributes()) {
      if (! first)
        os << ",";

      os << pn.first << "=" << pn.second;

      first = false;
    }

    os << "]";
  }
}

void
Edge::
outputCSV(std::ostream &os) const
{
  os << fromNode_->name() << "," << toNode_->name() << "," << attributesCSVStr() << ",";

  if (graph())
    os << graph()->hierName();

  os << "\n";
}

std::string
Edge::
attributesCSVStr() const
{
  std::string str = "\"";

  bool first = true;

  for (const auto &pn : attributes()) {
    const auto &name  = pn.first;
    const auto &value = pn.second;

    if      (name == "shape") {
    }
    else if (name == "label") {
    }
    else if (name == "arrowhead") {
    }
    else {
      continue;
    }

    if (! first)
      str += ",";

    if (value.substr(0, 1) == "\"")
      str += name + "='" + value.substr(1, value.size() - 2) + "'";
    else
      str += name + "=" + value;

    first = false;
  }

  str += "\"";

  return str;
}

//---

Node::
Node(Graph *graph, const std::string &name) :
 graph_(graph), name_(name)
{
  assert(graph);

  setAttribute("shape", "circle");
}

void
Node::
setAttribute(const std::string &name, const std::string &value)
{
  if      (name == "color")
    setColor(value);
  else if (name == "label")
    setLabel(value);

  attributes_.setNameValue(name, value);
}

EdgeP
Node::
addNodeEdge(Node *node)
{
  // TODO: check for existing edge
  auto edge = EdgeP(graph()->parse()->makeEdge(this, node));

  addEdge(edge);

  for (const auto &pn : graph()->nodeAttributes())
    edge->setAttribute(pn.first, pn.second);

  graph_->addEdge(edge);

  if (node->graph_ != graph_)
    graph_->addEdge(edge);

  return edge;
}

void
Node::
addEdge(EdgeP edge)
{
  edges_.push_back(edge);
}

void
Node::
print(std::ostream &os) const
{
  os << "Node: " << name();

  if (! attributes().empty()) {
    os << " [";

    bool first = true;

    for (const auto &pn : attributes()) {
      if (! first)
        os << ",";

      os << pn.first << "=" << pn.second;

      first = false;
    }

    os << "]";
  }

  if (! edges_.empty()) {
    for (const auto &edge : edges_) {
      os << "\n "; edge->print(os);
    }
  }
}

void
Node::
outputCSV(std::ostream &os) const
{
  os << name() << ",," << attributesCSVStr() << ",";

  if (graph())
    os << graph()->hierName();

  os << "\n";

  for (const auto &edge : edges_) {
    edge->outputCSV(os);
  }
}

std::string
Node::
attributesCSVStr() const
{
  std::string str = "\"";

  bool first = true;

  for (const auto &pn : attributes()) {
    auto name  = pn.first;
    auto value = pn.second;

    if      (name == "shape") {
    }
    else if (name == "sides") {
      name = "num_sides";
    }
    else if (name == "label") {
    }
    else if (name == "color" || name == "fillcolor") {
    }
    else if (name == "style") {
    }
    else if (name == "gradientangle") {
    }
    else if (name == "fontname") {
      name = "font";
    }
    else if (name == "orientation") {
      name = "angle";
    }
    else {
      continue;
    }

    if (! first)
      str += ",";

    if (value.substr(0, 1) == "\"")
      str += name + "='" + value.substr(1, value.size() - 2) + "'";
    else
      str += name + "=" + value;

    first = false;
  }

  str += "\"";

  return str;
}

}
