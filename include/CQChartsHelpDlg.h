#ifndef CQChartsHelpDlg_H
#define CQChartsHelpDlg_H

#include <QDialog>

class CQChartsHelpDlg;
class CQCharts;

class QTreeWidget;
class QTreeWidgetItem;
class QTextBrowser;

#define CQChartsHelpDlgMgrInst CQChartsHelpDlgMgr::instance()

/*!
 * \brief Help Dialog Manager singleton
 */
class CQChartsHelpDlgMgr {
 public:
  using TclCommands = std::vector<QString>;

 public:
  static CQChartsHelpDlgMgr *instance() {
    static CQChartsHelpDlgMgr *inst;

    if (! inst)
      inst = new CQChartsHelpDlgMgr;

    return inst;
  }

 ~CQChartsHelpDlgMgr() { }

  void showDialog(CQCharts *charts);

  const TclCommands &tclCommands() const { return tclCommands_; }

  void addTclCommand(const QString &cmd) {
    tclCommands_.push_back(cmd);
  }

 private:
  CQChartsHelpDlgMgr();

 private:
  CQChartsHelpDlg* dlg_ { nullptr };
  TclCommands      tclCommands_;
};

//---

/*!
 * \brief Help Dialog
 */
class CQChartsHelpDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsHelpDlg(CQCharts *charts, QWidget *parent=nullptr);

  CQCharts *charts() const { return charts_; }

  QSize sizeHint() const override;

 private:
  void addItems();

  void setHtml(const QString &text);

 private slots:
  void prevSlot();
  void nextSlot();

  void treeItemSlot();

  void treeAnchorSlot(const QUrl &url);

  void selectTreeItem(const QString &path) const;

  QTreeWidgetItem *getTreeItem(QTreeWidgetItem *parent, const QStringList &parts, int i) const;

 private:
  CQCharts*     charts_ { nullptr };
  QTreeWidget*  tree_   { nullptr };
  QTextBrowser *text_   { nullptr };
};

#endif
