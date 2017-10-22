#ifndef CQExprModel_H
#define CQExprModel_H

#include <CQBaseModel.h>
#include <QAbstractProxyModel>
#include <boost/optional.hpp>

class CExpr;

class CQExprModel : public QAbstractProxyModel {
  Q_OBJECT

 public:
  CQExprModel(QAbstractItemModel *model);

 ~CQExprModel();

  //---

  void addExtraColumn(const QString &expr);

  bool removeExtraColumn(int column);

  bool assignExtraColumn(int column, const QString &expr);

  void processExpr(const QString &expr);

  int columnStringBucket(int column, const QString &value) const;

  bool columnRange(int column, double &minVal, double &maxVal) const;
  bool columnRange(int column, int &minVal, int &maxVal) const;

  //---

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex index(int row, int column, const QModelIndex &parent) const override;

  QModelIndex parent(const QModelIndex &child) const override;

  bool hasChildren(const QModelIndex &parent) const override;

  QVariant data(const QModelIndex &index, int role) const override;

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role) override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  QModelIndex mapToSource(const QModelIndex &index) const;

  QModelIndex mapFromSource(const QModelIndex &index) const;

  //---

  int currentRow() const { return currentRow_; }
  int currentCol() const { return currentCol_; }

 private:
  QVariant getExtraColumnValue(int row, int column) const;

 private:
  typedef boost::optional<int>    OptInt;
  typedef boost::optional<double> OptReal;
  typedef std::map<QString,int>   StringMap;
  typedef std::map<int,QVariant>  VariantMap;
  typedef std::vector<QVariant>   Values;

  struct ExtraColumn {
    QString           expr;                             // expression
    QString           header;                           // header
    CQBaseModel::Type type { CQBaseModel::Type::NONE }; // type
    QString           typeValues;                       // type extra values
    VariantMap        variantMap;                       // calculated values
    Values            values;

    ExtraColumn(const QString &expr) :
     expr(expr) {
    }
  };

  typedef std::vector<ExtraColumn> ExtraColumns;

  struct ColumnData {
    StringMap stringMap;  // uniquified strings and ids
    OptInt    imin, imax; // integer range
    OptReal   rmin, rmax; // real range
  };

  typedef std::map<int,ColumnData> ColumnDatas;

 private:
  int numExtraColumns() const { return extraColumns_.size(); }

  const ExtraColumn &extraColumn(int i) const { return extraColumns_[i]; }

  ExtraColumn &extraColumn(int i) { return extraColumns_[i]; }

  QString replaceDollarColumns(const QString &expr) const;

 private:
  QAbstractItemModel* model_      { nullptr };
  CExpr*              expr_       { nullptr };
  ExtraColumns        extraColumns_;
  mutable int         currentRow_ { 0 };
  mutable int         currentCol_ { 0 };
  mutable ColumnDatas columnDatas_;
  mutable bool        evaluating_ { false };
};

#endif
