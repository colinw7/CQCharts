#ifndef CQChartsColumnEval_H
#define CQChartsColumnEval_H

#include <CQTclUtil.h>
#include <mutex>

class CQChartsExprTcl;

#define CQChartsColumnEvalInst CQChartsColumnEval::instance()

/*!
 * \brief singleton class to evaluate tcl expression
 * \ingroup Charts
 *
 * Used by CQChartsColumn expression handling
 *
 * Tcl expression with column() and color() functions
 */
class CQChartsColumnEval {
 public:
  using Values = std::vector<QVariant>;

 public:
  static CQChartsColumnEval *instance();

 ~CQChartsColumnEval();

  CQTcl *qtcl() const;

  //---

  //! get/set is debug
  int isDebug() const { return debug_; }
  void setDebug(int i) { debug_ = i; }

  //! get/set model
  const QAbstractItemModel *model() const;
  void setModel(const QAbstractItemModel *model);

  //! get/set row
  int row() const;
  void setRow(int r);

  //---

  // evaluate expression
  bool evaluateExpression(const QString &exprStr, QVariant &var, bool showError);

  void setVar(const QString &name, int row, int column);

 private:
  CQChartsColumnEval();

  void addFunc(const QString &name, CQTcl::ObjCmdProc proc);

 private:
  // column command
  static int columnCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);
  // color command
  static int colorCmd (ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);

  void parseCmd(int objc, const Tcl_Obj **objv, Values &vars);

  int setResult(const QVariant &res);

  bool getColumnValue(const Values &values, int &ind, int &row) const;
  bool getRowValue   (const Values &values, int &ind, int &row) const;

  bool checkIndex(int row, int col) const;

  QVariant getModelData(int row, int col) const;

 private:
  CQChartsExprTcl*   qtcl_  { nullptr }; //!< tcl expressio
  bool               debug_ { false };   //!< is debug
  mutable std::mutex mutex_;             //!< mutex
};

#endif
