#include <CQTclCmd.h>
#include <CQTclUtil.h>
#include <cassert>

namespace CQTclCmd {

//------

Mgr::
Mgr(CQTcl *qtcl) :
 qtcl_(qtcl)
{
}

Mgr::
~Mgr()
{
}

void
Mgr::
addCommand(const QString &name, CmdProc *proc)
{
  proc->setName(name);

  auto *cmd = createCmd(name);

  proc->setCmd(cmd);

  commandNames_.push_back(name);

  commandProcs_[name] = proc;
}

bool
Mgr::
processCmd(const QString &name, const Vars &vars)
{
  auto p = commandProcs_.find(name);

  if (p != commandProcs_.end()) {
    auto *proc = (*p).second;

    auto *args = createArgs(name, vars);

    bool rc = proc->exec(*args);

    delete args;

    return rc;
  }

  //---

  return false;
}

CmdProc *
Mgr::
getCommand(const QString &name) const
{
  auto p = commandProcs_.find(name);

  return (p != commandProcs_.end() ? (*p).second : nullptr);
}

Cmd *
Mgr::
createCmd(const QString &name)
{
  return new Cmd(this, name);
}

CmdArgs *
Mgr::
createArgs(const QString &name, const Vars &vars)
{
  return new CmdArgs(name, vars);
}

bool
Mgr::
help(const QString &pattern, bool verbose, bool hidden)
{
  using Procs = std::vector<CmdProc *>;

  Procs procs;

  auto p = commandProcs_.find(pattern);

  if (p != commandProcs_.end()) {
    procs.push_back((*p).second);
  }
  else {
    QRegExp re(pattern, Qt::CaseSensitive, QRegExp::Wildcard);

    for (auto &p : commandProcs_) {
      if (re.exactMatch(p.first))
        procs.push_back(p.second);
    }
  }

  if (procs.empty()) {
    std::cout << "Command not found\n";
    return false;
  }

  for (const auto &proc : procs) {
    if (verbose) {
      Vars vars;

      auto *args = createArgs(proc->name(), vars);

      proc->addArgs(*args);

      args->help(hidden);

      delete args;
    }
    else {
      std::cout << proc->name().toStdString() << "\n";
    }
  }

  return true;
}

void
Mgr::
helpAll(bool verbose, bool hidden)
{
  // all procs
  for (auto &p : commandProcs_) {
    auto *proc = p.second;

    if (verbose) {
      Vars vars;

      auto *args = createArgs(proc->name(), vars);

      proc->addArgs(*args);

      args->help(hidden);

      delete args;
    }
    else {
      std::cout << proc->name().toStdString() << "\n";
    }
  }
}

//------

CmdArg::
CmdArg(int ind, const QString &name, int type, const QString &argDesc, const QString &desc) :
 ind_(ind), name_(name), type_(type), argDesc_(argDesc), desc_(desc)
{
  if (name_.left(1) == "-") {
    isOpt_ = true;

    name_ = name_.mid(1);
  }
}

CmdArg &
CmdArg::
addNameValue(const QString &name, int value)
{
  nameValues_.push_back(NameValue(name, value));
  return *this;
}

//---

CmdGroup::
CmdGroup(int ind, Type type) :
 ind_(ind), type_(type)
{
}

//---

CmdArgs::
CmdArgs(const QString &cmdName, const Args &argv) :
 cmdName_(cmdName), argv_(argv), argc_(int(argv_.size()))
{
}

CmdGroup &
CmdArgs::
startCmdGroup(CmdGroup::Type type)
{
  int ind = int(cmdGroups_.size() + 1);

  cmdGroups_.emplace_back(ind, type);

  groupInd_ = ind;

  return cmdGroups_.back();
}

void
CmdArgs::
endCmdGroup()
{
  groupInd_ = -1;
}

//---

CmdArg &
CmdArgs::
addCmdArg(const QString &name, int type, const QString &argDesc, const QString &desc)
{
  int ind = int(cmdArgs_.size() + 1);

  cmdArgs_.emplace_back(ind, name, type, argDesc, desc);

  CmdArg &cmdArg = cmdArgs_.back();

  cmdArg.setGroupInd(groupInd_);

  return cmdArg;
}

//---

bool
CmdArgs::
eof() const
{
  return (i_ >= argc_);
}

void
CmdArgs::
rewind()
{
  i_ = 0;
}

const CmdArgs::Arg &
CmdArgs::
getArg()
{
  assert(i_ < argc_);

  lastArg_ = Arg(argv_[size_t(i_++)]);

  return lastArg_;
}

//---

bool
CmdArgs::
getOptValue(QStringList &strs)
{
  if (eof()) return false;

  strs = toStringList(argv_[size_t(i_++)]);

  return true;
}

bool
CmdArgs::
getOptValue(QString &str)
{
  if (eof()) return false;

  str = toString(argv_[size_t(i_++)]);

  return true;
}

bool
CmdArgs::
getOptValue(int &i)
{
  QString str;

  if (! getOptValue(str))
    return false;

  bool ok;

  i = str.toInt(&ok);

  return ok;
}

bool
CmdArgs::
getOptValue(double &r)
{
  QString str;

  if (! getOptValue(str))
    return false;

  bool ok;

  r = str.toDouble(&ok);

  return ok;
}

bool
CmdArgs::
getOptValue(OptReal &r)
{
  QString str;

  if (! getOptValue(str))
    return false;

  bool ok;

  r = str.toDouble(&ok);

  return ok;
}

bool
CmdArgs::
getOptValue(bool &b)
{
  QString str;

  if (! getOptValue(str))
    return false;

  str = str.toLower();

  bool ok;

  b = stringToBool(str, &ok);

  return ok;
}

//---

bool
CmdArgs::
parse(bool &rc)
{
  rc = false;

  // clear parsed values
  parseInt_ .clear();
  parseReal_.clear();
  parseStr_ .clear();
  parseBool_.clear();
  parseArgs_.clear();

  //---

  using Names      = std::set<QString>;
  using GroupNames = std::map<int, Names>;

  GroupNames groupNames;

  //---

  bool help       = false;
  bool showHidden = false;
  bool allowOpt   = true;

  while (! eof()) {
    // get next arg
    auto arg = getArg();

    if (! allowOpt && arg.isOpt())
      arg.setIsOpt(false);

    // handle option (starts with '-')
    if (arg.isOpt()) {
      // get option name
      auto opt = arg.opt();

      //---

      // flag if help option
      if (opt == "help") {
        help = true;
        continue;
      }

      // flag if hidden option (don't skip argument for command)
      if (opt == "hidden") {
        showHidden = true;
      }

      // handle '--' for no more options
      if (opt == "-") {
        allowOpt = false;
        continue;
      }

      //---

      // get arg data for option (flag error if not found)
      auto *cmdArg = getCmdOpt(opt);

      if (! cmdArg) {
        if (opt != "hidden")
          return this->error();
      }

      //---

      // record option for associated group (if any)
      if (cmdArg->groupInd() >= 0) {
        groupNames[cmdArg->groupInd()].insert(cmdArg->name());
      }

      //---

      if (! handleParseArg(cmdArg, opt))
        continue;
    }
    // handle argument (no '-')
    else {
      // save argument
      parseArgs_.push_back(arg.var());
    }
  }

  //---

  // if help option specified ignore other options and process help
  if (help) {
    this->help(showHidden);

    if (! isDebug()) {
      rc = true;

      return false;
    }
  }

  //---

  // check options specified for cmd groups
  for (const auto &cmdGroup : cmdGroups_) {
    int groupInd = cmdGroup.ind();

    auto p = groupNames.find(groupInd);

    // handle no options for required group
    if (p == groupNames.end()) {
      if (cmdGroup.isRequired()) {
        std::string names = getGroupArgNames(groupInd).join(", ").toStdString();
        std::cerr << "One of " << names << " required\n";
        return false;
      }
    }
    // handle multiple options for one of group
    else {
      const Names &names = (*p).second;

      if (names.size() > 1) {
        std::string names = getGroupArgNames(groupInd).join(", ").toStdString();
        std::cerr << "Only one of " << names << " allowed\n";
        return false;
      }
    }
  }

  //---

  // display parsed data for debug
  if (isDebug()) {
    for (auto &pi : parseInt_) {
      std::cerr << pi.first.toStdString() << "=" << pi.second << "\n";
    }
    for (auto &pr : parseReal_) {
      std::cerr << pr.first.toStdString() << "=" << pr.second << "\n";
    }
    for (auto &ps : parseStr_) {
      const QString     &name   = ps.first;
      const QStringList &values = ps.second;

      for (int i = 0; i < ps.second.length(); ++i) {
        std::cerr << name.toStdString() << "=" << values[i].toStdString() << "\n";
      }
    }
    for (auto &ps : parseBool_) {
      std::cerr << ps.first.toStdString() << "=" << ps.second << "\n";
    }
    for (auto &a : parseArgs_) {
      std::cerr << toString(a).toStdString() << "\n";
    }
  }

  //---

  rc = true;

  return true;
}

bool
CmdArgs::
handleParseArg(CmdArg *cmdArg, const QString &opt)
{
  // handle bool option (no value)
  if      (cmdArg->type() == int(CmdArg::Type::Boolean)) {
    parseBool_[opt] = true;
  }
  // handle integer option
  else if (cmdArg->type() == int(CmdArg::Type::Integer)) {
    int i = 0;

    if (getOptValue(i)) {
      parseInt_[opt] = i;
    }
    else {
      return valueError(opt);
    }
  }
  // handle real option
  else if (cmdArg->type() == int(CmdArg::Type::Real)) {
    double r = 0.0;

    if (getOptValue(r)) {
      parseReal_[opt] = r;
    }
    else {
      return valueError(opt);
    }
  }
  // handle string option
  else if (cmdArg->type() == int(CmdArg::Type::String)) {
    if (cmdArg->isMultiple()) {
      QStringList strs;

      if (getOptValue(strs)) {
        for (int i = 0; i < strs.length(); ++i)
          parseStr_[opt].push_back(strs[i]);
      }
      else
        return valueError(opt);
    }
    else {
      QString str;

      if (getOptValue(str))
        parseStr_[opt].push_back(str);
      else
        return valueError(opt);
    }
  }
  // handle string bool option
  else if (cmdArg->type() == int(CmdArg::Type::SBool)) {
    bool b;

    if (getOptValue(b)) {
      parseBool_[opt] = b;
    }
    else {
      return valueError(opt);
    }
  }
  // handle enum option (string)
  else if (cmdArg->type() == int(CmdArg::Type::Enum)) {
    QString str;

    if (getOptValue(str)) {
      bool found = false;

      for (auto &nv : cmdArg->nameValues()) {
        if (str == nv.first) {
          parseInt_[opt] = nv.second;
          found = true;
          break;
        }
      }

      if (! found) {
        QString msg =
          QString("Invalid value '%1' for '-%2' should be one of :\n ").arg(str).arg(opt);
        for (auto &nv : cmdArg->nameValues())
          msg += " " + nv.first;
        errorMsg(msg);
        return false;
      }
    }
    else {
      return valueError(opt);
    }
  }
  // invalid type (assert ?)
  else {
    std::cerr << "Invalid type for '" << opt.toStdString() << "'\n";
    return false;
  }

  return true;
}

//---

bool
CmdArgs::
hasParseArg(const QString &name) const
{
  if      (parseInt_ .find(name) != parseInt_ .end()) return true;
  else if (parseReal_.find(name) != parseReal_.end()) return true;
  else if (parseStr_ .find(name) != parseStr_ .end()) return true;
  else if (parseBool_.find(name) != parseBool_.end()) return true;

  return false;
}

//---

int
CmdArgs::
getParseInt(const QString &name, int def) const
{
  auto p = parseInt_.find(name);
  if (p == parseInt_.end()) return def;

  return (*p).second;
}

CmdArgs::OptInt
CmdArgs::
getParseOptInt(const QString &name) const
{
  auto p = parseInt_.find(name);
  if (p == parseInt_.end()) return OptInt();

  return OptInt((*p).second);
}
double
CmdArgs::
getParseReal(const QString &name, double def) const
{
  auto p = parseReal_.find(name);
  if (p == parseReal_.end()) return def;

  return (*p).second;
}

CmdArgs::OptReal
CmdArgs::
getParseOptReal(const QString &name) const
{
  auto p = parseReal_.find(name);
  if (p == parseReal_.end()) return OptReal();

  return OptReal((*p).second);
}

int
CmdArgs::
getNumParseStrs(const QString &name) const
{
  auto p = parseStr_.find(name);
  if (p == parseStr_.end()) return 0;

  return (*p).second.size();
}

QStringList
CmdArgs::
getParseStrs(const QString &name) const
{
  auto p = parseStr_.find(name);
  if (p == parseStr_.end()) return QStringList();

  return (*p).second;
}

QString
CmdArgs::
getParseStr(const QString &name, const QString &def) const
{
  auto p = parseStr_.find(name);
  if (p == parseStr_.end()) return def;

  return (*p).second[0];
}

bool
CmdArgs::
getParseBool(const QString &name, bool def) const
{
  auto p = parseBool_.find(name);
  if (p == parseBool_.end()) return def;

  return (*p).second;
}

CmdArgs::OptBool
CmdArgs::
getParseOptBool(const QString &name) const
{
  auto p = parseBool_.find(name);
  if (p == parseBool_.end()) return OptBool();

  return OptBool((*p).second);
}

//---

CmdArg *
CmdArgs::
getCmdOpt(const QString &name)
{
  for (auto &cmdArg : cmdArgs_) {
    if (cmdArg.isOpt() && cmdArg.name() == name)
      return &cmdArg;
  }

  return nullptr;
}

//---

QStringList
CmdArgs::
getCmdArgNames() const
{
  QStringList names;

  for (auto &cmdArg : cmdArgs_) {
    if (cmdArg.isOpt())
      names.push_back("-" + cmdArg.name());
  }

  return names;
}

//---

bool
CmdArgs::
valueError(const QString &opt)
{
  errorMsg(QString("Missing value for '-%1'").arg(opt));
  return false;
}

bool
CmdArgs::
error()
{
  if (lastArg_.isOpt())
    errorMsg("Invalid option '" + lastArg_.opt() + "'");
  else
    errorMsg("Invalid arg '" + toString(lastArg_.var()) + "'");

  return false;
}

//---

void
CmdArgs::
help(bool showHidden) const
{
  using GroupIds = std::set<int>;

  GroupIds groupInds;

  std::cerr << cmdName_.toStdString() << "\n";

  for (auto &cmdArg : cmdArgs_) {
    if (! showHidden && cmdArg.isHidden())
      continue;

    int groupInd = cmdArg.groupInd();

    if (groupInd > 0) {
      auto p = groupInds.find(groupInd);

      if (p == groupInds.end()) {
        std::cerr << "  ";

        helpGroup(groupInd, showHidden);

        std::cerr << "\n";

        groupInds.insert(groupInd);
      }
      else
        continue;
    }
    else {
      std::cerr << "  ";

      if (! cmdArg.isRequired())
        std::cerr << "[";

      helpArg(cmdArg);

      if (! cmdArg.isRequired())
        std::cerr << "]";

      std::cerr << "\n";
    }
  }

  std::cerr << "  [-help]\n";
}

void
CmdArgs::
helpGroup(int groupInd, bool showHidden) const
{
  assert(groupInd > 0);

  const CmdGroup &cmdGroup = cmdGroups_[size_t(groupInd - 1)];

  if (! cmdGroup.isRequired())
    std::cerr << "[";

  CmdArgArray cmdArgs;

  getGroupCmdArgs(groupInd, cmdArgs);

  int i = 0;

  for (const auto &cmdArg : cmdArgs) {
    if (! showHidden && cmdArg.isHidden())
      continue;

    if (i > 0)
      std::cerr << "|";

    helpArg(cmdArg);

    ++i;
  }

  if (! cmdGroup.isRequired())
    std::cerr << "]";
}

void
CmdArgs::
helpArg(const CmdArg &cmdArg) const
{
  if (cmdArg.isOpt()) {
    std::cerr << "-" << cmdArg.name().toStdString() << " ";

    if (cmdArg.type() != int(CmdArg::Type::Boolean))
      std::cerr << "<" << cmdArg.argDesc().toStdString() << ">";
  }
  else {
    std::cerr << "<" << cmdArg.argDesc().toStdString() << ">";
  }
}

//---

QStringList
CmdArgs::
toStringList(const QVariant &var)
{
  QStringList strs;

  if (var.type() == QVariant::List) {
    QList<QVariant> vars = var.toList();

    for (int i = 0; i < vars.length(); ++i) {
      QString str = toString(vars[i]);

      strs.push_back(str);
    }
  }
  else {
    strs.push_back(var.toString());
  }

  return strs;
}

QString
CmdArgs::
toString(const QVariant &var)
{
  if (var.type() == QVariant::List) {
    QList<QVariant> vars = var.toList();

    QStringList strs;

    for (int i = 0; i < vars.length(); ++i) {
      QString str = toString(vars[i]);

      strs.push_back(str);
    }

    CQTcl tcl;

    return tcl.mergeList(strs);
  }
  else
    return var.toString();
}

bool
CmdArgs::
stringToBool(const QString &str, bool *ok)
{
  QString lstr = str.toLower();

  if (lstr == "0" || lstr == "false" || lstr == "no") {
    *ok = true;
    return false;
  }

  if (lstr == "1" || lstr == "true" || lstr == "yes") {
    *ok = true;
    return true;
  }

  *ok = false;

  return false;
}

//---

QStringList
CmdArgs::
getGroupArgNames(int groupInd) const
{
  CmdArgArray cmdArgs;

  getGroupCmdArgs(groupInd, cmdArgs);

  QStringList names;

  for (const auto &cmdArg : cmdArgs)
    names << cmdArg.name();

  return names;
}

void
CmdArgs::
getGroupCmdArgs(int groupInd, CmdArgArray &cmdArgs) const
{
  for (auto &cmdArg : cmdArgs_) {
    int groupInd1 = cmdArg.groupInd();

    if (groupInd1 != groupInd)
      continue;

    cmdArgs.push_back(cmdArg);
  }
}

void
CmdArgs::
errorMsg(const QString &msg)
{
  std::cerr << msg.toStdString() << "\n";
}

//------

CmdArgs::Arg::
Arg(const QVariant &var) :
 var_(var)
{
  QString varStr = toString(var_);

  isOpt_ = (varStr.length() && varStr[0] == '-');
}

//------

Cmd::
Cmd(Mgr *mgr, const QString &name) :
 mgr_(mgr), name_(name)
{
  auto *qtcl = mgr->qtcl();

  cmdId_ = qtcl->createObjCommand(name_,
    reinterpret_cast<CQTcl::ObjCmdProc>(&Cmd::commandProc),
    static_cast<CQTcl::ObjCmdData>(this));
}

int
Cmd::
commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *command = static_cast<Cmd *>(clientData);

  return command->exec(objc, objv);
}

int
Cmd::
exec(int objc, const Tcl_Obj **objv)
{
  auto *qtcl = mgr_->qtcl();

  Vars vars;

  for (int i = 1; i < objc; ++i) {
    auto *obj = const_cast<Tcl_Obj *>(objv[i]);

    vars.push_back(qtcl->variantFromObj(obj));
  }

  if (! mgr_->processCmd(name_, vars))
    return TCL_ERROR;

  return TCL_OK;
}

//------

CmdProc::
CmdProc(Mgr *mgr) :
 mgr_(mgr)
{
}

//------

}
