#ifndef CQChartsHelpDlg_H
#define CQChartsHelpDlg_H

#include <QDialog>

class CQChartsHelpDlg;
class CQCharts;

class QTreeWidget;
class QTreeWidgetItem;
class QTextBrowser;
class QToolButton;

#define CQChartsHelpDlgMgrInst CQChartsHelpDlgMgr::instance()

/*!
 * \brief Help Dialog Manager singleton
 * \ingroup Charts
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
 * \ingroup Charts
 */
class CQChartsHelpDlg : public QDialog {
  Q_OBJECT

  Q_PROPERTY(QString section READ currentSection WRITE setCurrentSection)

 public:
  CQChartsHelpDlg(CQCharts *charts, QWidget *parent=nullptr);

  CQCharts *charts() const { return charts_; }

  QString currentSection() const { return currentSection_; }
  bool setCurrentSection(const QString &section, bool updateUndoRedo=true);

  QSize sizeHint() const override;

 private:
  void addItems();

  QTreeWidgetItem *getTreeItem(QTreeWidgetItem *parent, const QStringList &parts, int i) const;

  void updateCurrentSection(const QString &section, bool updateUndoRedo=true);

  void updatePrevNext();

  QString itemPath(QTreeWidgetItem *item) const;

  void loadSectionText();

  void setHtml(const QString &text);

 private slots:
  void prevSlot();
  void nextSlot();

  void treeItemSlot();

  void treeAnchorSlot(const QUrl &url);

  void selectTreeItem(const QString &path);

 private:
  using Sections = std::vector<QString>;

  CQCharts*     charts_         { nullptr }; //!< charts
  QTreeWidget*  tree_           { nullptr }; //!< index tree
  QTextBrowser *text_           { nullptr }; //!< text browser
  QToolButton  *prevButton_     { nullptr }; //!< previous section button
  QToolButton  *nextButton_     { nullptr }; //!< next section button
  QString       currentSection_;             //!< current section
  Sections      undoSections_;
  Sections      redoSections_;
};

#endif
