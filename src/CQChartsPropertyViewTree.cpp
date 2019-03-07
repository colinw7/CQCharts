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

CQChartsPropertyViewTree::
CQChartsPropertyViewTree(CQChartsViewSettings *settings, CQPropertyViewModel *model) :
 CQPropertyViewTree(settings, model), settings_(settings)
{
}

void
CQChartsPropertyViewTree::
addMenuItems(QMenu *menu)
{
  CQPropertyViewItem *item = CQPropertyViewTree::menuItem();

  if (item) {
    QAction *editAction = new QAction("Edit");

    connect(editAction, SIGNAL(triggered()), this, SLOT(editSlot()));

    menu->addAction(editAction);
  }

  //---

  QAction *showHideAction = new QAction("Show Filter");

  showHideAction->setCheckable(true);
  showHideAction->setChecked(isFilterDisplayed());

  connect(showHideAction, SIGNAL(triggered(bool)), this, SLOT(showHideFilterSlot(bool)));

  menu->addAction(showHideAction);

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

    titleDlg_ = new CQChartsEditTitleDlg(title);

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

    keyDlg_ = new CQChartsEditKeyDlg(key);

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

    axisDlg_ = new CQChartsEditAxisDlg(axis);

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
showHideFilterSlot(bool b)
{
  setFilterDisplayed(b);

  emit filterStateChanged(b);
}
