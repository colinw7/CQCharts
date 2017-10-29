#include <CQPoint2DEdit.h>
#include <CQRealSpin.h>

namespace CQPoint2DEditUtil {

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

inline std::string toString(double r) {
  static char buffer[128];

#ifdef ALLOW_NAN
  if (COS::is_nan(real))
    strcpy(buffer, "NaN");
  else
    ::sprintf(buffer, "%g", r);
#else
  ::sprintf(buffer, "%g", r);
#endif

  return buffer;
}

}

//---

CQPoint2DEdit::
CQPoint2DEdit(QWidget *parent, const CPoint2D &value, bool spin) :
 QFrame(parent), spin_(spin)
{
  init(value);
}

CQPoint2DEdit::
CQPoint2DEdit(const CPoint2D &value, bool spin) :
 QFrame(0), spin_(spin)
{
  init(value);
}

CQPoint2DEdit::
CQPoint2DEdit(QWidget *parent, const QPointF &value, bool spin) :
 QFrame(parent), spin_(spin)
{
  init(CPoint2D(value.x(), value.y()));
}

CQPoint2DEdit::
CQPoint2DEdit(const QPointF &value, bool spin) :
 QFrame(0), spin_(spin)
{
  init(CPoint2D(value.x(), value.y()));
}

void
CQPoint2DEdit::
setSpin(bool spin)
{
  if (spin == spin_) return;

  spin_ = spin;

  x_edit_->setVisible(! spin_);
  y_edit_->setVisible(! spin_);
  x_spin_->setVisible(  spin_);
  y_spin_->setVisible(  spin_);

  layout_->invalidate();
}

void
CQPoint2DEdit::
setDecimals(int i)
{
  decimals_ = i;

  updateRange();
}

void
CQPoint2DEdit::
setMinimum(const CPoint2D &p)
{
  min_ = p;

  updateRange();
}

void
CQPoint2DEdit::
setMaximum(const CPoint2D &p)
{
  max_ = p;

  updateRange();
}

void
CQPoint2DEdit::
setStep(const CPoint2D &p)
{
  step_ = p;

  updateRange();
}

void
CQPoint2DEdit::
updateRange()
{
  x_validator_->setRange(min_.x, max_.x, decimals_);
  y_validator_->setRange(min_.y, max_.y, decimals_);

  x_spin_->setRange(min_.x, max_.x);
  y_spin_->setRange(min_.y, max_.y);

  x_spin_->setSingleStep(step_.x);
  y_spin_->setSingleStep(step_.y);
}

void
CQPoint2DEdit::
init(const CPoint2D &value)
{
  setObjectName("edit");

  setFrameStyle(QFrame::NoFrame | QFrame::Plain);

  //---

  x_validator_ = new QDoubleValidator(this);
  y_validator_ = new QDoubleValidator(this);

  x_edit_ = new QLineEdit; x_edit_->setObjectName("xedit");
  y_edit_ = new QLineEdit; y_edit_->setObjectName("yedit");

  x_edit_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  y_edit_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  x_edit_->setValidator(x_validator_);
  y_edit_->setValidator(y_validator_);

  connect(x_edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));
  connect(y_edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));

  //---

  x_spin_ = new CQRealSpin; x_spin_->setObjectName("xspin");
  y_spin_ = new CQRealSpin; y_spin_->setObjectName("yspin");

  x_spin_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  y_spin_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  connect(x_spin_, SIGNAL(valueChanged(double)), this, SLOT(editingFinishedI()));
  connect(y_spin_, SIGNAL(valueChanged(double)), this, SLOT(editingFinishedI()));

  //---

  updateRange();

  //---

  layout_ = new QHBoxLayout(this);
  layout_->setMargin(0); layout_->setSpacing(0);

  layout_->addWidget(x_edit_);
  layout_->addWidget(x_spin_);
  layout_->addWidget(y_edit_);
  layout_->addWidget(y_spin_);

  //---

  x_edit_->setVisible(! spin_);
  y_edit_->setVisible(! spin_);
  x_spin_->setVisible(  spin_);
  y_spin_->setVisible(  spin_);

  //---

  setValue(value);
}

void
CQPoint2DEdit::
setValue(const CPoint2D &point)
{
  point_ = point;

  pointToWidget();
}

void
CQPoint2DEdit::
setValue(const QPointF &point)
{
  setValue(CPoint2D(point.x(), point.y()));
}

const CPoint2D &
CQPoint2DEdit::
getValue() const
{
  return point_;
}

QPointF
CQPoint2DEdit::
getQValue() const
{
  return QPointF(point_.x, point_.y);
}

void
CQPoint2DEdit::
editingFinishedI()
{
  if (disableSignals_) return;

  widgetToPoint();

  emit valueChanged();
}

void
CQPoint2DEdit::
pointToWidget()
{
  disableSignals_ = true;

  x_edit_->setText(CQPoint2DEditUtil::toString(point_.getX()).c_str());
  y_edit_->setText(CQPoint2DEditUtil::toString(point_.getY()).c_str());

  x_spin_->setValue(point_.getX());
  y_spin_->setValue(point_.getY());

  disableSignals_ = false;
}

void
CQPoint2DEdit::
widgetToPoint()
{
  if (! spin_) {
    double x = 0.0, y = 0.0;

    if (! CQPoint2DEditUtil::toReal(x_edit_->text().toStdString(), &x)) x = 0.0;
    if (! CQPoint2DEditUtil::toReal(y_edit_->text().toStdString(), &y)) y = 0.0;

    point_.setX(x);
    point_.setY(y);
  }
  else {
    point_.setX(x_spin_->value());
    point_.setY(y_spin_->value());
 }
}
