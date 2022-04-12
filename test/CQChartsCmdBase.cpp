#include <CQChartsCmdBase.h>
#include <CQChartsInput.h>
#include <CQChartsEnv.h>
#include <CQBaseModel.h>
#include <CQWidgetFactory.h>
#include <CQPerfMonitor.h>
#include <CQUtil.h>
#include <CQStrUtil.h>
#include <CQTclUtil.h>
#include <CQWidgetTest.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QComboBox>

namespace {

bool
errorMsg(const QString &msg)
{
  std::cerr << msg.toStdString() << "\n";

  if (CQChartsEnv::getBool("CQ_CHARTS_PEDANTIC"))
    assert(false);

  return false;
}

}

//---

CQChartsTclCmd::
CQChartsTclCmd(CQChartsCmdBase *cmdBase, const QString &name) :
 CQTclCmd::Cmd(cmdBase, name), cmdBase_(cmdBase)
{
}

//----

CQChartsCmdProc::
CQChartsCmdProc(CQChartsCmdBase *cmdBase) :
 CQTclCmd::CmdProc(cmdBase), cmdBase_(cmdBase)
{
}

//----

CQChartsCmdBase::
CQChartsCmdBase(CQCharts *charts) :
 CQTclCmd::Mgr(charts->cmdTcl()), charts_(charts)
{
}

CQChartsCmdBase::
~CQChartsCmdBase()
{
}

void
CQChartsCmdBase::
addCommands()
{
  static bool cmdsAdded;

  if (! cmdsAdded) {
    addCommand("help"    , new CQChartsBaseHelpCmd    (this));
    addCommand("complete", new CQChartsBaseCompleteCmd(this));

    // qt generic
    addCommand("qt_create_widget"   , new CQChartsBaseCreateWidgetCmd  (this));
    addCommand("qt_create_layout"   , new CQChartsBaseCreateLayoutCmd  (this));
    addCommand("qt_add_child_widget", new CQChartsBaseAddChildWidgetCmd(this));
    addCommand("qt_add_stretch"     , new CQChartsBaseAddStretchCmd    (this));
    addCommand("qt_connect_widget"  , new CQChartsBaseConnectWidgetCmd (this));
    addCommand("qt_activate_slot"   , new CQChartsBaseActivateSlotCmd  (this));

    addCommand("qt_get_property", new CQChartsBaseGetPropertyCmd(this));
    addCommand("qt_set_property", new CQChartsBaseSetPropertyCmd(this));
    addCommand("qt_has_property", new CQChartsBaseHasPropertyCmd(this));

    addCommand("qt_sync", new CQChartsBaseQtSyncCmd(this));

    addCommand("qt_timer", new CQChartsBaseTimerCmd(this));

    addCommand("widget_test", new CQChartsBaseWidgetTestCmd(this));

    addCommand("perf", new CQChartsBasePerfCmd(this));

    qtcl()->createAlias("echo", "puts");

    addCommand("assert", new CQChartsBaseAssertCmd(this));

    addCommand("sh", new CQChartsBaseShellCmd(this));

    addCommand("exit", new CQChartsBaseExitCmd(this));

    //---

    cmdsAdded = true;
  }
}

void
CQChartsCmdBase::
addCommand(const QString &name, CQChartsCmdProc *proc)
{
  CQTclCmd::Mgr::addCommand(name, proc);
}

CQChartsCmdProc *
CQChartsCmdBase::
getCommand(const QString &name) const
{
  return static_cast<CQChartsCmdProc *>(CQTclCmd::Mgr::getCommand(name));
}

CQTclCmd::Cmd *
CQChartsCmdBase::
createCmd(const QString &name)
{
  return new CQChartsTclCmd(this, name);
}

CQTclCmd::CmdArgs *
CQChartsCmdBase::
createArgs(const QString &name, const Vars &vars)
{
  return new CQChartsCmdArgs(name, vars);
}

//------

