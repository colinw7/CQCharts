#ifndef CQTclUtil_H
#define CQTclUtil_H

#include <CTclUtil.h>
#include <QVariant>

namespace CQTclUtil {

using Vars = std::vector<QVariant>;

inline int eval(Tcl_Interp *interp, const QString &str) {
  return Tcl_EvalEx(interp, str.toLatin1().constData(), -1, 0);
}

inline Tcl_Obj *variantToObj(Tcl_Interp *, const QVariant &var) {
  if      (var.type() == QVariant::Double) {
    return Tcl_NewDoubleObj(var.value<double>());
  }
  else if (var.type() == QVariant::Int) {
    return Tcl_NewIntObj(var.value<int>());
  }
  else if (var.type() == QVariant::Bool) {
    return Tcl_NewBooleanObj(var.value<bool>());
  }
  else {
    QString str = var.toString();

    return Tcl_NewStringObj(str.toLatin1().constData(), -1);
  }
}

inline QVariant variantFromObj(Tcl_Interp *interp, Tcl_Obj *obj) {
  double real    = 0.0;
  int    integer = 0;

  Tcl_IncrRefCount(obj);

  QVariant var;

  if      (Tcl_GetIntFromObj(interp, obj, &integer) == TCL_OK)
    var = QVariant(integer);
  else if (Tcl_GetDoubleFromObj(interp, obj, &real) == TCL_OK)
    var = QVariant(real);
  else {
    int len = 0;

    char *str = Tcl_GetStringFromObj(obj, &len);

    std::string cstr(str, len);

    var = QVariant(QString(cstr.c_str()));
  }

  Tcl_DecrRefCount(obj);

  return var;
}

inline void createVar(Tcl_Interp *interp, const QString &name, const QVariant &var) {
  Tcl_ObjSetVar2(interp, variantToObj(interp, name), nullptr,
                 variantToObj(interp, var), TCL_GLOBAL_ONLY);
}

inline QVariant getVar(Tcl_Interp *interp, const QString &name) {
  Tcl_Obj *obj = Tcl_ObjGetVar2(interp, variantToObj(interp, name), nullptr, TCL_GLOBAL_ONLY);

  if (! obj)
    return QVariant();

  return variantFromObj(interp, obj);
}

inline Vars getListVar(Tcl_Interp *interp, const QString &name) {
  Tcl_Obj *obj = Tcl_ObjGetVar2(interp, variantToObj(interp, name), nullptr, TCL_GLOBAL_ONLY);

  if (! obj)
    return Vars();

  int       n = 0;
  Tcl_Obj **objs = nullptr;

  int rc = Tcl_ListObjGetElements(interp, obj, &n, &objs);

  if (rc != TCL_OK)
    return Vars();

  Vars vars;

  for (int i = 0; i < n; ++i) {
    vars.push_back(variantFromObj(interp, objs[i]));
  }

  return vars;
}

inline void setResult(Tcl_Interp *interp, const QVariant &var) {
  Tcl_SetObjResult(interp, variantToObj(interp, var));
}

inline QVariant getResult(Tcl_Interp *interp) {
  Tcl_Obj *res = Tcl_GetObjResult(interp);

  return variantFromObj(interp, res);
}

}

//---

class CQTcl : public CTcl {
 public:
  using Vars       = std::vector<QVariant>;
  using ObjCmdProc = Tcl_ObjCmdProc *;
  using ObjCmdData = ClientData;

 public:
  CQTcl() { }

  void createVar(const QString &name, const QVariant &var) {
    CQTclUtil::createVar(interp(), name, var);
  }

  QVariant getVar(const QString &name) const {
    return CQTclUtil::getVar(interp(), name);
  }

  Vars getListVar(const QString &name) const {
    return CQTclUtil::getListVar(interp(), name);
  }

  Tcl_Command createObjCommand(const QString &name, Tcl_ObjCmdProc *proc, ClientData data) {
    return Tcl_CreateObjCommand(interp(), (char *) name.toLatin1().constData(),
                                proc, data, nullptr);
  }

  bool evalExpr(const QString &expr, QVariant &res, bool showError=false) {
    return eval("expr {" + expr + "}", res, showError);
  }

  int evalExpr(const QString &expr, bool showError=false) {
    return eval("expr {" + expr + "}", showError);
  }

  bool eval(const QString &cmd, QVariant &res, bool showError=false) {
    int rc = eval(cmd, showError);

    if (rc != TCL_OK)
      return false;

    res = getResult();

    return true;
  }

  int eval(const QString &cmd, bool showError=false) {
    int rc = CQTclUtil::eval(interp(), cmd);

    if (rc != TCL_OK) {
      if (showError)
        std::cerr << errorInfo(rc).toStdString() << std::endl;
    }

    return rc;
  }

  void setResult(const QVariant &rc) {
    CQTclUtil::setResult(interp(), rc);
  }

  QVariant getResult() const {
    return CQTclUtil::getResult(interp());
  }

  QString errorInfo(int rc) const {
    return CTclUtil::errorInfo(interp(), rc).c_str();
  }
};

#endif
