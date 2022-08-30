#include <CQHierSepModel.h>
#include <CQBaseModel.h>
#include <CQModelUtil.h>
#include <CQTclUtil.h>

#include <cassert>
#include <map>
#include <set>
#include <iostream>

//------

CQHierSepModel::
CQHierSepModel(QAbstractItemModel *model, const CQHierSepData &data)
{
  setObjectName("hierSepModel");

  data_ = data;

  CQHierSepModel::setSourceModel(model);
}

CQHierSepModel::
~CQHierSepModel()
{
  clear();
}

QAbstractItemModel *
CQHierSepModel::
sourceModel() const
{
  auto *sourceModel = QAbstractProxyModel::sourceModel();

  return sourceModel;
}

void
CQHierSepModel::
setSourceModel(QAbstractItemModel *sourceModel)
{
  connectSlots(false);

  QAbstractProxyModel::setSourceModel(sourceModel);

  baseModel_ = qobject_cast<CQBaseModel *>(CQModelUtil::getBaseModel(sourceModel));

  connectSlots(true);

  fold();
}

//---

void
CQHierSepModel::
setSeparator(const QChar &c)
{
  data_.separator = c;

  doResetModel();
}

void
CQHierSepModel::
setFoldColumn(int i)
{
  if (i != data_.column) {
    data_.column = i;

    doResetModel();
  }
}

void
CQHierSepModel::
setConnectionsColumn(int i)
{
  if (i != data_.connectionColumn) {
    data_.connectionColumn = i;

    doResetModel();
  }
}

void
CQHierSepModel::
setNodeColumn(int i)
{
  if (i != data_.nodeColumn) {
    data_.nodeColumn = i;

    doResetModel();
  }
}

void
CQHierSepModel::
setPropagateValue(const PropagateValue &v)
{
  if (v != propagateValue_) {
    propagateValue_ = v;

    resetNode(root_);

    beginResetModel();
    endResetModel();
  }
}

//---

void
CQHierSepModel::
resetNode(Node *node)
{
  node->reset();

  for (int i = 0; i < node->numChildren(); ++i)
    resetNode(node->child(i));
}

//---

void
CQHierSepModel::
doResetModel()
{
  auto *model = this->sourceModel();
  if (! model) return;

  beginResetModel();

  fold();

  endResetModel();
}

void
CQHierSepModel::
connectSlots(bool b)
{
  auto *model = this->sourceModel();
  if (! model) return;

  auto connectDisconnect = [&](bool b, const char *from, const char *to) {
    if (b)
      QObject::connect(model, from, this, to);
    else
      QObject::disconnect(model, from, this, to);
  };

  connectDisconnect(b, SIGNAL(columnsInserted(const QModelIndex &, int, int)), SLOT(foldSlot()));
  connectDisconnect(b, SIGNAL(columnsMoved(const QModelIndex &, int, int,
                              const QModelIndex &, int)), SLOT(foldSlot()));
  connectDisconnect(b, SIGNAL(columnsRemoved(const QModelIndex &, int, int)), SLOT(foldSlot()));

  connectDisconnect(b, SIGNAL(modelReset()), SLOT(foldSlot()));

  connectDisconnect(b, SIGNAL(rowsInserted(const QModelIndex &, int, int)), SLOT(foldSlot()));
  connectDisconnect(b, SIGNAL(rowsMoved(const QModelIndex &, int, int,
                              const QModelIndex &, int)), SLOT(foldSlot()));
  connectDisconnect(b, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), SLOT(foldSlot()));
}

void
CQHierSepModel::
foldSlot()
{
  fold();
}

void
CQHierSepModel::
fold()
{
  clear();

  //---

  // check column valid
  auto *model = this->sourceModel();
  if (! model) return;

  numColumns_ = model->columnCount();

  if (foldColumn() < 0 || foldColumn() >= numColumns_)
    return;

  //---

  root_ = new Node(nullptr, "root");

  if (data_.connectionType != CQHierConnectionType::NONE)
    createConnections();
  else
    foldNode();
}

