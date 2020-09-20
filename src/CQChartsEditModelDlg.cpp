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

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDir>

CQChartsEditModelDlg::
CQChartsEditModelDlg(CQCharts *charts, CQChartsModelData *modelData) :
 charts_(charts), modelData_(modelData)
{
  assert(modelData_);

  model_ = modelData_->currentModel();

  init();
}

void
CQChartsEditModelDlg::
init()
{
  setObjectName("editModelDlg");

  if (modelData_) {
    connect(modelData_, SIGNAL(currentModelChanged()), this, SLOT(currentModelChangedSlot()));
    connect(modelData_, SIGNAL(deleted()), this, SLOT(cancelSlot()));
  }

  if (modelData_)
    setWindowTitle(QString("Edit Model %1").arg(modelData_->id()));
  else
    setWindowTitle("Edit Model");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //---

  auto *split = CQUtil::makeWidget<CQTabSplit>("split");

  split->setOrientation(Qt::Vertical);
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

  // Bottom Buttons
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
  QString dir = QDir::current().dirName() + "/model.csv";

  QString fileName = QFileDialog::getSaveFileName(this, "Write Model", dir, "Files (*.csv)");
  if (! fileName.length()) return; // cancelled

  modelData_->writeCSV(fileName);
  //writeCSVModel(fileName);
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

    QString value = typeData->name();

    bool first = true;

    for (const auto &param : typeData->params()) {
      QVariant var;

      if (! columnTypeData.nameValues.nameValue(param->name(), var))
        continue;

      if (! var.isValid())
        continue;

      if      (param->type() == CQBaseModelType::BOOLEAN) {
        if (var.toBool() == param->def().toBool())
          continue;
      }
      else if (param->type() == CQBaseModelType::REAL) {
        bool ok1, ok2;

        double r1 = CQChartsVariant::toReal(var         , ok1);
        double r2 = CQChartsVariant::toReal(param->def(), ok2);

        if (ok1 && ok2 && r1 == r2)
          continue;
      }
      else if (param->type() == CQBaseModelType::INTEGER) {
        bool ok1, ok2;

        int i1 = CQChartsVariant::toInt(var         , ok1);
        int i2 = CQChartsVariant::toInt(param->def(), ok2);

        if (ok1 && ok2 && i1 == i2)
          continue;
      }
      else if (param->type() == CQBaseModelType::STRING) {
        if (var.toString() == param->def().toString())
          continue;
      }

      QString str = var.toString();

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

  return QSize(fm.width("X")*60, fm.height()*40);
}
