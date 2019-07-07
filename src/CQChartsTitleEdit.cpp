#include <CQChartsTitleEdit.h>
#include <CQChartsTitle.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsTitleLocationEdit.h>
#include <CQChartsPositionEdit.h>
#include <CQChartsRectEdit.h>
#include <CQChartsTextDataEdit.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQGroupBox.h>
#include <CQCheckBox.h>
#include <CQUtil.h>

#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>

CQChartsEditTitleDlg::
CQChartsEditTitleDlg(QWidget *parent, CQChartsTitle *title) :
 QDialog(parent), title_(title)
{
  setWindowTitle(QString("Edit Plot Title (%1)").arg(title->plot()->id()));

  //---

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  edit_ = new CQChartsTitleEdit(nullptr, title_);

  layout->addWidget(edit_);

  //---

  CQChartsDialogButtons *buttons = new CQChartsDialogButtons;

  buttons->connect(this, SLOT(okSlot()), SLOT(applySlot()), SLOT(cancelSlot()));

  layout->addWidget(buttons);
}

void
CQChartsEditTitleDlg::
okSlot()
{
  if (applySlot())
    cancelSlot();
}

bool
CQChartsEditTitleDlg::
applySlot()
{
  edit_->applyData();

  return true;
}

void
CQChartsEditTitleDlg::
cancelSlot()
{
  close();
}

//------

CQChartsTitleEdit::
CQChartsTitleEdit(QWidget *parent, CQChartsTitle *title) :
 QFrame(parent), title_(title)
{
  setObjectName("titleEdit");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  data_.visible    = title->isVisible();
  data_.location   = title->location();
  data_.position   = title->absolutePosition();
  data_.rect       = title->absoluteRectangle();
  data_.insidePlot = title->isInsidePlot();
  data_.textData   = title->textData();

  //---

  // visible
  groupBox_ = CQUtil::makeWidget<CQGroupBox>("groupBox");

  groupBox_->setCheckable(true);
  groupBox_->setChecked(data_.visible);
  groupBox_->setTitle("Visible");

  layout->addWidget(groupBox_);

  //---

  QGridLayout *groupLayout = CQUtil::makeLayout<QGridLayout>(groupBox_, 2, 2);

  int row = 0;

  //--

  // location
  locationEdit_ = CQUtil::makeWidget<CQChartsTitleLocationEdit>("location");

  locationEdit_->setTitleLocation(data_.location);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Location", locationEdit_, row);

  //--

  // position
  positionEdit_ = CQUtil::makeWidget<CQChartsPositionEdit>("positionEdit");

  positionEdit_->setPosition(data_.position);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Position", positionEdit_, row);

  //--

  // rect
  rectEdit_ = CQUtil::makeWidget<CQChartsRectEdit>("rectEdit");

  rectEdit_->setRect(data_.rect);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Rect", rectEdit_, row);

  //--

  // inside plot
  insideEdit_ = CQUtil::makeWidget<CQCheckBox>("insideEdit");

  insideEdit_->setChecked(data_.insidePlot);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Inside Plot", insideEdit_, row);

  //--

  // box (margin, passing, fill, stroke, text)
  textEdit_ = CQUtil::makeWidget<CQChartsTextDataEdit>("textEdit");

  textEdit_->setTitle("Box");
  textEdit_->setPreview(false);
  textEdit_->setPlot(title_->plot());
  textEdit_->setView(title_->view());
  textEdit_->setData(data_.textData);

  groupLayout->addWidget(textEdit_, row, 0, 1, 2); ++row;

  //---

  groupLayout->setRowStretch(row, 1);

  //---

  connectSlots(true);

  widgetsToData();
}

void
CQChartsTitleEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      connect(w, from, this, to);
    else
      disconnect(w, from, this, to);
  };

  connectDisconnect(b, groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));
  connectDisconnect(b, locationEdit_, SIGNAL(titleLocationChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, positionEdit_, SIGNAL(positionChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, rectEdit_, SIGNAL(rectChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, insideEdit_, SIGNAL(toggled(bool)), SLOT(widgetsToData()));
  connectDisconnect(b, textEdit_, SIGNAL(textDataChanged()), SLOT(widgetsToData()));
}

void
CQChartsTitleEdit::
dataToWidgets()
{
  connectSlots(false);

  groupBox_    ->setChecked      (data_.visible);
  locationEdit_->setTitleLocation(data_.location);
  positionEdit_->setPosition     (data_.position);
  rectEdit_    ->setRect         (data_.rect);
  insideEdit_  ->setChecked      (data_.insidePlot);
  textEdit_    ->setData         (data_.textData);

  connectSlots(true);
}

void
CQChartsTitleEdit::
widgetsToData()
{
  data_.visible    = groupBox_->isChecked();
  data_.location   = locationEdit_->titleLocation();
  data_.position   = positionEdit_->position();
  data_.rect       = rectEdit_->rect();
  data_.insidePlot = insideEdit_->isChecked();
  data_.textData   = textEdit_->data();

  emit titleChanged();
}

void
CQChartsTitleEdit::
applyData()
{
  title_->setVisible          (data_.visible);
  title_->setLocation         (data_.location);
  title_->setAbsolutePosition (data_.position);
  title_->setAbsoluteRectangle(data_.rect);
  title_->setInsidePlot       (data_.insidePlot);
  title_->setTextData         (data_.textData);
}
