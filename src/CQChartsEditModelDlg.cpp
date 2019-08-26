#include <CQChartsEditModelDlg.h>
#include <CQChartsModelDataWidget.h>
#include <CQChartsModelControl.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnType.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>

#include <CQCsvModel.h>
#include <CQColorsPalette.h>
#include <CQTabSplit.h>

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDir>

#include <fstream>

CQChartsEditModelDlg::
CQChartsEditModelDlg(CQCharts *charts, CQChartsModelData *modelData) :
 charts_(charts), modelData_(modelData)
{
  setObjectName("editModelDlg");

  if (modelData_)
    setWindowTitle(QString("Edit Model %1").arg(modelData_->id()));
  else
    setWindowTitle("Edit Model");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //---

  CQTabSplit *split = CQUtil::makeWidget<CQTabSplit>("split");

  split->setOrientation(Qt::Vertical);

  layout->addWidget(split);

  //---

  // create model widget
  modelWidget_ = new CQChartsModelDataWidget(charts_, modelData_);

  split->addWidget(modelWidget_, "Model");

  //---

  // create model control
  modelControl_ = new CQChartsModelControl(charts_, modelData);

  split->addWidget(modelControl_, "Control");

  //---

  // Bottom Buttons
  auto createButton = [&](const QString &label, const QString &name, const QString &tip,
                          const char *slotName) {
    QPushButton *button = CQUtil::makeLabelWidget<QPushButton>(label, name);

    button->setToolTip(tip);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  QHBoxLayout *buttonLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  QPushButton *writeButton = createButton("Write", "write", "Write Model" , SLOT(writeSlot()));
  QPushButton *plotButton  = createButton("Plot" , "plot" , "Create Plot" , SLOT(plotSlot()));
  QPushButton *doneButton  = createButton("Done" , "done" , "Close Dialog", SLOT(cancelSlot()));

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
  QString dir = QDir::current().dirName() + "/model.csv";

  QString fileName = QFileDialog::getSaveFileName(this, "Write Model", dir, "Files (*.csv)");

  writeCSVModel(fileName);
}

bool
CQChartsEditModelDlg::
writeCSVModel(const QString &fileName)
{
  CQChartsModelDetails *details = modelData_->details();

  if (details->isHierarchical())
    return false;

  QAbstractItemModel *model = modelData_->currentModel().data();

  //---

  int nr = details->numRows();
  int nc = details->numColumns();

  if (nr < 0 || nc < 0)
    return false;

  //---

  auto fs = std::ofstream(fileName.toStdString(), std::ofstream::out);

  // write meta data
  fs << "#META_DATA\n";

  for (int c = 0; c < nc; ++c) {
    CQChartsModelColumnDetails *columnDetails = details->columnDetails(c);

    QString header = model->headerData(c, Qt::Horizontal, Qt::DisplayRole).toString();

    auto writeMetaColumnData = [&](const QString &name, const QString &value) {
      fs << "#  column," << header.toStdString() << "," <<
            name.toStdString() << "," << value.toStdString() << "\n";
    };

    auto writeMetaColumnNameValue = [&](const QString &name) {
      QString value;

      if (! columnDetails->columnNameValue(name, value))
        return;

      writeMetaColumnData(name, value);
    };

    CQBaseModelType type     = columnDetails->type();
    QString         typeName = columnDetails->typeName();

    writeMetaColumnData("type", typeName);

    const CQChartsColor &drawColor = columnDetails->tableDrawColor();

    CQChartsModelColumnDetails::TableDrawType tableDrawType  = columnDetails->tableDrawType();
    CQChartsColorStops                        tableDrawStops = columnDetails->tableDrawStops();

    writeMetaColumnData("key", "1");

    if (drawColor.isValid())
      writeMetaColumnData("draw_color", drawColor.toString());

    if      (tableDrawType == CQChartsModelColumnDetails::TableDrawType::HEATMAP)
      writeMetaColumnData("draw_type", "heatmap");
    else if (tableDrawType == CQChartsModelColumnDetails::TableDrawType::BARCHART)
      writeMetaColumnData("draw_type", "barchart");

    if (tableDrawStops.isValid())
      writeMetaColumnData("draw_stops", tableDrawStops.toString());

    if      (type == CQBaseModelType::REAL) {
      writeMetaColumnNameValue("format");
      writeMetaColumnNameValue("format_scale");
      writeMetaColumnNameValue("min");
      writeMetaColumnNameValue("max");
    }
    else if (type == CQBaseModelType::INTEGER) {
      writeMetaColumnNameValue("format");
      writeMetaColumnNameValue("min");
      writeMetaColumnNameValue("max");
    }
    else if (type == CQBaseModelType::TIME) {
      writeMetaColumnNameValue("format");
      writeMetaColumnNameValue("iformat");
      writeMetaColumnNameValue("oformat");
    }
    else if (type == CQBaseModelType::COLOR) {
      writeMetaColumnNameValue("format");
      writeMetaColumnNameValue("min");
      writeMetaColumnNameValue("max");
    }
    else if (type == CQBaseModelType::SYMBOL) {
      writeMetaColumnNameValue("mapped");
      writeMetaColumnNameValue("min");
      writeMetaColumnNameValue("max");
    }
    else if (type == CQBaseModelType::SYMBOL_SIZE) {
      writeMetaColumnNameValue("mapped");
      writeMetaColumnNameValue("min");
      writeMetaColumnNameValue("max");
      writeMetaColumnNameValue("size_min");
      writeMetaColumnNameValue("size_max");
    }
    else if (type == CQBaseModelType::FONT_SIZE) {
      writeMetaColumnNameValue("mapped");
      writeMetaColumnNameValue("min");
      writeMetaColumnNameValue("max");
      writeMetaColumnNameValue("size_min");
      writeMetaColumnNameValue("size_max");
    }
  }

  fs << "#END_META_DATA\n";

  //---

  // write header
  for (int c = 0; c < nc; ++c) {
    QString header = model->headerData(c, Qt::Horizontal, Qt::DisplayRole).toString();

    if (c > 0)
      fs << ",";

    fs << CQCsvModel::encodeString(header).toStdString();
  }

  fs << "\n";

  //---

  // write data
  for (int r = 0; r < nr; ++r) {
    for (int c = 0; c < nc; ++c) {
      QModelIndex ind = model->index(r, c);

      QVariant var;

      CQChartsModelColumnDetails *columnDetails = details->columnDetails(c);

      CQBaseModelType type = columnDetails->type();

      bool converted = false;

      if (type == CQBaseModelType::TIME) {
        var = model->data(ind, Qt::EditRole);

        if (var.isValid()) {
          bool ok;

          double r = var.toDouble(&ok);

          if (ok) {
            const CQChartsColumnTimeType *timeType =
              dynamic_cast<const CQChartsColumnTimeType *>(columnDetails->columnType());
            assert(timeType);

            QString fmt = timeType->getIFormat(columnDetails->nameValues());

            if (fmt.simplified() != "") {
              var = CQChartsUtil::timeToString(fmt, r);

              converted = true;
            }
          }
        }
      }

      if (! converted) {
        var = model->data(ind, Qt::EditRole);

        if (! var.isValid())
          var = model->data(ind, Qt::DisplayRole);
      }

      if (c > 0)
        fs << ",";

      fs << CQCsvModel::encodeString(var.toString()).toStdString();
    }

    fs << "\n";
  }

  return true;
}

#if 0
void
CQChartsEditModelDlg::
writeModelCmds()
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
