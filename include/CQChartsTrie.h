#ifndef CQChartsTrie_H
#define CQChartsTrie_H

#include <QString>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>

class CQChartsTriePatterns;

/*!
 * \brief Trie Node
 * \ingroup Charts
 */
class CQChartsTrieNode {
 public:
  using CharNodeMap = std::map<QChar,CQChartsTrieNode *>;
  using String      = QString;

 public:
  CQChartsTrieNode(CQChartsTrieNode *parent=nullptr, QChar c='\0');

 ~CQChartsTrieNode();

  CQChartsTrieNode *parent() const { return parent_; }

  QChar c() const { return c_; }

  const CharNodeMap &children() const { return nodes_; }

  int numChildren() const { return nodes_.size(); }

  int count() const { return count_; }

  void incCount() { ++count_; }

  CQChartsTrieNode *addChar(QChar c);

  String str() const;

  void patterns(int depth, CQChartsTriePatterns &patterns) const;

  void subPatterns(const String &prefix, int depth, CQChartsTriePatterns &patterns) const;

  int patternIndex(const String &str, const CQChartsTriePatterns &patterns) const;

  int subPatternIndex(const String &str, int i, const CQChartsTriePatterns &patterns) const;

#if 0
  String pattern() const;

  String parentPattern() const;

  String childPattern() const;

  String nodesPattern() const;

  void nodeChars(CharSet &charSet) const;
#endif

  int numWords() const;

  String escapeChar(QChar c) const;

 private:
  CQChartsTrieNode* parent_ { nullptr };
  QChar             c_      { '\0' };
  CharNodeMap       nodes_;
  int               count_  { 0 };
};

//------

/*!
 * \brief Trie
 * \ingroup Charts
 */
class CQChartsTrie {
 public:
  using CharSet = std::set<QChar>;
  using String  = QString;
  using Strings = std::vector<String>;

 public:
  CQChartsTrie();

 ~CQChartsTrie();

  void clear();

  void addWord(const String &str);

 private:
  template<class VISITOR>
  void visit(VISITOR &v, CQChartsTrieNode *node, const String &str) {
    for (const auto &n : node->children()) {
      if (n.first.isNull())
        v.visit(str, n.second->count());
      else
        visit(v, n.second, str + n.first);
    }
  }

 public:
  CQChartsTrieNode *root() const;

  int numWords() const;

 public:
  template<class VISITOR>
  void visit(VISITOR &v) {
    CQChartsTrieNode *node = root();

    String str;

    visit(v, node, str);
  }

  void dump(std::ostream &os=std::cerr);

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
  void complete(const String &match, Strings &strs);

  void patterns(int depth, CQChartsTriePatterns &patterns) const;

  int patternIndex(const String &str, const CQChartsTriePatterns &patterns) const;

  String indexPattern(int i, const CQChartsTriePatterns &patterns) const;

 private:
  void complete(CQChartsTrieNode *node, const String &str, Strings &strs, MatchData &matchData);

  CQChartsTrieNode *addNode(CQChartsTrieNode *parent, QChar c);

 private:
  CQChartsTrieNode* root_ { nullptr };
};

//------

/*!
 * Class used to return matching patterns in Trie
 */
class CQChartsTriePatterns {
 public:
  using String     = QString;
  using Strings    = std::vector<String>;
  using NodeIndMap = std::map<const CQChartsTrieNode *,int>;

 public:
  CQChartsTriePatterns();

  int depth() const { return depth_; }
  void setDepth(int i) { depth_ = i; }

  void clear();

  int numPatterns() const;

  void addPattern(const CQChartsTrieNode *node, const String &pattern);

  void addPatterns(const CQChartsTriePatterns &patterns);

  int nodeInd(const CQChartsTrieNode *node) const;

  String pattern(int i) const;

#if 0
  void print(std::ostream &os) const;
#endif

 private:
  int        depth_ { -1 };
  NodeIndMap nodeIndMap_;
  Strings    patterns_;
};

#endif
