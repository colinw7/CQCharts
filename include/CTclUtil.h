#ifndef CTclUtil_H
#define CTclUtil_H

// TODO: See CTclValue.h

#include <tcl.h>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <cstring>
#include <cassert>

namespace CTclUtil {

using StringList = std::vector<std::string>;

//---

bool isCompleteLine(const std::string &line);

inline int eval(Tcl_Interp *interp, const char *str) {
  return Tcl_EvalEx(interp, str, -1, 0);
}

//---

inline Tcl_Obj *createStrObj(const std::string &str) {
  return Tcl_NewStringObj(str.c_str(), int(str.size()));
}

inline Tcl_Obj *createIntObj(int i) {
  return Tcl_NewIntObj(i);
}

inline Tcl_Obj *createRealObj(double r) {
  return Tcl_NewDoubleObj(r);
}

inline Tcl_Obj *createIntsObj(Tcl_Interp *interp, const std::vector<int> &ivals) {
  auto *obj = Tcl_NewListObj(0, nullptr);

  for (const auto &i : ivals) {
    auto *iobj = Tcl_NewIntObj(i);

    Tcl_ListObjAppendElement(interp, obj, iobj);
  }

  return obj;
}

inline std::string stringFromObj(Tcl_Obj *obj) {
  int len = 0;

  char *str = Tcl_GetStringFromObj(obj, &len);

  return std::string(str, size_t(len));
}

inline void createVar(Tcl_Interp *interp, const std::string &name, const std::string &value) {
  auto *nameObj  = createStrObj(name ); Tcl_IncrRefCount(nameObj);
  auto *valueObj = createStrObj(value);

  Tcl_ObjSetVar2(interp, nameObj, nullptr, valueObj, TCL_GLOBAL_ONLY);

  Tcl_DecrRefCount(nameObj);
}

inline void createVar(Tcl_Interp *interp, const std::string &name, int value) {
  auto *nameObj  = createStrObj(name ); Tcl_IncrRefCount(nameObj);
  auto *valueObj = createIntObj(value);

  Tcl_ObjSetVar2(interp, nameObj, nullptr, valueObj, TCL_GLOBAL_ONLY);

  Tcl_DecrRefCount(nameObj);
}

inline void createVar(Tcl_Interp *interp, const std::string &name, double value) {
  auto *nameObj  = createStrObj(name ); Tcl_IncrRefCount(nameObj);
  auto *valueObj = createRealObj(value);

  Tcl_ObjSetVar2(interp, nameObj, nullptr, valueObj, TCL_GLOBAL_ONLY);

  Tcl_DecrRefCount(nameObj);
}

inline void createVar(Tcl_Interp *interp, const std::string &name, const std::vector<int> &values) {
  auto *nameObj  = createStrObj(name ); Tcl_IncrRefCount(nameObj);
  auto *valueObj = createIntsObj(interp, values);

  Tcl_ObjSetVar2(interp, nameObj, nullptr, valueObj, TCL_GLOBAL_ONLY);

  Tcl_DecrRefCount(nameObj);
}

//---

inline std::string getVar(Tcl_Interp *interp, const std::string &name) {
  auto *nameObj = createStrObj(name); Tcl_IncrRefCount(nameObj);

  auto *obj = Tcl_ObjGetVar2(interp, nameObj, nullptr, TCL_GLOBAL_ONLY);

  Tcl_DecrRefCount(nameObj);

  if (! obj)
    return std::string();

  return stringFromObj(obj);
}

//---

inline bool splitList(const std::string &str, StringList &strs) {
  int    argc;
  char **argv;

  int rc = Tcl_SplitList(nullptr, str.c_str(), &argc, const_cast<const char ***>(&argv));

  if (rc != TCL_OK)
    return false;

  for (int i = 0; i < argc; ++i)
    strs.push_back(std::string(argv[i]));

  Tcl_Free(reinterpret_cast<char *>(argv));

  return true;
}

inline std::string mergeList(const StringList &strs) {
  auto argc = strs.size();

  std::vector<char *> argv;

  argv.resize(argc);

  for (size_t i = 0; i < argc; ++i)
    argv[i] = strdup(strs[i].c_str());

  char *res = Tcl_Merge(int(argc), &argv[0]);

  std::string str(res);

  for (size_t i = 0; i < argc; ++i)
    free(argv[i]);

  Tcl_Free(res);

  return str;
}

//---

inline bool isDomainError(Tcl_Interp *interp, int rc) {
  Tcl_Obj *options = Tcl_GetReturnOptions(interp, rc);

  Tcl_Obj *key = Tcl_NewStringObj("-errorcode", -1);

  Tcl_Obj *errorMsg;
  Tcl_IncrRefCount(key);
  Tcl_DictObjGet(NULL, options, key, &errorMsg);
  Tcl_DecrRefCount(key);

  auto msg = stringFromObj(errorMsg);

  Tcl_DecrRefCount(options);

  return (msg.size() > 12 && msg.substr(0, 12) == "ARITH DOMAIN");
}

//---

inline std::string errorInfo(Tcl_Interp *interp, int rc) {
  Tcl_Obj *options = Tcl_GetReturnOptions(interp, rc);

  Tcl_Obj *key1 = Tcl_NewStringObj("-errorcode", -1);
  Tcl_Obj *key2 = Tcl_NewStringObj("-errorinfo", -1);

  Tcl_Obj *errorMsg;
  Tcl_IncrRefCount(key1);
  Tcl_DictObjGet(NULL, options, key1, &errorMsg);
  Tcl_DecrRefCount(key1);

  auto msg = stringFromObj(errorMsg);

  Tcl_Obj *stackTrace;
  Tcl_IncrRefCount(key2);
  Tcl_DictObjGet(NULL, options, key2, &stackTrace);
  Tcl_DecrRefCount(key2);

  auto trace = stringFromObj(stackTrace);

  Tcl_DecrRefCount(options);

  return msg + "\n" + trace;
}

}

