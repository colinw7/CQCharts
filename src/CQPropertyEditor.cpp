#include <CQPropertyEditor.h>
#include <CQColorChooser.h>
#include <CQFontChooser.h>
#include <CQBBox2DEdit.h>
#include <CQPoint2DEdit.h>

#if 0
#include <CQPaletteChooser.h>
#include <CQLineDash.h>
#include <CQAngleSpinBox.h>
#include <CQSlider.h>
#include <CQComboSlider.h>
#endif

#include <CQRealSpin.h>
#include <CQUtil.h>
#include <QSpinBox>
#include <cassert>

CQPropertyEditorMgr *
CQPropertyEditorMgr::
instance()
{
  static CQPropertyEditorMgr *instance;

  if (! instance)
    instance = new CQPropertyEditorMgr;

  return instance;
}

CQPropertyEditorMgr::
CQPropertyEditorMgr()
{
#if 0
  setEditor("CAngle"   , new CQPropertyAngleEditor);
  setEditor("CLineDash", new CQPropertyLineDashEditor);
#endif
  setEditor("QColor"   , new CQPropertyColorEditor   );
  setEditor("QFont"    , new CQPropertyFontEditor    );
#if 0
  setEditor("QPalette" , new CQPropertyPaletteEditor );
#endif
  setEditor("QPointF"  , new CQPropertyPointEditor   );
  setEditor("QSizeF"   , new CQPropertySizeFEditor   );
  setEditor("QRectF"   , new CQPropertyRectFEditor   );
}

void
CQPropertyEditorMgr::
setEditor(const QString &typeName, CQPropertyEditorFactory *editor)
{
  Editors::iterator p = editors_.find(typeName);

  if (p != editors_.end()) {
    std::swap((*p).second, editor);

    delete editor;
  }
  else
    (void) editors_.insert(p, Editors::value_type(typeName, editor));
}

CQPropertyEditorFactory *
CQPropertyEditorMgr::
getEditor(const QString &typeName) const
{
  Editors::const_iterator p = editors_.find(typeName);

  if (p != editors_.end())
    return (*p).second;
  else
    return 0;
}

//------

CQPropertyIntegerEditor::
CQPropertyIntegerEditor(int min, int max, int step) :
 min_(min), max_(max), step_(step)
{
}

QWidget *
CQPropertyIntegerEditor::
createEdit(QWidget *parent)
{
  QSpinBox *spin = new QSpinBox(parent);

  spin->setRange(min_, max_);
  spin->setSingleStep(step_);

  return spin;
}

void
CQPropertyIntegerEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  QSpinBox *spin = qobject_cast<QSpinBox *>(w);
  assert(spin);

  QObject::connect(spin, SIGNAL(valueChanged(int)), obj, method);
}

QVariant
CQPropertyIntegerEditor::
getValue(QWidget *w)
{
  QSpinBox *spin = qobject_cast<QSpinBox *>(w);
  assert(spin);

  return QVariant(spin->value());
}

void
CQPropertyIntegerEditor::
setValue(QWidget *w, const QVariant &var)
{
  QSpinBox *spin = qobject_cast<QSpinBox *>(w);
  assert(spin);

  int i = var.toInt();

  spin->setValue(i);
}

//------

CQPropertyRealEditor::
CQPropertyRealEditor(double min, double max, double step) :
 type_(Type::DoubleSpin), min_(min), max_(max), step_(step)
{
}

QWidget *
CQPropertyRealEditor::
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
CQPropertyRealEditor::
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
CQPropertyRealEditor::
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
CQPropertyRealEditor::
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

//------

CQPropertyPointEditor::
CQPropertyPointEditor(double min, double max, double step) :
 min_(min), max_(max), step_(step)
{
}

QWidget *
CQPropertyPointEditor::
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
CQPropertyPointEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQPropertyPointEditor::
getValue(QWidget *w)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  return edit->getQValue();
}

void
CQPropertyPointEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  QPointF p = var.toPointF();

  edit->setValue(p);
}

//------

CQPropertySizeFEditor::
CQPropertySizeFEditor(double max, double step) :
 max_(max), step_(step)
{
}

QWidget *
CQPropertySizeFEditor::
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
CQPropertySizeFEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQPropertySizeFEditor::
getValue(QWidget *w)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  QPointF p = edit->getQValue();

  return QSizeF(p.x(), p.y());
}

void
CQPropertySizeFEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  QSizeF s = var.toSizeF();

  edit->setValue(QPointF(s.width(), s.height()));
}

//------

CQPropertyRectFEditor::
CQPropertyRectFEditor()
{
}

QWidget *
CQPropertyRectFEditor::
createEdit(QWidget *parent)
{
  CQBBox2DEdit *edit = new CQBBox2DEdit(parent);

  return edit;
}

void
CQPropertyRectFEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQBBox2DEdit *edit = qobject_cast<CQBBox2DEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQPropertyRectFEditor::
getValue(QWidget *w)
{
  CQBBox2DEdit *edit = qobject_cast<CQBBox2DEdit *>(w);
  assert(edit);

  QRectF rect = edit->getQValue();

  return rect;
}

