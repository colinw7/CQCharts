#ifndef CQChartsCmdBase_H
#define CQChartsCmdBase_H

#include <CQChartsCmdArgs.h>
#include <QString>
#include <QVariant>
#include <vector>

class CQChartsCmdProc;
class CQTcl;

class CQChartsCmdBase {
 public:
  using Args = std::vector<QString>;
  using Vars = std::vector<QVariant>;

 public:
  CQChartsCmdBase();
 ~CQChartsCmdBase();

  void addCommands();

  void addCommand(const QString &name, CQChartsCmdProc *proc);

  bool processCmd(const QString &cmd, const Vars &vars);

  //---

  static bool isCompleteLine(QString &line, bool &join);

  void parseLine(const QString &line, bool log=true);

  void parseScriptLine(const QString &line);

  //---

#ifdef CQCharts_USE_TCL
  CQTcl *qtcl() const { return qtcl_; }
#endif

 public:
  QStringList stringToCmds(const QString &str) const;

  QStringList stringToCmdData(const QString &str) const;

  //---

  void setCmdRc(int rc);
  void setCmdRc(double rc);
  void setCmdRc(const QString &rc);
  void setCmdRc(const QVariant &rc);
  void setCmdRc(const QStringList &rc);
  void setCmdRc(const QVariantList &rc);

  void setCmdError(const QString &msg);

  //---

 public:
  bool qtGetPropertyCmd(CQChartsCmdArgs &args);
  bool qtSetPropertyCmd(CQChartsCmdArgs &args);
  bool qtSyncCmd       (CQChartsCmdArgs &args);

  bool perfCmd(CQChartsCmdArgs &args);

  bool shellCmd(CQChartsCmdArgs &args);

  bool helpCmd(CQChartsCmdArgs &args);

  //---

 private:
  using CommandNames = std::vector<QString>;
  using CommandProcs = std::map<QString,CQChartsCmdProc *>;

  bool         continueFlag_ { false };
#ifdef CQCharts_USE_TCL
  CQTcl*       qtcl_         { nullptr };
#endif
  CommandNames commandNames_;
  CommandProcs commandProcs_;
};

//---

class CQChartsTclCmd;

class CQChartsCmdProc {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQChartsCmdProc(CQChartsCmdBase *cmdBase) :
   cmdBase_(cmdBase) {
  }

  virtual ~CQChartsCmdProc() { }

  const QString &name() const { return name_; }
  void setName(const QString &v) { name_ = v; }

#ifdef CQCharts_USE_TCL
  CQChartsTclCmd *tclCmd() const { return tclCmd_; }
  void setTclCmd(CQChartsTclCmd *tclCmd) { tclCmd_ = tclCmd; }
#endif

  virtual bool exec(CQChartsCmdArgs &args) = 0;

 protected:
  CQChartsCmdBase* cmdBase_   { nullptr };
  QString          name_;
#ifdef CQCharts_USE_TCL
  CQChartsTclCmd*  tclCmd_ { nullptr };
#endif
};

//---

#define CQCHARTS_BASE_DEF_CMD(NAME, PROC) \
class CQChartsBase##NAME##Cmd : public CQChartsCmdProc { \
 public: \
  CQChartsBase##NAME##Cmd(CQChartsCmdBase *cmdBase) : CQChartsCmdProc(cmdBase) { } \
 \
  bool exec(CQChartsCmdArgs &args) override { return cmdBase_->PROC(args); } \
};

//---

CQCHARTS_BASE_DEF_CMD(GetProperty, qtGetPropertyCmd)
CQCHARTS_BASE_DEF_CMD(SetProperty, qtSetPropertyCmd)
CQCHARTS_BASE_DEF_CMD(QtSync     , qtSyncCmd)

CQCHARTS_BASE_DEF_CMD(Perf, perfCmd)

CQCHARTS_BASE_DEF_CMD(Shell, shellCmd)

CQCHARTS_BASE_DEF_CMD(Help, helpCmd)

#endif
