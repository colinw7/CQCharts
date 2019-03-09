#include <CQChartsEditModelDlg.h>
#include <CQChartsModelDataWidget.h>
#include <CQChartsModelControl.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsCreatePlotDlg.h>
#include <CQChartsColumnType.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

CQChartsEditModelDlg::
CQChartsEditModelDlg(CQCharts *charts, CQChartsModelData *modelData) :
 charts_(charts), modelData_(modelData)
{
  setObjectName("editModelDlg");

  if (modelData_)
    setWindowTitle(QString("Edit Model %1").arg(modelData_->id()));
  else
    setWindowTitle("Edit Model");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  // create model widget
  modelWidget_ = new CQChartsModelDataWidget(charts_, modelData_);

  layout->addWidget(modelWidget_);

  //---

  // create model control
  modelControl_ = new CQChartsModelControl(charts_, modelData);

  layout->addWidget(modelControl_);

  //---

  // Bottom Buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout;

  QPushButton *writeButton = new QPushButton("Write");
  writeButton->setObjectName("write");

  connect(writeButton, SIGNAL(clicked()), this, SLOT(writeSlot()));

  QPushButton *plotButton = new QPushButton("Plot");
  plotButton->setObjectName("plot");

  connect(plotButton, SIGNAL(clicked()), this, SLOT(plotSlot()));

  QPushButton *doneButton = new QPushButton("Done");
  doneButton->setObjectName("done");

  connect(doneButton, SIGNAL(clicked()), this, SLOT(cancelSlot()));

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
  modelData_ = modelData;

  if (modelData_)
    setWindowTitle(QString("Edit Model %1").arg(modelData_->id()));
  else
    setWindowTitle("Edit Model");

  modelWidget_->setModelData(modelData_);

  modelControl_->setModelData(modelData_);
}

void
CQChartsEditModelDlg::
writeSlot()
{
  if (! modelData_)
    return;

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  QAbstractItemModel *model = modelData_->currentModel().data();

  for (int i = 0; i < model->columnCount(); ++i) {
    CQChartsColumn column(i);

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    if (! CQChartsModelUtil::columnValueType(charts_, model, column, columnType,
                                             columnBaseType, nameValues))
      continue;

    const CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

    QString value = typeData->name();

    bool first = true;

    for (const auto &param : typeData->params()) {
      QVariant var;

      if (! nameValues.nameValue(param.name(), var))
        continue;

      if (! var.isValid())
        continue;

      if      (param.type() == CQBaseModelType::BOOLEAN) {
        if (var.toBool() == param.def().toBool())
          continue;
      }
      else if (param.type() == CQBaseModelType::REAL) {
        bool ok1, ok2;

        double r1 = CQChartsVariant::toReal(var        , ok1);
        double r2 = CQChartsVariant::toReal(param.def(), ok2);

        if (ok1 && ok2 && r1 == r2)
          continue;
      }
      else if (param.type() == CQBaseModelType::INTEGER) {
        bool ok1, ok2;

        int i1 = CQChartsVariant::toInt(var        , ok1);
        int i2 = CQChartsVariant::toInt(param.def(), ok2);

        if (ok1 && ok2 && i1 == i2)
          continue;
      }
      else if (param.type() == CQBaseModelType::STRING) {
        if (var.toString() == param.def().toString())
          continue;
      }

      QString str = var.toString();

      if (first)
        value += ":";

      value += param.name() + "=" + str;
    }

    std::cerr << "set_charts_data -model " << modelData_->ind() <<
                 " -column " << i << " -name column_type" <<
                 " -value {" << value.toStdString() << "}\n";
  }

  //---

  // TODO: write what ?
  modelData_->write();
}

void
CQChartsEditModelDlg::
plotSlot()
{
  delete createPlotDlg_;

  createPlotDlg_ = new CQChartsCreatePlotDlg(charts_, modelData_);

  createPlotDlg_->show();
}

void
CQChartsEditModelDlg::
cancelSlot()
{
  hide();
}