void
CQPropertyRectFEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQBBox2DEdit *edit = qobject_cast<CQBBox2DEdit *>(w);
  assert(edit);

  QRectF rect = var.toRectF();

  edit->setValue(rect);
}

//------

CQPropertyColorEditor::
CQPropertyColorEditor()
{
}

QWidget *
CQPropertyColorEditor::
createEdit(QWidget *parent)
{
  CQColorChooser *chooser = new CQColorChooser(parent);

  chooser->setAutoFillBackground(true);

  chooser->setStyles(CQColorChooser::Text | CQColorChooser::ColorButton);

  return chooser;
}

void
CQPropertyColorEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQColorChooser *chooser = qobject_cast<CQColorChooser *>(w);
  assert(chooser);

  QObject::connect(chooser, SIGNAL(colorChanged(const QColor&)), obj, method);
}

QVariant
CQPropertyColorEditor::
getValue(QWidget *w)
{
  CQColorChooser *chooser = qobject_cast<CQColorChooser *>(w);
  assert(chooser);

  return chooser->colorName();
}

void
CQPropertyColorEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQColorChooser *chooser = qobject_cast<CQColorChooser *>(w);
  assert(chooser);

  QString str = var.toString();

  chooser->setColorName(str);
}

//------

CQPropertyFontEditor::
CQPropertyFontEditor()
{
}

QWidget *
CQPropertyFontEditor::
createEdit(QWidget *parent)
{
  CQFontChooser *chooser = new CQFontChooser(parent);

  return chooser;
}

void
CQPropertyFontEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQFontChooser *chooser = qobject_cast<CQFontChooser *>(w);
  assert(chooser);

  QObject::connect(chooser, SIGNAL(fontChanged(const QString&)), obj, method);
}

QVariant
CQPropertyFontEditor::
getValue(QWidget *w)
{
  CQFontChooser *chooser = qobject_cast<CQFontChooser *>(w);
  assert(chooser);

  return chooser->fontName();
}

void
CQPropertyFontEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQFontChooser *chooser = qobject_cast<CQFontChooser *>(w);
  assert(chooser);

  QString str = var.toString();

  chooser->setFontName(str);
}

//------

#if 0
CQPropertyPaletteEditor::
CQPropertyPaletteEditor()
{
}

QWidget *
CQPropertyPaletteEditor::
createEdit(QWidget *parent)
{
  CQPaletteChooser *chooser = new CQPaletteChooser(parent);

  return chooser;
}

void
CQPropertyPaletteEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQPaletteChooser *chooser = qobject_cast<CQPaletteChooser *>(w);
  assert(chooser);

  QObject::connect(chooser, SIGNAL(paletteChanged(const QString&)), obj, method);
}

QVariant
CQPropertyPaletteEditor::
getValue(QWidget *w)
{
  CQPaletteChooser *chooser = qobject_cast<CQPaletteChooser *>(w);
  assert(chooser);

  return chooser->palette();
}

void
CQPropertyPaletteEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQPaletteChooser *chooser = qobject_cast<CQPaletteChooser *>(w);
  assert(chooser);

  QString str = var.toString();

  chooser->setPaletteDef(str);
}
#endif

//------

#if 0
CQPropertyLineDashEditor::
CQPropertyLineDashEditor()
{
}

QWidget *
CQPropertyLineDashEditor::
createEdit(QWidget *parent)
{
  CQLineDash *edit = new CQLineDash(parent);

  return edit;
}

void
CQPropertyLineDashEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQLineDash *edit = qobject_cast<CQLineDash *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged(const CLineDash &)), obj, method);
}

QVariant
CQPropertyLineDashEditor::
getValue(QWidget *w)
{
  CQLineDash *edit = qobject_cast<CQLineDash *>(w);
  assert(edit);

  return QVariant::fromValue(edit->getLineDash());
}

void
CQPropertyLineDashEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQLineDash *edit = qobject_cast<CQLineDash *>(w);
  assert(edit);

  CLineDash dash = var.value<CLineDash>();

  edit->setLineDash(dash);
}
#endif

//------

#if 0
CQPropertyAngleEditor::
CQPropertyAngleEditor()
{
}

QWidget *
CQPropertyAngleEditor::
createEdit(QWidget *parent)
{
  CQAngleSpinBox *edit = new CQAngleSpinBox(parent);

  return edit;
}

void
CQPropertyAngleEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQAngleSpinBox *edit = qobject_cast<CQAngleSpinBox *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(angleChanged(const CAngle &)), obj, method);
}

QVariant
CQPropertyAngleEditor::
getValue(QWidget *w)
{
  CQAngleSpinBox *edit = qobject_cast<CQAngleSpinBox *>(w);
  assert(edit);

  return QVariant::fromValue(edit->getAngle());
}

void
CQPropertyAngleEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQAngleSpinBox *edit = qobject_cast<CQAngleSpinBox *>(w);
  assert(edit);

  CAngle angle = var.value<CAngle>();

  edit->setAngle(angle);
}
#endif
