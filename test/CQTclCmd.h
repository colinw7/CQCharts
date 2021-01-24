#ifndef CQTclCmd_H
#define CQTclCmd_H

//#include <CQTclUtil.h>

#include <QString>
#include <QVariant>

#include <tcl.h>

#include <boost/optional.hpp>

#include <vector>
#include <set>
#include <map>
#include <iostream>

class CQTcl;

namespace CQTclCmd {

class CmdProc;
class Cmd;
class CmdArgs;

/*!
 * \brief Tcl Command Manager
 */
class Mgr {
 public:
  using Vars = std::vector<QVariant>;

 public:
  Mgr(CQTcl *qtcl);

  virtual ~Mgr();

  CQTcl *qtcl() const { return qtcl_; }

  //---

  void addCommand(const QString &name, CmdProc *proc);

  bool processCmd(const QString &name, const Vars &vars);

  CmdProc *getCommand(const QString &name) const;

  virtual Cmd *createCmd(const QString &name);

  virtual CmdArgs *createArgs(const QString &name, const Vars &vars);

  //---

  bool help(const QString &pattern, bool verbose, bool hidden);

  void helpAll(bool verbose, bool hidden);

 protected:
  using CommandNames = std::vector<QString>;
  using CommandProcs = std::map<QString, CmdProc *>;

  CQTcl*       qtcl_ { nullptr };
  CommandNames commandNames_;
  CommandProcs commandProcs_;
};

/*!
 * \brief Tcl Command Argument
 */
class CmdArg {
 public:
  //! types
  enum class Type {
    None,
    Boolean,
    Integer,
    Real,
    String,
    SBool,
    Enum,
    Extra
  };

  using NameValue  = std::pair<QString, int>;
  using NameValues = std::vector<NameValue>;

 public:
  CmdArg(int ind, const QString &name, int type, const QString &argDesc="",
         const QString &desc="");

  virtual ~CmdArg() { }

  int ind() const { return ind_; }

  const QString &name() const { return name_; }

  bool isOpt() const { return isOpt_; }

  int type() const { return type_; }

  const QString &argDesc() const { return argDesc_; }

  const QString &desc() const { return desc_; }

  bool isRequired() const { return required_; }
  CmdArg &setRequired(bool b=true) { required_ = b; return *this; }

  bool isHidden() const { return hidden_; }
  CmdArg &setHidden(bool b=true) { hidden_ = b; return *this; }

  bool isMultiple() const { return multiple_; }
  CmdArg &setMultiple(bool b=true) { multiple_ = b; return *this; }

  int groupInd() const { return groupInd_; }
  void setGroupInd(int i) { groupInd_ = i; }

  CmdArg &addNameValue(const QString &name, int value);

  const NameValues &nameValues() const { return nameValues_; }

 private:
  int        ind_      { -1 };              //!< command ind
  QString    name_;                         //!< arg name
  bool       isOpt_    { false };           //!< is option
  int        type_     { int(Type::None) }; //!< value type
  QString    argDesc_;                      //!< short description
  QString    desc_;                         //!< long description
  bool       required_ { false };           //!< is required
  bool       hidden_   { false };           //!< is hidden
  bool       multiple_ { false };           //!< can have multiple values
  int        groupInd_ { -1 };              //!< cmd group ind
  NameValues nameValues_;                   //!< enum name values
};

//---

class CmdGroup {
 public:
  enum class Type {
    None,
    OneOpt,
    OneReq
  };

 public:
  CmdGroup(int ind, Type type);

  virtual ~CmdGroup() { }

  int ind() const { return ind_; }

  bool isRequired() const { return (type_ == Type::OneReq); }

 private:
  int  ind_  { -1 };
  Type type_ { Type::None };
};

//---

/*!
 * \brief base class for handling command arguments
 */
class CmdArgs {
 public:
  using Args    = std::vector<QVariant>;
  using OptReal = boost::optional<double>;
  using OptInt  = boost::optional<int>;
  using OptBool = boost::optional<bool>;

