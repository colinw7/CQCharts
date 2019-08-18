#ifndef CQChartsExprModel_H
#define CQChartsExprModel_H

#include <CQBaseModelTypes.h>
#include <CQBucketer.h>
#include <QAbstractProxyModel>
#include <boost/optional.hpp>
#include <set>
#include <vector>
#include <future>

class CQChartsModelFilter;
class CQChartsExprModelFn;
class CQChartsModelData;
class CQCharts;

class CQChartsExprTcl;
class CQTcl;

//---

/*!
 * \brief Expression model
 * \ingroup Charts
 */
class CQChartsExprModel : public QAbstractProxyModel {
  Q_OBJECT

  Q_PROPERTY(bool debug READ debug WRITE setDebug)

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

  bool isOrigColumn (int column) const;
  bool isExtraColumn(int column) const;

  bool isReadOnly() const;
  void setReadOnly(bool b);

  //---

  bool decodeExpressionFn(const QString &exprStr, Function &function,
                          int &column, QString &expr) const;

  bool addExtraColumn(const QString &expr, int &column);
  bool addExtraColumn(const QString &header, const QString &expr, int &column);

  bool removeExtraColumn(int column);

  bool assignColumn(int column, const QString &expr);
  bool assignColumn(const QString &header, int column, const QString &expr);

  bool assignExtraColumn(int column, const QString &expr);
  bool assignExtraColumn(const QString &header, int column, const QString &expr);

  void calcColumn(int column, const QString &expr, Values &values) const;

  bool queryColumn(int column, const QString &expr, Rows &rows) const;

  bool processExpr(const QString &expr);

  int columnStringBucket(int column, const QString &value) const;

  bool columnRange(int column, double &minVal, double &maxVal) const;
  bool columnRange(int column, int &minVal, int &maxVal) const;

  int numExtraColumns() const { return extraColumns_.size(); }

  bool getExtraColumnDetails(int column, QString &header, QString &expr) const;

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

  void setVar(const QString &name, int row, int column);

 private:
  bool calcColumnRange(int column, double &minVal, double &maxVal);
  bool calcColumnRange(int column, int &minVal, int &maxVal);

 private:
  using OptInt     = boost::optional<int>;
  using OptReal    = boost::optional<double>;
  using VariantMap = std::map<int,QVariant>;
  using NameValues = std::map<QString,QVariant>;
  using Args       = std::vector<QString>;

  struct ExtraColumn {
    QString           expr;                                 //!< expression
    QString           header;                               //!< header
    CQBaseModelType   type       { CQBaseModelType::NONE }; //!< value type
    CQBaseModelType   baseType   { CQBaseModelType::NONE }; //!< value base type
    QString           typeValues;                           //!< type extra values
    NameValues        nameValues;                           //!< type named values
    VariantMap        variantMap;                           //!< calculated values
    Values            values;                               //!< assign values
    Function          function   { Function::EVAL };        //!< current eval function
    std::atomic<bool> evaluating { false };                 //!< is evaluating column

    ExtraColumn(const QString &expr, const QString &header="") :
     expr(expr), header(header) {
    }
  };

  struct ColumnData {
    CQBucketer bucketer;   //!< bucketer
    OptInt     imin, imax; //!< integer range
    OptReal    rmin, rmax; //!< real range
  };

  using TclCmds = std::vector<CQChartsExprModelFn *>;

  friend class CQChartsExprModelFn;

 private:
  void addBuiltinFunctions();

  CQTcl *qtcl() const;

  //---

  const ExtraColumn &extraColumn(int i) const { return *extraColumns_[i]; }

  ExtraColumn &extraColumn(int i) { return *extraColumns_[i]; }

  void calcExtraColumn(int column, int ecolumn);

  QVariant getExtraColumnValue(int row, int column, int ecolumn) const;

  QVariant calcExtraColumnValue(int row, int column, int ecolumn);

  //---

  void initCalc();
  void initCalc() const;

  //---

  bool decodeExpression(const QString &exprStr, QString &header, QString &expr) const;

  bool evaluateExpression(const QString &expr, QVariant &var) const;

  //---

  // get/set model data
  QVariant columnCmd   (const Values &values) const;
  QVariant rowCmd      (const Values &values) const;
  QVariant cellCmd     (const Values &values) const;
  QVariant setColumnCmd(const Values &values);
  QVariant setRowCmd   (const Values &values);
  QVariant setCellCmd  (const Values &values);

  // get/set header data
  QVariant headerCmd   (const Values &values) const;
  QVariant setHeaderCmd(const Values &values);

  // get/set column type
  QVariant typeCmd   (const Values &values) const;
  QVariant setTypeCmd(const Values &values);

  // map values
  QVariant mapCmd   (const Values &values) const;
  QVariant remapCmd (const Values &values);
  QVariant bucketCmd(const Values &values) const;
  QVariant normCmd  (const Values &values) const;
  QVariant scaleCmd (const Values &values) const;

  // random
  QVariant randCmd (const Values &values) const;
  QVariant rnormCmd(const Values &values) const;

  // string
  QVariant matchCmd(const Values &values) const;

  // color
  QVariant colorCmd(const Values &values) const;

  // time
  QVariant timevalCmd(const Values &values);

  //---

  QString replaceExprColumns(const QString &expr, int row, int column) const;

  bool setTclResult(const QVariant &rc);
  bool getTclResult(QVariant &rc) const;

  bool getColumnRange(const QModelIndex &ind, double &rmin, double &rmax);

  QVariant getCmdData(int row, int col) const;
  bool     setCmdData(int row, int col, const QVariant &var);

 protected:
  CQChartsModelData *getModelData() const;

 protected slots:
  void dataChangedSlot(const QModelIndex &from, const QModelIndex &to);

 protected:
  using ExtraColumns = std::vector<ExtraColumn *>;
  using ColumnDatas  = std::map<int,ColumnData>;
  using ColumnNames  = std::map<int,QString>;
  using NameColumns  = std::map<QString,int>;

  CQCharts*            charts_     { nullptr }; //!< charts
  CQChartsModelFilter* filter_     { nullptr }; //!< parent filter model
  QAbstractItemModel*  model_      { nullptr }; //!< child data model
  CQChartsExprTcl*     qtcl_       { nullptr }; //!< tcl expression
  TclCmds              tclCmds_;                //!< tcl commands
  bool                 editable_   { true };    //!< is editable
  bool                 debug_      { false };   //!< is debug
  ExtraColumns         extraColumns_;           //!< extra columns
  mutable int          nr_         { 0 };       //!< cached number of rows
  mutable int          nc_         { 0 };       //!< cached number of columns
  mutable int          currentRow_ { 0 };       //!< cached current row
  mutable int          currentCol_ { 0 };       //!< cached current column
  ColumnDatas          columnDatas_;            //!< cached column datas
  ColumnNames          columnNames_;            //!< cached column names
  NameColumns          nameColumns_;            //!< cached named columns
  mutable std::mutex   mutex_;
};

#endif