//---

class CTcl {
 public:
  using ObjCmdProc = Tcl_ObjCmdProc *;
  using ObjCmdData = ClientData;
  using StringList = std::vector<std::string>;
  using Traces     = std::set<std::string>;

  struct EvalData {
    bool        showError  { false };
    bool        showResult { false };
    int         rc { 0 };
    std::string res;
    std::string errMsg;
  };

 public:
  CTcl() {
    interp_ = Tcl_CreateInterp();
  }

  virtual ~CTcl() {
    Tcl_DeleteInterp(interp_);
  }

  Tcl_Interp *interp() const { return interp_; }

  //---

  bool init() {
    if (Tcl_Init(interp()) == TCL_ERROR)
      return false;

    return true;
  }

  //---

  // set/get variable
  void createVar(const std::string &name, const std::string &value) {
    CTclUtil::createVar(interp(), name, value);
  }

  void createVar(const std::string &name, int value) {
    CTclUtil::createVar(interp(), name, value);
  }

  void createVar(const std::string &name, double value) {
    CTclUtil::createVar(interp(), name, value);
  }

  void createVar(const std::string &name, const std::vector<int> &values) {
    CTclUtil::createVar(interp(), name, values);
  }

  std::string getVar(const std::string &name) const {
    return CTclUtil::getVar(interp(), name);
  }

  //---

  // create command
  Tcl_Command createExprCommand(const std::string &name, ObjCmdProc proc, ObjCmdData data) {
    auto mathName = "tcl::mathfunc::" + name;

    return createObjCommandI(mathName, proc, data);
  }

  Tcl_Command createObjCommand(const std::string &name, ObjCmdProc proc, ObjCmdData data) {
    commandNames_.push_back(name);

    return createObjCommandI(name, proc, data);
  }

  const StringList &commandNames() const { return commandNames_; }

  //---

  // create alias
  int createAlias(const std::string &newName, const std::string &oldName) {
    return Tcl_CreateAlias(interp(), newName.c_str(), interp(), oldName.c_str(), 0, nullptr);
  }

  //---

  // evaluate expression
  bool evalExpr(const std::string &expr, std::string &res, bool showError=false) {
    EvalData evalData;

    evalData.showError = showError;

    auto rc = evalExpr(expr, evalData);

    res = evalData.res;

    return rc;
  }

  bool evalExpr(const std::string &expr, bool showError=false) {
    EvalData evalData;

    evalData.showError = showError;

    return evalExpr(expr, evalData);
  }

  bool evalExpr(const std::string &expr, EvalData &evalData) {
    return eval("expr {" + expr + "}", evalData);
  }

  //---

  bool eval(const std::string &cmd, std::string &res, bool showError=false) {
    EvalData evalData;

    evalData.showError = showError;

    auto rc = eval(cmd, evalData);

    res = evalData.res;

    return rc;
  }

  bool eval(const std::string &cmd, bool showError=false, bool showResult=false) {
    EvalData evalData;

    evalData.showError  = showError;
    evalData.showResult = showResult;

    return eval(cmd, evalData);
  }

