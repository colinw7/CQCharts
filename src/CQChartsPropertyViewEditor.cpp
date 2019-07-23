#include <CQChartsPropertyViewEditor.h>
#include <CQChartsPropertyViewTree.h>
#include <CQChartsLineEditBase.h>

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQPropertyView.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>

#include <QPainter>

namespace CQChartsPropertyViewUtil {

void getPropertyItemPlotView(CQPropertyViewItem *item, CQChartsPlot* &plot, CQChartsView * &view) {
  QObject *obj = (item ? item->object() : nullptr);

  plot = qobject_cast<CQChartsPlot *>(obj);
  view = qobject_cast<CQChartsView *>(obj);
}

}

//---

bool
CQChartsPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  //---

  CQPropertyViewItem *item = CQPropertyViewMgrInst->drawItem();

  CQChartsPlot *plot;
  CQChartsView *view;

  CQChartsPropertyViewUtil::getPropertyItemPlotView(item, plot, view);

  //---

  painter->setRenderHints(QPainter::Antialiasing);

  drawPreview(painter, option.rect, value, plot, view);
}

//---

QWidget *
CQChartsPropertyViewEditorFactory::
createEdit(QWidget *parent)
{
  CQPropertyViewItem *item = CQPropertyViewMgrInst->editItem();

  CQChartsPropertyViewTree *tree =
    (parent ? qobject_cast<CQChartsPropertyViewTree *>(parent->parentWidget()) : nullptr);

  CQChartsPlot *plot;
  CQChartsView *view;

  CQChartsPropertyViewUtil::getPropertyItemPlotView(item, plot, view);

  CQChartsLineEditBase *edit = createPropertyEdit(parent);

  edit->setPropertyViewTree(tree);

  if      (plot) edit->setPlot(plot);
  else if (view) edit->setView(view);

  return edit;
}
