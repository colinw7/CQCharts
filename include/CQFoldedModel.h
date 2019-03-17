#ifndef CQFoldedModel_H
#define CQFoldedModel_H

#include <CQBucketer.h>
#include <QAbstractProxyModel>
#include <set>
#include <cassert>

class CQFoldedModelFolded;

class CQFoldData {
 public:
  using Type = CQBucketer::Type;

 public:
  CQFoldData(int column=-1) :
   column_(column) {
  }

  // get/set type
  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; }

  // get/set column to fold
  int column() const { return column_; }
  void setColumn(int i) { column_ = i; }

  // get/set show column data in child column
  bool showColumnData() const { return showColumnData_; }
  void setShowColumnData(bool b) { showColumnData_ = b; }

  // get/set keep fold column
  bool isKeepColumn() const { return keepColumn_; }
  void setKeepColumn(bool b) { keepColumn_ = b; }

  // get/set delta for real number range
  double delta() const { return delta_; }
  void setDelta(double r) { delta_ = r; }

  // get/set num auto
  int numAuto() const { return numAuto_; }
  void setNumAuto(int i) { numAuto_ = i; }

  // get/set integral
  bool isIntegral() const { return integral_; }
  void setIntegral(bool b) { integral_ = b; }

 private:
  Type   type_           { Type::STRING }; // data type
  int    column_         { -1 };           // column number
  bool   showColumnData_ { false };        // show data in folded column
  bool   keepColumn_     { false };        // keep folded column
  double delta_          { 1.0 };          // bucket delta
  int    numAuto_        { 10 };           // bucket auto count
  bool   integral_       { false };        // is integral
};

//---

/*!
 * fold model into hierarchical model using fold (bucketed) column
 */
class CQFoldedModel : public QAbstractProxyModel {
  Q_OBJECT

  Q_PROPERTY(int  foldColumn         READ foldColumn         WRITE setFoldColumn)
  Q_PROPERTY(bool showFoldColumnData READ showFoldColumnData WRITE setShowFoldColumnData)
  Q_PROPERTY(bool keepFoldColumn     READ isKeepFoldColumn   WRITE setKeepFoldColumn)

 public:
  CQFoldedModel(QAbstractItemModel *model, const CQFoldData &foldData=CQFoldData());

 ~CQFoldedModel();

  //---

  // get/set source model
  QAbstractItemModel *sourceModel() const;
  void setSourceModel(QAbstractItemModel *sourceModel) override;

  //---

  // get/set fold data
  const CQFoldData &foldData() const { return foldData_; }
  void setFoldData(const CQFoldData &data);

  // get/set fold column
  int foldColumn() const { return foldData_.column(); }
  void setFoldColumn(int i);

  // get/set show column data
  bool showFoldColumnData() const { return foldData_.showColumnData(); }
  void setShowFoldColumnData(bool b);

  // get/set keep fold column
  bool isKeepFoldColumn() const { return foldData_.isKeepColumn(); }
  void setKeepFoldColumn(bool b);

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
  struct Node;

  using RowArray      = std::vector<int>;
  using RowSet        = std::set<int>;
  using ColumnArray   = std::vector<int>;
  using BucketNodeMap = std::map<int,Node *>;
  using Children      = std::vector<Node *>;

  // Node structure for tree construct
  struct Node {
    Node*          parent        { nullptr }; // parent node
    int            bucket        { -1 };      // bucket number
    QString        str;                       // node string
    int            depth         { 0 };       // node depth
    int            foldRow       { -1 };      // node fold row
    QModelIndex    sourceInd;                 // source model index of node
    RowArray       sourceRows;                // source rows of folded node
    RowSet         sourceRowSet;              // source row set of folded node
    BucketNodeMap  bucketNodeMap;             // string to node for folded nodes
    Children       children;                  // child nodes

    Node(const QModelIndex &sourceInd) :
     sourceInd(sourceInd) {
    }

    Node(Node *parent, const QModelIndex &sourceInd) :
     parent(parent), sourceInd(sourceInd) {
      assert(parent && parent != this);

      parent->children.push_back(this);
    }

   ~Node() {
      for (auto &child : children)
        delete child;
    }

    Node *getBucketNode(int bucket, int depth, bool &isNew) {
      auto p = bucketNodeMap.find(bucket);

      if (p == bucketNodeMap.end()) {
        Node *node = new Node(this, sourceInd);

        node->depth   = depth;
        node->foldRow = bucketNodeMap.size(); // folded row index

        p = bucketNodeMap.insert(p, BucketNodeMap::value_type(bucket, node));

        isNew = true;
      }
      else {
        isNew = false;
      }

      return p->second;
    }

    bool isFolded() const { return foldRow >= 0; }

    int numRows() const {
      // if folded then return num folded rows
      if (isFolded())
        return sourceRows.size();

      // if not folded, return child count
      return children.size();
    }

    bool hasSourceRow(int r) const {
      return (sourceRowSet.find(r) != sourceRowSet.end());
    }

    void addSourceRow(int r) {
      sourceRows  .push_back(r);
      sourceRowSet.insert   (r);
    }
  };

  // parent and child data for node data of proxy index
  //  parent: null    , child: <root>  ; root node       (row is zero)
  //  parent: <parent>, child: <child> ; non-folded node (child is parent->children[row])
  //  parent: <parent>, child: null    ; folded node, row is parent->sourceRows[row]
  struct NodeData {
    Node *parent { nullptr };
    Node *child  { nullptr };

    NodeData(Node *parent, Node *child) :
     parent(parent), child(child) {
    }
  };

 private slots:
  void foldSlot();

 private:
  void doResetModel();

  void fold();

  void calcRMinMax(QAbstractItemModel *model, const QModelIndex &parent,
                   bool &rset, double &rmin, double &rmax) const;

  // clear model
  void clear();

  // fold parent node (hierarchical source model)
  void foldNode(Node *node, int depth);

  // connect/disconnect model slots
  void connectSlots();
  void disconnectSlots();

  // map proxy column to source column
  int mapColumnToSource  (int column) const;
  // map source column to proxy column
  int mapColumnFromSource(int column) const;

  // init column number map
  void initSourceColumns();

  // get source index from folded parent node, row and column
  QModelIndex foldedChildIndex(Node *pnode, int row, int column) const;

  // get folded index for source index given folded parent node
  QModelIndex findSourceIndex(Node *pnode, const QModelIndex &sourceIndex) const;

  // get node data for proxy model index from internal data
  NodeData indexNode(const QModelIndex &ind) const;

  //---

 private:
  CQFoldData     foldData_;                      // fold data
  CQFoldedModel* sourceFoldedModel_ { nullptr }; // source folded model
  int            foldPos_           { 0 };       // fold column position
  int            numColumns_        { 0 };       // source model column count
  ColumnArray    sourceColumns_;
  Node*          root_              { nullptr }; // root node
  bool           folded_            { false };   // is folded
  CQBucketer     bucketer_;
};

#endif
