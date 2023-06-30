#ifndef CQTclUtil_H
#define CQTclUtil_H

#include <CTclUtil.h>
#include <cassert>
#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QPointF>
#include <QRectF>
#include <QPolygonF>
#include <set>

namespace CQTclUtil {

using Vars = std::vector<QVariant>;

inline int eval(Tcl_Interp *interp, const QString &str) {
  return Tcl_EvalEx(interp, str.toLatin1().constData(), -1, 0);
}

//---

inline bool splitList(const QString &str, QStringList &strs) {
  int    argc;
  char **argv;

  std::string cstr = str.toStdString();

  int rc = Tcl_SplitList(nullptr, cstr.c_str(), &argc, const_cast<const char ***>(&argv));

  if (rc != TCL_OK)
    return false;

  for (int i = 0; i < argc; ++i)
    strs << QString(argv[i]);

  Tcl_Free(reinterpret_cast<char *>(argv));

  return true;
}

inline QString mergeList(const QStringList &strs) {
  auto argc = size_t(strs.size());

  std::vector<char *> argv;

  argv.resize(argc);

  for (size_t i = 0; i < argc; ++i)
    argv[i] = strdup(strs[int(i)].toLatin1().constData());

  char *res = Tcl_Merge(int(argc), &argv[0]);

  QString str(res);

  for (size_t i = 0; i < argc; ++i)
    free(argv[i]);

  Tcl_Free(res);

  return str;
}

inline bool stringToModelIndex(const QString &str, int &row, int &col) {
  QStringList strs;

  if (! splitList(str, strs))
    return false;

  if (strs.length() < 2)
    return false;

  bool ok1, ok2;

  row = strs[0].toInt(&ok1);
  col = strs[1].toInt(&ok2);

  return (ok1 && ok2);
}

//---

inline bool isSupportedVariant(const QVariant &var) {
  if (var.type() == QVariant::String || var.type() == QVariant::Double ||
      var.type() == QVariant::Int || var.type() == QVariant::LongLong ||
      var.type() == QVariant::Bool ||
      var.type() == QVariant::Point || var.type() == QVariant::PointF ||
      var.type() == QVariant::Size || var.type() == QVariant::SizeF ||
      var.type() == QVariant::Rect || var.type() == QVariant::RectF ||
      var.type() == QVariant::PolygonF ||
      var.type() == QVariant::ModelIndex ||
      var.type() == QVariant::StringList)
    return true;

  if (var.type() == QVariant::List) {
    auto vars = var.value<QVariantList>();

    int nv = vars.length();

    for (int i = 0; i < nv; ++i)
      if (! isSupportedVariant(vars[i]))
        return false;

    return true;
  }

  return var.canConvert(QVariant::String);
}

inline Tcl_Obj *variantToObj(Tcl_Interp *interp, const QVariant &var) {
  if      (var.type() == QVariant::String) {
    auto str = var.toString();

    return Tcl_NewStringObj(str.toLatin1().constData(), -1);
  }
  else if (var.type() == QVariant::Double) {
    return Tcl_NewDoubleObj(var.value<double>());
  }
  else if (var.type() == QVariant::Int) {
    return Tcl_NewIntObj(var.value<int>());
  }
  else if (var.type() == QVariant::LongLong) {
    return Tcl_NewIntObj(int(var.toLongLong()));
  }
  else if (var.type() == QVariant::Bool) {
    return Tcl_NewBooleanObj(var.value<bool>());
  }
  else if (var.type() == QVariant::PointF ||
           var.type() == QVariant::PointF) {
    QPointF p;

    if (var.type() == QVariant::Point)
      p = QPointF(var.value<QPoint>());
    else
      p = var.value<QPointF>();

    auto *obj = Tcl_NewListObj(0, nullptr);

    auto *xObj = Tcl_NewDoubleObj(p.x());
    auto *yObj = Tcl_NewDoubleObj(p.y());

    Tcl_ListObjAppendElement(interp, obj, xObj);
    Tcl_ListObjAppendElement(interp, obj, yObj);

    return obj;
  }
  else if (var.type() == QVariant::Rect ||
           var.type() == QVariant::RectF) {
    QRectF r;

    if (var.type() == QVariant::Rect)
      r = QRectF(var.value<QRect>());
    else
      r = var.value<QRectF>();

    auto *obj = Tcl_NewListObj(0, nullptr);

    auto *x1Obj = Tcl_NewDoubleObj(r.left  ());
    auto *y1Obj = Tcl_NewDoubleObj(r.bottom());
    auto *x2Obj = Tcl_NewDoubleObj(r.right ());
    auto *y2Obj = Tcl_NewDoubleObj(r.top   ());

    Tcl_ListObjAppendElement(interp, obj, x1Obj);
    Tcl_ListObjAppendElement(interp, obj, y1Obj);
    Tcl_ListObjAppendElement(interp, obj, x2Obj);
    Tcl_ListObjAppendElement(interp, obj, y2Obj);

    return obj;
  }
  else if (var.type() == QVariant::PolygonF) {
    const auto &p = var.value<QPolygonF>();

    auto *obj = Tcl_NewListObj(0, nullptr);

    for (int i = 0; i < p.length(); ++i) {
      const auto &p1 = p[i];

      auto *xObj = Tcl_NewDoubleObj(p1.x());
      auto *yObj = Tcl_NewDoubleObj(p1.y());

      Tcl_ListObjAppendElement(interp, obj, xObj);
      Tcl_ListObjAppendElement(interp, obj, yObj);
    }

    return obj;
  }
  else if (var.type() == QVariant::ModelIndex) {
    auto ind = var.value<QModelIndex>();

    int row = ind.row   ();
    int col = ind.column();

    // TODO: parent
    auto *obj = Tcl_NewListObj(0, nullptr);

    auto *robj = Tcl_NewIntObj(row);
    auto *cobj = Tcl_NewIntObj(col);

    Tcl_ListObjAppendElement(interp, obj, robj);
    Tcl_ListObjAppendElement(interp, obj, cobj);

    return obj;
  }
  else if (var.type() == QVariant::StringList) {
    auto strs = var.value<QStringList>();

    auto *obj = Tcl_NewListObj(0, nullptr);

    int ns = strs.length();

    for (int i = 0; i < ns; ++i) {
      auto *sobj = variantToObj(interp, strs[i]);

      Tcl_ListObjAppendElement(interp, obj, sobj);
    }

    return obj;
  }
  else if (var.type() == QVariant::List) {
    auto vars = var.value<QVariantList>();

    auto *obj = Tcl_NewListObj(0, nullptr);

    int nv = vars.length();

    for (int i = 0; i < nv; ++i) {
      if (! vars[i].isValid())
        continue;

      auto *sobj = variantToObj(interp, vars[i]);

      Tcl_ListObjAppendElement(interp, obj, sobj);
    }

    return obj;
  }
  else {
    QString str;

    if (var.canConvert(QVariant::String))
      str = var.toString();

    return Tcl_NewStringObj(str.toLatin1().constData(), -1);
  }
}

//---

inline QVariant variantFromObj(Tcl_Interp *interp, const Tcl_Obj *obj) {
  static const Tcl_ObjType *itype;
  static const Tcl_ObjType *rtype;
  static const Tcl_ObjType *stype;
  static const Tcl_ObjType *ltype;

  if (! itype) {
    itype = Tcl_GetObjType("int");
    rtype = Tcl_GetObjType("double");
    stype = Tcl_GetObjType("string");
    ltype = Tcl_GetObjType("list");
  }

  auto *obj1 = const_cast<Tcl_Obj *>(obj);

  double real    = 0.0;
  int    integer = 0;

  Tcl_IncrRefCount(obj1);

  QVariant var;

  const auto *type = obj1->typePtr;

  if (type) {
    if      (type == itype) {
      if (Tcl_GetIntFromObj(interp, obj1, &integer) == TCL_OK)
        var = QVariant(integer);
    }
    else if (type == rtype) {
      if (Tcl_GetDoubleFromObj(interp, obj1, &real) == TCL_OK)
        var = QVariant(real);
    }
    else if (type == ltype) {
      QList<QVariant> lvars;

      int len = 0;

      if (Tcl_ListObjLength(interp, obj1, &len) == TCL_OK) {
        for (int i = 0; i < len; ++i) {
          Tcl_Obj *lobj;

          if (Tcl_ListObjIndex(interp, obj1, i, &lobj) == TCL_OK) {
            auto lvar = variantFromObj(interp, lobj);

            lvars.push_back(lvar);
          }
        }

        var = lvars;
      }
    }
    else if (type == stype) {
    }
    else {
      //assert(false);
    }
  }

  if (! var.isValid()) {
    int len = 0;

    char *str = Tcl_GetStringFromObj(obj1, &len);

    std::string cstr(str, size_t(len));

    var = QVariant(QString(cstr.c_str()));
  }

  Tcl_DecrRefCount(obj1);

  return var;
}

//---

inline QString variantListToString(const QVariantList &vars) {
  auto *obj = Tcl_NewListObj(0, nullptr);

  for (int i = 0; i < vars.length(); ++i) {
    const auto &var = vars[i];

    auto *obj1 = variantToObj(nullptr, var);

    Tcl_ListObjAppendElement(nullptr, obj, obj1);
  }

  Tcl_IncrRefCount(obj);

  int len = 0;

  char *str = Tcl_GetStringFromObj(obj, &len);

  QString qstr(str);

  Tcl_DecrRefCount(obj);

  return qstr;
}

//---

inline void createVar(Tcl_Interp *interp, const QString &name, const QVariant &var) {
  if (var.isValid()) {
    auto *nameObj  = variantToObj(interp, name); Tcl_IncrRefCount(nameObj);
    auto *valueObj = variantToObj(interp, var );

    Tcl_ObjSetVar2(interp, nameObj, nullptr, valueObj, TCL_GLOBAL_ONLY);

    Tcl_DecrRefCount(nameObj);
  }
}

inline void deleteVar(Tcl_Interp *interp, const QString &name) {
  Tcl_UnsetVar(interp, name.toLatin1().constData(), TCL_GLOBAL_ONLY);
}

//---

inline QVariant getVar(Tcl_Interp *interp, const QString &name) {
  auto *nameObj = variantToObj(interp, name); Tcl_IncrRefCount(nameObj);

  auto *obj = Tcl_ObjGetVar2(interp, nameObj, nullptr, TCL_GLOBAL_ONLY);

  Tcl_DecrRefCount(nameObj);

  if (! obj)
    return QVariant();

  return variantFromObj(interp, obj);
}

//---

inline Vars getListVar(Tcl_Interp *interp, const QString &name) {
  auto *nameObj = variantToObj(interp, name); Tcl_IncrRefCount(nameObj);

  auto *obj = Tcl_ObjGetVar2(interp, nameObj, nullptr, TCL_GLOBAL_ONLY);

  Tcl_DecrRefCount(nameObj);

  if (! obj)
    return Vars();

  int       n    = 0;
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

//---

inline void setResult(Tcl_Interp *interp, const QVariant &var) {
  if (var.isValid())
    Tcl_SetObjResult(interp, variantToObj(interp, var));
}

//---

inline void setResult(Tcl_Interp *interp, const QStringList &strs) {
  auto *obj = variantToObj(interp, strs);

  Tcl_SetObjResult(interp, obj);
}

//---

inline void setResult(Tcl_Interp *interp, const QVariantList &vars) {
  auto *obj = variantToObj(interp, vars);

  Tcl_SetObjResult(interp, obj);
}

//---

inline QVariant getResult(Tcl_Interp *interp) {
  auto *res = Tcl_GetObjResult(interp);

  auto *res1 = Tcl_DuplicateObj(res);

  auto var = variantFromObj(interp, res);

  Tcl_SetObjResult(interp, res1);

  return var;
}

//---

inline double toReal(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Double)
    return var.value<double>();

  if (var.type() == QVariant::Int)
    return var.value<int>();

  if (var.type() == QVariant::LongLong)
    return double(var.value<qlonglong>());

  auto str = var.toString();

  return str.toDouble(&ok);
}

//---

inline long toInt(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Int)
    return var.value<int>();

