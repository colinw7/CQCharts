#ifndef CQChartsExprModelFn_H
#define CQChartsExprModelFn_H

#include <QVariant>

class CQChartsExprModel;

class CQTcl;

#include <tcl.h>

/*!
 * \brief Expression model function base class
 * \ingroup Charts
 */
class CQChartsExprModelFn {
 public:
  using Values = std::vector<QVariant>;

 public:
  CQChartsExprModelFn(CQChartsExprModel *model, const QString &name);

  virtual ~CQChartsExprModelFn() { }

  CQTcl *qtcl() const { return qtcl_; }

  static int commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);

  QVariant exec(const Values &values);

  bool checkColumn(int col) const;

  bool checkIndex(int row, int col) const;

 protected:
  CQChartsExprModel* model_ { nullptr };
  QString            name_;
  CQTcl*             qtcl_  { nullptr };
  Tcl_Command        cmdId_ { nullptr };
};

#endif
