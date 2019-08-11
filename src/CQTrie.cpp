#include <CQTrie.h>

CQTrieNode::
CQTrieNode(CQTrieNode *parent, QChar c) :
 parent_(parent), c_(c)
{
}

CQTrieNode::
~CQTrieNode()
{
  for (auto &node : nodes_)
    delete node.second;
}

CQTrieNode *
CQTrieNode::
addChar(QChar c)
{
  auto p = nodes_.find(c);

  if (p == nodes_.end())
    p = nodes_.insert(p, CharNodeMap::value_type(c, new CQTrieNode(this, c)));

  return (*p).second;
}

CQTrieNode::String
CQTrieNode::
str() const
{
  if (! parent_)
    return "";

  return parent_->str() + c_;
}

void
CQTrieNode::
patterns(int depth, CQTriePatterns &patterns) const
{
  patterns.setDepth(depth);

  String prefix;

  subPatterns(prefix, depth, patterns);
}

void
CQTrieNode::
subPatterns(const String &prefix, int depth, CQTriePatterns &patterns) const
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
      CQTrieNode *child = node.second;

      String pattern = prefix1;

      if (! c.isNull()) {
        pattern += escapeChar(c);

        CQTrieNode *node1 = child;

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

      CQTriePatterns patterns1;

      node.second->subPatterns(prefix1, depth - 1, patterns1);

      patterns.addPatterns(patterns1);
    }
  }
}

int
CQTrieNode::
patternIndex(const String &str, const CQTriePatterns &patterns) const
{
  int i = 0;

  return subPatternIndex(str, i, patterns);
}

int
CQTrieNode::
subPatternIndex(const String &str, int i, const CQTriePatterns &patterns) const
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
CQTrieNode::String
CQTrieNode::
pattern() const
{
  String childStr  = childPattern ();
  String parentStr = parentPattern();

  return parentStr + childStr;
}

CQTrieNode::String
CQTrieNode::
parentPattern() const
{
  CQTrieNode *pnode = parent();

  if (! pnode) // root
    return "";

  return pnode->parentPattern() + pnode->nodesPattern();
}

CQTrieNode::String
CQTrieNode::
childPattern() const
{
  String pattern = nodesPattern();

  using NodeSet = std::set<CQTrieNode *>;

  NodeSet nodeSet;

  for (const auto &node : nodes_)
    nodeSet.insert(node.second);

  if (nodeSet.empty())
    return pattern;

  if (nodeSet.size() == 1)
    return (*nodeSet.begin())->childPattern();

  return pattern + "*";
}

CQTrieNode::String
CQTrieNode::
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
CQTrieNode::
nodeChars(CharSet &charSet) const
{
  for (const auto &node : nodes_) {
    charSet.insert(node.first);
  }
}
#endif
int
CQTrieNode::
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

CQTrieNode::String
CQTrieNode::
escapeChar(QChar c) const
{
  if      (c == '*' ) return "\\*";
  else if (c == '?' ) return "\\?";
  else if (c == '\\') return "\\\\";

  return String(&c, 1);
}

//------

CQTriePatterns::
CQTriePatterns()
{
}

void
CQTriePatterns::
clear()
{
  depth_ = -1;

  nodeIndMap_.clear();
  patterns_  .clear();
}

int
CQTriePatterns::
numPatterns() const
{
  return patterns_.size();
}

void
CQTriePatterns::
addPattern(const CQTrieNode *node, const String &pattern)
{
//std::cerr << node->str().toStdString() << " : " << pattern.toStdString() <<
//             " (#" << patterns_.size() << ")\n";

  nodeIndMap_[node] = numPatterns();

  patterns_.push_back(pattern);
}

void
CQTriePatterns::
addPatterns(const CQTriePatterns &patterns)
{
  for (const auto &nodeInd : patterns.nodeIndMap_) {
    addPattern(nodeInd.first, patterns.patterns_[nodeInd.second]);
  }
}

int
CQTriePatterns::
nodeInd(const CQTrieNode *node) const
{
  auto p = nodeIndMap_.find(node);

  if (p != nodeIndMap_.end())
    return (*p).second;

  return -1;
}

CQTriePatterns::String
CQTriePatterns::
pattern(int i) const
{
  if (i < 0 || i >= int(patterns_.size()))
    return "";

  return patterns_[i];
}

#if 0
void
CQTriePatterns::
print(std::ostream &os) const
{
  os << numPatterns() << " patterns\n";

  for (int i = 0; i < numPatterns(); ++i)
    os << " " << pattern(i).toStdString() << "\n";
}
#endif

//------

CQTrie::
CQTrie()
{
}

CQTrie::
~CQTrie()
{
  clear();
}

void
CQTrie::
clear()
{
  delete root_;

  root_ = nullptr;
}

void
CQTrie::
addWord(const String &str)
{
  CQTrieNode *node = root();

  for (const auto &c : str) {
    node = addNode(node, c);
  }

  node = addNode(node, '\0');

  node->incCount();
}

CQTrieNode *
CQTrie::
root() const
{
  if (! root_) {
    CQTrie *th = const_cast<CQTrie *>(this);

    th->root_ = new CQTrieNode;
  }

  return root_;
}

int
CQTrie::
numWords() const
{
  CQTrieNode *node = root();

  return node->numWords();
}

void
CQTrie::
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
CQTrie::
complete(const String &match, Strings &strs)
{
  MatchData matchData(match);

  CQTrieNode *node = root();

  String str;

  complete(node, str, strs, matchData);
}

void
CQTrie::
patterns(int depth, CQTriePatterns &patterns) const
{
  CQTrieNode *root = this->root();

  return root->patterns(depth, patterns);
}

int
CQTrie::
patternIndex(const String &str, const CQTriePatterns &patterns) const
{
  CQTrieNode *root = this->root();

  return root->patternIndex(str, patterns);
}

CQTrie::String
CQTrie::
indexPattern(int i, const CQTriePatterns &patterns) const
{
  return patterns.pattern(i);
}

void
CQTrie::
complete(CQTrieNode *node, const String &str, Strings &strs, MatchData &matchData)
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

CQTrieNode *
CQTrie::
addNode(CQTrieNode *parent, QChar c)
{
  return parent->addChar(c);
}
