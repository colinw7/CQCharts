#include <CQChartsHelpDlg.h>
#include <CQCharts.h>
#include <CQChartsModelData.h>
#include <CQChartsColumnType.h>
#include <CQChartsView.h>
#include <CQChartsPlotType.h>

#include <CQPixmapCache.h>
#include <CQUtil.h>

#include <QFrame>
#include <QSplitter>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QTextBrowser>
#include <QToolButton>
#include <QVBoxLayout>

#include <iostream>

#include <svg/left_svg.h>
#include <svg/right_svg.h>

CQChartsHelpDlgMgr::
CQChartsHelpDlgMgr()
{
}

void
CQChartsHelpDlgMgr::
showDialog(CQCharts *charts)
{
  if (dlg_ && dlg_->charts() != charts) {
    delete dlg_;

    dlg_ = nullptr;
  }

  if (! dlg_)
    dlg_ = new CQChartsHelpDlg(charts);

  dlg_->show();

  dlg_->raise();
}

//---

CQChartsHelpDlg::
CQChartsHelpDlg(CQCharts *charts, QWidget *parent) :
 QDialog(parent), charts_(charts)
{
  setWindowTitle("Charts Help");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //---

  QFrame *controlFrame = CQUtil::makeWidget<QFrame>("controlFrame");

  controlFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  QHBoxLayout *controlLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame, 2, 2);

  layout->addWidget(controlFrame);

  //--

  auto createButton = [&](const QString &name, const QString &iconName, const QString &tip,
                          const char *receiver) {
    QToolButton *button = CQUtil::makeWidget<QToolButton>(name);

    button->setIcon(CQPixmapCacheInst->getIcon(iconName));

    connect(button, SIGNAL(clicked()), this, receiver);

    button->setToolTip(tip);

    return button;
  };

  //--

  QToolButton *prevButton = createButton("prev", "LEFT" , "Previous section", SLOT(prevSlot()));
  QToolButton *nextButton = createButton("next", "RIGHT", "Next section"    , SLOT(nextSlot()));

  controlLayout->addWidget(prevButton);
  controlLayout->addWidget(nextButton);
  controlLayout->addStretch(1);

  //---

  QSplitter *splitter = CQUtil::makeWidget<QSplitter>("splitter");

  splitter->setOrientation(Qt::Horizontal);

  layout->addWidget(splitter);

  //---

  tree_ = CQUtil::makeWidget<QTreeWidget>("tree");

  connect(tree_, SIGNAL(itemSelectionChanged()), this, SLOT(treeItemSlot()));

  tree_->header()->setVisible(false);

  splitter->addWidget(tree_);

  //---

  text_ = CQUtil::makeWidget<QTextBrowser>("text");

  text_->setOpenLinks(false);
  text_->setOpenExternalLinks(false);

  connect(text_, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(treeAnchorSlot(const QUrl &)));

  splitter->addWidget(text_);

  //---

  int i1 = INT_MAX*0.2;
  int i2 = INT_MAX - i1;

  splitter->setSizes(QList<int>({i1, i2}));

  //---

  addItems();
}

void
CQChartsHelpDlg::
addItems()
{
  auto addItem = [&](QTreeWidgetItem *parent, const QString &name, const QString &id) {
    QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << name);

    if (! parent)
      tree_->addTopLevelItem(item);
    else
      parent->addChild(item);

    item->setData(0, Qt::UserRole, id);

    return item;
  };

  auto addTopItem = [&](const QString &name, const QString &id) {
    return addItem(nullptr, name, id);
  };

  //---

  addTopItem("Introduction", "introduction");

  QTreeWidgetItem *modelsItem = addTopItem("Models", "models");
  assert(modelsItem);

  QStringList modelTypeNames;

  charts_->getModelTypeNames(modelTypeNames);

  for (const auto &name : modelTypeNames)
    addItem(modelsItem, name, "model_type");

  QTreeWidgetItem *columnTypesItem = addItem(modelsItem, "Types", "column_types");

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  QStringList columnTypeNames;

  columnTypeMgr->typeNames(columnTypeNames);

  for (const auto &name : columnTypeNames)
    addItem(columnTypesItem, name, "column_type");

  //---

  QTreeWidgetItem *viewItem = addTopItem("View", "view");
  assert(viewItem);

  //---

  QTreeWidgetItem *plotsItem = addTopItem("Plots", "plot_types");

  QStringList names, descs;

  charts_->getPlotTypeNames(names, descs);

  for (const auto &name : names)
    addItem(plotsItem, name, "plot_type");

  //---

  QTreeWidgetItem *annotationsItem = addTopItem("Annotations", "annotation_types");
  assert(annotationsItem);

  //---

  QTreeWidgetItem *commandsItem = addTopItem("Commands", "commands");

  for (const auto &command : CQChartsHelpDlgMgrInst->tclCommands())
    addItem(commandsItem, command, "tcl_command");
}

