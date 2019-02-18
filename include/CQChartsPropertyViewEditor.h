#ifndef CQChartsPropertyViewEditor_H
#define CQChartsPropertyViewEditor_H

class CQChartsPlot;
class CQChartsView;
class CQPropertyViewItem;

namespace CQChartsPropertyViewUtil {

void getPropertyItemPlotView(CQPropertyViewItem *item, CQChartsPlot* &plot, CQChartsView * &view);

}

//---

#include <CQPropertyViewType.h>

class QRect;

// property view type
class CQChartsPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsPropertyViewType() { }

  virtual ~CQChartsPropertyViewType() { }

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &ind,
            const QVariant &value, bool inside) override;

  virtual void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                           CQChartsPlot *plot, CQChartsView *view) = 0;
};

//---

#include <CQPropertyViewEditor.h>

class CQChartsLineEditBase;

// property view editor factory
class CQChartsPropertyViewEditorFactory : public CQPropertyViewEditorFactory {
 public:
  CQChartsPropertyViewEditorFactory() { }

  virtual ~CQChartsPropertyViewEditorFactory() { }

  QWidget *createEdit(QWidget *parent);

  virtual CQChartsLineEditBase *createPropertyEdit(QWidget *parent) = 0;
};

#endif