  if (var.type() == QVariant::LongLong)
    return var.value<qlonglong>();

  if (var.type() == QVariant::Double) {
    double r = var.value<double>();

//  if (isInteger(r))
    return long(r);
  }

  auto str = var.toString();

  if (! ok)
    return 0;

  int i = var.toInt(&ok);

  return i;
}

}

//---

class CQTcl : public QObject, public CTcl {
  Q_OBJECT

 public:
  using Vars   = std::vector<QVariant>;
  using Traces = std::set<QString>;

  struct EvalData {
    bool     showError  { false };
    bool     showResult { false };
    int      rc { 0 };
    QVariant res;
    QString  errMsg;
  };

 public:
  CQTcl() { }

  virtual ~CQTcl() {
    int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

    for (const auto &name : traces_) {
      Tcl_UntraceVar(interp(), name.toLatin1().constData(), flags,
        &CQTcl::traceProc, static_cast<ClientData>(this));
    }
  }

  void createVar(const QString &name, const QVariant &var) {
    CQTclUtil::createVar(interp(), name, var);
  }

  void deleteVar(const QString &name) {
    CQTclUtil::deleteVar(interp(), name);
  }

  QVariant getVar(const QString &name) const {
    return CQTclUtil::getVar(interp(), name);
  }

  Vars getListVar(const QString &name) const {
    return CQTclUtil::getListVar(interp(), name);
  }

