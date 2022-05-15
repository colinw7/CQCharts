#include <CQChartsEditModelDlg.h>
#include <CQChartsModelDataWidget.h>
#include <CQChartsModelControl.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnType.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQCsvModel.h>
#include <CQColorsPalette.h>
#include <CQTabSplit.h>
#ifdef CQCHARTS_EXCEL
#include <CQExcelFrame.h>
#endif

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDir>

CQChartsEditModelDlg::
CQChartsEditModelDlg(CQCharts *charts, CQChartsModelData *modelData) :
 QFrame(nullptr), charts_(charts), modelData_(modelData)
{
  assert(modelData_);

  model_ = modelData_->currentModel();

  //setModal(false);

  init();
}

void
CQChartsEditModelDlg::
init()
{
  setObjectName("editModelDlg");

  if (modelData_)
    setWindowTitle(QString("Edit Model %1").arg(modelData_->id()));
  else
    setWindowTitle("Edit Model");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //---

  if (modelData_) {
    connect(modelData_, SIGNAL(currentModelChanged()), this, SLOT(currentModelChangedSlot()));
    connect(modelData_, SIGNAL(deleted()), this, SLOT(cancelSlot()));
  }

  //---

#ifdef CQCHARTS_EXCEL
  auto *excelModel = CQChartsModelUtil::getExcelModel(model_.data());

  if (excelModel) {
    auto *excelFrame = new CQExcel::Frame;

    excelFrame->setModel(model_.data());

    layout->addWidget(excelFrame);

    return;
  }
#endif

  //---

  auto *split = CQUtil::makeWidget<CQTabSplit>("split");

  split->setOrientation(Qt::Horizontal);
  split->setGrouped(true);

  layout->addWidget(split);

  //---

  // create model widget
  modelWidget_ = new CQChartsModelDataWidget(charts_, modelData_);

  split->addWidget(modelWidget_, "Model");

  //---

  // create model control
  modelControl_ = new CQChartsModelControl(charts_, modelData_);

  split->addWidget(modelControl_, "Control");

  //---

  split->setSizes(QList<int>({int(INT_MAX*0.7), int(INT_MAX*0.3)}));

  //---

  // bottom buttons
  auto createButton = [&](const QString &label, const QString &name, const QString &tip,
                          const char *slotName) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, name);

    button->setToolTip(tip);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  auto *buttonLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  auto *writeButton = createButton("Write", "write", "Write Model" , SLOT(writeSlot()));
  auto *plotButton  = createButton("Plot" , "plot" , "Create Plot" , SLOT(plotSlot()));
  auto *doneButton  = createButton("Done" , "done" , "Close Dialog", SLOT(cancelSlot()));

  buttonLayout->addWidget(writeButton);
  buttonLayout->addWidget(plotButton);
  buttonLayout->addStretch(1);
  buttonLayout->addWidget(doneButton);

  layout->addLayout(buttonLayout);
}

CQChartsEditModelDlg::
~CQChartsEditModelDlg()
{
}

void
CQChartsEditModelDlg::
setModelData(CQChartsModelData *modelData)
{
  connectSlots(false);

  modelData_ = modelData;

  if (modelData_)
    model_ = modelData_->currentModel();
  else
    model_ = ModelP();

  connectSlots(true);

  if (modelData_)
    setWindowTitle(QString("Edit Model %1").arg(modelData_->id()));
  else
    setWindowTitle("Edit Model");

  modelWidget_ ->setModelData(modelData_);
  modelControl_->setModelData(modelData_);
}

void
CQChartsEditModelDlg::
connectSlots(bool b)
{
  if (! modelData_)
    return;

  CQChartsWidgetUtil::connectDisconnect(b,
    modelData_, SIGNAL(currentModelChanged()), this, SLOT(currentModelChangedSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    modelData_, SIGNAL(deleted()), this, SLOT(cancelSlot()));
}

void
CQChartsEditModelDlg::
currentModelChangedSlot()
{
  model_ = modelData_->currentModel();

  modelWidget_->reloadModel();
}

void
CQChartsEditModelDlg::
writeSlot()
{
  auto dir = QDir::current().dirName() + "/model.csv";

  auto filename = QFileDialog::getSaveFileName(this, "Write Model", dir, "Files (*.csv)");
  if (! filename.length()) return; // cancelled

  modelData_->writeCSV(filename);
  //writeCSVModel(filename);
}

#if 0
void
CQChartsEditModelDlg::
writeModelCmds()
{
  if (! modelData_)
    return;

  auto *columnTypeMgr = charts_->columnTypeMgr();

  auto *model = model_.data();

  for (int i = 0; i < model->columnCount(); ++i) {
    CQChartsColumn column(i);

    CQChartsModelTypeData columnTypeData;

    if (! CQChartsModelUtil::columnValueType(charts_, model, column, columnTypeData))
      continue;

    const auto *typeData = columnTypeMgr->getType(columnTypeData.type);

    auto value = typeData->name();

    bool first = true;

    for (const auto &param : typeData->params()) {
      QVariant var;

      if (! columnTypeData.nameValues.nameValue(param->name(), var))
        continue;

      if (! var.isValid())
        continue;

      if      (param->type() == CQBaseModelType::BOOLEAN) {
        bool ok1, ok2;
        auto i1 = CQChartsVariant::toBool(var         , ok1);
        auto i2 = CQChartsVariant::toBool(param->def(), ok2);

        if (ok1 && ok2 && b1 == b2)
          continue;
      }
      else if (param->type() == CQBaseModelType::REAL) {
        bool ok1, ok2;
        auto r1 = CQChartsVariant::toReal(var         , ok1);
        auto r2 = CQChartsVariant::toReal(param->def(), ok2);

        if (ok1 && ok2 && r1 == r2)
          continue;
      }
      else if (param->type() == CQBaseModelType::INTEGER) {
        bool ok1, ok2;
        auto i1 = CQChartsVariant::toInt(var         , ok1);
        auto i2 = CQChartsVariant::toInt(param->def(), ok2);

        if (ok1 && ok2 && i1 == i2)
          continue;
      }
      else if (param->type() == CQBaseModelType::STRING) {
        if (var.toString() == param->def().toString())
          continue;
      }

      auto str = var.toString();

      if (first)
        value += ":";

      value += param->name() + "=" + str;
    }

    std::cerr << "set_charts_data -model " << modelData_->ind() <<
                 " -column " << i << " -name column_type" <<
                 " -value {" << value.toStdString() << "}\n";
  }

  //---

  // TODO: write what ?
  modelData_->write(std::cerr);
}
#endif

void
CQChartsEditModelDlg::
plotSlot()
{
  charts_->createPlotDlg(modelData_);
}

void
CQChartsEditModelDlg::
cancelSlot()
{
  hide();
}

QSize
CQChartsEditModelDlg::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.horizontalAdvance("X")*140, fm.height()*80);
}