void
CQChartsBaseCreateWidgetCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-parent", ArgType::String, "parent name");
  addArg(argv, "-type"  , ArgType::String, "widget type");
  addArg(argv, "-name"  , ArgType::String, "widget name");
}

QStringList
CQChartsBaseCreateWidgetCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "type")
    return CQWidgetFactoryMgrInst->widgetFactoryNames();

  return QStringList();
}

bool
CQChartsBaseCreateWidgetCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseCreateWidgetCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  auto typeName = argv.getParseStr("type");

  if (typeName == "?")
    return cmdBase_->setCmdRc(CQWidgetFactoryMgrInst->widgetFactoryNames());

  QWidget *parentWidget = nullptr;

  if (argv.hasParseArg("parent")) {
    auto parentName = argv.getParseStr("parent");

    auto *parent = CQUtil::nameToObject(parentName);

    parentWidget = qobject_cast<QWidget *>(parent);

    if (! parentWidget)
      return errorMsg(QString("No parent '%1'").arg(parentName));
  }

  auto name = argv.getParseStr("name");

  if (! CQWidgetFactoryMgrInst->isWidgetFactory(typeName))
    return errorMsg(QString("Invalid type '%1'").arg(typeName));

  auto *w = CQWidgetFactoryMgrInst->createWidget(typeName, parentWidget);

  if (! w)
    return errorMsg(QString("Failed to create '%1'").arg(typeName));

  if (name != "")
    w->setObjectName(name);

  return cmdBase_->setCmdRc(CQUtil::fullName(w));
}

//------

void
CQChartsBaseAddChildWidgetCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-parent" , ArgType::String , "parent name");
  addArg(argv, "-child"  , ArgType::String , "child name");
  addArg(argv, "-stretch", ArgType::Integer, "stretch");
}

QStringList
CQChartsBaseAddChildWidgetCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseAddChildWidgetCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseAddChildWidgetCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  auto  parentName   = argv.getParseStr("parent");
  auto *parentWidget = qobject_cast<QWidget *>(CQUtil::nameToObject(parentName));

  if (! parentWidget)
    return errorMsg(QString("No parent '%1'").arg(parentName));

  if      (argv.hasParseArg("child")) {
    auto  childName   = argv.getParseStr("child");
    auto *childWidget = qobject_cast<QWidget *>(CQUtil::nameToObject(childName));

    if (! childWidget)
      return errorMsg(QString("No widget '%1'").arg(childName));

    auto *layout = parentWidget->layout();

    if (! layout)
      layout = new QVBoxLayout(parentWidget);

    layout->addWidget(childWidget);

    return cmdBase_->setCmdRc(CQUtil::fullName(childWidget));
  }
  else if (argv.hasParseArg("stretch")) {
    int stretch = argv.getParseInt("stretch");

    auto *layout = qobject_cast<QBoxLayout *>(parentWidget->layout());

    if (! layout)
      return errorMsg(QString("No layout for '%1'").arg(parentName));

    layout->addStretch(stretch);

    return cmdBase_->setCmdRc(QString());
  }
  else {
    return errorMsg("Specify -child or -layout");
  }
}

//------

void
CQChartsBaseAddStretchCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-parent", ArgType::String, "parent name");
}

QStringList
CQChartsBaseAddStretchCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseAddStretchCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseAddStretchCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  auto  parentName   = argv.getParseStr("parent");
  auto *parentWidget = qobject_cast<QWidget *>(CQUtil::nameToObject(parentName));

  if (! parentWidget)
    return errorMsg(QString("No parent '%1'").arg(parentName));

  auto *layout = parentWidget->layout();

  if (! layout)
    layout = new QVBoxLayout(parentWidget);

  auto *boxLayout = qobject_cast<QBoxLayout *>(layout);

  if (boxLayout)
    boxLayout->addStretch(1);

  return true;
}

//------

void
CQChartsBaseCreateLayoutCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-parent", ArgType::String, "parent name");
  addArg(argv, "-type"  , ArgType::String, "layout type");
}

