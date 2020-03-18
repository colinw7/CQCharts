#include <CQChartsPaletteNameEdit.h>
#include <CQCharts.h>
#include <CQChartsPlot.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQColorsTheme.h>
#include <CQColorsPalette.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QHBoxLayout>
#include <QPainter>

CQChartsPaletteNameEdit::
CQChartsPaletteNameEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("paletteName");

  setToolTip("Palette Name");

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  combo_ = CQUtil::makeWidget<QComboBox>("combo");

  layout->addWidget(combo_);

  connectSlots(true);
}

void
CQChartsPaletteNameEdit::
setCharts(const CQCharts *charts)
{
  const CQColorsTheme *theme = charts->theme();

  int n = theme->numPalettes();

  combo_->clear();

  combo_->addItem("");

  for (int i = 0; i < n; ++i)
    combo_->addItem(theme->palette(i)->name());
}

void
CQChartsPaletteNameEdit::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
}

const CQChartsPaletteName &
CQChartsPaletteNameEdit::
paletteName() const
{
  return name_;
}

void
CQChartsPaletteNameEdit::
setPaletteName(const CQChartsPaletteName &name)
{
  connectSlots(false);

  name_ = name;

  combo_->setCurrentIndex(combo_->findText(name_.toString()));

  connectSlots(true);
}

void
CQChartsPaletteNameEdit::
comboChanged()
{
  connectSlots(false);

  name_ = CQChartsPaletteName(combo_->currentText());

  connectSlots(true);

  emit nameChanged();
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsPaletteNamePropertyViewType::
CQChartsPaletteNamePropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsPaletteNamePropertyViewType::
getEditor() const
{
  return new CQChartsPaletteNamePropertyViewEditor;
}

bool
CQChartsPaletteNamePropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

QString
CQChartsPaletteNamePropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsPaletteName>().toString();

  return str;
}

//------

CQChartsPaletteNamePropertyViewEditor::
CQChartsPaletteNamePropertyViewEditor()
{
}

QWidget *
CQChartsPaletteNamePropertyViewEditor::
createEdit(QWidget *parent)
{
  CQPropertyViewItem *item = CQPropertyViewMgrInst->editItem();

  QObject *obj = (item ? item->object() : nullptr);

  auto *plot = qobject_cast<CQChartsPlot *>(obj);

  //---

  CQChartsPaletteNameEdit *edit = new CQChartsPaletteNameEdit(parent);

  if (plot)
    edit->setCharts(plot->charts());

  return edit;
}

void
CQChartsPaletteNamePropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsPaletteNameEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(nameChanged()), obj, method);
}

QVariant
CQChartsPaletteNamePropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsPaletteNameEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->paletteName());
}

void
CQChartsPaletteNamePropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsPaletteNameEdit *>(w);
  assert(edit);

  CQChartsPaletteName name = var.value<CQChartsPaletteName>();

  edit->setPaletteName(name);
}
