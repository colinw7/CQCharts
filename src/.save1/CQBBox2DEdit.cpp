#include <CQBBox2DEdit.h>

namespace CQBBox2DEditUtil {

bool toReal(const std::string &s, double *r) {
  bool ok = true;

  *r = 0.0;

  try {
    *r = std::stod(s);
  }
  catch (...) {
    ok = false;
  }

  return ok;
}

CPoint2D fromQPoint(const QPointF &point) {
  return CPoint2D(point.x(), point.y());
}

CBBox2D fromQRect(const QRectF &rect) {
  return CBBox2D(fromQPoint(rect.bottomLeft()), fromQPoint(rect.topRight()));
}

QPointF toQPoint(const CPoint2D &point) {
  return QPointF(point.x, point.y);
}

QRectF toQRect(const CBBox2D &rect) {
  return QRectF(toQPoint(rect.getLL()), toQPoint(rect.getUR())).normalized();
}

}

//---

CQBBox2DEdit::
CQBBox2DEdit(QWidget *parent, const CBBox2D &value) :
 QFrame(parent)
{
  init(value);
}

CQBBox2DEdit::
CQBBox2DEdit(const CBBox2D &value) :
 QFrame(0)
{
  init(value);
}

CQBBox2DEdit::
CQBBox2DEdit(QWidget *parent, const QRectF &value) :
 QFrame(parent)
{
  init(CQBBox2DEditUtil::fromQRect(value));
}

CQBBox2DEdit::
CQBBox2DEdit(const QRectF &value) :
 QFrame(0)
{
  init(CQBBox2DEditUtil::fromQRect(value));
}

void
CQBBox2DEdit::
init(const CBBox2D &value)
{
  setObjectName("edit");

  setFrameStyle(QFrame::NoFrame | QFrame::Plain);

  //---

  edit_ = new QLineEdit;

  edit_->setObjectName("edit");
  edit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  connect(edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));

  //---

  layout_ = new QHBoxLayout(this);
  layout_->setMargin(0); layout_->setSpacing(0);

  layout_->addWidget(edit_);

  //---

  setValue(value);
}

void
CQBBox2DEdit::
setValue(const CBBox2D &rect)
{
  rect_ = rect;

  rectToWidget();
}

void
CQBBox2DEdit::
setValue(const QRectF &rect)
{
  setValue(CQBBox2DEditUtil::fromQRect(rect));
}

const CBBox2D &
CQBBox2DEdit::
getValue() const
{
  return rect_;
}

QRectF
CQBBox2DEdit::
getQValue() const
{
  return CQBBox2DEditUtil::toQRect(rect_);
}

void
CQBBox2DEdit::
editingFinishedI()
{
  if (disableSignals_) return;

  widgetToPoint();

  emit valueChanged();
}

void
CQBBox2DEdit::
rectToWidget()
{
  disableSignals_ = true;

  QString str = QString("%1 %2 %3 %4").arg(rect_.getXMin()).arg(rect_.getYMin()).
                                       arg(rect_.getXMax()).arg(rect_.getYMax());

  edit_->setText(str);

  disableSignals_ = false;
}

bool
CQBBox2DEdit::
widgetToPoint()
{
  QStringList strs = edit_->text().split(" ", QString::SkipEmptyParts);

  if (strs.length() != 4)
    return false;

  double x1, y1, x2, y2;

  if (! CQBBox2DEditUtil::toReal(strs[0].toStdString(), &x1)) return false;
  if (! CQBBox2DEditUtil::toReal(strs[1].toStdString(), &y1)) return false;
  if (! CQBBox2DEditUtil::toReal(strs[2].toStdString(), &x2)) return false;
  if (! CQBBox2DEditUtil::toReal(strs[3].toStdString(), &y2)) return false;

  rect_ = CBBox2D(x1, y1, x2, y2);

  return true;
}
