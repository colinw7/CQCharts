#ifndef CQChartsCmdLine_H
#define CQChartsCmdLine_H

#include <QFrame>

class CQChartsCmdLine;
class CQCharts;

namespace CQCommand {

class ScrollArea;

}

class CQTcl;

#define CQChartsCmdLineDlgMgrInst CQChartsCmdLineDlgMgr::instance()

/*!
 * \brief CmdLine Dialog Manager singleton
 * \ingroup Charts
 */
class CQChartsCmdLineDlgMgr {
 public:
  static CQChartsCmdLineDlgMgr *instance() {
    static CQChartsCmdLineDlgMgr *inst;

    if (! inst)
      inst = new CQChartsCmdLineDlgMgr;

    return inst;
  }

 ~CQChartsCmdLineDlgMgr() = default;

  void showDialog(CQCharts *charts);

 private:
  CQChartsCmdLineDlgMgr();

 private:
  CQChartsCmdLine* dlg_ { nullptr };
};

//---

class CQChartsCmdWidget;

class CQChartsCmdLine : public QFrame {
  Q_OBJECT

 public:
  using OptionValues = std::map<std::string, std::string>;

 public:
  CQChartsCmdLine(CQCharts *charts, QWidget *parent=nullptr);
 ~CQChartsCmdLine();

  CQCharts *charts() const { return charts_; }

  bool complete(CQChartsCmdWidget *widget, const QString &text, int pos,
                QString &newText, bool interactive) const;

  bool completeCommand(CQChartsCmdWidget *widget, const QString &lhs, const QString &str,
                       const QString &rhs, bool interactive, QString &command) const;

  bool completeOption(CQChartsCmdWidget *widget, const QString &command, const QString &lhs,
                      const QString &option, const QString &rhs, bool interactive,
                      QString &newText) const;

  bool completeArg(CQChartsCmdWidget *widget, const QString &cmdName, const QString &option,
                   const OptionValues &optionValues, const QString &lhs, const QString &arg,
                   const QString &rhs, bool interactive, QString &newText) const;

  QStringList resultToStrings(const QVariant &var) const;

  QSize sizeHint() const override;

 private Q_SLOTS:
  void executeCommand(const QString &);

 private:
  CQCharts*              charts_  { nullptr }; //!< charts
  CQCommand::ScrollArea* command_ { nullptr }; //!< command widget
  CQTcl*                 qtcl_    { nullptr }; //!< qtcl interface
};

#endif
