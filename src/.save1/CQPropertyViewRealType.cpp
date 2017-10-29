#include <CQPropertyViewRealType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <cassert>

CQPropertyViewRealType::
CQPropertyViewRealType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewRealType::
getEditor() const
{
  return new CQPropertyViewRealEditor;
}

bool
CQPropertyViewRealType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewRealType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, bool inside)
{
  delegate->drawString(painter, option, value.toString(), index, inside);
}

//------

#if 0
#include <CQSlider.h>
#include <CQComboSlider.h>
#endif

#include <CQRealSpin.h>

CQPropertyViewRealEditor::
CQPropertyViewRealEditor(double min, double max, double step) :
 type_(Type::DoubleSpin), min_(min), max_(max), step_(step)
{
}

QWidget *
CQPropertyViewRealEditor::
createEdit(QWidget *parent)
{
#if 0
  if      (type_ == Type::RealSlider) {
    CQRealSlider *slider = new CQRealSlider(parent);

    slider->setAutoFillBackground(true);

    slider->setMinimum(min_);
    slider->setMaximum(max_);
  //slider->setDecimals(6);

    slider->setSingleStep(step_);

    return slider;
  }
  else if (type_ == Type::ComboSlider) {
    CQComboSlider *combo = new CQComboSlider(parent, min_, min_, max_);

    combo->setAutoFillBackground(true);

    return combo;
  }
  else {
    CQRealSpin *spin = new CQRealSpin(parent);

    spin->setRange(min_, max_);
    spin->setSingleStep(step_);
    spin->setDecimals(6);

    return spin;
  }
#else
  CQRealSpin *spin = new CQRealSpin(parent);

  spin->setRange(min_, max_);
  spin->setSingleStep(step_);
  spin->setDecimals(6);

  return spin;
#endif
}

void
CQPropertyViewRealEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
#if 0
  if      (type_ == Type::RealSlider) {
    CQRealSlider *slider = qobject_cast<CQRealSlider *>(w);
    assert(slider);

    QObject::connect(slider, SIGNAL(valueChanged(double)), obj, method);
  }
  else if (type_ == Type::ComboSlider) {
    CQComboSlider *combo = qobject_cast<CQComboSlider *>(w);
    assert(combo);

    QObject::connect(combo, SIGNAL(valueChanged(double)), obj, method);
  }
  else {
    CQRealSpin *spin = qobject_cast<CQRealSpin *>(w);
    assert(spin);

    QObject::connect(spin, SIGNAL(valueChanged(double)), obj, method);
  }
#else
  CQRealSpin *spin = qobject_cast<CQRealSpin *>(w);
  assert(spin);

  QObject::connect(spin, SIGNAL(valueChanged(double)), obj, method);
#endif
}

QVariant
CQPropertyViewRealEditor::
getValue(QWidget *w)
{
#if 0
  if      (type_ == Type::RealSlider) {
    CQRealSlider *slider = qobject_cast<CQRealSlider *>(w);
    assert(slider);

    return QVariant(slider->value());
  }
  else if (type_ == Type::ComboSlider) {
    CQComboSlider *combo = qobject_cast<CQComboSlider *>(w);
    assert(combo);

    return QVariant(combo->value());
  }
  else {
    CQRealSpin *spin = qobject_cast<CQRealSpin *>(w);
    assert(spin);

    return QVariant(spin->value());
  }
#else
  CQRealSpin *spin = qobject_cast<CQRealSpin *>(w);
  assert(spin);

  return QVariant(spin->value());
#endif
}

void
CQPropertyViewRealEditor::
setValue(QWidget *w, const QVariant &var)
{
#if 0
  if      (type_ == Type::RealSlider) {
    CQRealSlider *slider = qobject_cast<CQRealSlider *>(w);
    assert(slider);

    double r = var.toDouble();

    slider->setValue(r);
  }
  else if (type_ == Type::ComboSlider) {
    CQComboSlider *combo = qobject_cast<CQComboSlider *>(w);
    assert(combo);

    double r = var.toDouble();

    combo->setValue(r);
  }
  else {
    CQRealSpin *spin = qobject_cast<CQRealSpin *>(w);
    assert(spin);

    double r = var.toDouble();

    spin->setValue(r);
  }
#else
  CQRealSpin *spin = qobject_cast<CQRealSpin *>(w);
  assert(spin);

  double r = var.toDouble();

  spin->setValue(r);
#endif
}