QStringList
CQChartsBaseCreateLayoutCmd::
getArgValues(const QString &arg, const NameValueMap &)
{
  if (arg == "type")
    return CQWidgetFactoryMgrInst->layoutFactoryNames();

  return QStringList();
}

bool
CQChartsBaseCreateLayoutCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseCreateLayoutCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  auto typeName = argv.getParseStr("type");

  if (typeName == "?")
    return cmdBase_->setCmdRc(CQWidgetFactoryMgrInst->layoutFactoryNames());

  QWidget *parentWidget = nullptr;

  if (argv.hasParseArg("parent")) {
    auto parentName = argv.getParseStr("parent");

    parentWidget = qobject_cast<QWidget *>(CQUtil::nameToObject(parentName));

    if (! parentWidget)
      return errorMsg(QString("No parent '%1'").arg(parentName));
  }
  else {
    return errorMsg("No parent");
  }

  if (! CQWidgetFactoryMgrInst->isLayoutFactory(typeName))
    return errorMsg(QString("Invalid type '%1'").arg(typeName));

  auto *l = CQWidgetFactoryMgrInst->createLayout(typeName, parentWidget);

  if (! l)
    return errorMsg(QString("Failed to create '%1'").arg(typeName));

  return true;
}

//------

void
CQChartsBaseConnectWidgetCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-name"  , ArgType::String, "widget name");
  addArg(argv, "-signal", ArgType::String, "signal name");
  addArg(argv, "-proc"  , ArgType::String, "tcl proc name");
}

QStringList
CQChartsBaseConnectWidgetCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseConnectWidgetCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseConnectWidgetCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  auto  name   = argv.getParseStr("name");
  auto *widget = qobject_cast<QWidget *>(CQUtil::nameToObject(name));

  if (! widget)
    return errorMsg(QString("No widget '%1'").arg(name));

  auto signalName = argv.getParseStr("signal");
  auto procName   = argv.getParseStr("proc");

  auto *slot = new CQChartsCmdBaseSlot(cmdBase_, procName);

  auto signalName2 = (QString("2") + signalName).toStdString();
  auto slotName1   = (QString("1") + signalName).toStdString();

  QObject::connect(widget, signalName2.c_str(), slot, slotName1.c_str());

  return true;
}

//------

void
CQChartsBaseActivateSlotCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-name", ArgType::String, "widget name");
  addArg(argv, "-slot", ArgType::String, "slot name");
  addArg(argv, "-args", ArgType::String, "slot args");
}

QStringList
CQChartsBaseActivateSlotCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseActivateSlotCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseActivateSlotCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  auto  name   = argv.getParseStr("name");
  auto *object = CQUtil::nameToObject(name);

  if (! object)
    return errorMsg(QString("No object '%1'").arg(name));

  auto slotName = argv.getParseStr("slot").toStdString();
  auto argsStr  = argv.getParseStr("args").toStdString();

  if (! CQUtil::activateSlot(object, slotName.c_str(), argsStr.c_str()))
    return errorMsg(QString("Invalid slot '%1'").arg(slotName.c_str()));

  return true;
}

//------

void
CQChartsBaseGetPropertyCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-object"  , ArgType::String, "object name");
  addArg(argv, "-property", ArgType::String, "property name");
}

QStringList
CQChartsBaseGetPropertyCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseGetPropertyCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseGetPropertyCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  auto objectName = argv.getParseStr("object");
  auto propName   = argv.getParseStr("property");

  auto *obj = CQUtil::nameToObject(objectName);

  if (! obj)
    return errorMsg(QString("No object '%1'").arg(objectName));

  QVariant v;

  if (! CQUtil::getProperty(obj, propName, v))
    return errorMsg(QString("Failed to get property '%1' for '%2'").arg(propName).arg(objectName));

  return cmdBase_->setCmdRc(v);
}

//------