  Tcl_Command createExprCommand(const QString &name, ObjCmdProc proc, ObjCmdData data) {
    auto mathName = "tcl::mathfunc::" + name;

    return createObjCommandI(mathName, proc, data);
  }

  Tcl_Command createObjCommand(const QString &name, ObjCmdProc proc, ObjCmdData data) {
    commandNames_.push_back(name);

    return createObjCommandI(name, proc, data);
  }

  const QStringList &commandNames() const { return commandNames_; }

  int createAlias(const QString &newName, const QString &oldName) {
    return Tcl_CreateAlias(interp(), newName.toLatin1().constData(),
                           interp(), oldName.toLatin1().constData(),
                           0, nullptr);
  }

  //---

  bool evalExpr(const QString &expr, QVariant &res, bool showError=false) {
    EvalData evalData;

    evalData.showError = showError;

    auto rc = evalExpr(expr, evalData);

    res = evalData.res;

    return rc;
  }

  bool evalExpr(const QString &expr, bool showError=false) {
    EvalData evalData;

    evalData.showError = showError;

    return evalExpr(expr, evalData);
  }

  bool evalExpr(const QString &expr, EvalData &evalData) {
    return eval("expr {" + expr + "}", evalData);
  }

  //---

  bool eval(const QString &cmd, QVariant &res, bool showError=false) {
    EvalData evalData;

    evalData.showError = showError;

    auto rc = eval(cmd, evalData);

    res = evalData.res;

    return rc;
  }

