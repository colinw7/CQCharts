#include <CQChartsHelpDlg.h>
#include <CQCharts.h>
#include <CQChartsModelData.h>
#include <CQChartsColumnType.h>
#include <CQChartsView.h>
#include <CQChartsPlotType.h>
#include <CQChartsDocument.h>
#include <CQChartsWidgetUtil.h>

#include <CQPixmapCache.h>
#include <CQUtil.h>

#include <QFrame>
#include <QSplitter>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
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

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //---

  auto *controlFrame = CQUtil::makeWidget<QFrame>("controlFrame");

  controlFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *controlLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame, 2, 2);

  layout->addWidget(controlFrame);

  //--

  auto createButton = [&](const QString &name, const QString &iconName, const QString &tip,
                          const char *receiver) {
    auto *button = CQUtil::makeWidget<QToolButton>(name);

    button->setIcon(CQPixmapCacheInst->getIcon(iconName));

    connect(button, SIGNAL(clicked()), this, receiver);

    button->setToolTip(tip);

    return button;
  };

  //--

  prevButton_ = createButton("prev", "LEFT" , "Previous section", SLOT(prevSlot()));
  nextButton_ = createButton("next", "RIGHT", "Next section"    , SLOT(nextSlot()));

  controlLayout->addWidget(prevButton_);
  controlLayout->addWidget(nextButton_);
  controlLayout->addStretch(1);

  //---

  auto *splitter = CQUtil::makeWidget<QSplitter>("splitter");

  splitter->setOrientation(Qt::Horizontal);

  layout->addWidget(splitter);

  //---

  tree_ = CQUtil::makeWidget<QTreeWidget>("tree");

  connect(tree_, SIGNAL(itemSelectionChanged()), this, SLOT(treeItemSlot()));

  tree_->header()->setVisible(false);

  splitter->addWidget(tree_);

  //---

  text_ = CQUtil::makeWidget<CQChartsDocument>("text");

  connect(text_, SIGNAL(linkClicked(const QString &)),
          this, SLOT(treeLinkSlot(const QString &)));

  splitter->addWidget(text_);

  //---

  int i1 = INT_MAX*0.2;
  int i2 = INT_MAX - i1;

  splitter->setSizes(QList<int>({i1, i2}));

  //---

  addItems();

  //---

  setCurrentSection("introduction", false);

  updatePrevNext();
}

