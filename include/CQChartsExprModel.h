#ifndef CQChartsExprModel_H
#define CQChartsExprModel_H

#include <CQBaseModel.h>
#include <CQBucketer.h>
#include <QAbstractProxyModel>
#include <boost/optional.hpp>
#include <set>
#include <vector>

class CQChartsModelFilter;
class CQChartsExprModelFn;
class CQChartsModelData;
class CQCharts;

#ifdef CQCharts_USE_TCL
class CQChartsExprTcl;
class CQTcl;
#endif

//---

class CQChartsExprModel : public QAbstractProxyModel {
  Q_OBJECT

 public:
  enum class Function {
    EVAL,
    ADD,
    DELETE,
    ASSIGN
  };

  using Values = std::vector<QVariant>;
  using Rows   = std::vector<int>;

 public:
  CQChartsExprModel(CQCharts *charts, CQChartsModelFilter *filter, QAbstractItemModel *model);

  virtual ~CQChartsExprModel();

  //---

  CQChartsModelFilter *filter() const { return filter_; }

  QAbstractItemModel *model() const { return model_; }

  //---

  int debug() const { return debug_; }
  void setDebug(int i) { debug_ = i; }

  //---

  void addFunction(const QString &name);

  //---

  bool decodeExpressionFn(const QString &exprStr, Function &function,
                          int &column, QString &expr) const;

  bool addExtraColumn(const QString &expr, int &column);
  bool addExtraColumn(const QString &header, const QString &expr, int &column);

  bool removeExtraColumn(int column);

  bool assignExtraColumn(int column, const QString &expr);
  bool assignExtraColumn(const QString &header, int column, const QString &expr);

  void calcColumn(int column, const QString &expr, Values &values) const;

  bool queryColumn(int column, const QString &expr, Rows &rows) const;

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

  QModelIndex mapToSource(const QModelIndex &index) const override;

  QModelIndex mapFromSource(const QModelIndex &index) const override;

  //---

  int currentRow() const { return currentRow_; }
  int currentCol() const { return currentCol_; }

  bool checkColumn(int col) const;
  bool checkIndex(int row, int col) const;

  virtual QVariant processCmd(const QString &name, const Values &values);

  void setVar(const QString &name, int row);

 private:
  using OptInt     = boost::optional<int>;
  using OptReal    = boost::optional<double>;
  using VariantMap = std::map<int,QVariant>;
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

  struct ColumnData {
    CQBucketer bucketer;   // bucketer
    OptInt     imin, imax; // integer range
    OptReal    rmin, rmax; // real range
  };

  using TclCmds = std::vector<CQChartsExprModelFn *>;

  friend class CQChartsExprModelFn;

 private:
  void addBuiltinFunctions();

#ifdef CQCharts_USE_TCL
  CQTcl *qtcl() const;
#endif

  //---

  int numExtraColumns() const { return extraColumns_.size(); }

  const ExtraColumn &extraColumn(int i) const { return extraColumns_[i]; }

  ExtraColumn &extraColumn(int i) { return extraColumns_[i]; }

  void calcColumn(int column, int ecolumn);

  QVariant getExtraColumnValue(int row, int column, int ecolumn) const;

  //---

  void initCalc();

  //---

  bool decodeExpression(const QString &exprStr, QString &header, QString &expr) const;

  bool evaluateExpression(const QString &expr, QVariant &var) const;

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
  QVariant scaleCmd    (const Values &values) const;
  QVariant randCmd     (const Values &values) const;
  QVariant rnormCmd    (const Values &values) const;
//QVariant keyCmd      (const Values &values) const;
//QVariant concatCmd   (const Values &values) const;
  QVariant colorCmd    (const Values &values) const;

  QVariant remapCmd    (const Values &values);
  QVariant timevalCmd  (const Values &values);

  QString replaceExprColumns(const QString &expr, int row, int column) const;

  bool setTclResult(const QVariant &rc);
  bool getTclResult(QVariant &rc) const;

  bool getColumnRange(const QModelIndex &ind, double &rmin, double &rmax);

  QVariant getCmdData(int row, int col) const;
  bool     setCmdData(int row, int col, const QVariant &var);

 protected:
  CQChartsModelData *getModelData() const;

 protected:
  using ExtraColumns = std::vector<ExtraColumn>;
  using ColumnDatas  = std::map<int,ColumnData>;
  using ColumnNames  = std::map<int,QString>;
  using NameColumns  = std::map<QString,int>;

  CQCharts*            charts_     { nullptr }; // charts
  CQChartsModelFilter* filter_     { nullptr }; // parent filter model
  QAbstractItemModel*  model_      { nullptr }; // child data model
#ifdef CQCharts_USE_TCL
  CQChartsExprTcl*     qtcl_       { nullptr }; // tcl expression
#endif
  TclCmds              tclCmds_;                // tcl commands
  bool                 editable_   { true };    // is editable
  bool                 debug_      { false };   // is debug
  ExtraColumns         extraColumns_;           // extra columns
  mutable int          nr_         { 0 };       // cached number of rows
  mutable int          nc_         { 0 };       // cached number of columns
  mutable int          currentRow_ { 0 };       // cached current row
  mutable int          currentCol_ { 0 };       // cached current column
  mutable ColumnDatas  columnDatas_;            // cached column datas
  ColumnNames          columnNames_;            // cached column names
  NameColumns          nameColumns_;            // cached named columns
};

#endif