  /*!
   * \brief command argument
   */
  class Arg {
   public:
    Arg(const QVariant &var=QVariant());

  //QString  str() const { assert(! isOpt_); return toString(var_); }
    QVariant var() const { assert(! isOpt_); return var_; }

    bool isOpt() const { return isOpt_; }
    void setIsOpt(bool b) { isOpt_ = b; }

    QString opt() const { assert(isOpt_); return toString(var_).mid(1); }

   private:
    QVariant var_;             //!< arg value
    bool     isOpt_ { false }; //!< is option
  };

  //---

 public:
  CmdArgs(const QString &cmdName, const Args &argv);

  virtual ~CmdArgs() { }

  //---

  // start command group
  CmdGroup &startCmdGroup(CmdGroup::Type type);

  // end command group
  void endCmdGroup();

  //---

  // add argument
  CmdArg &addCmdArg(const QString &name, int type,
                    const QString &argDesc="", const QString &desc="");

  //---

  // has processed last argument
  bool eof() const;

  // move to first argument
  void rewind();

  // get next arg
  const Arg &getArg();

  //---

  // get string or string list value of current option
  bool getOptValue(QStringList &strs);

  // get string value of current option
  bool getOptValue(QString &str);

  // get integer value of current option
  bool getOptValue(int &i);

  // get real value of current option
  bool getOptValue(double &r);

  // get optional real value of current option
  bool getOptValue(OptReal &r);

  // get optional boolean value of current option
  bool getOptValue(bool &b);

  // get generic value of current option
  template<typename T>
  bool getOptValue(T &t) {
    QString str;

    if (! getOptValue(str))
      return false;

    t = T(str);

    return true;
  }

  //---

  // get/set debug
  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  //---

  // parse command arguments
  bool parse() { bool rc; return parse(rc); }

  bool parse(bool &rc);

  virtual bool handleParseArg(CmdArg *cmdArg, const QString &opt);

  //---

  // check if option found by parse
  bool hasParseArg(const QString &name) const;

  //---

  // get parsed int for option (default returned if not found)
  int getParseInt(const QString &name, int def=0) const;

  // get parsed optional int for option
  OptInt getParseOptInt(const QString &name) const;

  // get parsed real for option (default returned if not found)
  double getParseReal(const QString &name, double def=0.0) const;

  // get parsed optional real for option
  OptReal getParseOptReal(const QString &name) const;

  // get number of parsed strings for option
  int getNumParseStrs(const QString &name) const;

  // get parsed strings for option
  QStringList getParseStrs(const QString &name) const;

  // get parsed string for option (if multiple return first) (default returned if not found)
  QString getParseStr(const QString &name, const QString &def="") const;

  // get parsed boolean for option (default returned if not found)
  bool getParseBool(const QString &name, bool def=false) const;

  // get parsed optional boolean for option
  OptBool getParseOptBool(const QString &name) const;

  //---

  // get parsed generic value for option
  template<typename T>
  T getParseValue(const QString &name, const T &def=T()) const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return def;

    return T((*p).second[0]);
  }

  //---

  // get parsed args (non options)
  const Args &getParseArgs() const { return parseArgs_; }

  //---

  // get arg data for option
  CmdArg *getCmdOpt(const QString &name);

  //---

  QStringList getCmdArgNames() const;

  //---

  // handle missing value error
  bool valueError(const QString &opt);

  // handle invalid option/arg error
  bool error();

  //---

  // display help
  void help(bool showHidden=false) const;

  // display help for group
  void helpGroup(int groupInd, bool showHidden) const;

  // display help for arg
  void helpArg(const CmdArg &cmdArg) const;

  //---

  // variant to string list
  static QStringList toStringList(const QVariant &var);

  // variant to string
  static QString toString(const QVariant &var);

  // string to bool
  static bool stringToBool(const QString &str, bool *ok);

