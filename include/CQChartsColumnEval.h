#ifndef CQChartsColumnEval_H
#define CQChartsColumnEval_H

#include <CQTclUtil.h>
#include <mutex>

class CQChartsColumnEvalTcl;

#define CQChartsColumnEvalInst CQChartsColumnEval::instance()

/*!
 * \brief class to evaluate tcl expression
 * \ingroup Charts
 */
class CQChartsColumnEval {
 public:
  static CQChartsColumnEval *instance();

 ~CQChartsColumnEval();

  CQTcl *qtcl() const;

  //---

  int isDebug() const { return debug_; }
  void setDebug(int i) { debug_ = i; }

  const QAbstractItemModel *model() const { return model_; }
  void setModel(const QAbstractItemModel *model) { model_ = model; }

  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  //---

  bool evaluateExpression(const QString &exprStr, QVariant &var);

  void setVar(const QString &name, int row, int column);

 private:
  CQChartsColumnEval();

  void addFunc(const QString &name, CQTcl::ObjCmdProc proc);

  bool setTclResult(const QVariant &rc);
  bool getTclResult(QVariant &rc) const;

 private:
  using Values = std::vector<QVariant>;

  static int columnCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);
  static int colorCmd (ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);

  void parseCmd(int objc, const Tcl_Obj **objv, Values &vars);

  int setResult(const QVariant &res);

  bool getColumnValue(const Values &values, int &ind, int &row) const;
  bool getRowValue   (const Values &values, int &ind, int &row) const;

  bool checkIndex(int row, int col) const;

  QVariant getModelData(int row, int col) const;

 private:
  CQChartsColumnEvalTcl*    qtcl_  { nullptr }; //!< tcl expressio
  bool                      debug_ { false };   //!< is debug
  const QAbstractItemModel* model_ { nullptr }; //!< model
  int                       row_   { 0 };       //!< current row
  mutable QVariant          lastValue_;         //!< last evaluated value
  mutable std::mutex        mutex_;             //!< mutex
};

#endif
