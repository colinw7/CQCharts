#ifndef CQChartsCmdsArgs_H
#define CQChartsCmdsArgs_H

#include <CQChartsLength.h>
#include <CQChartsPosition.h>
#include <CQChartsColor.h>
#include <CQChartsLineDash.h>
#include <CQChartsUtil.h>

#include <CQAlignEdit.h>
#include <CQStrParse.h>

#include <QFont>

class CQChartsCmdArg {
 public:
  enum class Type {
    None,
    Boolean,
    Integer,
    Real,
    String,
    SBool,
    Color,
    Font,
    LineDash,
    Length,
    Position,
    Polygon,
    Align
  };

 public:
  CQChartsCmdArg(int ind, const QString &name, Type type, const QString &argDesc="",
                 const QString &desc="") :
   ind_(ind), name_(name), type_(type), argDesc_(argDesc), desc_(desc) {
    if (name_.left(1) == "-") {
      isOpt_ = true;

      name_ = name_.mid(1);
    }
  }

  const QString &name() const { return name_; }

  bool isOpt() const { return isOpt_; }

  Type type() const { return type_; }

  const QString &argDesc() const { return argDesc_; }

  const QString &desc() const { return desc_; }

  bool isRequired() const { return required_; }
  CQChartsCmdArg &setRequired(bool b=true) { required_ = b; return *this; }

  int groupInd() const { return groupInd_; }
  void setGroupInd(int i) { groupInd_ = i; }

 private:
  int     ind_      { -1 };
  QString name_;
  bool    isOpt_    { false };
  Type    type_     { Type::None };
  QString argDesc_;
  QString desc_;
  bool    required_ { false };
  int     groupInd_ { -1 };
};

//------

class CQChartsCmdGroup {
 public:
  enum class Type {
    None,
    OneOpt,
    OneReq
  };

 public:
  CQChartsCmdGroup(int ind, Type type) :
   ind_(ind), type_(type) {
  }

  int ind() const { return ind_; }

  bool isRequired() const { return (type_ == Type::OneReq); }

 private:
  int  ind_ { -1 };
  Type type_ { Type::None };
};

//------

class CQChartsCmdsArgs {
 public:
  using Args    = std::vector<QVariant>;
  using OptReal = boost::optional<double>;
  using OptInt  = boost::optional<int>;
  using OptBool = boost::optional<bool>;

  class Arg {
   public:
    Arg(const QVariant &var=QVariant()) :
     var_(var) {
      QString varStr = var_.toString();

      isOpt_ = (varStr.length() && varStr[0] == '-');
    }

  //QString  str() const { assert(! isOpt_); return var_.toString(); }
    QVariant var() const { assert(! isOpt_); return var_; }

    bool isOpt() const { return isOpt_; }

    QString opt() const { assert(isOpt_); return var_.toString().mid(1); }

   private:
    QVariant var_;
    bool     isOpt_ { false };
  };

 public:
  CQChartsCmdsArgs(const QString &cmdName, const Args &argv) :
   cmdName_(cmdName), argv_(argv), argc_(argv_.size()) {
  }

  CQChartsCmdGroup &startCmdGroup(CQChartsCmdGroup::Type type) {
    int ind = cmdGroups_.size() + 1;

    cmdGroups_.emplace_back(ind, type);

    groupInd_ = ind;

    return cmdGroups_.back();
  }

  void endCmdGroup() {
    groupInd_ = -1;
  }

  CQChartsCmdArg &addCmdArg(const QString &name, CQChartsCmdArg::Type type,
                            const QString &argDesc="", const QString &desc="") {
    int ind = cmdArgs_.size() + 1;

    cmdArgs_.emplace_back(ind, name, type, argDesc, desc);

    CQChartsCmdArg &cmdArg = cmdArgs_.back();

    cmdArg.setGroupInd(groupInd_);

    return cmdArg;
  }

  bool eof() const { return (i_ >= argc_); }

  void rewind() { i_ = 0; }

  const Arg &getArg() {
    assert(i_ < argc_);

    lastArg_ = Arg(argv_[i_++]);

    return lastArg_;
  }

  bool getOptValue(QString &str) {
    if (eof()) return false;

    str = argv_[i_++].toString();

    return true;
  }

  bool getOptValue(int &i) {
    QString str;

    if (! getOptValue(str))
      return false;

    bool ok;

    i = str.toInt(&ok);

    return ok;
  }

  bool getOptValue(double &r) {
    QString str;

    if (! getOptValue(str))
      return false;

    bool ok;

    r = str.toDouble(&ok);

    return ok;
  }

