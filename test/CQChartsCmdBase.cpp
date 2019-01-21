#include <CQChartsCmdBase.h>
#include <CQChartsInput.h>
#include <CQPerfMonitor.h>
#include <CQUtil.h>

#ifdef CQCharts_USE_TCL
#include <CQTclUtil.h>
#endif

#include <QApplication>

namespace {

void
errorMsg(const QString &msg)
{
  std::cerr << msg.toStdString() << "\n";
}

}

#ifdef CQCharts_USE_TCL
class CQChartsTclCmd {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQChartsTclCmd(CQChartsCmdBase *cmdBase, const QString &name) :
   cmdBase_(cmdBase), name_(name) {
    cmdId_ = cmdBase_->qtcl()->createObjCommand(name_,
               (CQTcl::ObjCmdProc) &CQChartsTclCmd::commandProc, (CQTcl::ObjCmdData) this);
  }

  static int commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv) {
    CQChartsTclCmd *command = (CQChartsTclCmd *) clientData;

    Vars vars;

    for (int i = 1; i < objc; ++i) {
      Tcl_Obj *obj = const_cast<Tcl_Obj *>(objv[i]);

      vars.push_back(command->cmdBase_->qtcl()->variantFromObj(obj));
    }

    if (! command->cmdBase_->processCmd(command->name_, vars))
      return TCL_ERROR;

    return TCL_OK;
  }

 private:
  CQChartsCmdBase *cmdBase_  { nullptr };
  QString          name_;
  Tcl_Command      cmdId_ { nullptr };
};
#endif

//----

CQChartsCmdBase::
CQChartsCmdBase()
{
#ifdef CQCharts_USE_TCL
  qtcl_ = new CQTcl();
#endif
}

CQChartsCmdBase::
~CQChartsCmdBase()
{
#ifdef CQCharts_USE_TCL
  delete qtcl_;
#endif
}

void
CQChartsCmdBase::
addCommands()
{
  static bool cmdsAdded;

  if (! cmdsAdded) {
    addCommand("help", new CQChartsBaseHelpCmd(this));

    // qt generic
    addCommand("qt_get_property", new CQChartsBaseGetPropertyCmd(this));
    addCommand("qt_set_property", new CQChartsBaseSetPropertyCmd(this));

    addCommand("qt_sync", new CQChartsBaseQtSyncCmd(this));

    addCommand("perf", new CQChartsBasePerfCmd(this));

#ifdef CQCharts_USE_TCL
    qtcl()->createAlias("echo", "puts");
#endif

    addCommand("sh", new CQChartsBaseShellCmd(this));

    //---

    cmdsAdded = true;
  }
}

void
CQChartsCmdBase::
addCommand(const QString &name, CQChartsCmdProc *proc)
{
  proc->setName(name);

#ifdef CQCharts_USE_TCL
  CQChartsTclCmd *tclCmd = new CQChartsTclCmd(this, name);

  proc->setTclCmd(tclCmd);
#else
  assert(false);
#endif

  commandNames_.push_back(name);

  commandProcs_[name] = proc;
}

bool
CQChartsCmdBase::
processCmd(const QString &cmd, const Vars &vars)
{
  auto p = commandProcs_.find(cmd);

  if (p != commandProcs_.end()) {
    CQChartsCmdProc *proc = (*p).second;

    CQChartsCmdArgs argv(cmd, vars);

    proc->exec(argv);

    return true;
  }

  //---

  if (cmd == "exit") { exit(0); }

  //---

  return false;
}

//------

void
CQChartsCmdBase::
qtGetPropertyCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::qtGetPropertyCmd");

  argv.addCmdArg("-object"  , CQChartsCmdArg::Type::String, "object name");
  argv.addCmdArg("-property", CQChartsCmdArg::Type::String, "property name");

  if (! argv.parse())
    return;

  QString objectName = argv.getParseStr("object");
  QString propName   = argv.getParseStr("property");

  QObject *obj = CQUtil::nameToObject(objectName);

  if (! obj) {
    errorMsg(QString("No object '%1'").arg(objectName));
    return;
  }

  QVariant v;

  if (! CQUtil::getProperty(obj, propName, v)) {
    errorMsg(QString("Failed to get property '%1' for '%2'").arg(propName).arg(objectName));
    return;
  }

  setCmdRc(v);
}

//------

void
CQChartsCmdBase::
qtSetPropertyCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::qtSetPropertyCmd");

  argv.addCmdArg("-object"  , CQChartsCmdArg::Type::String, "object name");
  argv.addCmdArg("-property", CQChartsCmdArg::Type::String, "property name");
  argv.addCmdArg("-value"   , CQChartsCmdArg::Type::String, "property value");

  if (! argv.parse())
    return;

  QString objectName = argv.getParseStr("object");
  QString propName   = argv.getParseStr("property");
  QString value      = argv.getParseStr("value");

  QObject *obj = CQUtil::nameToObject(objectName);

  if (! obj) {
    errorMsg(QString("No object '%1'").arg(objectName));
    return;
  }

  if (! CQUtil::setProperty(obj, propName, value)) {
    errorMsg(QString("Failed to set property '%1' for '%2'").arg(propName).arg(objectName));
    return;
  }
}

//------

