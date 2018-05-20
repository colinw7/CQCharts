#ifndef CQTclUtil_H
#define CQTclUtil_H

#include <CTclUtil.h>

namespace CQTclUtil {

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
    return Tcl_NewBooleanObj(var.value<double>());
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
  using ObjCmdProc = Tcl_ObjCmdProc *;
  using ObjCmdData = ClientData;

 public:
  CQTcl() { }

  void createVar(const QString &name, const QVariant &var) {
    CQTclUtil::createVar(interp(), name, var);
  }

  Tcl_Command createObjCommand(const QString &name, Tcl_ObjCmdProc *proc, ClientData data) {
    return Tcl_CreateObjCommand(interp(), (char *) name.toLatin1().constData(),
                                proc, data, nullptr);
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