  bool getOptValue(boost::optional<double> &r) {
    QString str;

    if (! getOptValue(str))
      return false;

    bool ok;

    r = str.toDouble(&ok);

    return ok;
  }

  bool getOptValue(bool &b) {
    QString str;

    if (! getOptValue(str))
      return false;

    str = str.toLower();

    if      (str == "0" || str == "no"  || str == "false" || str == "off") {
      b = false; return true;
    }
    else if (str == "1" || str == "yes" || str == "true"  || str == "on" ) {
      b = true; return true;
    }

    return false;
  }

  bool getOptValue(CQChartsLength &l) {
    QString str;

    if (! getOptValue(str))
      return false;

    l = CQChartsLength(str);

    return true;
  }

  bool getOptValue(CQChartsPosition &p) {
    QString str;

    if (! getOptValue(str))
      return false;

    p = CQChartsPosition(str);

    return true;
  }

  bool getOptValue(QFont &f) {
    QString str;

    if (! getOptValue(str))
      return false;

    f = QFont(str);

    return true;
  }

  bool getOptValue(CQChartsColor &c) {
    QString str;

    if (! getOptValue(str))
      return false;

    c = CQChartsColor(str);

    return true;
  }

  bool getOptValue(CQChartsLineDash &d) {
    QString str;

    if (! getOptValue(str))
      return false;

    d = CQChartsLineDash(str);

    return true;
  }

  bool getOptValue(Qt::Alignment &a) {
    QString str;

    if (! getOptValue(str))
      return false;

    a = CQAlignEdit::fromString(str);

    return true;
  }

  bool getOptValue(QPolygonF &poly) {
    QString str;

    if (! getOptValue(str))
      return false;

    CQStrParse parse(str);

    while (! parse.eof()) {
      parse.skipSpace();

      QString xstr;

      if (! parse.readNonSpace(xstr))
        break;

      parse.skipSpace();

      QString ystr;

      if (! parse.readNonSpace(ystr))
        break;

      parse.skipSpace();

      double x, y;

      if (! CQChartsUtil::toReal(xstr, x))
        break;

      if (! CQChartsUtil::toReal(ystr, y))
        break;

      QPointF p(x, y);

      poly << p;
    }

    return poly.length();
  }

  bool parse(bool debug=false) {
    parseInt_ .clear();
    parseReal_.clear();
    parseStr_ .clear();
    parseBool_.clear();
    parseArgs_.clear();

    using Args      = std::vector<QVariant>;
    using GroupArgs = std::map<int,Args>;

    GroupArgs groupArgs;

    bool help = false;

    while (! eof()) {
      const Arg &arg = getArg();

      if (arg.isOpt()) {
        QString opt = arg.opt();

        if (opt == "help") { help = true; continue; }

        CQChartsCmdArg *cmdArg = getCmdOpt(opt);

        if (cmdArg) {
          if (cmdArg->groupInd() >= 0)
            groupArgs[cmdArg->groupInd()].push_back(cmdArg->name());

          //---

          if      (cmdArg->type() == CQChartsCmdArg::Type::Boolean) {
            parseBool_[opt] = true;
          }
          else if (cmdArg->type() == CQChartsCmdArg::Type::Integer) {
            int i = 0;

            if (getOptValue(i)) {
              parseInt_[opt] = i;
            }
            else {
              std::cerr << "Missing value for '-" << opt.toStdString() << "'\n";
              continue;
            }
          }
          else if (cmdArg->type() == CQChartsCmdArg::Type::Real) {
            double r = 0.0;

            if (getOptValue(r)) {
              parseReal_[opt] = r;
            }
            else {
              std::cerr << "Missing value for '-" << opt.toStdString() << "'\n";
              continue;
            }
          }
          else if (cmdArg->type() == CQChartsCmdArg::Type::String) {
            QString str;

            if (getOptValue(str)) {
              parseStr_[opt].push_back(str);
            }
            else {
              std::cerr << "Missing value for '-" << opt.toStdString() << "'\n";
              continue;
            }
          }
          else if (cmdArg->type() == CQChartsCmdArg::Type::SBool) {
            bool b;

            if (getOptValue(b)) {
              parseBool_[opt] = b;
            }
            else {
              std::cerr << "Missing value for '-" << opt.toStdString() << "'\n";
              continue;
            }
          }
          else if (cmdArg->type() == CQChartsCmdArg::Type::Color) {
            QString str;

            if (getOptValue(str)) {
              parseStr_[opt].push_back(str);
            }
            else {
              std::cerr << "Missing value for '-" << opt.toStdString() << "'\n";
              continue;
            }
          }
          else if (cmdArg->type() == CQChartsCmdArg::Type::LineDash) {
            QString str;

            if (getOptValue(str)) {
              parseStr_[opt].push_back(str);
            }
            else {
              std::cerr << "Missing value for '-" << opt.toStdString() << "'\n";
              continue;
            }
          }
          else if (cmdArg->type() == CQChartsCmdArg::Type::Length) {
            QString str;

            if (getOptValue(str)) {
              parseStr_[opt].push_back(str);
            }
            else {
              std::cerr << "Missing value for '-" << opt.toStdString() << "'\n";
              continue;
            }
          }
          else if (cmdArg->type() == CQChartsCmdArg::Type::Position) {
            QString str;

            if (getOptValue(str)) {
              parseStr_[opt].push_back(str);
            }
            else {
              std::cerr << "Missing value for '-" << opt.toStdString() << "'\n";
              continue;
            }
          }
          else if (cmdArg->type() == CQChartsCmdArg::Type::Polygon) {
            QString str;

            if (getOptValue(str)) {
              parseStr_[opt].push_back(str);
            }
            else {
              std::cerr << "Missing value for '-" << opt.toStdString() << "'\n";
              continue;
            }
          }
          else if (cmdArg->type() == CQChartsCmdArg::Type::Align) {
            QString str;

            if (getOptValue(str)) {
              parseStr_[opt].push_back(str);
            }
            else {
              std::cerr << "Missing value for '-" << opt.toStdString() << "'\n";
              continue;
            }
          }
          else {
            std::cerr << "Invalid type for '" << opt.toStdString() << "'\n";
            continue;
          }
        }
        else {
          this->error();
        }
      }
      else {
        parseArgs_.push_back(arg.var());
      }
    }

    //---

    if (help) {
      this->help();

      if (! debug)
        return false;
    }

    //---

    for (const auto &cmdGroup : cmdGroups_) {
      int groupInd = cmdGroup.ind();

      auto p = groupArgs.find(groupInd);

      if (p == groupArgs.end()) {
        if (cmdGroup.isRequired()) {
          std::string names = getGroupCmdNames(groupInd).join(", ").toStdString();
          std::cerr << "One of " << names << " required\n";
          return false;
        }
      }
      else {
        const Args &args = (*p).second;

        if (args.size() > 1) {
          std::string names = getGroupCmdNames(groupInd).join(", ").toStdString();
          std::cerr << "Only one of " << names << " allowed\n";
          return false;
        }
      }
    }

    //---

    if (debug) {
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
        std::cerr << a.toString().toStdString() << "\n";
      }
    }

