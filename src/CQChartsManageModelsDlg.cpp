#include <CQChartsManageModelsDlg.h>
#include <CQChartsModelWidgets.h>
#include <CQChartsLoadModelDlg.h>
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
  init();
}

void
CQChartsManageModelsDlg::
init()
{
  setObjectName("manageModelsDlg");

  setWindowTitle("Manage Models");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  // create model widgets
  modelWidgets_ = new CQChartsModelWidgets(charts_);

  layout->addWidget(modelWidgets_);

  //---

  // Bottom Buttons
  auto createButton = [&](const QString &label, const QString &objName,
                          const QString &tip, const char *slotName) {
    auto *button  = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    button->setToolTip(tip);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  auto *loadButton  = createButton("Load Model..." , "load" , "Load Model"  , SLOT(loadSlot())  );
  auto *writeButton = createButton("Write Model"   , "write", "Write Model" , SLOT(writeSlot()) );
  auto *plotButton  = createButton("Create Plot...", "plot" , "Create Plot" , SLOT(plotSlot())  );
  auto *doneButton  = createButton("Done"          , "done" , "Close Dialog", SLOT(cancelSlot()));

  auto *buttonLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  buttonLayout->addWidget(loadButton);
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
loadSlot()
{
  (void) charts_->loadModelDlg();
}

void
CQChartsManageModelsDlg::
writeSlot()
{
  auto *columnTypeMgr = charts_->columnTypeMgr();

  auto *modelData = modelWidgets_->modelList()->currentModelData();
  if (! modelData) return;

  auto *model = modelData->currentModel().data();

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
        auto b1 = CQChartsVariant::toBool(var         , ok1);
        auto b2 = CQChartsVariant::toBool(param->def(), ok2);

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
  auto *modelData = modelWidgets_->modelList()->currentModelData();
  if (! modelData) return;

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

  return QSize(fm.horizontalAdvance("X")*160, fm.height()*100);
}
