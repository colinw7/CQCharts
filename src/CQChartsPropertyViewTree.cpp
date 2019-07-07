#include <CQChartsPropertyViewTree.h>
#include <CQChartsViewSettings.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsAnnotation.h>
#include <CQChartsTitleEdit.h>
#include <CQChartsKeyEdit.h>
#include <CQChartsAxisEdit.h>
#include <CQChartsTitle.h>
#include <CQChartsKey.h>
#include <CQChartsAxis.h>
#include <CQChartsEnv.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQApp.h>

#include <QMenu>
#include <QAction>
#include <QKeyEvent>

CQChartsPropertyViewTree::
CQChartsPropertyViewTree(CQChartsViewSettings *settings, CQPropertyViewModel *model) :
 CQPropertyViewTree(settings, model), settings_(settings)
{
}

void
CQChartsPropertyViewTree::
addMenuItems(QMenu *menu)
{
  auto addMenuAction = [&](QMenu *menu, const QString &name, const char *slot) -> QAction *{
    QAction *action = new QAction(name, menu);

    connect(action, SIGNAL(triggered()), this, slot);

    menu->addAction(action);

    return action;
  };

  auto addMenuCheckedAction = [&](QMenu *menu, const QString &name,
                                  bool isSet, const char *slot) -> QAction *{
    QAction *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(isSet);

    connect(action, SIGNAL(triggered(bool)), this, slot);

    menu->addAction(action);

    return action;
  };

  //--

  CQPropertyViewItem *item = CQPropertyViewTree::menuItem();

  if (item)
    addMenuAction(menu, "Edit", SLOT(editSlot()));

  addMenuCheckedAction(menu, "Show Filter", isFilterDisplayed(),
                       SLOT(showHideFilterSlot(bool)));
  addMenuCheckedAction(menu, "Show Style Items", isShowStyleItems(),
                       SLOT(showHideStyleItemsSlot(bool)));

  //---

  CQPropertyViewTree::addStandardMenuItems(menu);
}

void
CQChartsPropertyViewTree::
printItem(CQPropertyViewItem *item) const
{
  QObject *object = item->object();

  QString dataStr = item->dataStr();
  QString path    = item->path(".", /*alias*/true);

  CQChartsPlot       *plot       = qobject_cast<CQChartsPlot       *>(object);
  CQChartsAnnotation *annotation = qobject_cast<CQChartsAnnotation *>(object);

  if      (plot) {
    if (path.startsWith(plot->id()))
      path = path.mid(plot->id().length() + 1);

    std::cerr << "set_charts_property -plot " << plot->id().toStdString() <<
                 " -name " << path.toStdString() <<
                 " -value " << dataStr.toStdString() << "\n";
  }
  else if (annotation) {
    CQChartsPlot *plot = annotation->plot();

    if (plot) {
      if (path.startsWith(plot->id()))
        path = path.mid(plot->id().length() + 1);

      if (path.startsWith("annotations."))
        path = path.mid(12);

      if (path.startsWith(annotation->propertyId()))
        path = path.mid(annotation->propertyId().length() + 1);
    }
    else {
      if (path.startsWith("annotations."))
        path = path.mid(12);

      if (path.startsWith(annotation->propertyId()))
        path = path.mid(annotation->propertyId().length() + 1);
    }

    std::cerr << "set_charts_property -annotation " <<
                 annotation->pathId().toStdString() <<
                 " -name " << path.toStdString() <<
                 " -value " << dataStr.toStdString() << "\n";
  }
  else {
    CQChartsView *view = settings_->window()->view();

    std::cerr << "set_charts_property -view " << view->id().toStdString() <<
                 " -name " << path.toStdString() <<
                 " -value " << dataStr.toStdString() << "\n";
  }
}

void
CQChartsPropertyViewTree::
editSlot()
{
  CQPropertyViewItem *item = this->menuItem();

  QObject *obj = item->hierObject();

  //---

  if (CQChartsEnv::getBool("CQ_CHARTS_META_EDIT", false))
    CQApp::showMetaEdit(obj);

  //---

  CQChartsTitle *title = qobject_cast<CQChartsTitle *>(obj);
  CQChartsKey   *key   = qobject_cast<CQChartsKey   *>(obj);
  CQChartsAxis  *axis  = qobject_cast<CQChartsAxis  *>(obj);

  bool modal = CQChartsEnv::getBool("CQ_CHARTS_MODAL_DLG", true);

  if      (title) {
    delete titleDlg_;

    titleDlg_ = new CQChartsEditTitleDlg(this, title);

    if (modal) {
      titleDlg_->exec();

      delete titleDlg_;

      titleDlg_ = nullptr;
    }
    else
      titleDlg_->show();
  }
  else if (key) {
    delete keyDlg_;

    keyDlg_ = new CQChartsEditKeyDlg(this, key);

    if (modal) {
      keyDlg_->exec();

      delete keyDlg_;

      keyDlg_ = nullptr;
    }
    else
      keyDlg_->show();
  }
  else if (axis) {
    delete axisDlg_;

    axisDlg_ = new CQChartsEditAxisDlg(this, axis);

    if (modal) {
      axisDlg_->exec();

      delete axisDlg_;

      axisDlg_ = nullptr;
    }
    else
      axisDlg_->show();
  }
}

void
CQChartsPropertyViewTree::
setFilterDisplayed(bool show, bool focus)
{
  filterDisplayed_ = show;

  emit filterStateChanged(filterDisplayed_, focus);
}

void
CQChartsPropertyViewTree::
showHideFilterSlot(bool b)
{
  setFilterDisplayed(b);
}

void
CQChartsPropertyViewTree::
setShowStyleItems(bool show)
{
  showStyleItems_ = show;

  CQPropertyViewItem *item = propertyModel()->root();

  showStyleItems(item, show);
}

void
CQChartsPropertyViewTree::
showStyleItems(CQPropertyViewItem *item, bool show)
{
  if (CQCharts::getItemIsStyle(item)) {
    if (! CQCharts::getItemIsHidden(item))
      item->setHidden(! show);
  }

  for (auto &child : propertyModel()->itemChildren(item, /*hidden*/true)) {
    showStyleItems(child, show);
  }

  propertyModel()->reset();
}

void
CQChartsPropertyViewTree::
showHideStyleItemsSlot(bool b)
{
  setShowStyleItems(b);
}

void
CQChartsPropertyViewTree::
keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Slash) {
    setFilterDisplayed(true, true);
    return;
  }

  CQPropertyViewTree::keyPressEvent(e);
}
