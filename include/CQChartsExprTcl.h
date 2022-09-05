#ifndef CQChartsExprTcl_H
#define CQChartsExprTcl_H

#include <CQChartsVariant.h>
#include <CQTclUtil.h>
#include <CMathUtil.h>

#include <QAbstractItemModel>

/*!
 * \brief Tcl evaluation wrapper class for model data
 * \ingroup Charts
 *
 * The following variables are traced and allowed in expressions
 *  . <column_name> : encoded column name (initialized before processing model)
 *  . "row"         : current row
 *  . "x"           : same as "row"
 *  . "column"      : current column
 *  . "col"         : same as "column"
 *  . "PI"          : value of PI
 *  . "NaN"         : NaN value
 *  . "_"           : last computed value
 *
 * The following optional functions are supported
 *  . "column" : named/numbered column value in current row
 */
class CQChartsExprTcl : public CQTcl {
 public:
  struct ErrorData {
    bool        showError { false };
    QStringList messages;
  };

 public:
  using Values = std::vector<QVariant>;

 public:
  CQChartsExprTcl(QAbstractItemModel *model=nullptr);

  //! get/set model
  const QAbstractItemModel *model() const { return model_; }
  void setModel(const QAbstractItemModel *p) { model_ = p; }

  //! get/set row
  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  //! get/set column
  int column() const { return column_; }
  void setColumn(int i) { column_ = i; }

  //! get/set last value
  const QVariant &lastValue() const { return lastValue_; }
  void setLastValue(const QVariant &v) { lastValue_ = v; }
  void resetLastValue() { lastValue_ = QVariant(); }
  bool hasLastValue() const { return lastValue_.isValid(); }

  //---

  //! get/set name column
  int nameColumn(const QString &name) const;
  void setNameColumn(const QString &name, int column);

  //---

  static QString encodeColumnName(const QString &name);

  void setColumnRole(int column, int role) {
    columnRoles_[column] = role;
  }

  void resetColumns();

  void handleTrace(const char *name, int flags) override;

  void initVars();

  void initFunctions();

  void defineProc(const QString &name, const QString &args, const QString &body);

  bool evaluateExpression(const QString &expr, QVariant &value, bool showError=false) const;
  bool evaluateExpression(const QString &expr, QVariant &value, ErrorData &errorData) const;

  int errorCount() const { return errorCount_; }

  void incErrorCount() const { ++errorCount_; }

  //---

  void parseCmd(int objc, const Tcl_Obj **objv, Values &values);

  //---

  bool getColumnValue(const Values &values, int &ind, int &col) const;

  bool getRowValue(const Values &values, int &ind, int &row) const;

  QVariant getModelData(int row, int col) const;

  bool checkIndex(int row, int col) const;

 private:
  void setVar(const QString &name, int row, int column);

  static int columnCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);

  bool checkColumn(int col) const;

  QVariant getModelData(const QModelIndex &ind) const;

 private:
  using NameColumns = std::map<QString, int>;
  using ColumnRoles = std::map<int, int>;

  const QAbstractItemModel *model_      { nullptr }; //!< parent model
  int                       row_        { -1 };      //!< current row
  int                       column_     { -1 };      //!< current column
  QVariant                  lastValue_;              //!< last valid result
  mutable int               errorCount_ { 0 };       //!< number of errors
  NameColumns               nameColumns_;            //!< named columns
  ColumnRoles               columnRoles_;            //!< column roles
};

#endif
