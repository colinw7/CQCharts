#include <CQPropertyViewSizeFType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQPoint2DEdit.h>

CQPropertyViewSizeFType::
CQPropertyViewSizeFType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewSizeFType::
getEditor() const
{
  return new CQPropertyViewSizeFEditor;
}

bool
CQPropertyViewSizeFType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewSizeFType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, bool inside)
{
  delegate->drawSize(painter, option, value.value<QSizeF>(), index, inside);
}

QString
CQPropertyViewSizeFType::
tip(const QVariant &value) const
{
  QSizeF s = value.value<QSizeF>();

  return QString("(%1 %2)").arg(s.width()).arg(s.height());
}

//------

CQPropertyViewSizeFEditor::
CQPropertyViewSizeFEditor(double max, double step) :
 max_(max), step_(step)
{
}

QWidget *
CQPropertyViewSizeFEditor::
createEdit(QWidget *parent)
{
  CQPoint2DEdit *edit = new CQPoint2DEdit(parent);

  edit->setSpin(true);
  edit->setMinimum(CPoint2D(0, 0));
  edit->setMaximum(CPoint2D(max_, max_));
  edit->setStep(CPoint2D(step_, step_));

  return edit;
}

void
CQPropertyViewSizeFEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQPropertyViewSizeFEditor::
getValue(QWidget *w)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  QPointF p = edit->getQValue();

  return QSizeF(p.x(), p.y());
}

void
CQPropertyViewSizeFEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  QSizeF s = var.toSizeF();

  edit->setValue(QPointF(s.width(), s.height()));
}