void
CQChartsBaseSetPropertyCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-object"  , ArgType::String, "object name");
  addArg(argv, "-property", ArgType::String, "property name");
  addArg(argv, "-value"   , ArgType::String, "property value");
}

QStringList
CQChartsBaseSetPropertyCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseSetPropertyCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseSetPropertyCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  auto objectName = argv.getParseStr("object");
  auto propName   = argv.getParseStr("property");
  auto value      = argv.getParseStr("value");

  auto *obj = CQUtil::nameToObject(objectName);

  if (! obj)
    return errorMsg(QString("No object '%1'").arg(objectName));

  if (propName == "items" && qobject_cast<QComboBox *>(obj)) {
    auto *combo = qobject_cast<QComboBox *>(obj);

    QStringList strs;

    (void) CQTcl::splitList(value, strs);

    combo->clear();

    combo->addItems(strs);

    return true;
  }

  if (! CQUtil::setProperty(obj, propName, value))
    return errorMsg(QString("Failed to set property '%1' for '%2'").arg(propName).arg(objectName));

  return true;
}

//------

void
CQChartsBaseHasPropertyCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-object"  , ArgType::String , "object name");
  addArg(argv, "-property", ArgType::String , "property name");
  addArg(argv, "-writable", ArgType::Boolean, "property is writable");
}

QStringList
CQChartsBaseHasPropertyCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseHasPropertyCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseHasPropertyCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  auto objectName = argv.getParseStr("object");
  auto propName   = argv.getParseStr("property");

  auto *obj = CQUtil::nameToObject(objectName);

  if (! obj)
    return errorMsg(QString("No object '%1'").arg(objectName));

  bool b;

  if (argv.hasParseArg("writable"))
    b = CQUtil::hasWritableProperty(obj, propName);
  else
    b = CQUtil::hasProperty(obj, propName);

  return cmdBase_->setCmdRc(b);
}

//------

void
CQChartsBaseQtSyncCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-n", ArgType::Integer, "loop count");
}

QStringList
CQChartsBaseQtSyncCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseQtSyncCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseQtSyncCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  int n = 1;

  if (argv.hasParseArg("n"))
    n = argv.getParseInt("n");

  for (int i = 0; i < n; ++i) {
    qApp->flush();

    qApp->processEvents();
  }

  return true;
}

//------

void
CQChartsBaseTimerCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-delay", ArgType::Integer, "delay in ms");
  addArg(argv, "-proc" , ArgType::String , "proc to call");
}

QStringList
CQChartsBaseTimerCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseTimerCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseTimerCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  int delay = 100;

  if (argv.hasParseArg("delay"))
    delay = argv.getParseInt("delay");

  QString procName;

  if (argv.hasParseArg("proc"))
    procName = argv.getParseStr("proc");

  if (procName == "")
    return errorMsg("No proc");

  auto *slot = new CQChartsCmdBaseSlot(cmdBase_, procName);

  QTimer::singleShot(delay, slot, SLOT(timerSlot()));

  return true;
}

//------

void
CQChartsBaseWidgetTestCmd::
addCmdArgs(CQChartsCmdArgs &)
{
}

QStringList
CQChartsBaseWidgetTestCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseWidgetTestCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseWidgetTestCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  CQWidgetTest test;

  test.addType(CQWidgetTest::TestType::TOOLTIP);

  test.exec();

  return true;
}

//------

void
CQChartsBasePerfCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-start_recording", ArgType::Boolean, "start recording");
  addArg(argv, "-end_recording"  , ArgType::Boolean, "end recording"  );
  addArg(argv, "-tracing"        , ArgType::SBool  , "enable tracing" );
  addArg(argv, "-debug"          , ArgType::SBool  , "enable debug"   );
}

