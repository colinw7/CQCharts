#ifndef CQHierSepModel_H
#define CQHierSepModel_H

#include <QAbstractProxyModel>
#include <QString>
#include <vector>
#include <cassert>

class CQBaseModel;

//---

enum CQHierConnectionType {
  NONE,
  HIER,
  CONNECTIONS,
};

struct CQHierSepData {
  int                  column           { 0 };                          //!< fold column
  int                  connectionColumn { -1 };                         //!< connection column
  int                  nodeColumn       { -1 };                         //!< node column
  QChar                separator        { '/' };                        //!< separator
  CQHierConnectionType connectionType   { CQHierConnectionType::NONE }; //!< connection type

  CQHierSepData() { }

  CQHierSepData(int column, const QChar &separator='/') :
   column(column), separator(separator) {
  }
};

//---

/*!
 * fold model into hierarchical model using hier separated string in specified column
 */
class CQHierSepModel : public QAbstractProxyModel {
  Q_OBJECT

  Q_PROPERTY(QChar          separator         READ separator         WRITE setSeparator        )
  Q_PROPERTY(int            foldColumn        READ foldColumn        WRITE setFoldColumn       )
  Q_PROPERTY(int            connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)
  Q_PROPERTY(PropagateValue propagateValue    READ propagateValue    WRITE setPropagateValue   )

  Q_ENUMS(PropagateValue)

 public:
  enum class PropagateValue {
    NONE,
    SUM
  };

 public:
  CQHierSepModel(QAbstractItemModel *model, const CQHierSepData &data=CQHierSepData());

 ~CQHierSepModel();

  //---

  // get/set source model
  QAbstractItemModel *sourceModel() const;
  void setSourceModel(QAbstractItemModel *sourceModel) override;

  //---

  // get/set separator
  const QChar &separator() const { return data_.separator; }
  void setSeparator(const QChar &c);

  // get/set fold column
  int foldColumn() const { return data_.column; }
  void setFoldColumn(int i);

  // get/set connections column
  int connectionsColumn() const { return data_.connectionColumn; }
  void setConnectionsColumn(int i);

  // get/set node column
  int nodeColumn() const { return data_.nodeColumn; }
  void setNodeColumn(int i);

  // get/set propagate value
  const PropagateValue &propagateValue() const { return propagateValue_; }
  void setPropagateValue(const PropagateValue &v);

  //---

  // # Abstract Model APIS

  // get column count
  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  // get child row count of index
  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  // get child of parent at row/column
  QModelIndex index(int row, int column, const QModelIndex &parent) const override;

  // get parent of child
  QModelIndex parent(const QModelIndex &child) const override;

  // does parent have children
  bool hasChildren(const QModelIndex &parent) const override;

  // get role data for index
  QVariant data(const QModelIndex &index, int role) const override;

  // set role data for index
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

  // get header data for column/section
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  // set header data for column/section
  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role) override;

  // get flags for index
  Qt::ItemFlags flags(const QModelIndex &index) const override;

  //---

  // # Abstract Proxy Model APIS

  // map source index to proxy index
  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
  // map proxy index to source index
  QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

 private:
  class Node;

  // Node structure for tree construct
  class Node {
   public:
    Node() { }

    Node(Node *parent, const QString &str="") :
     parent_(parent), str_(str) {
      assert(parent_ != this);

      if (parent)
        parent->children_.push_back(this);
    }

   ~Node() {
      for (auto &child : children_)
        delete child;
    }

    Node *parent() const { return parent_; }

    const QString &str() const { return str_; }

    bool hasChildren() const { return ! children_.empty(); }

    int numChildren() const { return children_.size(); }

    Node *child(int i) {
      if (i < 0 || i >= numChildren())
        return nullptr;

      return children_[i];
    }

    void addInd(const QModelIndex &ind) { inds_.push_back(ind); }

    const QModelIndex &ind() const {
      static QModelIndex noInd;

      if (inds_.empty())
        return noInd;

      return inds_[0];
    }

    Node *findChild(const QString &str, bool create=false) const {
      for (const auto &child : children_)
        if (child->str() == str)
          return child;

      if (create)
        return new Node(const_cast<Node *>(this), str);

      return nullptr;
    }

    bool hasChild(Node *child) const {
      for (const auto &child1 : children_)
        if (child1 == child)
          return true;

      return false;
    }

    void addChild(Node *node) {
      node->parent_ = this;

      children_.push_back(node);
    }

    void resetChildren() { children_.clear(); }

    //---

    void reset() {
      propagateValues_.clear();
    }

    bool hasPropagateValue(int c) const {
      return propagateValues_.find(c) != propagateValues_.end();
    }

    void setPropagateValue(int c, const QVariant &var) {
      propagateValues_[c] = var;
    }

    QVariant getPropagateValue(int c) const {
      auto p = propagateValues_.find(c);
      if (p == propagateValues_.end()) return QVariant();

      return (*p).second;
    }

   private:
    using Inds     = std::vector<QModelIndex>;
    using Children = std::vector<Node *>;
    using ColValue = std::map<int,QVariant>;

    Node*    parent_ { nullptr }; //!< parent node
    QString  str_;                //!< node string
    ColValue propagateValues_;    //!< column propagate values
    Inds     inds_;               //!< source model ind
    Children children_;           //!< child nodes
  };

 private slots:
  void foldSlot();

 private:
  void doResetModel();

  void fold();

  // clear model
  void clear();

  void createConnections();

  // fold root node
  void foldNode();

  // connect/disconnect model slots
  void connectSlots(bool b);

  Node *findInd(Node *node, const QModelIndex &ind) const;

  void resetNode(Node *node);

  QVariant calcPropagateValue(Node *node, int c) const;

 private:
  CQHierSepData  data_;                                    //!< control data
  PropagateValue propagateValue_ { PropagateValue::NONE }; //!< propagate value
  CQBaseModel*   baseModel_      { nullptr };              //!< base model
  int            numColumns_     { 0 };                    //!< source model column count
  Node*          root_           { nullptr };              //!< root node
  bool           folded_         { false };                //!< is folded
};

#endif