void
CQHierSepModel::
createConnections()
{
  auto *model = this->sourceModel();
  assert(model);

  struct Connection {
    int    nodeId { -1 };
    double value  { 0.0 };

    Connection() { }

    Connection(int nodeId, double value) :
     nodeId(nodeId), value(value) {
    }
  };

  using NodeSet     = std::set<Node *>;
  using Connections = std::vector<Connection>;

  struct NodeData {
    Node*       node { nullptr };
    NodeSet     destNodes;
    Connections connections;

    NodeData() { }

    NodeData(Node *node) : node(node) { }
  };

  using NodeDataMap = std::map<Node *, NodeData>;
  using NodeIdData  = std::map<int, NodeData>;

  NodeDataMap nodeDataMap;
  NodeIdData  nodeIdData;

  Node *parentNode = new Node(nullptr);

  // create connections for rows at column
  int nr = model->rowCount();

  for (int r = 0; r < nr; ++r) {
    // get value for connection column
    auto ind = model->index(r, foldColumn());
    auto str = model->data(ind, Qt::DisplayRole).toString();

    // hier path (nodes separated by /)
    if      (data_.connectionType == CQHierConnectionType::HIER) {
      auto strs = str.split(separator(), Qt::KeepEmptyParts);
      if (strs.size() != 2) continue;

      Node *srcNode  = parentNode->findChild(strs[0], /*create*/true);
      Node *destNode = parentNode->findChild(strs[1], /*create*/true);

      auto p = nodeDataMap.find(srcNode);

      if (p == nodeDataMap.end())
        p = nodeDataMap.insert(p, NodeDataMap::value_type(srcNode, NodeData()));

      (*p).second.destNodes.insert(destNode);

      if (srcNode)
        srcNode->addInd(ind); // should only be one

      if (destNode)
        destNode->addInd(ind); // should only be one
    }
    else if (data_.connectionType == CQHierConnectionType::CONNECTIONS) {
      Node *node = parentNode->findChild(str, /*create*/true);

      NodeData nodeData(node);

      //---

      auto connectionsInd = model->index(r, connectionsColumn());
      auto connectionsStr = model->data(connectionsInd, Qt::DisplayRole).toString();

      QStringList connectionsStrs;

      if (! CQTclUtil::splitList(connectionsStr, connectionsStrs))
        continue;

      if (connectionsStrs.size() == 1) {
        QStringList connectionsStrs1;

        if (! CQTclUtil::splitList(connectionsStrs[0], connectionsStrs1))
          continue;

        connectionsStrs = connectionsStrs1;
      }

      for (const auto &s : connectionsStrs) {
        QStringList strs;

        if (! CQTclUtil::splitList(s, strs))
          continue;

        if (strs.length() != 2)
          continue;

        bool ok1, ok2;
        int    nodeId = strs[0].toInt(&ok1);
        double value  = strs[1].toDouble(&ok2);
        if (! ok1 || ! ok2) continue;

        nodeData.connections.push_back(Connection(nodeId, value));
      }

      //---

      int nodeId = r;

      if (nodeColumn() >= 0) {
        auto nodeInd = model->index(r, nodeColumn());
        auto nodeStr = model->data(nodeInd, Qt::DisplayRole).toString();

        bool ok1;
        nodeId = nodeStr.toInt(&ok1);
        if (! ok1) continue;
      }

      //---

      nodeIdData[nodeId] = nodeData;

      if (node)
        node->addInd(ind);
    }
  }

  if      (data_.connectionType == CQHierConnectionType::HIER) {
    for (const auto &pe : nodeDataMap) {
      auto *srcNode   = pe.first;
      auto &destNodes = pe.second.destNodes;

      for (auto *destNode : destNodes ) {
        if (destNode->parent() != parentNode)
          continue;

        if (srcNode != destNode)
          srcNode->addChild(destNode);
      }
    }

    bool added = false;

    for (const auto &pe : nodeDataMap) {
      auto *srcNode = pe.first;

      if (srcNode->parent() != parentNode)
        continue;

      root_->addChild(srcNode);

      added = true;
    }

    if (! added && ! nodeDataMap.empty())
      root_->addChild(nodeDataMap.begin()->first);
  }
  else if (data_.connectionType == CQHierConnectionType::CONNECTIONS) {
    int numNodes = int(nodeIdData.size());

    for (const auto &p : nodeIdData) {
      auto &nodeData = p.second;

      auto *srcNode = nodeData.node;

      for (const auto &connection : nodeData.connections) {
        if (connection.nodeId < 0 || connection.nodeId >= numNodes)
          continue;

        auto pn = nodeIdData.find(connection.nodeId);
        if (pn == nodeIdData.end()) continue;

        auto *destNode = (*pn).second.node;

        if (destNode->parent() != parentNode)
          continue;

        if (srcNode != destNode)
          srcNode->addChild(destNode);
      }
    }

    bool added = false;

    for (const auto &p : nodeIdData) {
      auto &nodeData = p.second;

      auto *srcNode = nodeData.node;

      if (srcNode->parent() != parentNode)
        continue;

      root_->addChild(srcNode);

      added = true;
    }

    if (! added && ! nodeIdData.empty())
      root_->addChild(nodeIdData.begin()->second.node);
  }

  parentNode->resetChildren();

  delete parentNode;

  folded_ = true;
}

