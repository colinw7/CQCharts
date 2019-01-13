#include <CQChartsTrie.h>

CQChartsTrieNode::
CQChartsTrieNode(CQChartsTrieNode *parent, QChar c) :
 parent_(parent), c_(c)
{
}

CQChartsTrieNode::
~CQChartsTrieNode()
{
  for (auto &node : nodes_)
    delete node.second;
}

CQChartsTrieNode *
CQChartsTrieNode::
addChar(QChar c)
{
  auto p = nodes_.find(c);

  if (p == nodes_.end())
    p = nodes_.insert(p, CharNodeMap::value_type(c, new CQChartsTrieNode(this, c)));

  return (*p).second;
}

CQChartsTrieNode::String
CQChartsTrieNode::
str() const
{
  if (! parent_)
    return "";

  return parent_->str() + c_;
}

void
CQChartsTrieNode::
patterns(int depth, CQChartsTriePatterns &patterns) const
{
  patterns.setDepth(depth);

  String prefix;

  subPatterns(prefix, depth, patterns);
}

void
CQChartsTrieNode::
subPatterns(const String &prefix, int depth, CQChartsTriePatterns &patterns) const
{
  if (depth == 0)
    return;

  if (nodes_.empty()) {
    patterns.addPattern(this, prefix);

    return;
  }

  if (nodes_.size() == 1) {
    String prefix1 = prefix;

    auto p = nodes_.begin();

    QChar c = (*p).first;

    if (! c.isNull())
      prefix1 += c;

    return (*p).second->subPatterns(prefix1, depth, patterns);
  }

  if (depth == 1) {
    for (const auto &node : nodes_) {
      String prefix1 = prefix;

      QChar             c     = node.first;
      CQChartsTrieNode *child = node.second;

      String pattern = prefix1;

      if (! c.isNull()) {
        pattern += escapeChar(c);

        CQChartsTrieNode *node1 = child;

        while (node1 && node1->nodes_.size() == 1) {
          auto p = *node1->nodes_.begin();

          QChar c1 = p.first;

          if (c1.isNull())
            break;

          pattern += escapeChar(c1);

          node1 = p.second;
        }

        if (node1 && node1->nodes_.size() > 1)
          pattern += "*";
      }

      patterns.addPattern(child, pattern);
    }
  }
  else {
    for (const auto &node : nodes_) {
      String prefix1 = prefix;

      QChar c = node.first;

      if (! c.isNull())
        prefix1 += escapeChar(c);

      CQChartsTriePatterns patterns1;

      node.second->subPatterns(prefix1, depth - 1, patterns1);

      patterns.addPatterns(patterns1);
    }
  }
}

int
CQChartsTrieNode::
patternIndex(const String &str, const CQChartsTriePatterns &patterns) const
{
  int i = 0;

  return subPatternIndex(str, i, patterns);
}

int
CQChartsTrieNode::
subPatternIndex(const String &str, int i, const CQChartsTriePatterns &patterns) const
{
  int ind = patterns.nodeInd(this);

  if (ind >= 0)
    return ind;

  QChar c;

  if (i >= str.length())
    c = 0;
  else
    c = str[i];

  for (const auto &node : nodes_) {
    QChar c1 = node.first;

    if (c1 == c)
      return node.second->subPatternIndex(str, i + 1, patterns);
  }

  return -1;
}

#if 0
CQChartsTrieNode::String
CQChartsTrieNode::
pattern() const
{
String childStr  = childPattern ();
String parentStr = parentPattern();

return parentStr + childStr;
}

CQChartsTrieNode::String
CQChartsTrieNode::
parentPattern() const
{
CQChartsTrieNode *pnode = parent();

if (! pnode) // root
  return "";

return pnode->parentPattern() + pnode->nodesPattern();
}

CQChartsTrieNode::String
CQChartsTrieNode::
childPattern() const
{
String pattern = nodesPattern();

using NodeSet = std::set<CQChartsTrieNode *>;

NodeSet nodeSet;

for (const auto &node : nodes_)
  nodeSet.insert(node.second);

if (nodeSet.empty())
  return pattern;

if (nodeSet.size() == 1)
  return (*nodeSet.begin())->childPattern();

return pattern + "*";
}

CQChartsTrieNode::String
CQChartsTrieNode::
nodesPattern() const
{
if (nodes_.empty())
  return "";

if (nodes_.size() == 1) {
  auto p = nodes_.begin();

  QChart c = (*p).first;

  if (! c)
    return "";

  return String(&c, 1);
}

CharSet charSet;

nodeChars(charSet);

//---

String pattern = "[";

QChar firstChar = 0;
QChar lastChar  = 0;

for (const auto &c : charSet) {
  if (! lastChar.isNull() && lastChar + 1 == c) {
    lastChar = c;
  }
  else {
    if (firstChar != 0) {
      QChar c1 = firstChar;
      QChar c2 = lastChar;

      pattern += c1;

      if (c1 != c2) {
        pattern += "-";
        pattern += c2;
      }
    }

    firstChar = c;
    lastChar  = c;
  }
}

if (firstChar != 0) {
  QChar c1 = firstChar;
  QChar c2 = lastChar;

  pattern += c1;

  if (c1 != c2) {
    pattern += "-";
    pattern += c2;
  }
}

pattern += "]";

return pattern;
}

