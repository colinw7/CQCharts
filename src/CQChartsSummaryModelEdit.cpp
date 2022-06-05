#include <CQChartsSummaryModelEdit.h>
#include <CQChartsModelData.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsWidgetUtil.h>

#include <CQSummaryModel.h>
#include <CQIntegerSpin.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QStackedWidget>
#include <QCheckBox>
#include <QLabel>

CQChartsSummaryModelEdit::
CQChartsSummaryModelEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("summaryModelEdit");

  auto *layout = CQUtil::makeLayout<QGridLayout>(this, 0, 2);

  //---

  enabledCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Enabled", "enabledCheck");

  enabledCheck_->setToolTip("Enable summary of model data");

  layout->addWidget(enabledCheck_, 0, 0);

  //---

  maxRowsSpin_ = CQUtil::makeWidget<CQIntegerSpin>("maxRowsSpin");

  layout->addWidget(CQUtil::makeLabelWidget<QLabel>("Max Rows", "maxRowsLabel"), 1, 0);
  layout->addWidget(maxRowsSpin_                                               , 1, 1);

  //---

  typeCombo_ = CQUtil::makeWidget<QComboBox>("typeCombo");

  typeCombo_->addItems(QStringList() << "Normal" << "Random" << "Sorted" << "Paged");

  typeCombo_->setToolTip("Summary data selection type");

  layout->addWidget(CQUtil::makeLabelWidget<QLabel>("Type", "typeLabel"), 2, 0);
  layout->addWidget(typeCombo_                                          , 2, 1);

  //----

  typeStack_ = CQUtil::makeWidget<QStackedWidget>("typeStack");

  layout->addWidget(typeStack_);

  //---

  auto *normalTypeFrame = CQUtil::makeWidget<QFrame>("normalTypeFrame");

  typeStack_->addWidget(normalTypeFrame);

  //---

  auto *randomTypeFrame = CQUtil::makeWidget<QFrame>("randomTypeFrame");

  typeStack_->addWidget(randomTypeFrame);

  //---

  auto *sortedTypeFrame  = CQUtil::makeWidget<QFrame>("sortedTypeFrame");
  auto *sortedTypeLayout = CQUtil::makeLayout<QVBoxLayout>(sortedTypeFrame, 0, 2);

  typeStack_->addWidget(sortedTypeFrame);

  //--

  auto *sortColFrame  = CQUtil::makeWidget<QFrame>("sortColFrame");
  auto *sortColLayout = CQUtil::makeLayout<QHBoxLayout>(sortColFrame, 0, 2);

  sortedTypeLayout->addWidget(sortColFrame);

  sortedColEdit_ = CQUtil::makeWidget<CQChartsColumnCombo>("sortedColEdit");

  sortColLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Sort Column", "sortCol"));
  sortColLayout->addWidget(sortedColEdit_);
  sortColLayout->addStretch(1);

  //--

  sortOrderCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Increasing", "sortOrderCheck");

  sortedTypeLayout->addWidget(sortOrderCheck_);

  //--

  sortedTypeLayout->addStretch(1);

  //---

  auto *pageSizeTypeFrame  = CQUtil::makeWidget<QFrame>("pageSizeTypeFrame");
  auto *pageSizeTypeLayout = CQUtil::makeLayout<QVBoxLayout>(pageSizeTypeFrame, 0, 2);

  typeStack_->addWidget(pageSizeTypeFrame);

  //--

  auto *pageSizeEditFrame  = CQUtil::makeWidget<QFrame>("pageSizeEditFrame");
  auto *pageSizeEditLayout = CQUtil::makeLayout<QHBoxLayout>(pageSizeEditFrame, 0, 2);

  pageSizeTypeLayout->addWidget(pageSizeEditFrame);

  pageSizeEdit_ = CQUtil::makeWidget<CQIntegerSpin>("pageSizeEdit");

  pageSizeEditLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Page Size", "pointSize"));
  pageSizeEditLayout->addWidget(pageSizeEdit_);
  pageSizeEditLayout->addStretch(1);

  //--

  auto *currentPageEditFrame  = CQUtil::makeWidget<QFrame>("currentPageEditFrame");
  auto *currentPageEditLayout = CQUtil::makeLayout<QHBoxLayout>(currentPageEditFrame, 0, 2);

  pageSizeTypeLayout->addWidget(currentPageEditFrame);

  currentPageEdit_ = CQUtil::makeWidget<CQIntegerSpin>("currentPageEdit");

  currentPageEditLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Current Page", "currentPage"));
  currentPageEditLayout->addWidget(currentPageEdit_);
  currentPageEditLayout->addStretch(1);

  //--

  pageSizeTypeLayout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQChartsSummaryModelEdit::
setModelData(ModelData *modelData)
{
  modelData_ = modelData;

  updateWidgetsFromModel();
}

//---

void
CQChartsSummaryModelEdit::
connectSlots(bool b)
{
  CQChartsWidgetUtil::optConnectDisconnect(b,
    modelData_, SIGNAL(dataChanged()), this, SLOT(updateWidgetsFromModel()));

  CQChartsWidgetUtil::connectDisconnect(b,
    enabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(enabledSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    maxRowsSpin_, SIGNAL(valueChanged(int)), this, SLOT(updateModelFromWidgets()));
  CQChartsWidgetUtil::connectDisconnect(b,
    typeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(typeSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    sortedColEdit_, SIGNAL(columnChanged()), this, SLOT(updateModelFromWidgets()));
  CQChartsWidgetUtil::connectDisconnect(b,
    sortOrderCheck_, SIGNAL(stateChanged(int)), this, SLOT(enabledSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    pageSizeEdit_, SIGNAL(valueChanged(int)), this, SLOT(updateModelFromWidgets()));
  CQChartsWidgetUtil::connectDisconnect(b,
    currentPageEdit_, SIGNAL(valueChanged(int)), this, SLOT(updateModelFromWidgets()));
}

//---

void
CQChartsSummaryModelEdit::
enabledSlot()
{
  if (modelData_) {
    bool enabled = enabledCheck_->isChecked();

    if (enabledCheck_->isChecked())
      modelData_->addSummaryModel();

    modelData_->setSummaryEnabled(enabled);
  }

  updateModelFromWidgets();
}

void
CQChartsSummaryModelEdit::
typeSlot()
{
  typeStack_->setCurrentIndex(typeCombo_->currentIndex());

  updateModelFromWidgets();
}

//---

// update widgets from state
void
CQChartsSummaryModelEdit::
updateWidgetsFromModel()
{
  connectSlots(false);

  auto *summaryModel = (modelData_ ? modelData_->summaryModel() : nullptr);

  if (modelData_) {
    auto model = modelData_->currentModel(/*proxy*/false);

    int nr = model.data()->rowCount();
  //int nc = model.data()->columnCount();

    //---

    enabledCheck_->setChecked(modelData_->isSummaryEnabled());

    //---

    maxRowsSpin_->setRange(1, nr);
    maxRowsSpin_->setToolTip(QString("Set Row Count (1 -> %1)").arg(nr));

    if (summaryModel)
      maxRowsSpin_->setValue(summaryModel->maxRows());
    else
      maxRowsSpin_->setValue(nr);

    //---

    if (summaryModel) {
      if      (summaryModel->mode() == CQSummaryModel::Mode::RANDOM)
        typeCombo_->setCurrentIndex(1);
      else if (summaryModel->mode() == CQSummaryModel::Mode::SORTED)
        typeCombo_->setCurrentIndex(2);
      else if (summaryModel->mode() == CQSummaryModel::Mode::PAGED)
        typeCombo_->setCurrentIndex(3);
    }

    //---

  //sortedColEdit_->setRange(0, nc - 1);
  //sortedColEdit_->setToolTip(QString("Set Sort Column (0 -> %1)").arg(nc - 1));

    sortedColEdit_->setProxy(false);
    sortedColEdit_->setModelData(modelData_);

    if (summaryModel) {
    //sortedColEdit_->setValue(summaryModel->sortColumn());
      sortedColEdit_->setColumn(CQChartsColumn(summaryModel->sortColumn()));

      sortOrderCheck_->setChecked(summaryModel->sortOrder() == Qt::AscendingOrder);
    }

    //---

    pageSizeEdit_->setRange(1, nr);
    pageSizeEdit_->setToolTip(QString("Set Preview Page Size (1 -> %1)").arg(nr));
      if (summaryModel)
      pageSizeEdit_->setValue(summaryModel->pageSize());

    int np = (nr + pageSizeEdit_->value() - 1)/pageSizeEdit_->value();

    currentPageEdit_->setRange(0, np - 1);
    currentPageEdit_->setToolTip(QString("Set Page Count (0 -> %1)").arg(np - 1));

    if (summaryModel)
      currentPageEdit_->setValue(summaryModel->currentPage());
  }

  //--

  maxRowsSpin_    ->setEnabled(summaryModel);
  typeCombo_      ->setEnabled(summaryModel);
  typeStack_      ->setEnabled(summaryModel);
  sortedColEdit_  ->setEnabled(summaryModel);
  sortOrderCheck_ ->setEnabled(summaryModel);
  pageSizeEdit_   ->setEnabled(summaryModel);
  currentPageEdit_->setEnabled(summaryModel);

  connectSlots(true);
}

// update state from widgets
void
CQChartsSummaryModelEdit::
updateModelFromWidgets()
{
  auto *summaryModel = (modelData_ ? modelData_->summaryModel() : nullptr);

  if (modelData_->isSummaryEnabled() && summaryModel) {
    bool changed = false;

    //--

    bool random = (typeCombo_->currentText() == "Random");
    bool sorted = (typeCombo_->currentText() == "Sorted");
    bool paged  = (typeCombo_->currentText() == "Paged" );

    //--

    int n = maxRowsSpin_->value();

    if (n >= 0 && n != summaryModel->maxRows()) {
      summaryModel->setMode(CQSummaryModel::Mode::NORMAL);
      summaryModel->setMaxRows(n);

      changed = true;
    }

    //--

    // random n rows
    if      (random) {
      if (! summaryModel->isRandomMode()) {
        summaryModel->setRandomMode(true);

        changed = true;
      }
    }
    // first n rows when sorted by column
    else if (sorted) {
    //int sortCol = sortedColEdit_->value();
      int sortCol = sortedColEdit_->getColumn().column();

      if (! summaryModel->isSortMode() || summaryModel->sortColumn() != sortCol) {
        summaryModel->setSortColumn(sortCol);
        summaryModel->setSortMode(true);

        changed = true;
      }

      auto sortOrder = (sortOrderCheck_->isChecked() ? Qt::AscendingOrder : Qt::DescendingOrder);

      if (summaryModel->sortOrder() != sortOrder) {
        summaryModel->setSortOrder(sortOrder);

        changed = true;
      }
    }
    // nth page of specified page size
    else if (paged) {
      auto model = modelData_->currentModel(/*proxy*/false);

      int ps = pageSizeEdit_   ->value();
      int np = currentPageEdit_->value();

      int nr = model.data()->rowCount();

      int np1 = (nr + ps - 1)/ps;

      np = std::min(np, np1 - 1);

      currentPageEdit_->setRange(0, np1 - 1);
      currentPageEdit_->setToolTip(QString("Set Page Count (0 -> %1)").arg(np1 - 1));

      if (! summaryModel->isPagedMode() || ps != summaryModel->pageSize() ||
          np != summaryModel->currentPage()) {
        summaryModel->setPageSize(ps);
        summaryModel->setCurrentPage(np);
        summaryModel->setPagedMode(true);

        changed = true;
      }
    }
    else {
      if (summaryModel->mode() != CQSummaryModel::Mode::NORMAL) {
        summaryModel->setMode(CQSummaryModel::Mode::NORMAL);

        changed = true;
      }
    }

    //---

    if (changed)
      modelData_->emitModelChanged();
  }
}
