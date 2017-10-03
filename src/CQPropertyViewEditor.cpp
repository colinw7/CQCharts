#include <CQPropertyViewEditor.h>
#include <CQColorChooser.h>
#include <CQFontChooser.h>
#include <CQBBox2DEdit.h>
#include <CQPoint2DEdit.h>
#include <CQAlignEdit.h>

#if 0
#include <CQPaletteChooser.h>
#include <CQLineDash.h>
#include <CQAngleSpinBox.h>
#include <CQSlider.h>
#include <CQComboSlider.h>
#endif

#include <CQRealSpin.h>
#include <QSpinBox>
#include <cassert>

CQPropertyViewEditorMgr *
CQPropertyViewEditorMgr::
instance()
{
  static CQPropertyViewEditorMgr *instance;

  if (! instance)
    instance = new CQPropertyViewEditorMgr;

  return instance;
}

CQPropertyViewEditorMgr::
CQPropertyViewEditorMgr()
{
  setEditor("int"      , new CQPropertyViewIntegerEditor );
  setEditor("double"   , new CQPropertyViewRealEditor    );
#if 0
  setEditor("CAngle"   , new CQPropertyViewAngleEditor);
  setEditor("CLineDash", new CQPropertyViewLineDashEditor);
#endif
  setEditor("QColor"   , new CQPropertyViewColorEditor   );
  setEditor("QFont"    , new CQPropertyViewFontEditor    );
#if 0
  setEditor("QPalette" , new CQPropertyViewPaletteEditor );
#endif
  setEditor("QPointF"  , new CQPropertyViewPointEditor   );
  setEditor("QSizeF"   , new CQPropertyViewSizeFEditor   );
  setEditor("QRectF"   , new CQPropertyViewRectFEditor   );

  setEditor("Qt::Alignment", new CQPropertyViewAlignEditor);
}

void
CQPropertyViewEditorMgr::
setEditor(const QString &typeName, CQPropertyViewEditorFactory *editor)
{
  Editors::iterator p = editors_.find(typeName);

  if (p != editors_.end()) {
    std::swap((*p).second, editor);

    delete editor;
  }
  else
    (void) editors_.insert(p, Editors::value_type(typeName, editor));
}

CQPropertyViewEditorFactory *
CQPropertyViewEditorMgr::
getEditor(const QString &typeName) const
{
  Editors::const_iterator p = editors_.find(typeName);

  if (p != editors_.end())
    return (*p).second;
  else
    return 0;
}

//------

CQPropertyViewIntegerEditor::
CQPropertyViewIntegerEditor(int min, int max, int step) :
 min_(min), max_(max), step_(step)
{
}

QWidget *
CQPropertyViewIntegerEditor::
createEdit(QWidget *parent)
{
  QSpinBox *spin = new QSpinBox(parent);

  spin->setRange(min_, max_);
  spin->setSingleStep(step_);

  return spin;
}

void
CQPropertyViewIntegerEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  QSpinBox *spin = qobject_cast<QSpinBox *>(w);
  assert(spin);

  QObject::connect(spin, SIGNAL(valueChanged(int)), obj, method);
}

QVariant
CQPropertyViewIntegerEditor::
getValue(QWidget *w)
{
  QSpinBox *spin = qobject_cast<QSpinBox *>(w);
  assert(spin);

  return QVariant(spin->value());
}

void
CQPropertyViewIntegerEditor::
setValue(QWidget *w, const QVariant &var)
{
  QSpinBox *spin = qobject_cast<QSpinBox *>(w);
  assert(spin);

  int i = var.toInt();

  spin->setValue(i);
}

//------

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

//------

CQPropertyViewPointEditor::
CQPropertyViewPointEditor(double min, double max, double step) :
 min_(min), max_(max), step_(step)
{
}

QWidget *
CQPropertyViewPointEditor::
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
CQPropertyViewPointEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQPropertyViewPointEditor::
getValue(QWidget *w)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  return edit->getQValue();
}

void
CQPropertyViewPointEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQPoint2DEdit *edit = qobject_cast<CQPoint2DEdit *>(w);
  assert(edit);

  QPointF p = var.toPointF();

  edit->setValue(p);
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

//------

CQPropertyViewRectFEditor::
CQPropertyViewRectFEditor()
{
}

QWidget *
CQPropertyViewRectFEditor::
createEdit(QWidget *parent)
{
  CQBBox2DEdit *edit = new CQBBox2DEdit(parent);

  return edit;
}

void
CQPropertyViewRectFEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQBBox2DEdit *edit = qobject_cast<CQBBox2DEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQPropertyViewRectFEditor::
getValue(QWidget *w)
{
  CQBBox2DEdit *edit = qobject_cast<CQBBox2DEdit *>(w);
  assert(edit);

  QRectF rect = edit->getQValue();

  return rect;
}

void
CQPropertyViewRectFEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQBBox2DEdit *edit = qobject_cast<CQBBox2DEdit *>(w);
  assert(edit);

  QRectF rect = var.toRectF();

  edit->setValue(rect);
}

//------

CQPropertyViewColorEditor::
CQPropertyViewColorEditor()
{
}

