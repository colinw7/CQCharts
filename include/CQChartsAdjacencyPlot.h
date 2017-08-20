#ifndef CQChartsAdjacencyPlot_H
#define CQChartsAdjacencyPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQUtil.h>

class CQChartsAdjacencyPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int nodeColumn        READ nodeColumn        WRITE setNodeColumn       )
  Q_PROPERTY(int connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)
  Q_PROPERTY(int groupColumn       READ groupColumn       WRITE setGroupColumn      )
  Q_PROPERTY(int nameColumn        READ nameColumn        WRITE setNameColumn       )

 public:
  class Node {
   public:
    typedef std::pair<Node *,int>   NodeValue;
    typedef std::map<int,NodeValue> NodeMap;

   public:
    Node(int id, const std::string &name, int group) :
     id_(id), name_(name), group_(group) {
    }

    int                id   () const { return id_   ; }
    const std::string &name () const { return name_ ; }
    int                group() const { return group_; }

    int count() const { return count_; }

    int maxCount() const { return maxCount_; }

    NodeMap nodes() const { return nodes_; }

    void addNode(Node *node, int count) {
      nodes_[node->id()] = NodeValue(node, count);

      count_ += count;

      maxCount_ = std::max(maxCount_, count);
    }

    int nodeValue(Node *node) const {
      if (node == this) return 1;

      NodeMap::const_iterator p = nodes_.find(node->id());

      if (p == nodes_.end())
        return 0;

      return (*p).second.second;
    }

   private:
    int         id_ { 0 };
    std::string name_;
    int         group_ { 0 };
    int         count_ { 0 };
    int         maxCount_ { 0 };
    NodeMap     nodes_;
  };

  typedef std::map<int,Node *> NodeMap;

 public:
  CQChartsAdjacencyPlot(CQChartsWindow *window, QAbstractItemModel *model);

  int nodeColumn() const { return nodeColumn_; }
  void setNodeColumn(int i) { nodeColumn_ = i; update(); }

  int groupColumn() const { return groupColumn_; }
  void setGroupColumn(int i) { groupColumn_ = i; update(); }

  int connectionsColumn() const { return connectionsColumn_; }
  void setConnectionsColumn(int i) { connectionsColumn_ = i; update(); }

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  //---

  void addProperties();

  void updateRange();

  void initObjs(bool force=false);

  //---

  void draw(QPainter *) override;

 private:
  struct ConnectionData {
    int node;
    int count;

    ConnectionData(int node=-1, int count=-1) :
     node(node), count(count) {
    }
  };

  typedef std::vector<ConnectionData> ConnectionDataArray;

  struct ConnectionsData {
    int                 node;
    QString             name;
    int                 group;
    ConnectionDataArray connections;
  };

  typedef std::map<int,ConnectionsData> IdConnectionsData;

 private:
  bool decodeConnections(const QString &str, ConnectionDataArray &connections);
  bool decodeConnection(const QString &pointStr, ConnectionData &connection);

  void sortNodes();

  void drawNodes(QPainter *);

  QColor groupColor(int) const;

 private:
  typedef std::vector<Node *> NodeArray;

  int               nodeColumn_        { 0 };
  int               connectionsColumn_ { 1 };
  int               nameColumn_        { -1 };
  int               groupColumn_       { -1 };
  IdConnectionsData idConnections_;
  NodeMap           nodes_;
  NodeArray         sortedNodes_;
  int               ts_ { 0 };
  double            cs_ { 0 };
  int               maxValue_ { 0 };
  int               maxGroup_ { 0 };
};

#endif