void
CQChartsHelpDlg::
prevSlot()
{
}

void
CQChartsHelpDlg::
nextSlot()
{
}

void
CQChartsHelpDlg::
treeItemSlot()
{
  QList<QTreeWidgetItem *> items = tree_->selectedItems();
  if (items.count() == 0) return;

  QTreeWidgetItem *item = items[0];

  QString id = item->data(0, Qt::UserRole).toString();

  if      (id == "introduction") {
    setHtml(CQCharts::description());
  }
  else if (id == "models") {
    setHtml(CQChartsModelData::description());
  }
  else if (id == "model_type") {
    QString typeName = item->text(0);

    setHtml(CQChartsModelData::typeDescription(typeName));
  }
  else if (id == "column_types") {
    setHtml(CQChartsColumnTypeMgr::description());
  }
  else if (id == "column_type") {
    QString typeName = item->text(0);

    CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

    const CQChartsColumnType *type = columnTypeMgr->getNamedType(typeName);
    assert(type);

    setHtml(type->description());
  }
  else if (id == "view") {
    setHtml(CQChartsView::description());
  }
  else if (id == "plot_type") {
    QString typeName = item->text(0);

    CQChartsPlotType *type = charts_->plotType(typeName);
    assert(type);

    setHtml(type->description());
  }
}

void
CQChartsHelpDlg::
setHtml(const QString &text)
{
  text_->setHtml(text);

std::cerr << text.toStdString() << "\n";
}

void
CQChartsHelpDlg::
treeAnchorSlot(const QUrl &url)
{
  QString chartsPrefix("charts://");

  QString str = url.toString();
std::cerr << str.toStdString() << "\n";

  if (str.startsWith(chartsPrefix)) {
    QString str1 = str.mid(chartsPrefix.length());

    if (str1.startsWith("column_type/")) {
      QString dest = str1.mid(12);

      selectTreeItem("Models/Types/" + dest);
    }
  }
}

void
CQChartsHelpDlg::
selectTreeItem(const QString &path) const
{
  QStringList parts = path.split("/");

  QTreeWidgetItem *item = getTreeItem(nullptr, parts, 0);

  if (item)
    tree_->setCurrentItem(item, 0, QItemSelectionModel::ClearAndSelect);
}

QTreeWidgetItem *
CQChartsHelpDlg::
getTreeItem(QTreeWidgetItem *parent, const QStringList &parts, int ind) const
{
  if (ind == parts.length())
    return parent;

  QString part = parts.at(ind);

  if (! parent) {
    int n = tree_->topLevelItemCount();

    for (int i = 0; i < n; ++i) {
      QTreeWidgetItem *item = tree_->topLevelItem(i);

      if (item->text(0) == part)
        return getTreeItem(item, parts, ind + 1);
    }
  }
  else {
    int n = parent->childCount();

    for (int i = 0; i < n; ++i) {
      QTreeWidgetItem *item = parent->child(i);

      if (item->text(0) == part)
        return getTreeItem(item, parts, ind + 1);
    }
  }

  return nullptr;
}

QSize
CQChartsHelpDlg::
sizeHint() const
{
  QFontMetrics fm(font());

  int w = fm.width("X")*80;
  int h = fm.height()*35;

  return QSize(w, h);
}
