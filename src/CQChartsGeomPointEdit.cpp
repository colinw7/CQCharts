#include <CQChartsGeomPointEdit.h>
#include <CQChartsLineEdit.h>
#include <CQChartsVariant.h>

#include <CQPropertyView.h>
#include <CQRealSpin.h>

#include <QHBoxLayout>

CQChartsGeomPointEdit::
CQChartsGeomPointEdit(QWidget *parent, const Point &value, bool spin) :
 QFrame(parent), spin_(spin)
{
  init(value);
}

CQChartsGeomPointEdit::
CQChartsGeomPointEdit(const Point &value, bool spin) :
 QFrame(0), spin_(spin)
{
  init(value);
}

void
CQChartsGeomPointEdit::
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
CQChartsGeomPointEdit::
setDecimals(int i)
{
  decimals_ = i;

  updateRange();
}

void
CQChartsGeomPointEdit::
setMinimum(const Point &p)
{
  min_ = p;

  updateRange();
}

void
CQChartsGeomPointEdit::
setMaximum(const Point &p)
{
  max_ = p;

  updateRange();
}

void
CQChartsGeomPointEdit::
setStep(const Point &p)
{
  step_ = p;

  updateRange();
}

void
CQChartsGeomPointEdit::
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
CQChartsGeomPointEdit::
init(const Point &value)
{
  setObjectName("edit");

  setFrameStyle(QFrame::NoFrame | QFrame::Plain);

  layout_ = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  //---

  x_validator_ = new QDoubleValidator(this);
  y_validator_ = new QDoubleValidator(this);

  x_edit_ = CQUtil::makeWidget<CQChartsLineEdit>("xedit");
  y_edit_ = CQUtil::makeWidget<CQChartsLineEdit>("yedit");

  x_edit_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  y_edit_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  x_edit_->setValidator(x_validator_);
  y_edit_->setValidator(y_validator_);

  connect(x_edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));
  connect(y_edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));

  //---

  x_spin_ = CQUtil::makeWidget<CQRealSpin>("xspin");
  y_spin_ = CQUtil::makeWidget<CQRealSpin>("yspin");

  x_spin_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  y_spin_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  connect(x_spin_, SIGNAL(valueChanged(double)), this, SLOT(editingFinishedI()));
  connect(y_spin_, SIGNAL(valueChanged(double)), this, SLOT(editingFinishedI()));

  //---

  updateRange();

  //---

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

  setFocusProxy(x_edit_);

  setValue(value);
}

void
CQChartsGeomPointEdit::
setValue(const Point &point)
{
  point_ = point;

  pointToWidget();
}

const CQChartsGeomPointEdit::Point &
CQChartsGeomPointEdit::
getValue() const
{
  return point_;
}

void
CQChartsGeomPointEdit::
editingFinishedI()
{
  if (disableSignals_) return;

  if (widgetToPoint())
    emit valueChanged();
}

void
CQChartsGeomPointEdit::
pointToWidget()
{
  disableSignals_ = true;

  x_edit_->setText(QString("%1").arg(point_.getX()));
  y_edit_->setText(QString("%1").arg(point_.getY()));

  x_spin_->setValue(point_.getX());
  y_spin_->setValue(point_.getY());

  disableSignals_ = false;
}

bool
CQChartsGeomPointEdit::
widgetToPoint()
{
  if (! spin_) {
    bool ok1, ok2;

    double x = x_edit_->text().toDouble(&ok1);
    double y = y_edit_->text().toDouble(&ok2);

    if (ok1)
      point_.setX(x);

    if (ok2)
      point_.setY(y);
  }
  else {
    point_.setX(x_spin_->value());
    point_.setY(y_spin_->value());
  }

  return true;
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsGeomPointPropertyViewType::
CQChartsGeomPointPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsGeomPointPropertyViewType::
getEditor() const
{
  return new CQChartsGeomPointPropertyViewEditor;
}

bool
CQChartsGeomPointPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsGeomPointPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  bool ok;

  CQChartsGeom::Point point = CQChartsVariant::toPoint(value, ok);
  if (! ok) return;

  QString str = point.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsGeomPointPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  CQChartsGeom::Point point = CQChartsVariant::toPoint(value, ok);
  if (! ok) return "";

  QString str = point.toString();

  return str;
}

//------

CQChartsGeomPointPropertyViewEditor::
CQChartsGeomPointPropertyViewEditor()
{
}

QWidget *
CQChartsGeomPointPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsGeomPointEdit *edit = new CQChartsGeomPointEdit(parent);

  return edit;
}

void
CQChartsGeomPointPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsGeomPointEdit *edit = qobject_cast<CQChartsGeomPointEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQChartsGeomPointPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsGeomPointEdit *edit = qobject_cast<CQChartsGeomPointEdit *>(w);
  assert(edit);

  return CQChartsVariant::fromPoint(edit->getValue());
}

void
CQChartsGeomPointPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsGeomPointEdit *edit = qobject_cast<CQChartsGeomPointEdit *>(w);
  assert(edit);

  bool ok;
  CQChartsGeom::Point point = CQChartsVariant::toPoint(var, ok);
  if (! ok) return;

  edit->setValue(point);
}
