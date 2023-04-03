#include <CQChartsModelIndEdit.h>
#include <CQChartsLineEdit.h>
#include <CQChartsPlot.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsObjUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>

#include <QGridLayout>
#include <QPainter>

CQChartsModelIndLineEdit::
CQChartsModelIndLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("modelIndLineEdit");

  setToolTip("Model Ind (use empty string to unset)");

  //---

  menuEdit_ = dataEdit_ = new CQChartsModelIndEdit(this);

  setNoFocus();

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

  modelIndToWidgets();
}

void
CQChartsModelIndLineEdit::
setView(CQChartsView *view)
{
  dataEdit_->setCharts(view->charts());

  CQChartsLineEditBase::setView(view);
}

void
CQChartsModelIndLineEdit::
setPlot(CQChartsPlot *plot)
{
  dataEdit_->setCharts(plot->charts());

  CQChartsLineEditBase::setPlot(plot);
}

const CQChartsModelInd &
CQChartsModelIndLineEdit::
modelInd() const
{
  return dataEdit_->modelInd();
}

void
CQChartsModelIndLineEdit::
setModelInd(const ModelInd &modelInd)
{
  updateModelInd(modelInd, /*updateText*/true);
}

void
CQChartsModelIndLineEdit::
setNoFocus()
{
  dataEdit_->setNoFocus();
}

void
CQChartsModelIndLineEdit::
updateModelInd(const ModelInd &modelInd, bool updateText)
{
  connectSlots(false);

  dataEdit_->setModelInd(modelInd);

  connectSlots(true);

  if (updateText)
    modelIndToWidgets();

  Q_EMIT modelIndChanged();
}

void
CQChartsModelIndLineEdit::
textChanged()
{
  // update modelInd from widget (text)
  ModelInd modelInd;

  auto text = edit_->text();

  if (text.trimmed() != "") {
    modelInd = ModelInd(text);

    if (dataEdit_->charts())
      modelInd.setCharts(dataEdit_->charts());
  }

  updateModelInd(modelInd, /*updateText*/false);
}

void
CQChartsModelIndLineEdit::
modelIndToWidgets()
{
  // update widget from current model ind
  connectSlots(false);

  QString text;

  if (modelInd().isValid())
    text = modelInd().toString();

  edit_->setText(text);

  edit_->setToolTip(text);

  connectSlots(true);
}

void
CQChartsModelIndLineEdit::
menuEditChanged()
{
  modelIndToWidgets();

  Q_EMIT modelIndChanged();
}

void
CQChartsModelIndLineEdit::
connectSlots(bool b)
{
  connectBaseSlots(b);

  CQUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(modelIndChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsModelIndLineEdit::
drawPreview(QPainter *painter, const QRect &)
{
  auto str = (modelInd().isValid() ? modelInd().toString() : "<none>");

  drawCenteredText(painter, str);
}

void
CQChartsModelIndLineEdit::
updateMenu()
{
  // update menu width
  CQChartsLineEditBase::updateMenu();

  // update menu height
  dataEdit_->updateMenu();
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsModelIndPropertyViewType::
CQChartsModelIndPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsModelIndPropertyViewType::
getEditor() const
{
  return new CQChartsModelIndPropertyViewEditor;
}

bool
CQChartsModelIndPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsModelIndPropertyViewType::
draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  //---

  bool ok;

  auto str = valueString(item, value, ok);

  auto font = option.font;

  if (! ok)
    font.setItalic(true);

  //---

  QFontMetrics fm(font);

  int w = fm.horizontalAdvance(str);

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  option1.font = font;

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsModelIndPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  auto str = valueString(nullptr, value, ok);

  return str;
}

QString
CQChartsModelIndPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  auto modelInd = ModelInd::fromVariant(value);

  QString str;

  if (modelInd.isValid()) {
    str = modelInd.toString();
    ok  = true;
  }
  else {
    str = "<none>";
    ok  = false;
  }

  return str;
}

//------

CQChartsModelIndPropertyViewEditor::
CQChartsModelIndPropertyViewEditor()
{
}

QWidget *
CQChartsModelIndPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *item = CQPropertyViewMgrInst->editItem();
  auto *obj  = (item ? item->object() : nullptr);
  if (! obj) return nullptr;

  //---

  CQChartsObjUtil::ObjData objData;

  CQChartsObjUtil::getObjData(obj, objData);

  //---

  auto *edit = new CQChartsModelIndLineEdit(parent);

  if      (objData.plot)
    edit->setPlot(objData.plot);
  else if (objData.view)
    edit->setView(objData.view);

  return edit;
}

void
CQChartsModelIndPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsModelIndLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(modelIndChanged()), obj, method);
}

QVariant
CQChartsModelIndPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsModelIndLineEdit *>(w);
  assert(edit);

  return CQChartsModelInd::toVariant(edit->modelInd());
}

void
CQChartsModelIndPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsModelIndLineEdit *>(w);
  assert(edit);

  auto modelInd = ModelInd::fromVariant(var);

  edit->setModelInd(modelInd);
}

//------

CQChartsModelIndEdit::
CQChartsModelIndEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("modelIndEdit");

  lineEdit_ = qobject_cast<CQChartsModelIndLineEdit *>(parent);

  //---

  auto *layout = CQUtil::makeLayout<QGridLayout>(this, 2, 2);

  //---

  int row = 0;

  layout->setRowStretch(row, 1);

  layout->setColumnStretch(1, 1);

  //---

  connectSlots(true);

  updateState();
}

void
CQChartsModelIndEdit::
setCharts(CQCharts *charts)
{
  charts_ = charts;

  modelIndToWidgets();

  updateState();
}

void
CQChartsModelIndEdit::
setModelInd(const ModelInd &modelInd)
{
  modelInd_ = modelInd;

  modelIndToWidgets();

  updateState();

  Q_EMIT modelIndChanged();
}

void
CQChartsModelIndEdit::
modelIndToWidgets()
{
  connectSlots(false);

  connectSlots(true);
}

void
CQChartsModelIndEdit::
widgetsToModelInd()
{
  ModelInd modelInd;

  //---

  modelInd_ = modelInd;

  //---

  updateState();

  Q_EMIT modelIndChanged();
}

void
CQChartsModelIndEdit::
updateMenu()
{
  if (lineEdit_)
    lineEdit_->updateMenuEditHeight();
}

void
CQChartsModelIndEdit::
setNoFocus()
{
}

void
CQChartsModelIndEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;
}

void
CQChartsModelIndEdit::
typeSlot()
{
  widgetsToModelIndSlot();

  if (lineEdit_)
    lineEdit_->updateMenuEditHeight();
}

void
CQChartsModelIndEdit::
widgetsToModelIndSlot()
{
  connectSlots(false);

  widgetsToModelInd();

  connectSlots(true);

  Q_EMIT modelIndChanged();
}

void
CQChartsModelIndEdit::
updateState()
{
}

QSize
CQChartsModelIndEdit::
sizeHint() const
{
  int w = width();

  return QSize(w, widgetHeight_);
}

QSize
CQChartsModelIndEdit::
minimumSizeHint() const
{
  QFontMetrics fm(font());

  int eh = fm.height() + 8;

  int n = 1;

  return QSize(0, eh*n);
}
