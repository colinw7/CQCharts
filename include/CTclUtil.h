#ifndef CTclUtil_H
#define CTclUtil_H

// TODO: See CTclValue.h

#include <tcl.h>
#include <iostream>

namespace CTclUtil {

inline int eval(Tcl_Interp *interp, const char *str) {
  return Tcl_EvalEx(interp, str, -1, 0);
}

inline std::string errorInfo(Tcl_Interp *interp, int rc) {
  Tcl_Obj *options = Tcl_GetReturnOptions(interp, rc);

  Tcl_Obj *key = Tcl_NewStringObj("-errorinfo", -1);

  Tcl_Obj *stackTrace;
  Tcl_IncrRefCount(key);
  Tcl_DictObjGet(NULL, options, key, &stackTrace);
  Tcl_DecrRefCount(key);

  int len = 0;
  std::string str = Tcl_GetStringFromObj(stackTrace, &len);

  Tcl_DecrRefCount(options);

  return str;
}

}

//---

class CTcl {
 public:
  CTcl() {
    interp_ = Tcl_CreateInterp();
  }

 ~CTcl() {
    Tcl_DeleteInterp(interp_);
  }

  Tcl_Interp *interp() const { return interp_; }

  void setResult(int rc) {
    Tcl_SetObjResult(interp(), Tcl_NewIntObj(rc));
  }

  void setResult(double rc) {
    Tcl_SetObjResult(interp(), Tcl_NewDoubleObj(rc));
  }

  void setResult(const char *rc) {
    Tcl_SetObjResult(interp(), Tcl_NewStringObj(rc, -1));
  }

  int eval(const char *str, bool showError=false) {
    int rc = CTclUtil::eval(interp(), str);

    if (rc != TCL_OK) {
      if (showError)
        std::cerr << errorInfo(rc) << std::endl;
    }

    return rc;
  }

  std::string errorInfo(int rc) const {
    return CTclUtil::errorInfo(interp(), rc);
  }

 protected:
  Tcl_Interp *interp_ { nullptr };
};

#endif