QWidget *
CQPropertyViewColorEditor::
createEdit(QWidget *parent)
{
  CQColorChooser *chooser = new CQColorChooser(parent);

  chooser->setAutoFillBackground(true);

  chooser->setStyles(CQColorChooser::Text | CQColorChooser::ColorButton);

  return chooser;
}

void
CQPropertyViewColorEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQColorChooser *chooser = qobject_cast<CQColorChooser *>(w);
  assert(chooser);

  QObject::connect(chooser, SIGNAL(colorChanged(const QColor&)), obj, method);
}

QVariant
CQPropertyViewColorEditor::
getValue(QWidget *w)
{
  CQColorChooser *chooser = qobject_cast<CQColorChooser *>(w);
  assert(chooser);

  return chooser->colorName();
}

void
CQPropertyViewColorEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQColorChooser *chooser = qobject_cast<CQColorChooser *>(w);
  assert(chooser);

  QString str = var.toString();

  chooser->setColorName(str);
}

//------

CQPropertyViewFontEditor::
CQPropertyViewFontEditor()
{
}

QWidget *
CQPropertyViewFontEditor::
createEdit(QWidget *parent)
{
  CQFontChooser *chooser = new CQFontChooser(parent);

  return chooser;
}

void
CQPropertyViewFontEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQFontChooser *chooser = qobject_cast<CQFontChooser *>(w);
  assert(chooser);

  QObject::connect(chooser, SIGNAL(fontChanged(const QString&)), obj, method);
}

QVariant
CQPropertyViewFontEditor::
getValue(QWidget *w)
{
  CQFontChooser *chooser = qobject_cast<CQFontChooser *>(w);
  assert(chooser);

  return chooser->fontName();
}

void
CQPropertyViewFontEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQFontChooser *chooser = qobject_cast<CQFontChooser *>(w);
  assert(chooser);

  QString str = var.toString();

  chooser->setFontName(str);
}

//------

#if 0
CQPropertyViewPaletteEditor::
CQPropertyViewPaletteEditor()
{
}

QWidget *
CQPropertyViewPaletteEditor::
createEdit(QWidget *parent)
{
  CQPaletteChooser *chooser = new CQPaletteChooser(parent);

  return chooser;
}

void
CQPropertyViewPaletteEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQPaletteChooser *chooser = qobject_cast<CQPaletteChooser *>(w);
  assert(chooser);

  QObject::connect(chooser, SIGNAL(paletteChanged(const QString&)), obj, method);
}

QVariant
CQPropertyViewPaletteEditor::
getValue(QWidget *w)
{
  CQPaletteChooser *chooser = qobject_cast<CQPaletteChooser *>(w);
  assert(chooser);

  return chooser->palette();
}

void
CQPropertyViewPaletteEditor::
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
CQPropertyViewLineDashEditor::
CQPropertyViewLineDashEditor()
{
}

QWidget *
CQPropertyViewLineDashEditor::
createEdit(QWidget *parent)
{
  CQLineDash *edit = new CQLineDash(parent);

  return edit;
}

void
CQPropertyViewLineDashEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQLineDash *edit = qobject_cast<CQLineDash *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged(const CLineDash &)), obj, method);
}

QVariant
CQPropertyViewLineDashEditor::
getValue(QWidget *w)
{
  CQLineDash *edit = qobject_cast<CQLineDash *>(w);
  assert(edit);

  return QVariant::fromValue(edit->getLineDash());
}

void
CQPropertyViewLineDashEditor::
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
CQPropertyViewAngleEditor::
CQPropertyViewAngleEditor()
{
}

QWidget *
CQPropertyViewAngleEditor::
createEdit(QWidget *parent)
{
  CQAngleSpinBox *edit = new CQAngleSpinBox(parent);

  return edit;
}

void
CQPropertyViewAngleEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQAngleSpinBox *edit = qobject_cast<CQAngleSpinBox *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(angleChanged(const CAngle &)), obj, method);
}

QVariant
CQPropertyViewAngleEditor::
getValue(QWidget *w)
{
  CQAngleSpinBox *edit = qobject_cast<CQAngleSpinBox *>(w);
  assert(edit);

  return QVariant::fromValue(edit->getAngle());
}

void
CQPropertyViewAngleEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQAngleSpinBox *edit = qobject_cast<CQAngleSpinBox *>(w);
  assert(edit);

  CAngle angle = var.value<CAngle>();

  edit->setAngle(angle);
}
#endif

//------

CQPropertyViewAlignEditor::
CQPropertyViewAlignEditor()
{
}

QWidget *
CQPropertyViewAlignEditor::
createEdit(QWidget *parent)
{
  CQAlignEdit *edit = new CQAlignEdit(parent);

  return edit;
}

void
CQPropertyViewAlignEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQAlignEdit *edit = qobject_cast<CQAlignEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged(Qt::Alignment)), obj, method);
}

QVariant
CQPropertyViewAlignEditor::
getValue(QWidget *w)
{
  CQAlignEdit *edit = qobject_cast<CQAlignEdit *>(w);
  assert(edit);

  return QVariant(edit->align());
}

void
CQPropertyViewAlignEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQAlignEdit *edit = qobject_cast<CQAlignEdit *>(w);
  assert(edit);

  int i = var.toInt();

  edit->setAlign((Qt::Alignment) i);
}
