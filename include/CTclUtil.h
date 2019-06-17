#ifndef CTclUtil_H
#define CTclUtil_H

// TODO: See CTclValue.h
// TODO: Use Tcl_SplitList to convert String to List

#include <tcl.h>
#include <iostream>

namespace CTclUtil {

inline int eval(Tcl_Interp *interp, const char *str) {
  return Tcl_EvalEx(interp, str, -1, 0);
}

inline bool isDomainError(Tcl_Interp *interp, int rc) {
  Tcl_Obj *options = Tcl_GetReturnOptions(interp, rc);

  Tcl_Obj *key = Tcl_NewStringObj("-errorcode", -1);

  Tcl_Obj *errorMsg;
  Tcl_IncrRefCount(key);
  Tcl_DictObjGet(NULL, options, key, &errorMsg);
  Tcl_DecrRefCount(key);

  int len = 0;
  std::string msg = Tcl_GetStringFromObj(errorMsg, &len);

  Tcl_DecrRefCount(options);

  return (msg.size() > 12 && msg.substr(0, 12) == "ARITH DOMAIN");
}

inline std::string errorInfo(Tcl_Interp *interp, int rc) {
  Tcl_Obj *options = Tcl_GetReturnOptions(interp, rc);

  Tcl_Obj *key1 = Tcl_NewStringObj("-errorcode", -1);
  Tcl_Obj *key2 = Tcl_NewStringObj("-errorinfo", -1);

  Tcl_Obj *errorMsg;
  Tcl_IncrRefCount(key1);
  Tcl_DictObjGet(NULL, options, key1, &errorMsg);
  Tcl_DecrRefCount(key1);

  int len1 = 0;
  std::string msg = Tcl_GetStringFromObj(errorMsg, &len1);

  Tcl_Obj *stackTrace;
  Tcl_IncrRefCount(key2);
  Tcl_DictObjGet(NULL, options, key2, &stackTrace);
  Tcl_DecrRefCount(key2);

  int len2 = 0;
  std::string trace = Tcl_GetStringFromObj(stackTrace, &len2);

  Tcl_DecrRefCount(options);

  return msg + "\n" + trace;
}

}

//---

class CTcl {
 public:
  CTcl() {
    interp_ = Tcl_CreateInterp();
  }

  virtual ~CTcl() {
    Tcl_DeleteInterp(interp_);
  }

   virtual Tcl_Interp *interp() const { return interp_; }

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