void
CQChartsTrieNode::
nodeChars(CharSet &charSet) const
{
for (const auto &node : nodes_) {
  charSet.insert(node.first);
}
}
#endif
int
CQChartsTrieNode::
numWords() const
{
  int n = 0;

  for (const auto &node : nodes_) {
    if (node.first == '\0')
      ++n;

    n += node.second->numWords();
  }

  return n;
}

CQChartsTrieNode::String
CQChartsTrieNode::
escapeChar(QChar c) const
{
  if      (c == '*' ) return "\\*";
  else if (c == '?' ) return "\\?";
  else if (c == '\\') return "\\\\";

  return String(&c, 1);
}

//------

CQChartsTriePatterns::
CQChartsTriePatterns()
{
}

void
CQChartsTriePatterns::
clear()
{
  depth_ = -1;

  nodeIndMap_.clear();
  patterns_  .clear();
}

int
CQChartsTriePatterns::
numPatterns() const
{
  return patterns_.size();
}

void
CQChartsTriePatterns::
addPattern(const CQChartsTrieNode *node, const String &pattern)
{
//std::cerr << node->str().toStdString() << " : " << pattern.toStdString() <<
//             " (#" << patterns_.size() << ")\n";

  nodeIndMap_[node] = numPatterns();

  patterns_.push_back(pattern);
}

void
CQChartsTriePatterns::
addPatterns(const CQChartsTriePatterns &patterns)
{
  for (const auto &nodeInd : patterns.nodeIndMap_) {
    addPattern(nodeInd.first, patterns.patterns_[nodeInd.second]);
  }
}

int
CQChartsTriePatterns::
nodeInd(const CQChartsTrieNode *node) const
{
  auto p = nodeIndMap_.find(node);

  if (p != nodeIndMap_.end())
    return (*p).second;

  return -1;
}

CQChartsTriePatterns::String
CQChartsTriePatterns::
pattern(int i) const
{
  if (i < 0 || i >= int(patterns_.size()))
    return "";

  return patterns_[i];
}

#if 0
void
CQChartsTriePatterns::
print(std::ostream &os) const
{
  os << numPatterns() << " patterns\n";

  for (int i = 0; i < numPatterns(); ++i)
    os << " " << pattern(i).toStdString() << "\n";
}
#endif

//------

CQChartsTrie::
CQChartsTrie()
{
}

CQChartsTrie::
~CQChartsTrie()
{
  clear();
}

void
CQChartsTrie::
clear()
{
  delete root_;

  root_ = nullptr;
}

void
CQChartsTrie::
addWord(const String &str)
{
  CQChartsTrieNode *node = root();

  for (const auto &c : str) {
    node = addNode(node, c);
  }

  node = addNode(node, '\0');

  node->incCount();
}

CQChartsTrieNode *
CQChartsTrie::
root() const
{
  if (! root_) {
    CQChartsTrie *th = const_cast<CQChartsTrie *>(this);

    th->root_ = new CQChartsTrieNode;
  }

  return root_;
}

int
CQChartsTrie::
numWords() const
{
  CQChartsTrieNode *node = root();

  return node->numWords();
}

void
CQChartsTrie::
dump(std::ostream &os)
{
  class Dumper {
   public:
    Dumper(std::ostream &os) :
     os_(os) {
    }

    void visit(const String &str, int n) {
      os_ << str.toStdString() << "(#" << n << ")\n";
    }

   private:
    std::ostream &os_;
  };

  Dumper dumper(os);

  visit(dumper);
}

void
CQChartsTrie::
complete(const String &match, Strings &strs)
{
  MatchData matchData(match);

  CQChartsTrieNode *node = root();

  String str;

  complete(node, str, strs, matchData);
}

void
CQChartsTrie::
patterns(int depth, CQChartsTriePatterns &patterns) const
{
  CQChartsTrieNode *root = this->root();

  return root->patterns(depth, patterns);
}

int
CQChartsTrie::
patternIndex(const String &str, const CQChartsTriePatterns &patterns) const
{
  CQChartsTrieNode *root = this->root();

  return root->patternIndex(str, patterns);
}

CQChartsTrie::String
CQChartsTrie::
indexPattern(int i, const CQChartsTriePatterns &patterns) const
{
  return patterns.pattern(i);
}

void
CQChartsTrie::
complete(CQChartsTrieNode *node, const String &str, Strings &strs, MatchData &matchData)
{
  for (const auto &n : node->children()) {
    if (matchData.pos == matchData.len) {
      if (n.first.isNull())
        strs.push_back(matchData.str + str);
      else
        complete(n.second, str + n.first, strs, matchData);
    }
    else {
      if (! n.first.isNull() && n.first == matchData.str[matchData.pos]) {
        ++matchData.pos;

        complete(n.second, str, strs, matchData);

        --matchData.pos;
      }
    }
  }
}

CQChartsTrieNode *
CQChartsTrie::
addNode(CQChartsTrieNode *parent, QChar c)
{
  return parent->addChar(c);
}
