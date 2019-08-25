#include <CQChartsManageModelsDlg.h>
#include <CQChartsModelWidgets.h>
#include <CQChartsModelList.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnType.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

CQChartsManageModelsDlg::
CQChartsManageModelsDlg(CQCharts *charts) :
 charts_(charts)
{
  setObjectName("manageModelsDlg");

  setWindowTitle("Manage Models");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  // create model widgets
  modelWidgets_ = new CQChartsModelWidgets(charts_);

  layout->addWidget(modelWidgets_);

  //---

  // Bottom Buttons
  QHBoxLayout *buttonLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  QPushButton *writeButton = CQUtil::makeLabelWidget<QPushButton>("Write", "write");
  QPushButton *plotButton  = CQUtil::makeLabelWidget<QPushButton>("Plot" , "plot" );
  QPushButton *doneButton  = CQUtil::makeLabelWidget<QPushButton>("Done" , "done" );

  connect(writeButton, SIGNAL(clicked()), this, SLOT(writeSlot()));
  connect(plotButton , SIGNAL(clicked()), this, SLOT(plotSlot()));
  connect(doneButton , SIGNAL(clicked()), this, SLOT(cancelSlot()));

  buttonLayout->addWidget(writeButton);
  buttonLayout->addWidget(plotButton);
  buttonLayout->addStretch(1);
  buttonLayout->addWidget(doneButton);

  layout->addLayout(buttonLayout);

  //----

  CQCharts::ModelDatas modelDatas;

  charts_->getModelDatas(modelDatas);

  for (const auto &modelData : modelDatas)
    modelWidgets_->addModelData(modelData);
}

CQChartsManageModelsDlg::
~CQChartsManageModelsDlg()
{
}

void
CQChartsManageModelsDlg::
writeSlot()
{
  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  CQChartsModelData *modelData = modelWidgets_->modelList()->currentModelData();

  QAbstractItemModel *model = modelData->currentModel().data();

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

      if (! nameValues.nameValue(param->name(), var))
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

    std::cerr << "set_charts_data -model " << modelData->ind() <<
                 " -column " << i << " -name column_type" <<
                 " -value {" << value.toStdString() << "}\n";
  }

  //---

  // TODO: write what ?
  modelData->write(std::cerr);
}

void
CQChartsManageModelsDlg::
plotSlot()
{
  CQChartsModelData *modelData = modelWidgets_->modelList()->currentModelData();

  charts_->createPlotDlg(modelData);
}

void
CQChartsManageModelsDlg::
cancelSlot()
{
  hide();
}

QSize
CQChartsManageModelsDlg::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("X")*60, fm.height()*40);
}
