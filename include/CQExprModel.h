#ifndef CQExprModel_H
#define CQExprModel_H

#include <CQBaseModel.h>
#include <QAbstractProxyModel>
#include <boost/optional.hpp>
#include <set>

class CQExprModelExpr;
class CQExprModelFn;

class CQExprModel : public QAbstractProxyModel {
  Q_OBJECT

 public:
  enum class Function {
    EVAL,
    ADD,
    DELETE,
    ASSIGN
  };

 public:
  CQExprModel(QAbstractItemModel *model);

 ~CQExprModel();

  //---

  QAbstractItemModel *model() const { return model_; }

  int debug() const { return debug_; }
  void setDebug(int i) { debug_ = i; }

  //---

  void addFunction(const QString &name, CQExprModelFn *fn);

  //---

  bool decodeExpressionFn(const QString &exprStr, Function &function,
                          int &column, QString &expr) const;

  bool addExtraColumn(const QString &expr);
  bool addExtraColumn(const QString &header, const QString &expr);

  bool removeExtraColumn(int column);

  bool assignExtraColumn(int column, const QString &expr);
  bool assignExtraColumn(const QString &header, int column, const QString &expr);

  bool processExpr(const QString &expr);

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
  typedef boost::optional<int>    OptInt;
  typedef boost::optional<double> OptReal;
  typedef std::map<QString,int>   StringMap;
  typedef std::map<int,QVariant>  VariantMap;
  typedef std::vector<QVariant>   Values;

  struct ExtraColumn {
    QString           expr;                                   // expression
    QString           header;                                 // header
    CQBaseModel::Type type       { CQBaseModel::Type::NONE }; // value type
    QString           typeValues;                             // type extra values
    VariantMap        variantMap;                             // calculated values
    Values            values;                                 // assign values
    Function          function   { Function::EVAL };          // current eval function
    bool              evaluating { false };                   // is evaluating column

    ExtraColumn(const QString &expr, const QString &header="") :
     expr(expr), header(header) {
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

  void calcColumn(int column, int ecolumn);

  QVariant getExtraColumnValue(int row, int column, int ecolumn) const;

  bool decodeExpression(const QString &exprStr, QString &header, QString &expr) const;

 private:
  QAbstractItemModel* model_      { nullptr };
  CQExprModelExpr*    expr_       { nullptr };
  bool                debug_      { false };
  ExtraColumns        extraColumns_;
  mutable int         currentRow_ { 0 };
  mutable int         currentCol_ { 0 };
  mutable ColumnDatas columnDatas_;
};

#endif