  bool eval(const QString &cmd, bool showError=false, bool showResult=false) {
    EvalData evalData;

    evalData.showError  = showError;
    evalData.showResult = showResult;

    return eval(cmd, evalData);
  }

  bool eval(const QString &cmd, EvalData &evalData) {
    res_    = QVariant();
    resSet_ = false;

    bool res = true;

    evalData.rc = CQTclUtil::eval(interp(), cmd);

    if (evalData.rc != TCL_OK) {
      evalData.errMsg = errorInfo(evalData.rc);

      if (evalData.showError)
        outputError(evalData.errMsg);

      res = false;
    }

    evalData.res = getResult();

    if (evalData.showResult) {
      if (evalData.res.isValid())
        outputResult(evalData.res);
    }

    return res;
  }

  QString resToString(const QVariant &res) const {
    if      (res.type() == QVariant::StringList) {
      auto strs = res.value<QStringList>();

      return mergeList(strs);
    }
    else if (res.type() == QVariant::List) {
      auto vars = res.toList();

      QStringList strs;

      for (int i = 0; i < vars.length(); ++i) {
        auto str = resToString(vars[i]);

        strs.push_back(str);
      }

      return mergeList(strs);
    }
    else
      return res.toString();
  }

  QStringList resToStrings(const QVariant &res) const {
    QStringList strs;

    if      (res.type() == QVariant::StringList) {
      strs = res.value<QStringList>();
    }
    else if (res.type() == QVariant::List) {
      auto vars = res.toList();

      for (int i = 0; i < vars.length(); ++i) {
        auto str = resToString(vars[i]);

        strs.push_back(str);
      }
    }
    else
      strs.push_back(res.toString());

    return strs;
  }

