#include <CDotParse.h>
#include <CFileParse.h>

CDotParse::
CDotParse(const std::string &filename)
{
  parse_ = std::make_unique<CFileParse>(filename);

  parse_->setStream(true);
}

bool
CDotParse::
parse()
{
  EnterLeave el(this, "parse");

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
      pg.second->print();
  }

  if (isCSV()) {
    std::cout << "From,To,Attributes,Graph\n";

    for (const auto &pg : graphs_)
      pg.second->outputCSV();
  }

  return true;
}

bool
CDotParse::
parseGraph()
{
  EnterLeave el(this, "parseGraph");

  skipSpace();

  // graph id
  std::string id;

  if (! parseID(id))
    return errorMsg("expected identfier");

  currentGraph_ = getGraph(id);

  skipSpace();

  if (parse_->isChar('{')) {
    parse_->skipChar();

    parseStatementList();

    skipSpace();

    if (! parse_->isChar('}'))
      return errorMsg("expected }");

    parse_->skipChar();
  }

  currentGraph_ = nullptr;
  currentNode_  = nullptr;

  return true;
}

bool
CDotParse::
parseStatementList()
{
  EnterLeave el(this, "parseStatementList");

  while (true) {
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
CDotParse::
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

    subGraph->setParent(currentGraph_);

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
    currentNode_ = getNode(id);

    parseAttrList("");
  }
  // edge
  else if (parse_->isString("->") || parse_->isString("--")) {
    parse_->skipChar(2);

    skipSpace();

    std::vector<Node *> nodes1;

    Node *node1 = getNode(id);

    nodes1.push_back(node1);

    while (true) {
      std::vector<Node *> nodes2;

      if (parse_->isChar('{')) {
        parse_->skipChar();

        skipSpace();

        while (! parse_->isChar('}')) {
          std::string id1;

          if (! parseID(id1))
            return errorMsg("expected identfier");

          Node *node2 = getNode(id1);

          nodes2.push_back(node2);

          for (const auto &n1 : nodes1)
            currentEdge_ = n1->addEdge(node2);

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

        Node *node2 = getNode(id1);

        nodes2.push_back(node2);

        for (const auto &n1 : nodes1)
          currentEdge_ = n1->addEdge(node2);

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
    currentNode_ = getNode(id);
  }
  else {
    currentNode_ = getNode(id);
  }

  return true;
}

bool
CDotParse::
parseAttrList(const std::string &id)
{
  EnterLeave el(this, "parseAttrList " + id);

  while (true) {
    skipSpace();

    if (! parse_->isChar('['))
      return errorMsg("expected [");

    parse_->skipChar();

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
CDotParse::
parseAList(const std::string &id)
{
  EnterLeave el(this, "parseAList " + id);

  while (true) {
    skipSpace();

    std::string id1;

    if (! parseID(id1))
      return errorMsg("expected identifier");

    skipSpace();

    if (! parse_->isChar('='))
      return errorMsg("expected =");

    parse_->skipChar();

    skipSpace();

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
CDotParse::
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
    while (parse_->isDigit())
      id += parse_->readChar();
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
CDotParse::
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
CDotParse::
skipSpace()
{
  while (true) {
    // skip space
    parse_->skipSpace();

    // skip comments
    if      (parse_->isString("//")) {
      while (parse_->isString("//")) {
        parse_->skipChar(2);

        while (! parse_->eof() && ! parse_->isChar('\n'))
          parse_->skipChar();

        if (parse_->isChar('\n'))
          parse_->skipChar();

        parse_->skipSpace();
      }
    }
    else if (parse_->isString("/*")) {
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
CDotParse::
enter(const std::string &proc) const
{
  if (isDebug()) {
    depthSpaces(); std::cerr << "> " << proc << "\n";
  }

  ++depth_;
}

void
CDotParse::
leave(const std::string &proc) const
{
  --depth_;

  if (isDebug()) {
    depthSpaces(); std::cerr << "< " << proc << "\n";
  }
}

void
CDotParse::
depthSpaces() const
{
  for (int i = 0; i < depth_; ++i)
    std::cerr << " ";
}

bool
CDotParse::
errorMsg(const std::string &msg) const
{
  std::cerr << "Error: " << parse_->fileName() << "@" <<
               parse_->lineNum() << ":" << parse_->charNum() << " ";
  std::cerr << msg << "\n";
  return false;
}

CDotParse::Graph *
CDotParse::
getGraph(const std::string &name)
{
  auto p = graphs_.find(name);

  if (p == graphs_.end())
    p = graphs_.insert(p, GraphMap::value_type(name, new Graph(name)));

  return (*p).second;
}

CDotParse::Node *
CDotParse::
getNode(const std::string &name)
{
  for (auto &pg : graphs_) {
    auto *graph = pg.second;

    auto *node = graph->getNode(name, /*create*/false);
    if (node) return node;
  }

  return currentGraph_->getNode(name, /*create*/true);
}

//---

CDotParse::Graph::
Graph(const std::string &name) :
 name_(name)
{
}

CDotParse::Node *
CDotParse::Graph::
getNode(const std::string &name, bool create)
{
  auto p = nodes_.find(name);

  if (p == nodes_.end()) {
    if (! create)
      return nullptr;

    //---

    Node *node = new Node(name);

    node->setGraph(this);

    p = nodes_.insert(p, NodeMap::value_type(name, node));

    for (const auto &pn : nodeAttributes()) {
      node->setAttribute(pn.first, pn.second);
    }
  }

  return (*p).second;
}

void
CDotParse::Graph::
setAttribute(const std::string &name, const std::string &value)
{
  attributes_.setNameValue(name, value);
}

void
CDotParse::Graph::
setNodeAttribute(const std::string &name, const std::string &value)
{
  nodeAttributes_.setNameValue(name, value);
}

void
CDotParse::Graph::
setEdgeAttribute(const std::string &name, const std::string &value)
{
  edgeAttributes_.setNameValue(name, value);
}

void
CDotParse::Graph::
print() const
{
  std::cout << "Graph " << name_ << "\n";

  for (auto &pn : nodes_) {
    std::cout << " "; pn.second->print(); std::cout << "\n";
  }
}

void
CDotParse::Graph::
outputCSV() const
{
  for (auto &pn : nodes_)
    pn.second->outputCSV();
}

//---

CDotParse::Edge::
Edge(Node *fromNode, Node *toNode) :
 fromNode_(fromNode), toNode_(toNode)
{
  setAttribute("shape", "arrow");
}

void
CDotParse::Edge::
setAttribute(const std::string &name, const std::string &value)
{
  attributes_.setNameValue(name, value);
}

void
CDotParse::Edge::
print() const
{
  std::cout << fromNode_->name() << " -> " << toNode_->name();

  if (! attributes().empty()) {
    std::cout << " [";

    bool first = true;

    for (const auto &pn : attributes()) {
      if (! first)
        std::cout << ",";

      std::cout << pn.first << "=" << pn.second;

      first = false;
    }

    std::cout << "]";
  }
}

void
CDotParse::Edge::
outputCSV() const
{
  std::cout << fromNode_->name() << "," << toNode_->name() << "," << attributesCSVStr() << ",";

  if (graph())
    std::cout << graph()->hierName();

  std::cout << "\n";
}

std::string
CDotParse::Edge::
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

CDotParse::Node::
Node(const std::string &name) :
 name_(name)
{
  setAttribute("shape", "circle");
}

void
CDotParse::Node::
setAttribute(const std::string &name, const std::string &value)
{
  attributes_.setNameValue(name, value);
}

CDotParse::Edge *
CDotParse::Node::
addEdge(Node *node)
{
  // TODO: check for existing edge
  Edge *edge = new Edge(this, node);

  edges_.push_back(edge);

  for (const auto &pn : graph()->nodeAttributes()) {
    edge->setAttribute(pn.first, pn.second);
  }

  return edge;
}

void
CDotParse::Node::
print() const
{
  std::cout << "Node: " << name();

  if (! attributes().empty()) {
    std::cout << " [";

    bool first = true;

    for (const auto &pn : attributes()) {
      if (! first)
        std::cout << ",";

      std::cout << pn.first << "=" << pn.second;

      first = false;
    }

    std::cout << "]";
  }

  if (! edges_.empty()) {
    for (const auto &edge : edges_) {
      std::cout << "\n "; edge->print();
    }
  }
}

void
CDotParse::Node::
outputCSV() const
{
  std::cout << name() << ",," << attributesCSVStr() << ",";

  if (graph())
    std::cout << graph()->hierName();

  std::cout << "\n";

  for (const auto &edge : edges_) {
    edge->outputCSV();
  }
}

std::string
CDotParse::Node::
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
