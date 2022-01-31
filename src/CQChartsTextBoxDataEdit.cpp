#include <CQChartsTextBoxDataEdit.h>
#include <CQChartsTextDataEdit.h>
#include <CQChartsBoxDataEdit.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQUtil.h>
#include <CQTabWidget.h>

#include <QLabel>
#include <QVBoxLayout>

CQChartsTextBoxDataLineEdit::
CQChartsTextBoxDataLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("textBoxDataLineEdit");

  setToolTip("Text Box Data");

  //---

  menuEdit_ = dataEdit_ = new CQChartsTextBoxDataEdit;

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

  textBoxDataToWidgets();
}

const CQChartsTextBoxData &
CQChartsTextBoxDataLineEdit::
textBoxData() const
{
  return dataEdit_->data();
}

void
CQChartsTextBoxDataLineEdit::
setTextBoxData(const CQChartsTextBoxData &textBoxData)
{
  updateTextBoxData(textBoxData, /*updateText*/ true);
}

void
CQChartsTextBoxDataLineEdit::
updateTextBoxData(const CQChartsTextBoxData &textBoxData, bool updateText)
{
  connectSlots(false);

  dataEdit_->setData(textBoxData);

  connectSlots(true);

  if (updateText)
    textBoxDataToWidgets();

  emit textBoxDataChanged();
}

void
CQChartsTextBoxDataLineEdit::
textChanged()
{
  CQChartsTextBoxData textBoxData(edit_->text());

  if (! textBoxData.isValid())
    return;

  updateTextBoxData(textBoxData, /*updateText*/ false);
}

void
CQChartsTextBoxDataLineEdit::
textBoxDataToWidgets()
{
  connectSlots(false);

  if (textBoxData().isValid())
    edit_->setText(textBoxData().toString());
  else
    edit_->setText("");

  auto tip = QString("%1 (%2)").arg(textBoxData().toString());

  edit_->setToolTip(tip);

  connectSlots(true);
}

void
CQChartsTextBoxDataLineEdit::
menuEditChanged()
{
  textBoxDataToWidgets();

  emit textBoxDataChanged();
}

void
CQChartsTextBoxDataLineEdit::
connectSlots(bool b)
{
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(textBoxDataChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsTextBoxDataLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  CQChartsTextBoxDataEditPreview::draw(painter, textBoxData(), rect, plot(), view());
}

//------

CQPropertyViewEditorFactory *
CQChartsTextBoxDataPropertyViewType::
getEditor() const
{
  return new CQChartsTextBoxDataPropertyViewEditor;
}

void
CQChartsTextBoxDataPropertyViewType::
drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
            CQChartsPlot *plot, CQChartsView *view)
{
  auto data = CQChartsTextBoxData::fromVariant(value);

  CQChartsTextBoxDataEditPreview::draw(painter, data, rect, plot, view);
}

QString
CQChartsTextBoxDataPropertyViewType::
tip(const QVariant &value) const
{
  auto str = CQChartsTextBoxData::fromVariant(value).toString();

  return str;
}

//------

CQChartsLineEditBase *
CQChartsTextBoxDataPropertyViewEditor::
createPropertyEdit(QWidget *parent)
{
  return new CQChartsTextBoxDataLineEdit(parent);
}

void
CQChartsTextBoxDataPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsTextBoxDataLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(textBoxDataChanged()), obj, method);
}

QVariant
CQChartsTextBoxDataPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsTextBoxDataLineEdit *>(w);
  assert(edit);

  return CQChartsTextBoxData::toVariant(edit->textBoxData());
}

void
CQChartsTextBoxDataPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsTextBoxDataLineEdit *>(w);
  assert(edit);

  auto data = CQChartsTextBoxData::fromVariant(var);

  edit->setTextBoxData(data);
}

//------

