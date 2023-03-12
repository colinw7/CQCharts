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
#include <CQChartsWidgetIFace.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQApp.h>

#include <QMenu>
#include <QAction>
#include <QKeyEvent>

CQChartsPropertyViewTree::
CQChartsPropertyViewTree(QWidget *parent, CQPropertyViewModel *model) :
 CQPropertyViewTree(parent, model)
{
}

void
CQChartsPropertyViewTree::
setView(View *view)
{
  view_ = view;
}

void
CQChartsPropertyViewTree::
setPlot(Plot *plot)
{
  plot_ = plot;
}

void
CQChartsPropertyViewTree::
addMenuItems(QMenu *menu)
{
  auto addMenuAction = [&](QMenu *menu, const QString &name, const char *slot) {
    return CQUtil::addAction(menu, name, this, slot);
  };

  auto addMenuCheckedAction = [&](QMenu *menu, const QString &name, bool isSet, const char *slot) {
    return CQUtil::addCheckedAction(menu, name, isSet, this, slot);
  };

  //--

  auto *item = CQPropertyViewTree::menuItem();

  if (item)
    addMenuAction(menu, "Edit", SLOT(editSlot()));

  addMenuCheckedAction(menu, "Show Filter", isFilterDisplayed(),
                       SLOT(showHideFilterSlot(bool)));
  addMenuCheckedAction(menu, "Show Style Items", isShowStyleItems(),
                       SLOT(showHideStyleItemsSlot(bool)));

  menu->addSeparator();

  //---

  CQPropertyViewTree::addStandardMenuItems(menu);
}

void
CQChartsPropertyViewTree::
printItem(CQPropertyViewItem *item) const
{
  auto *object = item->object();

  auto dataStr = item->dataStr();
  auto path    = item->path(".", /*alias*/true);

  auto *view       = qobject_cast<CQChartsView       *>(object);
  auto *plot       = qobject_cast<CQChartsPlot       *>(object);
  auto *annotation = qobject_cast<CQChartsAnnotation *>(object);

  if      (plot) {
    if (path.startsWith(plot->id()))
      path = path.mid(plot->id().length() + 1);

    std::cerr << "set_charts_property -plot " << plot->id().toStdString() <<
                 " -name " << path.toStdString() <<
                 " -value " << dataStr.toStdString() << "\n";
  }
  else if (annotation) {
    auto *plot = annotation->plot();

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
  else if (view) {
    std::cerr << "set_charts_property -view " << view->id().toStdString() <<
                 " -name " << path.toStdString() <<
                 " -value " << dataStr.toStdString() << "\n";
  }
}

void
CQChartsPropertyViewTree::
editSlot()
{
  auto *item = this->menuItem();

  auto *obj = item->hierObject();

  //---

  if (CQChartsEnv::getBool("CQ_CHARTS_META_EDIT", false))
    CQApp::showMetaEdit(obj);

  //---

  auto *title = qobject_cast<CQChartsTitle *>(obj);
  auto *key   = qobject_cast<CQChartsKey   *>(obj);
  auto *axis  = qobject_cast<CQChartsAxis  *>(obj);

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

  Q_EMIT filterStateChanged(filterDisplayed_, focus);
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

  auto *item = propertyModel()->root();

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
initEditor(QWidget *w)
{
  auto *iface = dynamic_cast<CQChartsWidgetIFace *>(w);

  if (iface) {
    if (view_) iface->setView(view_);
    if (plot_) iface->setPlot(plot_);
  }
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
