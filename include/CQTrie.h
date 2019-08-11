#ifndef CQTrie_H
#define CQTrie_H

#include <QString>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>

class CQTriePatterns;

/*!
 * \brief Trie Node
 * \ingroup 
 */
class CQTrieNode {
 public:
  using CharNodeMap = std::map<QChar,CQTrieNode *>;
  using String      = QString;

 public:
  CQTrieNode(CQTrieNode *parent=nullptr, QChar c='\0');

 ~CQTrieNode();

  CQTrieNode *parent() const { return parent_; }

  QChar c() const { return c_; }

  const CharNodeMap &children() const { return nodes_; }

  int numChildren() const { return nodes_.size(); }

  int count() const { return count_; }

  void incCount() { ++count_; }

  CQTrieNode *addChar(QChar c);

  String str() const;

  void patterns(int depth, CQTriePatterns &patterns) const;

  void subPatterns(const String &prefix, int depth, CQTriePatterns &patterns) const;

  int patternIndex(const String &str, const CQTriePatterns &patterns) const;

  int subPatternIndex(const String &str, int i, const CQTriePatterns &patterns) const;

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
  CQTrieNode* parent_ { nullptr };
  QChar             c_      { '\0' };
  CharNodeMap       nodes_;
  int               count_  { 0 };
};

//------

/*!
 * \brief Trie
 * \ingroup 
 */
class CQTrie {
 public:
  using CharSet = std::set<QChar>;
  using String  = QString;
  using Strings = std::vector<String>;

 public:
  CQTrie();

 ~CQTrie();

  void clear();

  void addWord(const String &str);

 private:
  template<class VISITOR>
  void visit(VISITOR &v, CQTrieNode *node, const String &str) {
    for (const auto &n : node->children()) {
      if (n.first.isNull())
        v.visit(str, n.second->count());
      else
        visit(v, n.second, str + n.first);
    }
  }

 public:
  CQTrieNode *root() const;

  int numWords() const;

 public:
  template<class VISITOR>
  void visit(VISITOR &v) {
    CQTrieNode *node = root();

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

  void patterns(int depth, CQTriePatterns &patterns) const;

  int patternIndex(const String &str, const CQTriePatterns &patterns) const;

  String indexPattern(int i, const CQTriePatterns &patterns) const;

 private:
  void complete(CQTrieNode *node, const String &str, Strings &strs, MatchData &matchData);

  CQTrieNode *addNode(CQTrieNode *parent, QChar c);

 private:
  CQTrieNode* root_ { nullptr };
};

//------

/*!
 * Class used to return matching patterns in Trie
 */
class CQTriePatterns {
 public:
  using String     = QString;
  using Strings    = std::vector<String>;
  using NodeIndMap = std::map<const CQTrieNode *,int>;

 public:
  CQTriePatterns();

  int depth() const { return depth_; }
  void setDepth(int i) { depth_ = i; }

  void clear();

  int numPatterns() const;

  void addPattern(const CQTrieNode *node, const String &pattern);

  void addPatterns(const CQTriePatterns &patterns);

  int nodeInd(const CQTrieNode *node) const;

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