QStringList
CQChartsBasePerfCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBasePerfCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBasePerfCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  //---

  if (argv.hasParseArg("start_recording"))
    CQPerfMonitorInst->startRecording();

  if (argv.hasParseArg("end_recording"))
    CQPerfMonitorInst->stopRecording();

  if (argv.hasParseArg("tracing"))
    CQPerfMonitorInst->setEnabled(argv.getParseBool("tracing"));

  if (argv.hasParseArg("debug"))
    CQPerfMonitorInst->setDebug(argv.getParseBool("debug"));

  return true;
}

//------

void
CQChartsBaseAssertCmd::
addCmdArgs(CQChartsCmdArgs &)
{
}

QStringList
CQChartsBaseAssertCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseAssertCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseAssertCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  //---

  const auto &pargs = argv.getParseArgs();

  auto expr = (! pargs.empty() ? pargs[0].toString() : QString());

  QVariant res;

  bool rc = cmdBase_->qtcl()->evalExpr(expr, res, /*showError*/true);
  if (! rc) return false;

  bool b = res.toBool();

  return b;
}

//------

void
CQChartsBaseShellCmd::
addCmdArgs(CQChartsCmdArgs &)
{
}

QStringList
CQChartsBaseShellCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseShellCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseShellCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  //---

  const auto &pargs = argv.getParseArgs();

  auto nargs = pargs.size();

  auto cmd = (nargs > 0 ? pargs[0].toString() : QString());

  //---

  if (cmd == "")
    return errorMsg("No command");

  auto cmd1 = cmd;

  for (size_t i = 1; i < nargs; ++i)
    cmd1 += " " + pargs[i].toString();

  int rc = system(cmd1.toLatin1().constData());

  return cmdBase_->setCmdRc(rc);
}

//------

void
CQChartsBaseExitCmd::
addCmdArgs(CQChartsCmdArgs &)
{
}

QStringList
CQChartsBaseExitCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseExitCmd::
execCmd(CQChartsCmdArgs &)
{
  exit(0);

  return false;
}

//------

void
CQChartsBaseHelpCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-hidden" , ArgType::Boolean, "show hidden");
  addArg(argv, "-verbose", ArgType::Boolean, "verbose help");
}

QStringList
CQChartsBaseHelpCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseHelpCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseHelpCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  auto hidden  = argv.getParseBool("hidden");
  auto verbose = argv.getParseBool("verbose");

  //---

  const auto &pargs = argv.getParseArgs();

  auto pattern = (! pargs.empty() ? pargs[0].toString() : QString());

  //---

  if (pattern.length())
    cmdBase_->help(pattern, verbose, hidden);
  else
    cmdBase_->helpAll(verbose, hidden);

  return true;
}

//------

void
CQChartsBaseCompleteCmd::
addCmdArgs(CQChartsCmdArgs &argv)
{
  addArg(argv, "-command", ArgType::String, "complete command").setRequired();

  addArg(argv, "-option"     , ArgType::String , "complete option");
  addArg(argv, "-value"      , ArgType::String , "complete value");
  addArg(argv, "-name_values", ArgType::String , "option name values");
  addArg(argv, "-all"        , ArgType::Boolean, "get all matches");
  addArg(argv, "-exact_space", ArgType::Boolean, "add space if exact");
}

QStringList
CQChartsBaseCompleteCmd::
getArgValues(const QString &, const NameValueMap &)
{
  return QStringList();
}