void
CQHierSepModel::
foldNode()
{
  auto *model = this->sourceModel();
  assert(model);

  // fold rows at column
  int nr = model->rowCount();

  for (int r = 0; r < nr; ++r) {
    // get value for fold column
    auto ind = model->index(r, foldColumn());

    auto str = model->data(ind, Qt::DisplayRole).toString();

    auto strs = str.split(separator(), Qt::KeepEmptyParts);

    Node *node = nullptr;

    Node *parent = root_;

    for (auto &str : strs) {
      node = parent->findChild(str);

      if (! node)
        node = new Node(parent, str);

      parent = node;
    }

    if (node)
      node->addInd(ind); // should only be one
  }

  folded_ = true;
}

void
CQHierSepModel::
clear()
{
  delete root_;

  root_ = nullptr;

  folded_ = false;
}

// get number of columns
int
CQHierSepModel::
columnCount(const QModelIndex &parent) const
{
  if (! folded_) {
    auto *model = this->sourceModel();
    if (! model) return 0;

    return model->columnCount(parent);
  }

  return numColumns_;
}

// get number of child rows for parent
int
CQHierSepModel::
rowCount(const QModelIndex &parent) const
{
  if (! folded_) {
    auto *model = this->sourceModel();
    if (! model) return 0;

    return model->rowCount(parent);
  }

  //---

  // children only at column 0
  if (parent.column() > 0)
    return 0;

  // get node data
  auto *pnode = static_cast<Node *>(parent.internalPointer());
  if (! pnode) pnode = root_;

  return pnode->numChildren();
}

// get child node for row/column of parent
QModelIndex
CQHierSepModel::
index(int row, int column, const QModelIndex &parent) const
{
  if (! folded_) {
    auto *model = this->sourceModel();
    if (! model) return QModelIndex();

    return model->index(row, column, parent);
  }

  //---

  // get node data
  auto *pnode = static_cast<Node *>(parent.internalPointer());
  if (! pnode) pnode = root_;

  if (row < 0 || row >= pnode->numChildren())
    return QModelIndex();

  Node *node = pnode->child(row);
  if (! node) return QModelIndex();

  // model index is row, column and node
  return createIndex(row, column, node);
}