void
CQChartsCmdBase::
qtSyncCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::qtSyncCmd");

  argv.addCmdArg("-n", CQChartsCmdArg::Type::Integer, "loop count");

  if (! argv.parse())
    return;

  int n = 1;

  if (argv.hasParseArg("n"))
    n = argv.getParseInt("n");

  for (int i = 0; i < n; ++i) {
    qApp->flush();

    qApp->processEvents();
  }
}

//------

void
CQChartsCmdBase::
perfCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::perfCmd");

  argv.addCmdArg("-start_recording", CQChartsCmdArg::Type::Boolean, "start recording");
  argv.addCmdArg("-end_recording"  , CQChartsCmdArg::Type::Boolean, "end recording"  );
  argv.addCmdArg("-tracing"        , CQChartsCmdArg::Type::SBool  , "enable tracing" );
  argv.addCmdArg("-debug"          , CQChartsCmdArg::Type::SBool  , "enable debug"   );

  if (! argv.parse())
    return;

  //---

  if (argv.hasParseArg("start_recording"))
    CQPerfMonitorInst->startRecording();

  if (argv.hasParseArg("end_recording"))
    CQPerfMonitorInst->stopRecording();

  if (argv.hasParseArg("tracing"))
    CQPerfMonitorInst->setEnabled(argv.getParseBool("tracing"));

  if (argv.hasParseArg("debug"))
    CQPerfMonitorInst->setDebug(argv.getParseBool("debug"));
}

//------

void
CQChartsCmdBase::
shellCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsCmdBase::shellCmd");

  if (! argv.parse())
    return;

  //---

  const Vars &shArgs = argv.getParseArgs();

  QString cmd = (! shArgs.empty() ? shArgs[0].toString() : "");

  //---

  if (cmd == "") {
    errorMsg("No command");
    return;
  }

  int rc = system(cmd.toLatin1().constData());

  setCmdRc(rc);
}

//------

void
CQChartsCmdBase::
helpCmd(CQChartsCmdArgs &)
{
  for (auto &name : commandNames_)
    std::cout << name.toStdString() << "\n";
}

//------

QStringList
CQChartsCmdBase::
stringToCmdData(const QString &str) const
{
  return CQChartsInput::stringToLines(str,
   [&](QString &line, bool &join) {
    return isCompleteLine(line, join);
   }
  );
}

//------

void
CQChartsCmdBase::
setCmdRc(int rc)
{
#ifdef CQCharts_USE_TCL
  qtcl()->setResult(rc);
#else
  std::cerr << rc << "\n";
#endif
}

void
CQChartsCmdBase::
setCmdRc(double rc)
{
#ifdef CQCharts_USE_TCL
  qtcl()->setResult(rc);
#else
  std::cerr << rc << "\n";
#endif
}

void
CQChartsCmdBase::
setCmdRc(const QString &rc)
{
#ifdef CQCharts_USE_TCL
  qtcl()->setResult(rc);
#else
  std::cerr << rc.toStdString() << "\n";
#endif
}

void
CQChartsCmdBase::
setCmdRc(const QVariant &rc)
{
#ifdef CQCharts_USE_TCL
  qtcl()->setResult(rc);
#else
  std::cerr << rc.toString().toStdString() << "\n";
#endif
}

void
CQChartsCmdBase::
setCmdRc(const QStringList &rc)
{
#ifdef CQCharts_USE_TCL
  qtcl()->setResult(rc);
#else
  for (int i = 0; i < rc.length(); ++i)
    std::cerr << rc[i].toStdString() << "\n";
#endif
}

void
CQChartsCmdBase::
setCmdRc(const QVariantList &rc)
{
#ifdef CQCharts_USE_TCL
  qtcl()->setResult(rc);
#else
  for (int i = 0; i < rc.length(); ++i)
    std::cerr << rc[i].toString().toStdString() << "\n";
#endif
}

void
CQChartsCmdBase::
setCmdError(const QString &msg)
{
  errorMsg(msg);

  setCmdRc(QString());
}

//------

QStringList
CQChartsCmdBase::
stringToCmds(const QString &str) const
{
  return CQChartsInput::stringToLines(str,
   [&](QString &line, bool &join) {
    return isCompleteLine(line, join);
   }
  );
}

//------

bool
CQChartsCmdBase::
isCompleteLine(QString &str, bool &join)
{
  join = false;

  if (! str.length())
    return true;

  if (str[str.size() - 1] == '\\') {
    str = str.mid(0, str.length() - 1);
    join = true;
    return false;
  }

  //---

  CQStrParse line(str);

  line.skipSpace();

  while (! line.eof()) {
    if      (line.isChar('{')) {
      if (! line.skipBracedString())
        return false;
    }
    else if (line.isChar('\"') || line.isChar('\'')) {
      if (! line.skipString())
        return false;
    }
    else {
      line.skipNonSpace();
    }

    line.skipSpace();
  }

  return true;
}

void
CQChartsCmdBase::
parseLine(const QString &line, bool log)
{
#ifdef CQCharts_USE_TCL
  int rc = qtcl()->eval(line, /*showError*/true, /*showResult*/log);

  if (rc != TCL_OK)
    errorMsg("Invalid line: '" + line + "'");
#else
  if (log)
    errorMsg("No eval");

  errorMsg("Invalid line: '" + line + "'");
#endif
}
