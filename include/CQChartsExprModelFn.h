#ifndef CQChartsExprModelFn_H
#define CQChartsExprModelFn_H

#include <QVariant>

class CQChartsExprModel;

#ifdef CQCharts_USE_TCL
class CQTcl;

#include <tcl.h>
#endif

class CQChartsExprModelFn {
 public:
  using Values = std::vector<QVariant>;

 public:
  CQChartsExprModelFn(CQChartsExprModel *model, const QString &name);

  virtual ~CQChartsExprModelFn() { }

#ifdef CQCharts_USE_TCL
  CQTcl *qtcl() const { return qtcl_; }

  static int commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);
#endif

  QVariant exec(const Values &values);

  bool checkColumn(int col) const;

  bool checkIndex(int row, int col) const;

 protected:
  CQChartsExprModel* model_ { nullptr };
  QString            name_;
#ifdef CQCharts_USE_TCL
  CQTcl*             qtcl_  { nullptr };
  Tcl_Command        cmdId_ { nullptr };
#endif
};

#endif