// get parent for child
QModelIndex
CQHierSepModel::
parent(const QModelIndex &child) const
{
  if (! child.isValid())
    return QModelIndex();

  //---

  if (! folded_) {
    auto *model = this->sourceModel();
    if (! model) return QModelIndex();

    return model->parent(child);
  }

  //---

  // get node data
  auto *cnode = static_cast<Node *>(child.internalPointer());
  if (! cnode) cnode = root_;

  Node *node = cnode->parent();
  if (! node) return QModelIndex();

  Node *pnode = node->parent();
  if (! pnode) return QModelIndex();

  // get row number for parent's child
  int row = -1;

  for (int i = 0; i < pnode->numChildren(); ++i) {
    if (pnode->child(i) == node) {
      row = i;
      break;
    }
  }

  if (row < 0)
    return QModelIndex();

  // model index is row, column and node
  return createIndex(row, 0, node);
}

bool
CQHierSepModel::
hasChildren(const QModelIndex &parent) const
{
  if (! parent.isValid())
    return true;

  //---

  if (! folded_) {
    auto *model = this->sourceModel();
    if (! model) return false;

    return model->hasChildren(parent);
  }

  //---

  // children only at column 0
  if (parent.column() > 0)
    return false;

  // get node data
  auto *pnode = static_cast<Node *>(parent.internalPointer());
  if (! pnode) pnode = root_;

  return pnode->hasChildren();
}

QVariant
CQHierSepModel::
data(const QModelIndex &index, int role) const
{
  auto *model = this->sourceModel();
  if (! model) return QVariant();

  //---

  if (! folded_)
    return model->data(index, role);

  //---

  // get node data
  auto *node = static_cast<Node *>(index.internalPointer());
  if (! node) node = root_;

  int c = index.column();

  // fold column or propagate value
  if (c == 0 || node->hasChildren()) {
    // fold column
    if (c == 0) {
      // TODO: other roles
      if      (role == Qt::DisplayRole) {
        return node->str();
      }
      else if (role == Qt::EditRole) {
        return node->str();
      }
      else if (role == Qt::ToolTipRole) {
        return node->str();
      }
      else {
        return QVariant();
      }
    }
    // propagate value
    else {
      if      (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
        if (baseModel_ && propagateValue_ != PropagateValue::NONE) {
          if (! node->hasPropagateValue(c)) {
            auto value = calcPropagateValue(node, c);

            node->setPropagateValue(c, value);
          }

          return node->getPropagateValue(c);
        }
        else {
          return QVariant();
        }
      }
      else if (role == Qt::TextAlignmentRole) {
        if (baseModel_ && propagateValue_ != PropagateValue::NONE)
          return QVariant(Qt::AlignRight | Qt::AlignVCenter);
        else
          return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else {
        return QVariant();
      }
    }
  }
  else {
    const auto &ind = node->ind();

    auto ind1 = model->index(ind.row(), index.column());

    return model->data(ind1, role);
  }
}

QVariant
CQHierSepModel::
calcPropagateValue(Node *node, int c) const
{
  auto *model = this->sourceModel();
  if (! model) return QVariant();

  auto type = baseModel_->columnType(c);

  double sum = 0.0;

  for (int i = 0; i < node->numChildren(); ++i) {
    Node *cnode = node->child(i);

    const auto &cind = cnode->ind();

    QVariant cvalue;

    if (cind.isValid()) {
      auto cind1 = model->index(cind.row(), c);

      cvalue = model->data(cind1, Qt::DisplayRole);
    }
    else
      cvalue = calcPropagateValue(cnode, c);

    if      (type == CQBaseModelType::INTEGER) {
      bool ok;

      int ivalue = cvalue.toInt(&ok);
      if (! ok) continue;

      if (propagateValue_ == PropagateValue::SUM)
        sum += ivalue;
    }
    else if (type == CQBaseModelType::REAL) {
      bool ok;

      double rvalue = cvalue.toReal(&ok);
      if (! ok) continue;

      if (propagateValue_ == PropagateValue::SUM)
        sum += rvalue;
    }
  }

  //---

  QVariant value;

  if (propagateValue_ == PropagateValue::SUM) {
    if      (type == CQBaseModelType::INTEGER)
      value = QVariant(int(sum));
    else
      value = QVariant(sum);
  }

  return value;
}

bool
CQHierSepModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  auto *model = this->sourceModel();
  if (! model) return false;

  //---

  if (! folded_) {
    return model->setData(index, value, role);
  }

  //---

  // get node data
  auto *node = static_cast<Node *>(index.internalPointer());
  if (! node) node = root_;

  int c = index.column();

  // fold column or propagate value
  if (c == 0 || node->hasChildren()) {
    // fold column
    if (c == 0) {
      if     (role == Qt::DisplayRole) {
        return false;
      }
      else if (role == Qt::EditRole) {
        return false;
      }
      else if (role == Qt::ToolTipRole) {
        return false;
      }
      else {
        return false;
      }
    }
    // propagate value
    else {
      return false;
    }
  }
  else {
    const auto &ind = node->ind();

    return model->setData(ind, value, role);
  }
}