bool
CQChartsBaseCompleteCmd::
execCmd(CQChartsCmdArgs &argv)
{
  CQPerfTrace trace("CQChartsBaseCompleteCmd::exec");

  addArgs(argv);

  if (! argv.parse())
    return false;

  auto command    = argv.getParseStr ("command");
  auto allFlag    = argv.getParseBool("all");
  auto exactSpace = argv.getParseBool("exact_space");

  //---

  using NameValueMap = std::map<QString, QString>;

  NameValueMap nameValueMap;

  auto nameValues = argv.getParseStr("name_values");

  QStringList nameValues1;

  (void) CQTcl::splitList(nameValues, nameValues1);

  for (const auto &nv : nameValues1) {
    QStringList nameValues2;

    (void) CQTcl::splitList(nv, nameValues2);

    if (nameValues2.length() == 2)
      nameValueMap[nameValues2[0]] = nameValues2[1];
  }

  //---

  if (argv.hasParseArg("option")) {
    // get option to complete
    auto option = argv.getParseStr("option");

    auto optionStr = "-" + option;

    //---

    // get proc
    auto *proc = cmdBase_->getCommand(command);
    if (! proc) return false;

    //---

    // get arg info
    Vars vars;

    CQChartsCmdArgs args(command, vars);

    proc->addArgs(args);

    //---

    if (argv.hasParseArg("value")) {
      // get option arg type
      auto *arg = args.getCmdOpt(option);
      if (! arg) return false;

      auto type = arg->type();

      //---

      // get value to complete
      auto value = argv.getParseStr("value");

      //---

      // complete by type
      QStringList strs;

      if      (type == int(ArgType::Boolean)) {
      }
      else if (type == int(ArgType::Integer)) {
      }
      else if (type == int(ArgType::Real)) {
      }
      else if (type == int(ArgType::SBool)) {
        strs << "0" << "1";
      }
      else if (type == int(ArgType::Enum)) {
        auto &nv = arg->nameValues();

        for (const auto &p : nv)
          strs.push_back(p.first);
      }
      else if (type == int(ArgType::Color)) {
      }
      else if (type == int(ArgType::Font)) {
      }
      else if (type == int(ArgType::LineDash)) {
      }
      else if (type == int(ArgType::Length)) {
      }
      else if (type == int(ArgType::Position)) {
      }
      else if (type == int(ArgType::Rect)) {
      }
      else if (type == int(ArgType::Polygon)) {
      }
      else if (type == int(ArgType::Align)) {
      }
      else if (type == int(ArgType::Sides)) {
      }
      else if (type == int(ArgType::Column)) {
      }
      else if (type == int(ArgType::Row)) {
      }
      else if (type == int(ArgType::Reals)) {
      }
      else if (type == int(ArgType::String)) {
        strs = proc->getArgValues(option, nameValueMap);
      }
      else {
        return cmdBase_->setCmdRc(strs);
      }

      //---

      if (allFlag)
        return cmdBase_->setCmdRc(strs);

      auto matchValues = CQStrUtil::matchStrs(value, strs);

      bool exact;

      auto newValue = CQStrUtil::longestMatch(matchValues, exact);

      if (newValue.length() >= value.length()) {
        if (exact && exactSpace)
          newValue += " ";

        return cmdBase_->setCmdRc(newValue);
      }

      return cmdBase_->setCmdRc(strs);
    }
    else {
      const auto &names = args.getCmdArgNames();

      if (allFlag) {
        QStringList strs;

        for (const auto &name : names)
          strs.push_back(name.mid(1));

        return cmdBase_->setCmdRc(strs);
      }

      auto matchArgs = CQStrUtil::matchStrs(optionStr, names);

      bool exact;

      auto newOption = CQStrUtil::longestMatch(matchArgs, exact);

      if (newOption.length() >= optionStr.length()) {
        if (exact && exactSpace)
          newOption += " ";

        return cmdBase_->setCmdRc(newOption);
      }
    }
  }
  else {
    const auto &cmds = cmdBase_->qtcl()->commandNames();

    auto matchCmds = CQStrUtil::matchStrs(command, cmds);

    if (allFlag)
      return cmdBase_->setCmdRc(matchCmds);

    bool exact;

    auto newCommand = CQStrUtil::longestMatch(matchCmds, exact);

    if (newCommand.length() >= command.length()) {
      if (exact && exactSpace)
        newCommand += " ";

      return cmdBase_->setCmdRc(newCommand);
    }
  }

  return cmdBase_->setCmdRc(QString());
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

bool
CQChartsCmdBase::
setCmdRc(int rc)
{
  qtcl()->setResult(rc);

  return true;
}

bool
CQChartsCmdBase::
setCmdRc(double rc)
{
  qtcl()->setResult(rc);

  return true;
}

bool
CQChartsCmdBase::
setCmdRc(const QString &rc)
{
  qtcl()->setResult(rc);

  return true;
}

bool
CQChartsCmdBase::
setCmdRc(const std::string &rc)
{
  qtcl()->setResult(QString(rc.c_str()));

  return true;
}

bool
CQChartsCmdBase::
setCmdRc(const QVariant &rc)
{
  if (qtcl()->isSupportedVariant(rc))
    qtcl()->setResult(rc);
  else {
    QString str;

    CQUtil::userVariantToString(rc, str);

    qtcl()->setResult(str);
  }

  return true;
}

bool
CQChartsCmdBase::
setCmdRc(const QStringList &rc)
{
  qtcl()->setResult(rc);

  return true;
}

bool
CQChartsCmdBase::
setCmdRc(const QVariantList &rc)
{
  qtcl()->setResult(rc);

  return true;
}

bool
CQChartsCmdBase::
setCmdRc(const std::vector<int> &rc)
{
  QVariantList vars;

  for (const auto &i : rc)
    vars.push_back(QVariant(i));

  qtcl()->setResult(vars);

  return true;
}

bool
CQChartsCmdBase::
setCmdError(const QString &msg)
{
  errorMsg(msg);

  setCmdRc(QString());

  return false;
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

void
CQChartsCmdBase::
setFilename(const QString &filename)
{
  qtcl()->createVar("filename", filename);
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
  int rc = qtcl()->eval(line, /*showError*/true, /*showResult*/log);

  if (rc != TCL_OK)
    errorMsg("Invalid line: '" + line + "'");
}

//----

CQChartsCmdBaseSlot::
CQChartsCmdBaseSlot(CQChartsCmdBase *base, const QString &procName) :
 base_(base), procName_(procName) {
}

void
CQChartsCmdBaseSlot::
clicked()
{
  execProc();
}

void
CQChartsCmdBaseSlot::
clicked(bool b)
{
  auto args = QString(b ? "1" : "0");

  execProc(args);
}

void
CQChartsCmdBaseSlot::
valueChanged(int i)
{
  QString args;

  args.setNum(i);

  execProc(args);
}

void
CQChartsCmdBaseSlot::
valueChanged(double r)
{
  QString args;

  args.setNum(r);

  execProc(args);
}

void
CQChartsCmdBaseSlot::
textEdited(const QString &s)
{
  auto args = QString("{%1}").arg(s);

  execProc(args);
}

void
CQChartsCmdBaseSlot::
returnPressed()
{
  execProc();
}

void
CQChartsCmdBaseSlot::
editingFinished()
{
  execProc();
}

void
CQChartsCmdBaseSlot::
currentIndexChanged(const QString &s)
{
  auto args = QString("{%1}").arg(s);

  execProc(args);
}

void
CQChartsCmdBaseSlot::
itemSelectionChanged()
{
  execProc();
}

void
CQChartsCmdBaseSlot::
columnClicked(int c)
{
  auto args = QString::number(c);

  execProc(args);
}

void
CQChartsCmdBaseSlot::
setNameChanged(const QString &s)
{
  auto args = QString("{%1}").arg(s);

  execProc(args);
}

void
CQChartsCmdBaseSlot::
nameChanged(const QString &s)
{
  auto args = QString("{%1}").arg(s);

  execProc(args);
}

void
CQChartsCmdBaseSlot::
symbolChanged()
{
  execProc();
}

void
CQChartsCmdBaseSlot::
timerSlot()
{
  execProc();

  this->deleteLater();
}

void
CQChartsCmdBaseSlot::
execProc(const QString &args)
{
  auto cmd = procName_;

  if (args != "")
   cmd += " " + args;

  base_->qtcl()->eval(cmd, /*showError*/true, /*showResult*/false);
}
