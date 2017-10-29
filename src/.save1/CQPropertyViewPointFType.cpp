#include <CQPropertyViewPointFType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQPoint2DEdit.h>

CQPropertyViewPointFType::
CQPropertyViewPointFType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewPointFType::
getEditor() const
{
  return new CQPropertyViewPointFEditor;
}

bool
CQPropertyViewPointFType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewPointFType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, bool inside)
{
  delegate->drawPoint(painter, option, value.value<QPointF>(), index, inside);
}

QString
CQPropertyViewPointFType::
tip(const QVariant &value) const
{
  QPointF p = value.value<QPointF>();

  return QString("(%1 %2)").arg(p.x()).arg(p.y());
}

//------

CQPropertyViewPointFEditor::
CQPropertyViewPointFEditor(double min, double max, double step) :
 min_(min), max_(max), step_(step)
{
}

QWidget *
CQPropertyViewPointFEditor::
createEdit(QWidget *parent)
{
  CQPoint2DEdit *edit = new CQPoint2DEdit(parent);

  edit->setSpin(true);
  edit->setMinimum(CPoint2D(min_, min_));
  edit->setMaximum(CPoint2D(max_, max_));
  edit->setStep(CPoint2D(step_, step_));

  return edit;
}

void
CQPropertyViewPointFEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQPropertyViewPointFEditor::
getValue(QWidget *w)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  return edit->getQValue();
}

void
CQPropertyViewPointFEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  QPointF p = var.toPointF();

  edit->setValue(p);
}
