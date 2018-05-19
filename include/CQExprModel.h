#ifndef CQExprModel_H
#define CQExprModel_H

#include <CQBaseModel.h>
#include <QAbstractProxyModel>
#include <boost/optional.hpp>
#include <set>

class CQExprModelFn;

class CExpr;
#ifdef CQExprModel_USE_CEXPR
#include <CExpr.h>
#else
#include <CRefPtr.h>
class CExprValue;
typedef CRefPtr<CExprValue> CExprValuePtr;
#endif

class Tcl_Interp;

//---

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

  bool decodeExpressionFn(const QString &exprStr, Function &function,
                          int &column, QString &expr) const;

  bool addExtraColumn(const QString &expr, int &column);
  bool addExtraColumn(const QString &header, const QString &expr, int &column);

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

  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &child) const override;

  bool hasChildren(const QModelIndex &parent=QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::DisplayRole) override;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role=Qt::DisplayRole) const override;

  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role=Qt::DisplayRole) override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  QModelIndex mapToSource(const QModelIndex &index) const;

  QModelIndex mapFromSource(const QModelIndex &index) const;

  //---

  int currentRow() const { return currentRow_; }
  int currentCol() const { return currentCol_; }

 private:
  void addBuiltinFunctions();

  Tcl_Interp* tclInterp() const { return interp_; }

 private:
  using OptInt     = boost::optional<int>;
  using OptReal    = boost::optional<double>;
  using StringMap  = std::map<QString,int>;
  using VariantMap = std::map<int,QVariant>;
  using Values     = std::vector<QVariant>;
  using Args       = std::vector<QString>;

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

  using TclCmds = std::vector<CQExprModelFn *>;

  friend class CQExprModelFn;
  friend class CQExprModelNameFn;

 private:
  int numExtraColumns() const { return extraColumns_.size(); }

  const ExtraColumn &extraColumn(int i) const { return extraColumns_[i]; }

  ExtraColumn &extraColumn(int i) { return extraColumns_[i]; }

  void calcColumn(int column, int ecolumn);

  QVariant getExtraColumnValue(int row, int column, int ecolumn) const;

  bool decodeExpression(const QString &exprStr, QString &header, QString &expr) const;

  bool evaluateExpression(const QString &expr, QVariant &var) const;

  QVariant processCmd(const QString &name, const Values &values);

  QVariant columnCmd   (const Values &values) const;
  QVariant rowCmd      (const Values &values) const;
  QVariant cellCmd     (const Values &values) const;
  QVariant setColumnCmd(const Values &values);
  QVariant setRowCmd   (const Values &values);
  QVariant setCellCmd  (const Values &values);
  QVariant headerCmd   (const Values &values) const;
  QVariant setHeaderCmd(const Values &values);
  QVariant typeCmd     (const Values &values) const;
  QVariant setTypeCmd  (const Values &values);
  QVariant mapCmd      (const Values &values) const;
  QVariant bucketCmd   (const Values &values) const;
  QVariant normCmd     (const Values &values) const;
  QVariant keyCmd      (const Values &values) const;
  QVariant randCmd     (const Values &values) const;

  QString replaceNumericColumns(const QString &expr, int row, int column) const;

  bool checkColumn(int col) const;
  bool checkIndex(int row, int col) const;

  QVariant valueToVariant(CExpr *expr, const CExprValuePtr &value) const;

  bool variantToValue(CExpr *expr, const QVariant &var, CExprValuePtr &value) const;

  bool setTclResult(const QVariant &rc);
  bool getTclResult(QVariant &rc) const;

 private:
  QAbstractItemModel* model_      { nullptr };
  CExpr*              expr_       { nullptr };
  Tcl_Interp*         interp_     { nullptr };
  TclCmds             tclCmds_;
  bool                debug_      { false };
  ExtraColumns        extraColumns_;
  mutable int         nr_         { 0 };
  mutable int         nc_         { 0 };
  mutable int         currentRow_ { 0 };
  mutable int         currentCol_ { 0 };
  mutable ColumnDatas columnDatas_;
};

#endif
