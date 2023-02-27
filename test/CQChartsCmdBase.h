#ifndef CQChartsCmdBase_H
#define CQChartsCmdBase_H

#include <CQChartsCmdArgs.h>
#include <QString>
#include <QVariant>
#include <vector>

class CQTcl;

class CQChartsCmdProc;

/*!
 * \brief Command Base Class
 * \ingroup Charts
 */
class CQChartsCmdBase : public CQTclCmd::Mgr {
 public:
  using CmdProc = CQChartsCmdProc;
  using Args    = std::vector<QString>;

 public:
  CQChartsCmdBase(CQCharts *charts);
 ~CQChartsCmdBase();

  //---

  CQCharts *charts() const { return charts_; } \

  //---

  void addCommands();

  //---

  void addCommand(const QString &name, CmdProc *proc);

  CmdProc *getCommand(const QString &cmd) const;

  CQTclCmd::Cmd *createCmd(const QString &name) override;

  CQTclCmd::CmdArgs *createArgs(const QString &name, const Vars &vars) override;

  //---

  void setFilename(const QString &filename);

  //---

  static bool isCompleteLine(QString &line, bool &join);

  void parseLine(const QString &line, bool log=true);

  void parseScriptLine(const QString &line);

  //---

 public:
  QStringList stringToCmds(const QString &str) const;

  QStringList stringToCmdData(const QString &str) const;

  //---

  bool setCmdRc(int rc);
  bool setCmdRc(double rc);
  bool setCmdRc(const std::string &rc);
  bool setCmdRc(const QString &rc);
  bool setCmdRc(const QVariant &rc);
  bool setCmdRc(const QStringList &rc);
  bool setCmdRc(const QVariantList &rc);
  bool setCmdRc(const std::vector<int> &rc);

  bool setCmdError(const QString &msg);

  //---

 public:
  bool perfCmd(CQChartsCmdArgs &args);

  bool assertCmd(CQChartsCmdArgs &args);

  bool shellCmd(CQChartsCmdArgs &args);

  bool helpCmd    (CQChartsCmdArgs &args);
  bool completeCmd(CQChartsCmdArgs &args);

  //---

 private:
  CQCharts* charts_       { nullptr };
  bool      continueFlag_ { false };
};

//---

class CQChartsTclCmd : public CQTclCmd::Cmd {
 public:
  CQChartsTclCmd(CQChartsCmdBase *cmdBase, const QString &name);

  CQChartsCmdBase *cmdBase() const { return cmdBase_; }

 private:
  CQChartsCmdBase *cmdBase_ { nullptr };
};

//---

/*!
 * \brief Command Callback Proc Base
 * \ingroup Charts
 */
class CQChartsCmdProc : public CQTclCmd::CmdProc {
 public:
  using CmdBase = CQChartsCmdBase;

 public:
  CQChartsCmdProc(CmdBase *cmdBase);

  virtual ~CQChartsCmdProc() { }

  //--

  CmdBase *cmdBase() const { return cmdBase_; }

  CQCharts *charts() const { return cmdBase_->charts(); } \

  //--

  bool exec(CQTclCmd::CmdArgs &args) override {
    return execCmd(static_cast<CQChartsCmdArgs &>(args));
  }

  void addArgs(CQTclCmd::CmdArgs &args) override {
    addCmdArgs(static_cast<CQChartsCmdArgs &>(args));
  }

  //---

  virtual bool execCmd(CQChartsCmdArgs &args) = 0;

  virtual void addCmdArgs(CQChartsCmdArgs & /*args*/) { }

 protected:
  CmdBase* cmdBase_ { nullptr };
};

//---

class CQChartsCmdBaseSlot : public QObject {
  Q_OBJECT

 public:
  CQChartsCmdBaseSlot(CQChartsCmdBase *base, const QString &procName);

  CQChartsCmdBase *base() const { return base_; }

  const QString &proc() const { return procName_; }

 public Q_SLOTS:
  // button
  void clicked();
  void clicked(bool);

  // spin box (integer/real)
  void valueChanged(int i);
  void valueChanged(double r);

  // line edit
  void textEdited(const QString &);
  void returnPressed();
  void editingFinished();

