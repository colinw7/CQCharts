#ifndef CQChartsCmdBase_H
#define CQChartsCmdBase_H

#include <CQChartsCmdArgs.h>
#include <QString>
#include <QVariant>
#include <vector>

class CQChartsCmdProc;
class CQTcl;

/*!
 * \brief Command Base Class
 * \ingroup Charts
 */
class CQChartsCmdBase {
 public:
  using Args = std::vector<QString>;
  using Vars = std::vector<QVariant>;

 public:
  CQChartsCmdBase(CQCharts *charts);
 ~CQChartsCmdBase();

  void addCommands();

  void addCommand(const QString &name, CQChartsCmdProc *proc);

  bool processCmd(const QString &cmd, const Vars &vars);

  //---

  static bool isCompleteLine(QString &line, bool &join);

  void parseLine(const QString &line, bool log=true);

  void parseScriptLine(const QString &line);

  //---

  CQTcl *qtcl() const { return qtcl_; }

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

  bool setCmdError(const QString &msg);

  //---

 public:
  bool qtCreateWidgetCmd  (CQChartsCmdArgs &args);
  bool qtCreateLayoutCmd  (CQChartsCmdArgs &args);
  bool qtAddChildWidgetCmd(CQChartsCmdArgs &args);
  bool qtAddStretchCmd    (CQChartsCmdArgs &args);
  bool qtConnectWidgetCmd (CQChartsCmdArgs &args);
  bool qtActivateSlotCmd  (CQChartsCmdArgs &args);
  bool qtGetPropertyCmd   (CQChartsCmdArgs &args);
  bool qtSetPropertyCmd   (CQChartsCmdArgs &args);
  bool qtHasPropertyCmd   (CQChartsCmdArgs &args);
  bool qtSyncCmd          (CQChartsCmdArgs &args);

  bool perfCmd(CQChartsCmdArgs &args);

  bool assertCmd(CQChartsCmdArgs &args);

  bool shellCmd(CQChartsCmdArgs &args);

  bool helpCmd    (CQChartsCmdArgs &args);
  bool completeCmd(CQChartsCmdArgs &args);

  //---

 private:
  using CommandNames = std::vector<QString>;
  using CommandProcs = std::map<QString, CQChartsCmdProc *>;

  CQCharts*    charts_       { nullptr };
  bool         continueFlag_ { false };
  CQTcl*       qtcl_         { nullptr };
  CommandNames commandNames_;
  CommandProcs commandProcs_;
};

//---

class CQChartsTclCmd;

/*!
 * \brief Command Callback Proc Base
 * \ingroup Charts
 */
class CQChartsCmdProc {
 public:
  using Vars         = std::vector<QVariant>;
  using NameValueMap = std::map<QString, QString>;

 public:
  CQChartsCmdProc(CQChartsCmdBase *cmdBase) :
   cmdBase_(cmdBase) {
  }

  virtual ~CQChartsCmdProc() { }

  const QString &name() const { return name_; }
  void setName(const QString &v) { name_ = v; }

  CQChartsTclCmd *tclCmd() const { return tclCmd_; }
  void setTclCmd(CQChartsTclCmd *tclCmd) { tclCmd_ = tclCmd; }

  virtual bool exec(CQChartsCmdArgs &args) = 0;

  virtual void addArgs(CQChartsCmdArgs & /*args*/) { }

  virtual QStringList getArgValues(const QString& /*arg*/, const NameValueMap& /*nameValueMap*/) {
    return QStringList();
  }

 protected:
  CQChartsCmdBase* cmdBase_ { nullptr };
  QString          name_;
  CQChartsTclCmd*  tclCmd_  { nullptr };
};

//---

class CQChartsCmdBaseSlot : public QObject {
  Q_OBJECT

 public:
  CQChartsCmdBaseSlot(CQChartsCmdBase *base, const QString &procName);

  CQChartsCmdBase *base() const { return base_; }

  const QString &proc() const { return procName_; }

 public slots:
  void clicked();
  void clicked(bool);

 private:
  void execProc();

 private:
  CQChartsCmdBase *base_ { nullptr };
  QString          procName_;
};

//---

/*!
 * \brief Charts Named Basic Command
 * \ingroup Charts
 */
#define CQCHARTS_BASE_DEF_CMD(NAME, PROC) \
class CQChartsBase##NAME##Cmd : public CQChartsCmdProc { \
 public: \
  CQChartsBase##NAME##Cmd(CQChartsCmdBase *cmdBase) : CQChartsCmdProc(cmdBase) { } \
 \
  bool exec(CQChartsCmdArgs &args) override { return cmdBase_->PROC(args); } \
};

//---

CQCHARTS_BASE_DEF_CMD(CreateWidget  , qtCreateWidgetCmd  )
CQCHARTS_BASE_DEF_CMD(CreateLayout  , qtCreateLayoutCmd  )
CQCHARTS_BASE_DEF_CMD(AddChildWidget, qtAddChildWidgetCmd)
CQCHARTS_BASE_DEF_CMD(AddStretch    , qtAddStretchCmd    )
CQCHARTS_BASE_DEF_CMD(ConnectWidget , qtConnectWidgetCmd )
CQCHARTS_BASE_DEF_CMD(ActivateSlot  , qtActivateSlotCmd  )
CQCHARTS_BASE_DEF_CMD(GetProperty   , qtGetPropertyCmd   )
CQCHARTS_BASE_DEF_CMD(SetProperty   , qtSetPropertyCmd   )
CQCHARTS_BASE_DEF_CMD(HasProperty   , qtHasPropertyCmd   )
CQCHARTS_BASE_DEF_CMD(QtSync        , qtSyncCmd          )

CQCHARTS_BASE_DEF_CMD(Perf, perfCmd)

CQCHARTS_BASE_DEF_CMD(Assert, assertCmd)

CQCHARTS_BASE_DEF_CMD(Shell, shellCmd)

CQCHARTS_BASE_DEF_CMD(Help    , helpCmd)
CQCHARTS_BASE_DEF_CMD(Complete, completeCmd)

#endif
