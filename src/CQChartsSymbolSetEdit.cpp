#include <CQChartsSymbolSetEdit.h>
#include <CQCharts.h>
#include <CQChartsPlot.h>
#include <CQChartsObjUtil.h>

//#include <CQPropertyView.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QHBoxLayout>
#include <QPainter>

CQChartsSymbolSetEdit::
CQChartsSymbolSetEdit(QWidget *parent) :
 CQChartsFrame(parent)
{
  setObjectName("symbolSet");

  setToolTip("Symbol Set Name");

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  combo_ = CQUtil::makeWidget<QComboBox>("combo");

  layout->addWidget(combo_);

  connectSlots(true);
}

void
CQChartsSymbolSetEdit::
setCharts(const CQCharts *charts)
{
  connectSlots(false);

  CQChartsFrame::setCharts(charts);

  const auto *symbolSetMgr = charts_->symbolSetMgr();

  int n = symbolSetMgr->numSymbolSets();

  combo_->clear();

  combo_->addItem("");

  for (int i = 0; i < n; ++i)
    combo_->addItem(symbolSetMgr->symbolSet(i)->name());

  connectSlots(true);
}

const QString &
CQChartsSymbolSetEdit::
symbolSetName() const
{
  return symbolSetName_;
}

void
CQChartsSymbolSetEdit::
setSymbolSetName(const QString &name)
{
  connectSlots(false);

  symbolSetName_ = name;

  combo_->setCurrentIndex(combo_->findText(symbolSetName_));

  connectSlots(true);
}

void
CQChartsSymbolSetEdit::
setChartsSymbolSetName(CQCharts *charts, const QString &name)
{
  setCharts(charts);

  setSymbolSetName(name);
}

void
CQChartsSymbolSetEdit::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b,
    combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
}

void
CQChartsSymbolSetEdit::
comboChanged()
{
  connectSlots(false);

  symbolSetName_ = combo_->currentText();

  connectSlots(true);

  Q_EMIT setChanged(symbolSetName_);
}

//------

#if 0
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsSymbolSetPropertyViewType::
CQChartsSymbolSetPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsSymbolSetPropertyViewType::
getEditor() const
{
  return new CQChartsSymbolSetPropertyViewEditor;
}

bool
CQChartsSymbolSetPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

QString
CQChartsSymbolSetPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;
  auto str = CQChartsVariant::toString(value, ok);

  return str;
}
#endif

//------

#if 0
CQChartsSymbolSetPropertyViewEditor::
CQChartsSymbolSetPropertyViewEditor()
{
}

QWidget *
CQChartsSymbolSetPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *item = CQPropertyViewMgrInst->editItem();

  auto *obj = (item ? item->object() : nullptr);

  CQChartsObjUtil::ObjData objData;

  CQChartsObjUtil::getObjData(obj, objData);

  //---

  auto *edit = new CQChartsSymbolSetEdit(parent);

  if (objData.charts)
    edit->setCharts(objData.charts);

  return edit;
}

void
CQChartsSymbolSetPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsSymbolSetEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(setChanged(const QString &)), obj, method);
}

QVariant
CQChartsSymbolSetPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsSymbolSetEdit *>(w);
  assert(edit);

  return CQChartsSymbolSet::toVariant(edit->symbolSetName());
}

void
CQChartsSymbolSetPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsSymbolSetEdit *>(w);
  assert(edit);

  bool ok;
  auto name = CQChartsVariant::toString(var, ok);

  edit->setSymbolSetName(name);
}
#endif
