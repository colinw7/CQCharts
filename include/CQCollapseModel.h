#ifndef CQCollapseModel_H
#define CQCollapseModel_H

#include <CQBaseModelTypes.h>
#include <QSortFilterProxyModel>
#include <vector>
#include <set>

class CQCollapseModel : public QAbstractProxyModel {
  Q_OBJECT

 public:
  enum class CollapseOp {
    UNIQUE,
    SUM,
    MEAN
  };

 public:
  CQCollapseModel(QAbstractItemModel *model);

 ~CQCollapseModel();

  //---

  // get/set source model
  QAbstractItemModel *sourceModel() const;
  void setSourceModel(QAbstractItemModel *sourceModel) override;

  //---

  void setColumnType(int column, CQBaseModelType type);

  void setColumnCollapseOp(int column, CollapseOp op);

  //---

  // # Abstract Model APIS

  // get column count
  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  // get child row count of index
  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  // get child of parent at row/column
  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  // get parent of child
  QModelIndex parent(const QModelIndex &child) const override;

  // does parent have children
  bool hasChildren(const QModelIndex &parent=QModelIndex()) const override;

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
  struct ColumnConfig;
  class  VariantData;

  ColumnConfig &getColumnConfig(int c);

  VariantData &getVariantData(int r, int c, int role) const;

  void collapseRowColumn(int row, int column, int role, VariantData &variantData) const;

 private:
  class VariantData {
   public:
    VariantData() { }

    void setParent(const QVariant &var) { parent_ = var; set_ = true; }

    void addChild(const QVariant &var) { children_.push_back(var); set_ = true; }

    bool isSet() const { return set_; }

    QVariant displayValue(const ColumnConfig &config) const;

    void resetDisplayValue() { displayValue_ = QVariant(); }

   private:
    QVariant     parent_;
    QVariantList children_;
    QVariant     displayValue_;
    bool         set_ { false };
  };

  using RoleDataMap = std::map<int,VariantData>;

  struct ColumnData {
    RoleDataMap roleDataMap;
  };

  using ColumnDataMap = std::map<int,ColumnData>;
  using RowDataMap    = std::map<int,ColumnDataMap>;

  struct ColumnConfig {
    CQBaseModelType type       { CQBaseModelType::NONE };
    CollapseOp      collapseOp { CollapseOp::UNIQUE };
  };

  using ColumnConfigMap = std::map<int,ColumnConfig>;

  RowDataMap      rowDataMap_;
  ColumnConfigMap columnConfigMap_;
};

#endif