    return true;
  }

  bool hasParseArg(const QString &name) const {
    if      (parseInt_ .find(name) != parseInt_ .end()) return true;
    else if (parseReal_.find(name) != parseReal_.end()) return true;
    else if (parseStr_ .find(name) != parseStr_ .end()) return true;
    else if (parseBool_.find(name) != parseBool_.end()) return true;

    return false;
  }

  int getParseInt(const QString &name, int def=0) const {
    auto p = parseInt_.find(name);
    if (p == parseInt_.end()) return def;

    return (*p).second;
  }

  OptInt getParseOptInt(const QString &name) const {
    auto p = parseInt_.find(name);
    if (p == parseInt_.end()) return OptInt();

    return OptInt((*p).second);
  }

  double getParseReal(const QString &name, double def=0.0) const {
    auto p = parseReal_.find(name);
    if (p == parseReal_.end()) return def;

    return (*p).second;
  }

  OptReal getParseOptReal(const QString &name) const {
    auto p = parseReal_.find(name);
    if (p == parseReal_.end()) return OptReal();

    return OptReal((*p).second);
  }

  int getNumParseStrs(const QString &name) const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return 0;

    return (*p).second.size();
  }

  QStringList getParseStrs(const QString &name) const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return QStringList();

    return (*p).second;
  }

  QString getParseStr(const QString &name, const QString &def="") const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return def;

    return (*p).second[0];
  }

  bool getParseBool(const QString &name, bool def=false) const {
    auto p = parseBool_.find(name);
    if (p == parseBool_.end()) return def;

    return (*p).second;
  }

  OptBool getParseOptBool(const QString &name) const {
    auto p = parseBool_.find(name);
    if (p == parseBool_.end()) return OptBool();

    return OptBool((*p).second);
  }

  QFont getParseFont(const QString &name, const QFont &def=QFont()) const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return def;

    return QFont((*p).second[0]);
  }

  CQChartsColor
  getParseColor(const QString &name, const CQChartsColor &def=CQChartsColor()) const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return def;

    return CQChartsColor((*p).second[0]);
  }

  CQChartsLineDash
  getParseLineDash(const QString &name, const CQChartsLineDash &def=CQChartsLineDash()) const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return def;

    return CQChartsLineDash((*p).second[0]);
  }

  CQChartsLength
  getParseLength(const QString &name, const CQChartsLength &def=CQChartsLength()) const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return def;

    return CQChartsLength((*p).second[0]);
  }

  QPolygonF getParsePoly(const QString &name, const QPolygonF &def=QPolygonF()) const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return def;

    QPolygonF poly;

    CQStrParse parse((*p).second[0]);

    while (! parse.eof()) {
      parse.skipSpace();

      QString xstr;

      if (! parse.readNonSpace(xstr))
        break;

      parse.skipSpace();

      QString ystr;

      if (! parse.readNonSpace(ystr))
        break;

      parse.skipSpace();

      double x, y;

      if (! CQChartsUtil::toReal(xstr, x))
        break;

      if (! CQChartsUtil::toReal(ystr, y))
        break;

      QPointF p(x, y);

      poly << p;
    }

    return poly;
  }

  Qt::Alignment getParseAlign(const QString &name, Qt::Alignment def=Qt::AlignCenter) const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return def;

    return CQAlignEdit::fromString((*p).second[0]);
  }

  const Args &getParseArgs() const { return parseArgs_; }

  CQChartsCmdArg *getCmdOpt(const QString &name) {
    for (auto &cmdArg : cmdArgs_) {
      if (cmdArg.isOpt() && cmdArg.name() == name)
        return &cmdArg;
    }

    return nullptr;
  }

  void error() {
    if (lastArg_.isOpt())
      errorMsg("Invalid option '" + lastArg_.opt() + "'");
    else
      errorMsg("Invalid arg '" + lastArg_.var().toString() + "'");
  }

  void help() const {
    using GroupIds = std::set<int>;

    GroupIds groupInds;

    std::cerr << cmdName_.toStdString() << "\n";

    for (auto &cmdArg : cmdArgs_) {
      int groupInd = cmdArg.groupInd();

      if (groupInd > 0) {
        auto p = groupInds.find(groupInd);

        if (p == groupInds.end()) {
          std::cerr << "  ";

          helpGroup(groupInd);

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

  void helpGroup(int groupInd) const {
    const CQChartsCmdGroup &cmdGroup = cmdGroups_[groupInd - 1];

    if (! cmdGroup.isRequired())
      std::cerr << "[";

    CmdArgs cmdArgs;

    getGroupCmds(groupInd, cmdArgs);

    int i = 0;

    for (const auto &cmdArg : cmdArgs) {
      if (i > 0)
        std::cerr << "|";

      helpArg(cmdArg);

      ++i;
    }

    if (! cmdGroup.isRequired())
      std::cerr << "]";
  }

  void helpArg(const CQChartsCmdArg &cmdArg) const {
    if (cmdArg.isOpt()) {
      std::cerr << "-" << cmdArg.name().toStdString();

      if (cmdArg.type() != CQChartsCmdArg::Type::Boolean)
        std::cerr << " <" << cmdArg.argDesc().toStdString() << ">";
    }
    else {
      std::cerr << "  <" << cmdArg.argDesc().toStdString() << ">";
    }
  }

 private:
  using CmdArgs     = std::vector<CQChartsCmdArg>;
  using CmdGroups   = std::vector<CQChartsCmdGroup>;
  using NameInt     = std::map<QString,int>;
  using NameReal    = std::map<QString,double>;
  using NameString  = std::map<QString,QString>;
  using NameStrings = std::map<QString,QStringList>;
  using NameBool    = std::map<QString,bool>;

 private:
  QStringList getGroupCmdNames(int groupInd) const {
    CmdArgs cmdArgs;

    getGroupCmds(groupInd, cmdArgs);

    QStringList names;

    for (const auto &cmdArg : cmdArgs)
      names << cmdArg.name();

    return names;
  }

  void getGroupCmds(int groupInd, CmdArgs &cmdArgs) const {
    for (auto &cmdArg : cmdArgs_) {
      int groupInd1 = cmdArg.groupInd();

      if (groupInd1 != groupInd)
        continue;

      cmdArgs.push_back(cmdArg);
    }
  }

 private:
  void errorMsg(const QString &msg) {
    std::cerr << msg.toStdString() << "\n";
  }

 private:
  QString     cmdName_;
  Args        argv_;
  int         i_    { 0 };
  int         argc_ { 0 };
  Arg         lastArg_;
  CmdArgs     cmdArgs_;
  CmdGroups   cmdGroups_;
  int         groupInd_ { -1 };
  NameInt     parseInt_;
  NameReal    parseReal_;
  NameStrings parseStr_;
  NameBool    parseBool_;
  Args        parseArgs_;
};

#endif