  // radio button
  void toggled(bool b);

  // check box
  void stateChanged(int i);

  // combo box
  void currentIndexChanged(const QString &s);

  // model table
  void itemSelectionChanged();
  void columnClicked(int);

  // symbol set list/symbol list
  void setNameChanged(const QString &s);
  void nameChanged(const QString &s);
  void symbolChanged();

  void timerSlot();

 private:
  void execProc(const QString &args="");

 private:
  CQChartsCmdBase *base_ { nullptr };
  QString          procName_;
};

//---

/*!
 * \brief Charts Named Basic Command
 * \ingroup Charts
 */
#define CQCHARTS_BASE_DEF_CMD(NAME) \
class CQChartsBase##NAME##Cmd : public CQChartsCmdProc { \
 public: \
  using CmdArg   = CQTclCmd::CmdArg; \
  using CmdGroup = CQTclCmd::CmdGroup; \
\
  enum class ArgType { \
    None      = int(CmdArg::Type::None), \
    Boolean   = int(CmdArg::Type::Boolean), \
    Integer   = int(CmdArg::Type::Integer), \
    Real      = int(CmdArg::Type::Real), \
    String    = int(CmdArg::Type::String), \
    SBool     = int(CmdArg::Type::SBool), \
    Enum      = int(CmdArg::Type::Enum), \
    Color     = int(CmdArg::Type::Extra) + 1, \
    Font      = int(CmdArg::Type::Extra) + 2, \
    LineDash  = int(CmdArg::Type::Extra) + 3, \
    Length    = int(CmdArg::Type::Extra) + 4, \
    Position  = int(CmdArg::Type::Extra) + 5, \
    Rect      = int(CmdArg::Type::Extra) + 6, \
    Polygon   = int(CmdArg::Type::Extra) + 7, \
    Align     = int(CmdArg::Type::Extra) + 8, \
    Sides     = int(CmdArg::Type::Extra) + 9, \
    Column    = int(CmdArg::Type::Extra) + 10, \
    Row       = int(CmdArg::Type::Extra) + 11, \
    Reals     = int(CmdArg::Type::Extra) + 12, \
    ObjRefPos = int(CmdArg::Type::Extra) + 13, \
  }; \
\
 public: \
  CQChartsBase##NAME##Cmd(CQChartsCmdBase *cmdBase) : \
   CQChartsCmdProc(cmdBase) { } \
\
  bool execCmd(CQChartsCmdArgs &args) override; \
\
  void addCmdArgs(CQChartsCmdArgs &args) override; \
\
  QStringList getArgValues(const QString &arg, \
                           const NameValueMap &nameValueMap=NameValueMap()) override; \
\
  CmdArg &addArg(CQChartsCmdArgs &args, const QString &name, ArgType type, \
                 const QString &argDesc="", const QString &desc="") { \
    return args.addCmdArg(name, int(type), argDesc, desc); \
  } \
};

//---

// widgets
CQCHARTS_BASE_DEF_CMD(CreateWidget  )
CQCHARTS_BASE_DEF_CMD(CreateLayout  )
CQCHARTS_BASE_DEF_CMD(AddChildWidget)
CQCHARTS_BASE_DEF_CMD(AddStretch    )

// signals, slots
CQCHARTS_BASE_DEF_CMD(ConnectWidget)
CQCHARTS_BASE_DEF_CMD(ActivateSlot )

// qt properties
CQCHARTS_BASE_DEF_CMD(GetProperty)
CQCHARTS_BASE_DEF_CMD(SetProperty)
CQCHARTS_BASE_DEF_CMD(HasProperty)

// misc
CQCHARTS_BASE_DEF_CMD(QtSync)
CQCHARTS_BASE_DEF_CMD(Timer)

CQCHARTS_BASE_DEF_CMD(WidgetTest)

CQCHARTS_BASE_DEF_CMD(Perf)

CQCHARTS_BASE_DEF_CMD(Assert)

CQCHARTS_BASE_DEF_CMD(Shell)

CQCHARTS_BASE_DEF_CMD(Exit)

CQCHARTS_BASE_DEF_CMD(Help)
CQCHARTS_BASE_DEF_CMD(Complete)

#endif