  QVariant variantFromObj(const Tcl_Obj *obj) {
    return CQTclUtil::variantFromObj(interp(), obj);
  }

  static bool splitList(const QString &str, QStringList &strs) {
    return CQTclUtil::splitList(str, strs);
  }

  static QString mergeList(const QStringList &strs) {
    return CQTclUtil::mergeList(strs);
  }

  void traceVar(const QString &name) {
    int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

    ClientData data =
      Tcl_VarTraceInfo(interp(), name.toLatin1().constData(), flags,
                       &CQTcl::traceProc, nullptr);

    if (! data) {
      Tcl_TraceVar(interp(), name.toLatin1().constData(), flags,
                   &CQTcl::traceProc, static_cast<ClientData>(this));

      traces_.insert(name);
    }
  }

  void untraceVar(const QString &name) {
    int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

    Tcl_UntraceVar(interp(), name.toLatin1().constData(), flags,
                   &CQTcl::traceProc, static_cast<ClientData>(this));

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

  virtual void handleRead(const char *name) override {
    std::cerr << "CQTcl::handleRead " << name << "\n";
  }

  virtual void handleWrite(const char *name) override {
    std::cerr << "CQTcl::handleWrite " << name << "\n";
  }

#if 0
  virtual void handleUnset(const char *name) {
    std::cerr << "CQTcl::handleUnset " << name << "\n";
  }
#endif

  virtual void outputError(const QString &msg) const {
    std::cerr << msg.toStdString() << std::endl;
  }

  virtual void outputResult() const {
    outputResult(getTclResult());
  }

  virtual void outputResult(const QVariant &res) const {
    auto resStr = resToString(res);

    if (resStr.length())
      std::cout << resStr.toStdString() << "\n";
  }

  bool isSupportedVariant(const QVariant &var) const {
    return CQTclUtil::isSupportedVariant(var);
  }

  void setResult(const QVariant &res) {
    res_    = res;
    resSet_ = true;

    CQTclUtil::setResult(interp(), res);
  }

  void setResult(const QStringList &res) {
    res_    = res;
    resSet_ = true;

    CQTclUtil::setResult(interp(), res);
  }

  void setResult(const QVariantList &res) {
    res_    = res;
    resSet_ = true;

    CQTclUtil::setResult(interp(), res);
  }

  QVariant getResult() const {
    if (resSet_)
      return res_;

    return getTclResult();
  }

  QVariant getTclResult() const {
    return CQTclUtil::getResult(interp());
  }

  bool isDomainError(int rc) const {
    return CTclUtil::isDomainError(interp(), rc);
  }

  QString errorInfo(int rc) const {
    return CTclUtil::errorInfo(interp(), rc).c_str();
  }

 private:
  Tcl_Command createObjCommandI(const QString &name, ObjCmdProc proc, ObjCmdData data) {
    return Tcl_CreateObjCommand(interp(), const_cast<char *>(name.toLatin1().constData()),
                                proc, data, nullptr);
  }

 private:
  static char *traceProc(ClientData data, Tcl_Interp *, const char *name1,
                         const char *, int flags) {
    auto *th = static_cast<CQTcl *>(data);
    assert(th);

    th->handleTrace(name1, flags);

    return nullptr;
  }

 private:
  Traces      traces_;
  QStringList commandNames_;
  QVariant    res_;
  bool        resSet_ { false };
};

#endif