 protected:
  using CmdArgArray = std::vector<CmdArg>;
  using CmdGroups   = std::vector<CmdGroup>;
  using NameInt     = std::map<QString, int>;
  using NameReal    = std::map<QString, double>;
  using NameString  = std::map<QString, QString>;
  using NameStrings = std::map<QString, QStringList>;
  using NameBool    = std::map<QString, bool>;

 protected:
  // get option names for group
  QStringList getGroupArgNames(int groupInd) const;

  // get option datas for group
  void getGroupCmdArgs(int groupInd, CmdArgArray &cmdArgs) const;

 protected:
  // display error message
  void errorMsg(const QString &msg);

 protected:
  QString     cmdName_;             //! command name being processed
  bool        debug_     { false }; //! is debug
  Args        argv_;                //! input args
  int         i_         { 0 };     //! current arg
  int         argc_      { 0 };     //! number of args
  Arg         lastArg_;             //! last processed arg
  CmdArgArray cmdArgs_;             //! command argument data
  CmdGroups   cmdGroups_;           //! command argument groups
  int         groupInd_  { -1 };    //! current group index
  NameInt     parseInt_;            //! parsed option integers
  NameReal    parseReal_;           //! parsed option reals
  NameStrings parseStr_;            //! parsed option strings
  NameBool    parseBool_;           //! parsed option booleans
  Args        parseArgs_;           //! parsed arguments
};

//---

class Cmd {
 public:
  using Vars = std::vector<QVariant>;

 public:
  Cmd(Mgr *mgr, const QString &name);

  virtual ~Cmd() { }

  virtual int exec(int objc, const Tcl_Obj **objv);

  static int commandProc(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);

 private:
  Mgr*        mgr_   { nullptr };
  QString     name_;
  Tcl_Command cmdId_ { nullptr };
};

//---

/*!
 * \brief Command Callback Proc Base
 */
class CmdProc {
 public:
  using Vars         = std::vector<QVariant>;
  using NameValueMap = std::map<QString, QString>;

 public:
  CmdProc(Mgr *mgr);

  virtual ~CmdProc() { }

  Mgr *mgr() const { return mgr_; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  Cmd *cmd() const { return cmd_; }
  void setCmd(Cmd *cmd) { cmd_ = cmd; }

  virtual bool exec(CmdArgs &args) = 0;

  virtual void addArgs(CmdArgs & /*args*/) { }

  virtual QStringList getArgValues(const QString& /*arg*/, const NameValueMap& /*nameValueMap*/) {
    return QStringList();
  }

 protected:
  Mgr*    mgr_ { nullptr };
  QString name_;
  Cmd*    cmd_ { nullptr };
};

//---

}

//------

#define CQTCL_CMD_DEF_TCL_CMD(NAME) \
class CQTclCmd##NAME##Cmd : public CQTclCmd::CmdProc { \
 public: \
  CQTclCmd##NAME##Cmd(CQTclCmd::Mgr *mgr) : CmdProc(mgr) { } \
\
  bool exec(CQTclCmd::CmdArgs &args) override; \
\
  void addArgs(CQTclCmd::CmdArgs &args) override; \
\
  QStringList getArgValues(const QString &arg, \
                           const NameValueMap &nameValueMap=NameValueMap()) override; \
};

#define CQTCL_CMD_DEF_INST_TCL_CMD(NAME) \
class CQTclCmd##NAME##InstCmd : public CQTclCmd::CmdProc { \
 public: \
  CQTclCmd##NAME##InstCmd(CQTclCmd::Mgr *mgr, const QString &id); \
 \
  bool exec(CQTclCmd::CmdArgs &args) override; \
 \
  void addArgs(CQTclCmd::CmdArgs &args) override; \
 \
  QStringList getArgValues(const QString &arg, \
                           const NameValueMap &nameValueMap=NameValueMap()) override; \
 \
 private: \
  QString id_; \
};

#endif