  bool eval(const std::string &cmd, EvalData &evalData) {
    bool res = true;

    evalData.rc = CTclUtil::eval(interp(), cmd.c_str());

    if (evalData.rc != TCL_OK) {
      evalData.errMsg = errorInfo(evalData.rc);

      if (evalData.showError)
        outputError(evalData.errMsg);

      res = false;
    }

    evalData.res = getResult();

    if (evalData.showResult) {
      if (evalData.res.size())
        outputResult(evalData.res);
    }

    return res;
  }

  static bool splitList(const std::string &str, StringList &strs) {
    return CTclUtil::splitList(str, strs);
  }

  static std::string mergeList(const StringList &strs) {
    return CTclUtil::mergeList(strs);
  }

  void traceVar(const std::string &name) {
    int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

    ClientData data =
      Tcl_VarTraceInfo(interp(), name.c_str(), flags, &CTcl::traceProc, nullptr);

    if (! data) {
      Tcl_TraceVar(interp(), name.c_str(), flags,
                   &CTcl::traceProc, static_cast<ClientData>(this));

      traces_.insert(name);
    }
  }

  void untraceVar(const std::string &name) {
    int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

    Tcl_UntraceVar(interp(), name.c_str(), flags,
                   &CTcl::traceProc, static_cast<ClientData>(this));

    traces_.erase(name);
  }

  void handleTrace(const char *name, int flags) {
    // ignore unset called on trace destruction
    if (flags & TCL_TRACE_UNSETS) return;

    bool handled = false;

    if (flags & TCL_TRACE_READS ) { handleRead (name); handled = true; }
    if (flags & TCL_TRACE_WRITES) { handleWrite(name); handled = true; }
  //if (flags & TCL_TRACE_UNSETS) { handleUnset(name); handled = true; }

    assert(handled);
  }

  virtual void handleRead(const char *name) {
    std::cerr << "CTcl::handleRead " << name << "\n";
  }

  virtual void handleWrite(const char *name) {
    std::cerr << "CTcl::handleWrite " << name << "\n";
  }

#if 0
  virtual void handleUnset(const char *name) {
    std::cerr << "CTcl::handleUnset " << name << "\n";
  }
#endif

  virtual void outputError(const std::string &msg) {
    std::cerr << msg << "\n";
  }

  virtual void outputResult(const std::string &res) {
    std::cout << res.c_str() << "\n";
  }

  void processEvents() {
    while (Tcl_DoOneEvent(TCL_DONT_WAIT));
  }

  void setResult(int rc) {
    Tcl_SetObjResult(interp(), Tcl_NewIntObj(rc));
  }

  void setResult(double rc) {
    Tcl_SetObjResult(interp(), Tcl_NewDoubleObj(rc));
  }

  void setResult(const char *rc) {
    Tcl_SetObjResult(interp(), Tcl_NewStringObj(rc, -1));
  }

  void setResult(const std::string &rc) {
    Tcl_SetObjResult(interp(), Tcl_NewStringObj(rc.c_str(), int(rc.size())));
  }

  void setResult(const std::vector<int> &rc) {
    auto *obj = Tcl_NewListObj(0, nullptr);

    for (const auto &i : rc) {
      auto *iobj = Tcl_NewIntObj(i);

      Tcl_ListObjAppendElement(interp(), obj, iobj);
    }

    Tcl_SetObjResult(interp(), obj);
  }

  //---

  std::string getResult() {
    Tcl_Obj *res = Tcl_GetObjResult(interp());

    Tcl_Obj *res1 = Tcl_DuplicateObj(res);

    auto str = CTclUtil::stringFromObj(res);

    Tcl_SetObjResult(interp(), res1);

    return str;
  }

  bool isDomainError(int rc) const {
    return CTclUtil::isDomainError(interp(), rc);
  }

  std::string errorInfo(int rc) const {
    return CTclUtil::errorInfo(interp(), rc);
  }

 private:
  Tcl_Command createObjCommandI(const std::string &name, ObjCmdProc proc, ObjCmdData data) {
    return Tcl_CreateObjCommand(interp(), const_cast<char *>(name.c_str()),
                                proc, data, nullptr);
  }

 private:
  static char *traceProc(ClientData data, Tcl_Interp *, const char *name1,
                         const char *, int flags) {
    auto *th = static_cast<CTcl *>(data);
    assert(th);

    th->handleTrace(name1, flags);

    return nullptr;
  }

 private:
  Tcl_Interp* interp_ { nullptr };
  Traces      traces_;
  StringList  commandNames_;
};

#endif
