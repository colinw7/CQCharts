#ifndef CQChartsTrie_H
#define CQChartsTrie_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>

class CQChartsTrie {
 public:
  class Node;

  using CharNodeMap = std::map<QChar,Node *>;
  using CharSet     = std::set<QChar>;
  using String      = QString;
  using Strings     = std::vector<String>;
  using NodeIndMap  = std::map<const Node *,int>;

  class Patterns {
   public:
    Patterns() { }

    int depth() const { return depth_; }
    void setDepth(int i) { depth_ = i; }

    void clear() {
      depth_ = -1;

      nodeIndMap_.clear();
      patterns_  .clear();
    }

    int numPatterns() const {
      return patterns_.size();
    }

    void addPattern(const Node *node, const String &pattern) {
//std::cerr << node->str().toStdString() << " : " << pattern.toStdString() <<
//             " (#" << patterns_.size() << ")\n";

      nodeIndMap_[node] = numPatterns();

      patterns_.push_back(pattern);
    }

    void addPatterns(const Patterns &patterns) {
      for (const auto &nodeInd : patterns.nodeIndMap_) {
        addPattern(nodeInd.first, patterns.patterns_[nodeInd.second]);
      }
    }

    int nodeInd(const Node *node) const {
      auto p = nodeIndMap_.find(node);

      if (p != nodeIndMap_.end())
        return (*p).second;

      return -1;
    }

    String pattern(int i) const {
      if (i < 0 || i >= int(patterns_.size()))
        return "";

      return patterns_[i];
    }

    void print(std::ostream &os) const {
      os << numPatterns() << " patterns\n";

      for (int i = 0; i < numPatterns(); ++i)
        os << " " << pattern(i).toStdString() << "\n";
    }

   private:
    int        depth_ { -1 };
    NodeIndMap nodeIndMap_;
    Strings    patterns_;
  };

  class Node {
   public:
    Node(Node *parent=nullptr, QChar c='\0') :
     parent_(parent), c_(c) {
    }

   ~Node() {
      for (auto &node : nodes_)
        delete node.second;
    }

    Node *parent() const { return parent_; }

    QChar c() const { return c_; }

    const CharNodeMap &children() const { return nodes_; }

    int numChildren() const { return nodes_.size(); }

    int count() const { return count_; }

    void incCount() { ++count_; }

    Node *addChar(QChar c) {
      auto p = nodes_.find(c);

      if (p == nodes_.end())
        p = nodes_.insert(p, CharNodeMap::value_type(c, new Node(this, c)));

      return (*p).second;
    }

    String str() const {
      if (! parent_)
        return "";

      return parent_->str() + c_;
    }

    void patterns(int depth, Patterns &patterns) const {
      patterns.setDepth(depth);

      String prefix;

      subPatterns(prefix, depth, patterns);
    }

    void subPatterns(const String &prefix, int depth, Patterns &patterns) const {
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

          QChar c = node.first;

          String pattern = prefix1;

          if (! c.isNull()) {
            pattern += escapeChar(c);
            pattern += "*";
          }

          patterns.addPattern(node.second, pattern);
        }
      }
      else {
        for (const auto &node : nodes_) {
          String prefix1 = prefix;

          QChar c = node.first;

          if (! c.isNull())
            prefix1 += escapeChar(c);

          Patterns patterns1;

          node.second->subPatterns(prefix1, depth - 1, patterns1);

          patterns.addPatterns(patterns1);
        }
      }
    }

    int patternIndex(const String &str, const Patterns &patterns) const {
      int i = 0;

      return subPatternIndex(str, i, patterns);
    }

    int subPatternIndex(const String &str, int i, const Patterns &patterns) const {
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
    String pattern() const {
      String childStr  = childPattern ();
      String parentStr = parentPattern();

      return parentStr + childStr;
    }

    String parentPattern() const {
      Node *pnode = parent();

      if (! pnode) // root
        return "";

      return pnode->parentPattern() + pnode->nodesPattern();
    }

    String childPattern() const {
      String pattern = nodesPattern();

      using NodeSet = std::set<Node *>;

      NodeSet nodeSet;

      for (const auto &node : nodes_)
        nodeSet.insert(node.second);

      if (nodeSet.empty())
        return pattern;

      if (nodeSet.size() == 1)
        return (*nodeSet.begin())->childPattern();

      return pattern + "*";
    }

    String nodesPattern() const {
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

    void nodeChars(CharSet &charSet) const {
      for (const auto &node : nodes_) {
        charSet.insert(node.first);
      }
    }
#endif

    int numWords() const {
      int n = 0;

      for (const auto &node : nodes_) {
        if (node.first == '\0')
          ++n;

        n += node.second->numWords();
      }

      return n;
    }

    String escapeChar(QChar c) const {
      if      (c == '*' ) return "\\*";
      else if (c == '?' ) return "\\?";
      else if (c == '\\') return "\\\\";

      return String(&c, 1);
    }

   private:
    Node*       parent_ { nullptr };
    QChar       c_      { '\0' };
    CharNodeMap nodes_;
    int         count_  { 0 };
  };

 public:
  CQChartsTrie() { }

 ~CQChartsTrie() {
    clear();
  }

  void clear() {
    delete root_;

    root_ = nullptr;
  }

  void addWord(const String &str) {
    Node *node = root();

    for (const auto &c : str) {
      node = addNode(node, c);
    }

    node = addNode(node, '\0');

    node->incCount();
  }

 private:
  template<class VISITOR>
  void visit(VISITOR &v, Node *node, const String &str) {
    for (const auto &n : node->children()) {
      if (n.first.isNull())
        v.visit(str, n.second->count());
      else
        visit(v, n.second, str + n.first);
    }
  }

 public:
  Node *root() const {
    if (! root_) {
      CQChartsTrie *th = const_cast<CQChartsTrie *>(this);

      th->root_ = new Node;
    }

    return root_;
  }

  int numWords() const {
    Node *node = root();

    return node->numWords();
  }

 public:
  template<class VISITOR>
  void visit(VISITOR &v) {
    Node *node = root();

    String str;

    visit(v, node, str);
  }

  void dump(std::ostream &os=std::cerr) {
    class Dumper {
     public:
      Dumper(std::ostream &os) :
       os_(os) {
      }

      void visit(const String &str, int n) {
        std::cerr << str.toStdString() << "(#" << n << ")\n";
      }

     private:
      std::ostream &os_;
    };

    Dumper dumper(os);

    visit(dumper);
  }

 private:
  struct MatchData {
    MatchData(const String &match) :
     str(match), len(match.size()) {
    }

    String str;
    uint   pos { 0 };
    uint   len;
  };

 public:
  void complete(const String &match, Strings &strs) {
    MatchData matchData(match);

    Node *node = root();

    String str;

    complete(node, str, strs, matchData);
  }

  void patterns(int depth, Patterns &patterns) const {
    Node *root = this->root();

    return root->patterns(depth, patterns);
  }

  int patternIndex(const String &str, const Patterns &patterns) const {
    Node *root = this->root();

    return root->patternIndex(str, patterns);
  }

  String indexPattern(int i, const Patterns &patterns) const {
    return patterns.pattern(i);
  }

 private:
  void complete(Node *node, const String &str, Strings &strs, MatchData &matchData) {
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

  Node *addNode(Node *parent, QChar c) {
    return parent->addChar(c);
  }

 private:
  Node *root_ { nullptr };
};

#endif