QVariant
CQHierSepModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  auto *model = this->sourceModel();
  if (! model) return QVariant();

  //---

  if (! folded_) {
    return model->headerData(section, orientation, role);
  }

  //---

  if (section < 0 || section >= columnCount())
    return QVariant();

  //---

  if (orientation != Qt::Horizontal)
    return model->headerData(section, orientation, role);

  //---

  return model->headerData(section, orientation, role);
}

bool
CQHierSepModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  auto *model = this->sourceModel();
  if (! model) return false;

  //---

  if (! folded_) {
    return model->setHeaderData(section, orientation, value, role);
  }

  //---

  if (section < 0 || section >= columnCount())
    return false;

  //---

  if (orientation != Qt::Horizontal)
    return model->setHeaderData(section, orientation, role);

  //---

  return model->setHeaderData(section, orientation, value, role);
}

Qt::ItemFlags
CQHierSepModel::
flags(const QModelIndex &index) const
{
  auto *model = this->sourceModel();
  if (! model) return Qt::ItemFlags();

  //---

  if (! folded_)
    return model->flags(index);

  //---

  // get node data
  auto *node = static_cast<Node *>(index.internalPointer());
  if (! node) node = root_;

  int c = index.column();

  // fold column or propagate value
  if (c == 0 || node->hasChildren()) {
    // fold column
    if (c == 0) {
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    // propagate value
    else {
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
  }
  else {
    const auto &ind = node->ind();

    return model->flags(ind);
  }
}

// map index in source model to folded model
QModelIndex
CQHierSepModel::
mapFromSource(const QModelIndex &sourceIndex) const
{
  if (! folded_)
    return sourceIndex;

  //---

  if (! sourceIndex.isValid())
    return QModelIndex();

  //---

  Node *node = findInd(root_, sourceIndex);
  if (! node) return QModelIndex();

  Node *pnode = node->parent();

  // no parent so return root index
  if (! pnode)
    return createIndex(0, 0, node);

  // get row number for parent's child
  int row = -1;

  for (int i = 0; i < pnode->numChildren(); ++i) {
    if (pnode->child(i) == node) {
      row = i;
      break;
    }
  }

  if (row < 0)
    return QModelIndex();

  // model index is row, column and node
  return createIndex(row, 0, node);
}

CQHierSepModel::Node *
CQHierSepModel::
findInd(Node *node, const QModelIndex &ind) const
{
  if (node->ind() == node->ind())
    return node;

  for (int i = 0; i < node->numChildren(); ++i) {
    Node *cnode = findInd(node->child(i), ind);

    if (cnode)
      return cnode;
  }

  return nullptr;
}

// map index in hier sep model to source model
QModelIndex
CQHierSepModel::
mapToSource(const QModelIndex &proxyIndex) const
{
  if (! folded_)
    return proxyIndex;

  //---

  if (! proxyIndex.isValid())
    return QModelIndex();

  //---

  // get node data
  auto *node = static_cast<Node *>(proxyIndex.internalPointer());
  if (! node) node = root_;

  return node->ind();
}