CQChartsTextBoxDataEdit::
CQChartsTextBoxDataEdit(QWidget *parent, bool tabbed) :
 CQChartsEditBase(parent), tabbed_(tabbed)
{
  setObjectName("textBoxDataEdit");

  auto *layout = CQUtil::makeLayout<QGridLayout>(this, 0, 2);

  int row = 0;

  //---

  if (tabbed_) {
    auto *tab = CQUtil::makeWidget<CQTabWidget>("tab");

    layout->addWidget(tab, row, 0, 1, 2); ++row;

    //----

    // text frame
    auto *textFrame       = CQUtil::makeWidget<QFrame>("textFrame");
    auto *textFrameLayout = CQUtil::makeLayout<QVBoxLayout>(textFrame, 0, 2);

    tab->addTab(textFrame, "Text");

    //--

    // text
    textEdit_ = new CQChartsTextDataEdit;

    textEdit_->setPreview(false);

    textFrameLayout->addWidget(textEdit_);

    //----

    // box frame
    auto *boxFrame       = CQUtil::makeWidget<QFrame>("boxFrame");
    auto *boxFrameLayout = CQUtil::makeLayout<QVBoxLayout>(boxFrame, 0, 2);

    tab->addTab(boxFrame, "Box");

    //--

    // box
    boxEdit_ = new CQChartsBoxDataEdit;

    boxEdit_->setPreview(false);

    boxFrameLayout->addWidget(boxEdit_);
  }
  else {
    // text
    textEdit_ = new CQChartsTextDataEdit;

    textEdit_->setTitle("Text");
    textEdit_->setPreview(false);

    layout->addWidget(textEdit_, row, 0, 1, 2); ++row;

    //--

    // box
    boxEdit_ = new CQChartsBoxDataEdit;

    boxEdit_->setTitle("Box");
    boxEdit_->setPreview(false);

    layout->addWidget(boxEdit_, row, 0, 1, 2); ++row;
  }

  //---

  // preview
  preview_ = new CQChartsTextBoxDataEditPreview(this);

  layout->addWidget(preview_, row, 1);

  //---

  layout->setRowStretch(row, 1);

  //---

  connectSlots(true);

  dataToWidgets();
}

void
CQChartsTextBoxDataEdit::
setData(const CQChartsTextBoxData &d)
{
  data_ = d;

  dataToWidgets();
}

void
CQChartsTextBoxDataEdit::
setPlot(CQChartsPlot *plot)
{
  CQChartsEditBase::setPlot(plot);

  textEdit_->setPlot(plot);
  boxEdit_ ->setPlot(plot);
}

void
CQChartsTextBoxDataEdit::
setView(CQChartsView *view)
{
  CQChartsEditBase::setView(view);

  textEdit_->setView(view);
  boxEdit_ ->setView(view);
}

void
CQChartsTextBoxDataEdit::
setTitle(const QString &)
{
  if (! tabbed_) {
    textEdit_->setTitle("Text");
    boxEdit_ ->setTitle("Box" );
  }
}

void
CQChartsTextBoxDataEdit::
setPreview(bool b)
{
  preview_->setVisible(b);

  textEdit_->setPreview(b);
  boxEdit_ ->setPreview(b);
}

void
CQChartsTextBoxDataEdit::
connectSlots(bool b)
{
  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(b, w, from, this, to);
  };

  connectDisconnect(b, textEdit_, SIGNAL(textDataChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, boxEdit_ , SIGNAL(boxDataChanged()) , SLOT(widgetsToData()));
}

void
CQChartsTextBoxDataEdit::
dataToWidgets()
{
  connectSlots(false);

  textEdit_->setData(data_.text());
  boxEdit_ ->setData(data_.box());

  preview_->update();
}

void
CQChartsTextBoxDataEdit::
widgetsToData()
{
  data_.setText(textEdit_->data());
  data_.setBox (boxEdit_ ->data());

  preview_->update();

  emit textBoxDataChanged();
}

//------

CQChartsTextBoxDataEditPreview::
CQChartsTextBoxDataEditPreview(CQChartsTextBoxDataEdit *edit) :
 CQChartsEditPreview(edit), edit_(edit)
{
  setToolTip("Text Box Preview");
}

void
CQChartsTextBoxDataEditPreview::
draw(QPainter *painter)
{
  const auto &data = edit_->data();

  draw(painter, data, rect(), edit_->plot(), edit_->view());
}

void
CQChartsTextBoxDataEditPreview::
draw(QPainter *painter, const CQChartsTextBoxData &data, const QRect &rect,
     CQChartsPlot *plot, CQChartsView *view)
{
  const auto &box   = data.box();
  const auto &shape = box.shape();

  //---

  // set pen
  auto pc = interpColor(plot, view, shape.stroke().color());

  QPen pen;

  double width = shape.stroke().width().value();

  width = (plot ? plot->limitLineWidth(width) : view->limitLineWidth(width));

  CQChartsUtil::setPen(pen, shape.stroke().isVisible(), pc,
                       shape.stroke().alpha(), width, shape.stroke().dash());

  painter->setPen(pen);

  //---

  // set brush
  auto fc = interpColor(plot, view, shape.fill().color());

  QBrush brush;

  CQChartsBrushData brushData;

  brushData.setVisible(shape.fill().isVisible());
  brushData.setColor  (fc);
  brushData.setAlpha  (shape.fill().alpha());
  brushData.setPattern(shape.fill().pattern());

  CQChartsDrawUtil::setBrush(brush, brushData);

  painter->setBrush(brush);

  //---

  // draw text box
  CQChartsPixelPaintDevice device(painter);

  CQChartsDrawUtil::drawRoundedRect(&device, CQChartsGeom::BBox(rect),
                                    shape.stroke().cornerSize());
}