void
CQChartsHelpDlg::
addItems()
{
  auto addItem = [&](QTreeWidgetItem *parent, const QString &name, const QString &id) {
    auto *item = new QTreeWidgetItem(QStringList() << name);

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

  auto *modelsItem = addTopItem("Models", "models");
  assert(modelsItem);

  QStringList modelTypeNames;

  charts_->getModelTypeNames(modelTypeNames);

  for (const auto &name : modelTypeNames)
    addItem(modelsItem, name, name);

  auto *columnTypesItem = addItem(modelsItem, "Types", "column_types");

  auto *columnTypeMgr = charts_->columnTypeMgr();

  QStringList columnTypeNames;

  columnTypeMgr->typeNames(columnTypeNames);

  for (const auto &name : columnTypeNames)
    addItem(columnTypesItem, name, name);

  //---

  auto *viewItem = addTopItem("View", "view");
  assert(viewItem);

  //---

  auto *plotsItem = addTopItem("Plots", "plot_types");

  QStringList names, descs;

  charts_->getPlotTypeNames(names, descs);

  for (const auto &name : names)
    addItem(plotsItem, name, name);

  //---

  auto *annotationsItem = addTopItem("Annotations", "annotation_types");
  assert(annotationsItem);

  //---

  auto *commandsItem = addTopItem("Commands", "commands");

  for (const auto &command : CQChartsHelpDlgMgrInst->tclCommands())
    addItem(commandsItem, command, command);
}

bool
CQChartsHelpDlg::
setCurrentSection(const QString &section, bool updateUndoRedo)
{
  QStringList parts = section.split("/");

  auto *item = getTreeItem(nullptr, parts, 0);

  if (! item)
    return false;

  //---

  {
  CQChartsWidgetUtil::AutoDisconnect autoDisconnect(
    tree_, SIGNAL(itemSelectionChanged()), this, SLOT(treeItemSlot()));

  tree_->setCurrentItem(item, 0, QItemSelectionModel::ClearAndSelect);
  }

  loadSectionText();

  //---

  updateCurrentSection(section, updateUndoRedo);

  return true;
}

void
CQChartsHelpDlg::
updateCurrentSection(const QString &section, bool updateUndoRedo)
{
  if (section != currentSection_) {
    if (updateUndoRedo) {
      if (currentSection_ != "") {
        undoSections_.push_back(currentSection_);
        redoSections_.clear();

        updatePrevNext();
      }
    }

    currentSection_ = section;
  }
}

void
CQChartsHelpDlg::
prevSlot()
{
  if (undoSections_.empty())
    return;

  QString section = undoSections_.back();

  undoSections_.pop_back();

  redoSections_.push_back(currentSection_);

  setCurrentSection(section, /*updateUndoRedo*/false);

  updatePrevNext();
}

void
CQChartsHelpDlg::
nextSlot()
{
  if (redoSections_.empty())
    return;

  QString section = redoSections_.back();

  redoSections_.pop_back();

  undoSections_.push_back(currentSection_);

  setCurrentSection(section, /*updateUndoRedo*/false);

  updatePrevNext();
}

void
CQChartsHelpDlg::
updatePrevNext()
{
  prevButton_->setEnabled(! undoSections_.empty());
  nextButton_->setEnabled(! redoSections_.empty());
}

void
CQChartsHelpDlg::
treeItemSlot()
{
  auto items = tree_->selectedItems();
  if (items.count() == 0) return;

  auto *item = items[0];

  updateCurrentSection(itemPath(item));

  //---

  loadSectionText();
}

void
CQChartsHelpDlg::
loadSectionText()
{
  auto items = tree_->selectedItems();
  if (items.count() == 0) return;

  auto *item = items[0];

  QString id = item->data(0, Qt::UserRole).toString();

  QString parentId;

  auto *parent = item->parent();

  if (parent)
    parentId = parent->data(0, Qt::UserRole).toString();

  //---

  if      (id == "introduction") {
    setHtml(CQCharts::description());
  }
  else if (id == "models") {
    setHtml(CQChartsModelData::description());
  }
  else if (id == "column_types") {
    setHtml(CQChartsColumnTypeMgr::description());
  }
  else if (id == "view") {
    setHtml(CQChartsView::description());
  }
  else if (id == "commands") {
  }
  else if (parentId == "models") {
    setHtml(CQChartsModelData::typeDescription(id));
  }
  else if (parentId == "column_types") {
    auto *columnTypeMgr = charts_->columnTypeMgr();

    const auto *type = columnTypeMgr->getNamedType(id);
    assert(type);

    setHtml(type->desc());
  }
  else if (parentId == "plot_types") {
    auto *type = charts_->plotType(id);
    assert(type);

    setHtml(type->description());
  }
  else if (parentId == "commands") {
  }
}

QString
CQChartsHelpDlg::
itemPath(QTreeWidgetItem *item) const
{
  QString name = item->data(0, Qt::UserRole).toString();

  auto *parent = item->parent();

  if (parent)
    name = itemPath(parent) + "/" + name;

  return name;
}

void
CQChartsHelpDlg::
setHtml(const QString &text)
{
  int pos = text.indexOf("@CHARTS_DOC_PATH@");

  if (pos >= 0) {
    QString text1 = text;

    while (pos >= 0) {
      QString lhs = text1.mid(0, pos);
      QString rhs = text1.mid(pos + 17, pos);

      text1 = lhs + "/home/colinw/dev/progs/charts/doc/CQCharts" + rhs;

      pos = text1.indexOf("@CHARTS_DOC_PATH@");
    }

    text_->setHtml(text1);
  }
  else {
    text_->setHtml(text);
  }
}

void
CQChartsHelpDlg::
treeLinkSlot(const QString &name)
{
  if (name.startsWith("column_type/")) {
    QString dest = name.mid(12);

    setCurrentSection("Models/Types/" + dest);
  }
}

void
CQChartsHelpDlg::
selectTreeItem(const QString &path)
{
  setCurrentSection(path);
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
      auto *item = tree_->topLevelItem(i);

      QString id = item->data(0, Qt::UserRole).toString();

      if (part == id)
        return getTreeItem(item, parts, ind + 1);
    }
  }
  else {
    int n = parent->childCount();

    for (int i = 0; i < n; ++i) {
      auto *item = parent->child(i);

      QString id = item->data(0, Qt::UserRole).toString();

      if (part == id)
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
