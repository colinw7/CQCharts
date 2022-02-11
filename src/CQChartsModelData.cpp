#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelUtil.h>
#include <CQChartsFilterModel.h>
#include <CQChartsVarsModel.h>
#include <CQChartsExprDataModel.h>
#include <CQChartsExprTcl.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsHtml.h>
#include <CQChartsWidgetUtil.h>

#include <CQSummaryModel.h>
#include <CQBucketModel.h>
#include <CQFoldedModel.h>
#include <CQCollapseModel.h>
#include <CQPivotModel.h>
#include <CQCsvModel.h>
#include <CQTsvModel.h>
#include <CQJsonModel.h>
#include <CQGnuDataModel.h>
#include <CQHierSepModel.h>
#include <CQModelUtil.h>
#include <CQPropertyViewModel.h>
#include <CQTclUtil.h>
#include <CQFileWatcher.h>

#include <QSortFilterProxyModel>
#include <QItemSelectionModel>

#include <fstream>

QString
CQChartsModelData::
description()
{
  auto LI  = [](const QString &str) { return CQChartsHtml::Str(str); };
//auto A   = [](const QString &ref, const QString &str) { return CQChartsHtml::Str::a(ref, str); };
  auto CMD = [](const QString &str) {
    return CQChartsHtml::Str::a("charts://command/" + str, str); };

  return CQChartsHtml().
   h2("Loading Data").
   p("To load data use the " + CMD("charts_load_model") + " tcl command.").
   p("This command supports loading files in the following formats:").
   ul({ LI("csv : Comma Separated Value data"),
        LI("tsv : Tab Separated Value Data"),
        LI("json : JSON Data"),
        LI("data : GNUPlot like data (space separated)") }).
   p("and also allows the data to be generated from a tcl expression or read "
     "from a tcl variable (list of lists).").
   p("For csv, csv and data formats the reader recognises lines starting with # as "
     "comments. Headers for the columns can be take from the first non-comment "
     "line or first comment line.").
   p("csv defaults to using a comma for the value separator. This can be changed "
     "to any character to support other separator types.").
   p("csv format also supports specially formatted comments to support meta data "
     "for specifying additional column data e.g. column types and formats.").
   p("The tcl expression is run on a specified number of rows (default 100).").
   p("The data processed can be limited by specifying a maximum number of rows, "
     "a tcl filter expression or specific columns.").
   p("The types of the columns can be specified.").
   p("The command returns a unique identifier the for model which can be used in "
      "other commands e.g. as the input model for a plot.");
}

QString
CQChartsModelData::
typeDescription(const QString &type)
{
  if      (type == "CSV") {
    return CQChartsHtml().
     h2("CSV");
  }
  else if (type == "TSV") {
    return CQChartsHtml().
     h2("CSV");
  }
  else if (type == "Json") {
    return CQChartsHtml().
     h2("Json");
  }
  else if (type == "Data") {
    return CQChartsHtml().
     h2("Data");
  }
  else if (type == "Expr") {
    return CQChartsHtml().
     h2("Expr");
  }
  else if (type == "Vars") {
    return CQChartsHtml().
     h2("Vars");
  }
  else {
    return CQChartsHtml();
  }
}

//---

CQChartsModelData::
CQChartsModelData(CQCharts *charts, ModelP &model) :
 charts_(charts), model_(model)
{
  connectModel(true);
}

CQChartsModelData::
~CQChartsModelData()
{
  delete propertyModel_;
  delete details_;
  delete summaryModelData_;

  delete fileWatcher_;
}

QString
CQChartsModelData::
id() const
{
  if (name().length())
    return name();

  return defId();
}

QString
CQChartsModelData::
defId() const
{
  return QString("model:%1").arg(ind());
}

void
CQChartsModelData::
setName(const QString &s)
{
  if (s != name_) {
    name_ = s;

    emitDataChanged();
  }
}

QString
CQChartsModelData::
filename() const
{
  if (filename_.length())
    return filename_;

  ModelP model = this->currentModel();

  auto *dataModel = CQChartsModelUtil::getDataModel(model.data());

  if (dataModel)
    return dataModel->filename();

  return "";
}

void
CQChartsModelData::
setFilename(const QString &filename)
{
  if (filename != filename_) {
    if (! fileWatcher_) {
      fileWatcher_ = new CQFileWatcher;

      connect(fileWatcher_, SIGNAL(fileChanged(const QString &)),
              this, SLOT(fileChangedSlot(const QString &)));
    }
    else
      fileWatcher_->removeFile(filename_);

    filename_ = filename;

    fileWatcher_->addFile(filename_);

    emitDataChanged();
  }
}

void
CQChartsModelData::
fileChangedSlot(const QString &)
{
  //std::cerr << "File Changed\n";

  ModelP model = this->currentModel();

  auto *absModel = CQChartsModelUtil::getBaseModel(model.data());
  auto *csvModel = qobject_cast<CQCsvModel *>(absModel);

  if (csvModel)
    csvModel->load(filename_);
  else
    return;

  emitModelChanged();
}

QString
CQChartsModelData::
desc() const
{
  if (name().length())
    return name();

  if (filename().length())
    return filename();

  return id();
}

CQChartsModelDetails *
CQChartsModelData::
details()
{
  if (! details_) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (! details_)
      details_ = new CQChartsModelDetails(this);
  }

  return details_;
}

const CQChartsModelDetails *
CQChartsModelData::
details() const
{
  return const_cast<CQChartsModelData *>(this)->details();
}

void
CQChartsModelData::
setCurrentColumn(int i)
{
  currentColumn_ = i;

  emit currentColumnChanged(i);
}

void
CQChartsModelData::
connectModel(bool b)
{
  if (! model().data())
    return;

  CQChartsWidgetUtil::connectDisconnect(b,
    model().data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
    this, SLOT(modelDataChangedSlot(const QModelIndex &, const QModelIndex &)));
  CQChartsWidgetUtil::connectDisconnect(b,
    model().data(), SIGNAL(layoutChanged()), this, SLOT(modelLayoutChangedSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    model().data(), SIGNAL(modelReset()), this, SLOT(modelResetSlot()));

  CQChartsWidgetUtil::connectDisconnect(b,
    model().data(), SIGNAL(rowsInserted(QModelIndex, int, int)),
    this, SLOT(modelRowsInsertedSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    model().data(), SIGNAL(rowsRemoved(QModelIndex, int, int)),
    this, SLOT(modelRowsRemovedSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    model().data(), SIGNAL(columnsInserted(QModelIndex, int, int)),
    this, SLOT(modelColumnsInsertedSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    model().data(), SIGNAL(columnsRemoved(QModelIndex, int, int)),
    this, SLOT(modelColumnsRemovedSlot()));
}

void
CQChartsModelData::
modelDataChangedSlot(const QModelIndex & /*tl*/, const QModelIndex & /*br*/)
{
#if 0
  // TODO: check if model uses changed columns
  int column1 = tl.column();
  int column2 = br.column();
#endif

  resetDetails();

  emitModelChanged();
}

void
CQChartsModelData::
modelLayoutChangedSlot()
{
  resetDetails();

  emitModelChanged();
}

void
CQChartsModelData::
modelResetSlot()
{
  resetDetails();

  emitModelChanged();
}

void
CQChartsModelData::
modelRowsInsertedSlot()
{
  resetDetails();

  emitModelChanged();
}

void
CQChartsModelData::
modelRowsRemovedSlot()
{
  resetDetails();

  emitModelChanged();
}

void
CQChartsModelData::
modelColumnsInsertedSlot()
{
  resetDetails();

  emitModelChanged();
}

void
CQChartsModelData::
modelColumnsRemovedSlot()
{
  resetDetails();

  emitModelChanged();
}

void
CQChartsModelData::
resetDetails()
{
  if (details_)
    details_->reset();
}

CQChartsModelData::ModelP
CQChartsModelData::
currentModel() const
{
#ifdef CQCHARTS_FOLDED_MODEL
  if (! foldedModels_.empty())
    return foldProxyModel_;

  if (hierSepModel_)
    return hierSepModel_;
#endif

  return model_;
}

//---

void
CQChartsModelData::
emitDeleted()
{
  emit deleted();
}

void
CQChartsModelData::
emitModelChanged()
{
  emit modelChanged();
}

void
CQChartsModelData::
emitDataChanged()
{
  emit dataChanged();
}

//---

void
CQChartsModelData::
addSelectionModel(QItemSelectionModel *model)
{
  connect(model, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this, SLOT(selectionSlot()));

  selectionModels_.emplace_back(model);
}

void
CQChartsModelData::
removeSelectionModel(QItemSelectionModel *model)
{
  size_t i   = 0;
  auto   len = selectionModels_.size();

  for ( ; i < len; ++i) {
    if (selectionModels_[size_t(i)] == model)
      break;
  }

  if (i >= len)
    return;

  disconnect(model, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
             this, SLOT(selectionSlot()));

  ++i;

  for ( ; i < len; ++i)
    selectionModels_[i - 1] = selectionModels_[i];

  selectionModels_.pop_back();
}

void
CQChartsModelData::
select(const QItemSelection &sel)
{
  for (auto &sm : selectionModels_) {
    if (! sm) continue;

    CQChartsWidgetUtil::AutoDisconnect autoDisconnect(
      sm, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
      this, SLOT(selectionSlot()));

    sm->select(sel, QItemSelectionModel::ClearAndSelect);
  }
}

void
CQChartsModelData::
selectionSlot()
{
  auto *sm = qobject_cast<QItemSelectionModel *>(sender());
  assert(sm);

  emit selectionChanged(sm);
}

//---

#ifdef CQCHARTS_FOLDED_MODEL
bool
CQChartsModelData::
foldModel(const FoldData &foldData)
{
  foldClear(/*notify*/false);

  //---

  if      (foldData.foldType == FoldData::FoldType::BUCKET) {
    auto *details = this->details();

    using FoldDatas = std::vector<CQFoldData>;

    FoldDatas foldDatas;

    auto columnStrs = foldData.columnsStr.split(",", QString::SkipEmptyParts);

    for (int i = 0; i < columnStrs.length(); ++i) {
      auto columnSubStrs = columnStrs[i].split(":", QString::SkipEmptyParts);

      if (columnSubStrs.length() == 0)
        continue;

      //---

      // get column
      auto columnStr = columnSubStrs[0];

      CQChartsColumn column;

      if (! CQChartsModelUtil::stringToColumn(model().data(), columnStr, column)) {
        bool ok;

        long icolumn = CQChartsUtil::toInt(columnStr, ok);

        if (! ok)
          continue;

        column = CQChartsColumn(int(icolumn));
      }

      //---

      // get type, count and delta
      auto type = CQFoldData::Type::STRING;

      int    count    = foldData.count;
      double delta    = foldData.delta;
      bool   integral = false;

      CQFoldData columnFoldData(column.column());

      if (columnSubStrs.length() > 1) {
        int i = 1;

        // <column>:<type>:<delta>
        if (columnSubStrs.length() > 2) {
          // get type
          auto typeStr = columnSubStrs[1];

          if      (typeStr == "i")
            type = CQFoldData::Type::INTEGER_RANGE;
          else if (typeStr == "r")
            type = CQFoldData::Type::REAL_RANGE;
          else if (typeStr == "a")
            type = CQFoldData::Type::REAL_AUTO;
          else if (typeStr == "s")
            type = CQFoldData::Type::STRING;
          else
            continue;

          //---

          // get delta
          bool ok;

          auto deltaStr = columnSubStrs[2];

          delta = CQChartsUtil::toReal(columnSubStrs[i], ok);

          if (! ok)
            continue;
        }
        // <column>:<delta>
        else {
          // set type
          type = CQFoldData::Type::REAL_RANGE;

          //---

          // get delta
          bool ok;

          auto deltaStr = columnSubStrs[1];

          delta = CQChartsUtil::toReal(columnSubStrs[i], ok);

          if (! ok)
            continue;
        }
      }
      else {
        // get type from column
        const auto *columnDetails = details->columnDetails(column);

        if      (columnDetails->type() == CQBaseModelType::REAL) {
          if (foldData.isAuto)
            type = CQFoldData::Type::REAL_AUTO;
          else
            type = CQFoldData::Type::REAL_RANGE;
        }
        else if (columnDetails->type() == CQBaseModelType::INTEGER) {
          integral = true;

          if (foldData.isAuto)
            type = CQFoldData::Type::REAL_AUTO;
          else
            type = CQFoldData::Type::REAL_RANGE;
        }
        else {
          type = CQFoldData::Type::STRING;
        }
      }

      //---

      // add fold data
      columnFoldData.setType    (type);
      columnFoldData.setNumAuto (count);
      columnFoldData.setDelta   (delta);
      columnFoldData.setIntegral(integral);

      foldDatas.push_back(std::move(columnFoldData));
    }

    //---

    ModelP modelp = this->model();

    // create folded models
    for (const auto &foldData : foldDatas) {
      auto *model = modelp.data();
      assert(model);

      auto *foldedModel = new CQFoldedModel(model, foldData);

      modelp = ModelP(foldedModel);

      foldedModels_.push_back(modelp);
    }

    //---

    if (! foldedModels_.empty()) {
      // add sort/filter proxy if needed
      auto *model = modelp.data();
      assert(model);

      auto *proxyModel = qobject_cast<QSortFilterProxyModel *>(model);

      if (! proxyModel) {
        auto *foldProxyModel = new QSortFilterProxyModel;

        foldProxyModel->setObjectName("foldProxyModel");

        foldProxyModel->setSortRole(Qt::EditRole);

        foldProxyModel->setSourceModel(model);

        modelp = ModelP(foldProxyModel);
      }

      foldProxyModel_ = modelp;
    }

    //---

    delete details_;

    details_ = nullptr;

    //---

    updatePropertyModel();

    emit currentModelChanged();
  }
  else if (foldData.foldType == FoldData::FoldType::SEPARATOR) {
    CQChartsColumn column;

    if (! CQChartsModelUtil::stringToColumn(model().data(), foldData.columnsStr, column)) {
      bool ok;

      long icolumn = CQChartsUtil::toInt(foldData.columnsStr, ok);

      if (! ok)
        return false;

      column = CQChartsColumn(int(icolumn));
    }

    auto *proxyModel = new QSortFilterProxyModel;

    proxyModel->setObjectName("foldProxyModel");

    proxyModel->setSortRole(Qt::EditRole);

    CQHierSepData data(column.column(), foldData.separator[0]);

    auto *hierSepModel = new CQHierSepModel(model().data(), data);

    proxyModel->setSourceModel(hierSepModel);

    //---

    hierSepModel_ = ModelP(proxyModel);

    //---

    updatePropertyModel();

    emit currentModelChanged();
  }

  return true;
}

void
CQChartsModelData::
foldClear(bool notify)
{
  foldedModels_.clear();

  foldProxyModel_ = ModelP();

  hierSepModel_ = ModelP();

  //---

  if (notify) {
    delete details_;

    details_ = nullptr;

    //---

    updatePropertyModel();

    emit currentModelChanged();
  }
}

CQChartsModelData::FoldedModels
CQChartsModelData::
foldedModels() const
{
  if (! foldProxyModel_)
    return FoldedModels();

  auto *proxyModel = qobject_cast<QSortFilterProxyModel *>(foldProxyModel_.data());
  if (! proxyModel) return FoldedModels();

  auto *foldedModel = qobject_cast<CQFoldedModel *>(proxyModel);

  if (! foldedModel) {
    auto *sourceModel = proxyModel->sourceModel();

    foldedModel = qobject_cast<CQFoldedModel *>(sourceModel);
  }

  FoldedModels foldedModels;

  while (foldedModel) {
    foldedModels.push_back(foldedModel);

    foldedModel = qobject_cast<CQFoldedModel *>(foldedModel->sourceModel());
  }

  //return foldedModels_;
  return foldedModels;
}
#endif

//------

void
CQChartsModelData::
setSummaryEnabled(bool b)
{
  if (b != summaryEnabled_) {
    summaryEnabled_ = b;

    emitDataChanged();
  }
}

CQSummaryModel *
CQChartsModelData::
addSummaryModel()
{
  if (! summaryModel_) {
    summaryModel_ = new CQSummaryModel(model().data());

    summaryModelP_ = ModelP(summaryModel_);

    summaryModelData_ = new CQChartsModelData(charts_, summaryModelP_);

    updatePropertyModel();
  }

  return summaryModel_;
}

//------

CQPropertyViewModel *
CQChartsModelData::
propertyViewModel()
{
  if (! propertyModel_) {
    propertyModel_ = new CQPropertyViewModel;

    updatePropertyModel();
  }

  return propertyModel_;
}

void
CQChartsModelData::
updatePropertyModel()
{
  if (! propertyModel_)
    return;

  //---

  propertyModel_->clear();

  //---

  IdModelNames idModelNames;

  getPropertyNameData(idModelNames);

  for (const auto &p1 : idModelNames) {
    auto id         = p1.first;
    auto modelNames = p1.second;

    for (const auto &p2 : modelNames) {
      auto obj = p2.first;

      auto nameAliases = p2.second;

      for (const auto &na : nameAliases.data) {
        propertyModel_->addProperty(id, obj, na.name, na.alias);
      }
    }
  }
}

bool
CQChartsModelData::
getPropertyData(const QString &name, QVariant &value) const
{
  auto *th = const_cast<CQChartsModelData *>(this);

  auto *propertyModel = th->propertyViewModel();

  //---

  IdModelNames idModelNames;

  getPropertyNameData(idModelNames);

  for (const auto &p1 : idModelNames) {
    auto id = p1.first;

    QString prefix;

    if (id != "")
      prefix = id + ".";

    if (prefix != "" && ! name.startsWith(prefix))
      continue;

    auto modelNames = p1.second;

    for (const auto &p2 : modelNames) {
      auto obj = p2.first;

      auto nameAliases = p2.second;

      for (const auto &na : nameAliases.data) {
        auto name1 = QString("%1%2").arg(prefix).arg(na.alias != "" ? na.alias : na.name);

        if (name == name1)
          return propertyModel->getProperty(obj, name1, value);
      }
    }
  }

  //---

  return false;
}

bool
CQChartsModelData::
setPropertyData(const QString &name, const QVariant &value)
{
  auto *th = const_cast<CQChartsModelData *>(this);

  auto *propertyModel = th->propertyViewModel();

  //---

  IdModelNames idModelNames;

  getPropertyNameData(idModelNames);

  for (const auto &p1 : idModelNames) {
    auto id = p1.first;

    QString prefix;

    if (id != "")
      prefix = id + ".";

    if (prefix != "" && ! name.startsWith(prefix))
      continue;

    auto modelNames = p1.second;

    for (const auto &p2 : modelNames) {
      auto obj = p2.first;

      auto nameAliases = p2.second;

      for (const auto &na : nameAliases.data) {
        auto name1 = QString("%1%2").arg(prefix).arg(na.alias != "" ? na.alias : na.name);

        if (name == name1)
          return propertyModel->setProperty(obj, na.name, value);
      }
    }
  }

  //---

  return false;
}

void
CQChartsModelData::
getPropertyNameData(IdModelNames &names) const
{
  auto *th = const_cast<CQChartsModelData *>(this);

  names[""][th] << "ind" << "id" << "name" << "filename" << "currentColumn";

  // needed ?
  names["modelSummary"][th] << NameAlias("summaryEnabled", "enabled");

  //---

  ModelP model = this->currentModel();

  auto *absModel = CQChartsModelUtil::getBaseModel(model.data());

  std::vector<QAbstractProxyModel *> proxyModels;
  QAbstractItemModel*                sourceModel;

  CQChartsModelUtil::getProxyModels(model.data(), proxyModels, sourceModel);

  //---

  auto *baseModel = qobject_cast<CQBaseModel *>(absModel);

  if (baseModel)
    names["base"][baseModel] << "dataType" << "title" << "maxTypeRows";

  auto *dataModel = CQChartsModelUtil::getDataModel(model.data());

  if (dataModel)
    names["data"][dataModel] << "readOnly" << "filter" /* << "filename" */;

  //---

  // proxy models
  auto *exprModel = CQChartsModelUtil::getExprModel(absModel);

  if (exprModel)
    names["expr"][exprModel] << "debug";

  auto *modelFilter = qobject_cast<CQChartsModelFilter *>(absModel);

  if (modelFilter)
    names["filter"][modelFilter] << "filter" << "type" << "invert";

  auto *pivotModel = qobject_cast<CQPivotModel *>(baseModel);

  if (pivotModel)
    names["pivot"][pivotModel] << "valueType" << "includeTotals";

  auto *hierSepModel = CQChartsModelUtil::getHierSepModel(model.data());

  if (hierSepModel)
    names["hierSep"][hierSepModel] <<
      NameAlias("foldSeparator", "separator") << "foldColumn" << "propagateValue";

  auto *summaryModel = summaryModel_;

  if (! summaryModel)
    summaryModel = qobject_cast<CQSummaryModel *>(model.data());

  if (summaryModel)
    names["summary"][summaryModel] << "mode" << "maxRows" << "sortColumn" << "sortRole" <<
                                      "sortOrder" << "pageSize" << "currentPage";

  auto *bucketModel = qobject_cast<CQBucketModel *>(model.data());

  if (bucketModel)
    names["bucket"][bucketModel] << "bucketColumn" << "bucketRole" << "bucketIntegral" <<
                                    "bucketStart" << "bucketDelta" << "bucketMin" <<
                                    "bucketMax" << "bucketCount" << "multiColumn";

  //---

  // folded model
  auto *foldedModel = qobject_cast<CQFoldedModel *>(model.data());

  if (! foldedModel)
    foldedModel = qobject_cast<CQFoldedModel *>(sourceModel);

  if (! foldedModel) {
    for (const auto &proxyModel : proxyModels) {
      foldedModel = qobject_cast<CQFoldedModel *>(proxyModel);

      if (foldedModel)
        break;
    }
  }

  if (foldedModel)
    names["folded"][foldedModel] << "foldColumn" << "showFoldColumnData" << "keepFoldColumn" <<
                                    "bucketCount" << "bucketDelta";

  //---

  // collapse model
  auto *collapseModel = qobject_cast<CQCollapseModel *>(model.data());

  if (! collapseModel)
    collapseModel = qobject_cast<CQCollapseModel *>(sourceModel);

  if (! collapseModel) {
    for (const auto &proxyModel : proxyModels) {
      collapseModel = qobject_cast<CQCollapseModel *>(proxyModel);

      if (collapseModel)
        break;
    }
  }

  if (collapseModel)
    names["collapse"][collapseModel] << "columnTypes" << "collapseOps";

  //---

  // data models
  auto *csvModel = qobject_cast<CQCsvModel *>(absModel);

  if (csvModel)
    names["csv"][csvModel] << "commentHeader" << "firstLineHeader" <<
                              "firstColumnHeader" << "separator";

  auto *tsvModel = qobject_cast<CQTsvModel *>(absModel);

  if (tsvModel)
    names["tsv"][tsvModel] << "commentHeader" << "firstLineHeader" << "firstColumnHeader";

  auto *gnuModel = qobject_cast<CQGnuDataModel *>(absModel);

  if (gnuModel)
    names["gnu"][gnuModel] << "commentHeader" << "firstLineHeader" << "firstColumnHeader" <<
                              "commentChars" << "missingStr" << "separator" << "parseStrings" <<
                              "setBlankLines" << "subSetBlankLines" << "keepQuotes";

  auto *jsonModel = qobject_cast<CQJsonModel *>(absModel);

  if (jsonModel)
    names["json"][jsonModel] << "hierarchical" << "flat";
}

void
CQChartsModelData::
getPropertyNames(QStringList &names) const
{
  IdModelNames idModelNames;

  getPropertyNameData(idModelNames);

  for (const auto &p1 : idModelNames) {
    auto id = p1.first;

    for (const auto &p2 : p1.second) {
      auto modelNames = p2.second;

      for (const auto &na : modelNames.data) {
        names << QString("%1.%2").arg(id).arg(na.alias);
      }
    }
  }
}

//------

bool
CQChartsModelData::
write(std::ostream &os, const QString &varName) const
{
  auto *model = this->model().data();
  if (! model) return false;

  const auto *exprModel = CQChartsModelUtil::getExprModel(model);
  const auto *dataModel = CQChartsModelUtil::getDataModel(model);
  const auto *baseModel = CQChartsModelUtil::getBaseModel(model);

  const auto *varsModel     = dynamic_cast<const CQChartsVarsModel     *>(dataModel);
  const auto *exprDataModel = dynamic_cast<const CQChartsExprDataModel *>(dataModel);
  const auto *csvModel      = dynamic_cast<const CQCsvModel            *>(dataModel);
  const auto *tsvModel      = dynamic_cast<const CQTsvModel            *>(dataModel);
  const auto *gnuModel      = dynamic_cast<const CQGnuDataModel        *>(dataModel);
  const auto *jsonModel     = dynamic_cast<const CQJsonModel           *>(baseModel);

  if (varName != "")
    os << "set " << varName.toStdString();
  else
    os << "set model";

  os << " [load_charts_model";

  if      (csvModel) {
    if (csvModel->filename().length())
      os << " -csv {" << csvModel->filename().toStdString() << "}";

    if (csvModel->isCommentHeader    ()) os << " -comment_header";
    if (csvModel->isFirstLineHeader  ()) os << " -first_line_header";
    if (csvModel->isFirstColumnHeader()) os << " -first_column_header";
  }
  else if (tsvModel) {
    if (tsvModel->filename().length())
      os << " -tsv {" << tsvModel->filename().toStdString() << "}";

    if (tsvModel->isCommentHeader    ()) os << " -comment_header";
    if (tsvModel->isFirstLineHeader  ()) os << " -first_line_header";
    if (tsvModel->isFirstColumnHeader()) os << " -first_column_header";
  }
  else if (jsonModel) {
    if (jsonModel->filename().length())
      os << " -json {" << jsonModel->filename().toStdString() << "}";
  }
  else if (gnuModel) {
    if (gnuModel->filename().length())
      os << " -data {" << gnuModel->filename().toStdString() << "}";

    if (gnuModel->isCommentHeader    ()) os << " -comment_header";
    if (gnuModel->isFirstLineHeader  ()) os << " -first_line_header";
    if (gnuModel->isFirstColumnHeader()) os << " -first_column_header";
  }
  else if (varsModel) {
    auto varNames = varsModel->varNames();

    os << " -var {";

    for (int i = 0; i < varNames.length(); ++i) {
      if (i > 0)
        os << " ";

      os << varNames[i].toStdString();
    }

    os << "}";

    if (varsModel->isTranspose())
      os << " -transpose";

    if (varsModel->isFirstColumnHeader())
      os << " -first_column_header";

    if (varsModel->isFirstLineHeader())
      os << " -first_line_header";
  }
  else if (exprDataModel) {
    os << " -expr -num_rows " << exprDataModel->n();
  }
  else {
    auto name = model->objectName();

    os << " -model {" << name.toStdString() << "}";
  }

  if (name() != "") {
    os << " -name {" << name().toStdString() << "}";
  }

  //---

  const auto *details = this->details();

  int nc = details->numColumns();

  int numExtra = 0;

  if (exprModel)
    numExtra = exprModel->numExtraColumns();

  auto *columnTypeMgr = charts_->columnTypeMgr();

  for (int i = 0; i < nc - numExtra; ++i) {
    CQChartsColumn column(i);

    CQChartsModelTypeData columnTypeData;

    if (! columnTypeMgr->getModelColumnType(model, column, columnTypeData))
      continue;

    if (columnTypeData.type != CQBaseModelType::STRING ||
        ! columnTypeData.nameValues.nameValues().empty()) {
      const auto *columnTypePtr = columnTypeMgr->getType(columnTypeData.type);

      os << " -column_type {{";

      os << "{" << i << " " << columnTypePtr->name().toStdString() << "}";

      for (const auto &nv : columnTypeData.nameValues.nameValues()) {
        const auto &name = nv.first;
        const auto &var  = nv.second;

        if (! var.isValid())
          continue;

        const auto *param = columnTypePtr->getParam(name);

        if (var != param->def()) {
          QStringList strs;

          strs << name;
          strs << var.toString();

          os << " {" << CQTcl::mergeList(strs).toStdString() << "}";
        }
      }

      os << "}}";
    }
  }

  os << "]\n";

  //---

  if (exprModel) {
    int nc = exprModel->columnCount();

    for (int i = nc - exprModel->numExtraColumns(); i < nc; ++i) {
      CQChartsColumn column(i);

      //---

      QString header, expr;

      exprModel->getExtraColumnDetails(i, header, expr);

      os << "\n";

      os << "process_charts_model -add";

      if (header != "")
        os << " -header {" << header.toStdString() << "}";

      if (expr != "")
        os << " -expr {" << expr.toStdString() << "}";

      //---

      CQChartsModelTypeData columnTypeData;

      if (! columnTypeMgr->getModelColumnType(model, column, columnTypeData))
        continue;

      if (columnTypeData.type != CQBaseModelType::STRING ||
          ! columnTypeData.nameValues.nameValues().empty()) {
        const auto *columnTypePtr = columnTypeMgr->getType(columnTypeData.type);

        os << " -type {";

        os << "{" << columnTypePtr->name().toStdString() << "}";

        for (const auto &nv : columnTypeData.nameValues.nameValues()) {
          const auto &name = nv.first;
          const auto &var  = nv.second;

          if (! var.isValid())
            continue;

          const auto *param = columnTypePtr->getParam(name);

          if (var != param->def()) {
            QStringList strs;

            strs << name;
            strs << var.toString();

            os << " {" << CQTcl::mergeList(strs).toStdString() << "}";
          }
        }

        os << "}";
      }

      os << "\n";
    }
  }

  return true;
}

bool
CQChartsModelData::
exportModel(const QString &filename, const CQBaseModelDataType &type, bool hheader, bool vheader)
{
  if (type == CQBaseModelDataType::CSV) {
    if (hheader && ! vheader)
      return writeCSV(filename);
  }

  //---

  if (filename != "-") {
    auto os = std::ofstream(filename.toStdString(), std::ofstream::out);

    return CQChartsModelUtil::exportModel(model().data(), type, hheader, vheader, os);
  }
  else
    return CQChartsModelUtil::exportModel(model().data(), type, hheader, vheader, std::cout);
}

bool
CQChartsModelData::
exportModel(std::ostream &fs, const CQBaseModelDataType &type, bool hheader, bool vheader)
{
  if (type == CQBaseModelDataType::CSV) {
    if (hheader && ! vheader)
      return writeCSV(fs);
  }

  //---

  return CQChartsModelUtil::exportModel(model().data(), type, hheader, vheader, fs);
}

bool
CQChartsModelData::
writeCSV(const QString &filename) const
{
  if (filename != "-") {
    auto os = std::ofstream(filename.toStdString(), std::ofstream::out);

    return writeCSV(os);
  }
  else
    return writeCSV(std::cout);
}

bool
CQChartsModelData::
writeCSV(std::ostream &fs) const
{
  const auto *details = this->details();

  if (details->isHierarchical())
    return false;

  auto *model = this->model().data();
  if (! model) return false;

  //---

  int nr = details->numRows();
  int nc = details->numColumns();

  if (nr < 0 || nc < 0)
    return false;

  //---

  // write meta data
  fs << "#META_DATA\n";

  for (int ic = 0; ic < nc; ++ic) {
    CQChartsColumn c(ic);

    const auto *columnDetails = details->columnDetails(c);

    bool ok;

    auto header = CQChartsModelUtil::modelHHeaderString(model, c, ok);

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

    auto type     = columnDetails->type();
    auto typeName = columnDetails->typeName();

    writeMetaColumnData("type", typeName);

    const auto &nullValue = columnDetails->nullValue();

    if (nullValue.length())
      writeMetaColumnData("null_value", nullValue);

    writeMetaColumnData("key", "1");

    const auto &drawColor = columnDetails->tableDrawColor();

    auto tableDrawType  = columnDetails->tableDrawType();
    auto tableDrawStops = columnDetails->tableDrawStops();

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
      writeMetaColumnNameValue("iformat");
      writeMetaColumnNameValue("oformat");
      writeMetaColumnNameValue("format_scale");
      writeMetaColumnNameValue("min");
      writeMetaColumnNameValue("max");
    }
    else if (type == CQBaseModelType::INTEGER) {
      writeMetaColumnNameValue("format");
      writeMetaColumnNameValue("iformat");
      writeMetaColumnNameValue("oformat");
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
  for (int ic = 0; ic < nc; ++ic) {
    CQChartsColumn c(ic);

    bool ok;

    auto header = CQChartsModelUtil::modelHHeaderString(model, c, ok);

    if (ic > 0)
      fs << ",";

    fs << CQCsvModel::encodeString(header).toStdString();
  }

  fs << "\n";

  //---

  // write data
  for (int r = 0; r < nr; ++r) {
    for (int ic = 0; ic < nc; ++ic) {
      CQChartsColumn c(ic);

      auto ind = model->index(r, ic);

      QVariant var;

      const auto *columnDetails = details->columnDetails(c);

      auto type = columnDetails->type();

      bool converted = false;

      if (type == CQBaseModelType::TIME) {
        var = model->data(ind, Qt::EditRole);

        if (var.isValid()) {
          bool ok;

          double r = CQChartsVariant::toReal(var, ok);

          if (ok) {
            const auto *timeType =
              dynamic_cast<const CQChartsColumnTimeType *>(columnDetails->columnType());
            assert(timeType);

            auto fmt = timeType->getIFormat(columnDetails->nameValues());

            if (fmt.trimmed() != "") {
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

      if (ic > 0)
        fs << ",";

      fs << CQCsvModel::encodeString(var.toString()).toStdString();
    }

    fs << "\n";
  }

  return true;
}

//------

int
CQChartsModelData::
replaceValue(const CQChartsColumn &column, const QVariant &oldValue, const QVariant &newValue)
{
  int icolumn = column.column();
  if (icolumn < 0) return 0;

  auto *model = this->model().data();
  if (! model) return 0;

  auto *details = this->details();

  int nr = details->numRows();

  //---

  // update model data
  int numReplaced = 0;

  auto *dataModel = CQChartsModelUtil::getDataModel(model);

  bool readOnly = (dataModel ? dataModel->isReadOnly() : false);

  if (readOnly)
    dataModel->setReadOnly(false);

  for (int r = 0; r < nr; ++r) {
    auto ind = model->index(r, icolumn);

    auto var = model->data(ind);

    if (var == oldValue) {
      model->setData(ind, newValue);

      ++numReplaced;
    }
  }

  if (readOnly)
    dataModel->setReadOnly(true);

  return numReplaced;
}

//------

int
CQChartsModelData::
replaceNullValues(const CQChartsColumn &column, const QVariant &newValue)
{
  int icolumn = column.column();
  if (icolumn < 0) return 0;

  auto *model = this->model().data();
  if (! model) return 0;

  auto *details = this->details();

  int nr = details->numRows();

  //---

  auto *columnDetails = details->columnDetails(column);

  auto nullStr = columnDetails->nullValue();

  auto isNullVar = [&](const QVariant &var) {
    return (! var.isValid() || var.toString() == nullStr);
  };

  //---

  // update model data
  int numReplaced = 0;

  auto *dataModel = CQChartsModelUtil::getDataModel(model);

  bool readOnly = (dataModel ? dataModel->isReadOnly() : false);

  if (readOnly)
    dataModel->setReadOnly(false);

  for (int r = 0; r < nr; ++r) {
    auto ind = model->index(r, icolumn);

    auto var = model->data(ind);

    if (isNullVar(var)) {
      model->setData(ind, newValue);

      ++numReplaced;
    }
  }

  if (readOnly)
    dataModel->setReadOnly(true);

  return numReplaced;
}

//------

QAbstractItemModel *
CQChartsModelData::
copy(const CopyData &copyData)
{
  auto *model = this->model().data();
  if (! model) return nullptr;

  CQChartsExprTcl *expr = nullptr;

  if (copyData.filter.size())
    expr = new CQChartsExprTcl(model);

  auto *details = this->details();

  int nc = details->numColumns();
  int nr = details->numRows();

  if (copyData.nr > 0)
    nr = copyData.nr;

  //---

  // get visible rows
  using RowVisible = std::map<int, bool>;

  RowVisible rowVisible;

  int nr1 = 0;

  if (expr) {
    bool showError = copyData.debug;

    // set column name variables
    for (int ic = 0; ic < nc; ++ic) {
      CQChartsColumn c(ic);

      bool ok;

      auto name = CQChartsModelUtil::modelHHeaderString(model_.data(), c, ok);

      expr->setNameColumn(expr->encodeColumnName(name), ic);

      //---

      const auto *columnDetails = details->columnDetails(c);

      if (columnDetails) {
        auto type = columnDetails->type();

        if (type == CQBaseModelType::TIME)
          expr->setColumnRole(ic, Qt::DisplayRole);
      }
    }

    expr->initVars();
    expr->initFunctions();

    //---

    // check each row visible
    for (int r = 0; r < nr; ++r) {
      bool visible = true;

      expr->setRow   (r);
      expr->setColumn(0);

      QVariant value;

      if (expr->evaluateExpression(copyData.filter, value, showError)) {
        bool ok;
        visible = CQChartsVariant::toBool(value, ok);
      }

      rowVisible[r] = visible;

      if (visible)
        ++nr1;
    }
  }
  else {
    for (int r = 0; r < nr; ++r) {
      rowVisible[r] = true;

      ++nr1;
    }
  }

  //---

  auto *dataModel = new CQDataModel(nc, nr1);

  //---

  // copy horizontal header data
  for (int ic = 0; ic < nc; ++ic) {
    CQChartsColumn c(ic);

    bool ok;

    auto var = CQChartsModelUtil::modelHHeaderString(model, c, ok);

    if (var.length())
      dataModel->setHeaderData(ic, Qt::Horizontal, var, Qt::DisplayRole);
  }

  // copy vertical header data
  int r1 = 0;

  for (int r = 0; r < nr; ++r) {
    if (! rowVisible[r])
      continue;

    auto var = model->headerData(r, Qt::Vertical, Qt::DisplayRole);

    if (var.isValid()) {
      auto var1 = dataModel->headerData(r1, Qt::Vertical, Qt::DisplayRole);

      if (var != var1)
        dataModel->setHeaderData(r1, Qt::Vertical, var, Qt::DisplayRole);
    }

    ++r1;
  }

  //---

  // copy model data
  r1 = 0;

  for (int r = 0; r < nr; ++r) {
    if (! rowVisible[r])
      continue;

    for (int ic = 0; ic < nc; ++ic) {
      auto ind1 = model->index(r, ic);

      auto var = model->data(ind1);

      if (var.isValid()) {
        auto ind2 = dataModel->index(r1, ic);

        dataModel->setData(ind2, var);
      }
    }

    ++r1;
  }

  //---

  // copy horizontal header data
  copyHeaderRoles(dataModel);

  //---

  // create model
  auto *filterModel = new CQChartsFilterModel(charts_, dataModel);

  filterModel->setObjectName("copyModel");

  //---

  delete expr;

  return filterModel;
}

//------

QAbstractItemModel *
CQChartsModelData::
join(CQChartsModelData *joinModelData, const Columns &joinColumns)
{
  if (joinColumns.empty())
    return nullptr;

  //--

  auto *details = this->details();

  int nc = details->numColumns();
  int nr = details->numRows();

  //---

  using ColumnSet = std::set<int>;

  ColumnSet columnSet;

  for (const auto &joinColumn : joinColumns) {
    int ijoinColumn = joinColumn.column();

    if (ijoinColumn < 0 || ijoinColumn >= nc)
      return nullptr;

    columnSet.insert(ijoinColumn);
  }

  //---

  auto *joinDetails = joinModelData->details();

  int joinNc = joinDetails->numColumns();
  int joinNr = joinDetails->numRows();

  //---

  auto *model = this->model().data();
  if (! model) return nullptr;

  auto *joinModel = joinModelData->model().data();
  if (! joinModel) return nullptr;

  //---

  // get name of join column(s)
  bool ok;

  QStringList columnNames;

  for (const auto &joinColumn : joinColumns) {
    auto columnName = CQChartsModelUtil::modelHHeaderString(model, joinColumn, ok);

    columnNames.push_back(columnName);
  }

  //---

  // find column with matching name in join model
  ColumnSet joinColumnSet;

  for (int ic = 0; ic < joinNc; ++ic) {
    CQChartsColumn c(ic);

    bool ok;

    auto joinColumnName = CQChartsModelUtil::modelHHeaderString(joinModel, c, ok);

    if (columnNames.contains(joinColumnName))
      joinColumnSet.insert(ic);
  }

  if (joinColumnSet.size() != joinColumns.size())
    return nullptr;

  //---

  auto encodeVariantList = [&](const QVariantList &vars) {
    if (vars.size() == 1)
      return vars[0];

    QStringList strs;

    for (const auto &var : vars)
      strs.push_back(var.toString());

    return QVariant(strs.join(":"));
  };

  //---

  // build map of join columns variant/name to row
  using VariantRowMap = std::map<QVariant, int>;

  VariantRowMap variantRowMap;

  for (int r = 0; r < joinNr; ++r) {
    QVariantList vars;

    for (const auto &ijoinColumn : joinColumnSet) {
      auto ind1 = joinModel->index(r, ijoinColumn);

      auto var = joinModel->data(ind1, Qt::EditRole);

      if (! var.isValid())
        var = joinModel->data(ind1, Qt::DisplayRole);

      vars.push_back(var);
    }

    auto var = encodeVariantList(vars);

    variantRowMap[var] = r;
  }

  //---

  auto *dataModel = new CQDataModel(int(size_t(nc + joinNc) - joinColumns.size()), nr);

  //---

  // copy model data
  for (int r = 0; r < nr; ++r) {
    // add model column data and generate join key
    QVariantList joinVars;

    for (int ic = 0; ic < nc; ++ic) {
      auto ind1 = model->index(r, ic);

      auto var = model->data(ind1);

      if (columnSet.find(ic) != columnSet.end())
        joinVars.push_back(var);

      if (var.isValid()) {
        auto ind2 = dataModel->index(r, ic);

        dataModel->setData(ind2, var);
      }
    }

    auto joinVar = encodeVariantList(joinVars);

    //---

    // get matching row in join model for key
    auto p = variantRowMap.find(joinVar);

    if (p == variantRowMap.end())
      continue;

    int joinRow = (*p).second;

    //---

    // add join module column values for matching row
    int c1 = nc;

    for (int ic = 0; ic < joinNc; ++ic) {
      // skip join columns
      if (joinColumnSet.find(ic) != joinColumnSet.end())
        continue;

      auto ind1 = joinModel->index(joinRow, ic);

      auto var = joinModel->data(ind1, Qt::EditRole);

      if (! var.isValid())
        var = joinModel->data(ind1, Qt::DisplayRole);

      if (var.isValid()) {
        auto ind2 = dataModel->index(r, c1);

        dataModel->setData(ind2, var);
      }

      ++c1;
    }
  }

  //---

  // copy horizontal header data
  copyHeaderRoles(dataModel);

  int c1 = nc;

  for (int ic = 0; ic < joinNc; ++ic) {
    // skip join columns
    if (joinColumnSet.find(ic) != joinColumnSet.end())
      continue;

    joinModelData->copyColumnHeaderRoles(dataModel, ic, c1);

    ++c1;
  }

  //---

  // create model
  auto *filterModel = new CQChartsFilterModel(charts_, dataModel);

  filterModel->setObjectName("joinModel");

  //---

  return filterModel;
}

//------

QAbstractItemModel *
CQChartsModelData::
groupColumns(const Columns &groupColumns)
{
  if (groupColumns.empty())
    return nullptr;

  //--

  auto *details = this->details();

  int nc = details->numColumns();
  int nr = details->numRows();

  //---

  using ColumnSet = std::set<int>;

  ColumnSet groupColumnSet;

  for (const auto &groupColumn : groupColumns) {
    int icolumn = groupColumn.column();

    if (icolumn < 0 || icolumn >= nc)
      return nullptr;

    groupColumnSet.insert(icolumn);
  }

  //---

  auto *model = this->model().data();
  if (! model) return nullptr;

  //---

  // create new model
  int ng = int(groupColumnSet.size());

  auto *dataModel = new CQDataModel(nc - ng + 2, nr*ng);

  //---

  // copy header data
  int c1 = 0;

  for (int c = 0; c < nc; ++c) {
    if (groupColumnSet.find(c) != groupColumnSet.end())
      continue;

    copyColumnHeaderRoles(dataModel, c, c1);

    ++c1;
  }

  dataModel->setHeaderData(c1, Qt::Horizontal, "Group", Qt::DisplayRole); ++c1;
  dataModel->setHeaderData(c1, Qt::Horizontal, "Value", Qt::DisplayRole); ++c1;

  //---

  // copy model data
  int r1 = 0;

  for (const auto &groupColumn : groupColumnSet) {
    bool ok;

    auto columnName = CQChartsModelUtil::modelHHeaderString(model, CQChartsColumn(groupColumn), ok);

    for (int r = 0; r < nr; ++r, ++r1) {
      int c1 = 0;

      for (int c = 0; c < nc; ++c) {
        if (groupColumnSet.find(c) != groupColumnSet.end())
          continue;

        auto ind1 = model->index(r, c);

        auto var = model->data(ind1);

        auto ind2 = dataModel->index(r1, c1);

        dataModel->setData(ind2, var);

        ++c1;
      }

      //---

      auto ind3 = dataModel->index(r1, c1); ++c1;

      dataModel->setData(ind3, columnName);

      auto ind4 = model->index(r, groupColumn);

      auto var = model->data(ind4);

      auto ind5 = dataModel->index(r1, c1); ++c1;

      dataModel->setData(ind5, var);
    }
  }

  //---

  // create model
  auto *filterModel = new CQChartsFilterModel(charts_, dataModel);

  filterModel->setObjectName("groupModel");

  //---

  return filterModel;
}

//------

void
CQChartsModelData::
copyHeaderRoles(QAbstractItemModel *toModel) const
{
  const auto *details = this->details();

  int nc = details->numColumns();

  for (int ic = 0; ic < nc; ++ic)
    copyColumnHeaderRoles(toModel, ic, ic);
}

void
CQChartsModelData::
copyColumnHeaderRoles(QAbstractItemModel *toModel, int c1, int c2) const
{
  static std::vector<int> hroles = {{
    Qt::DisplayRole,
    CQModelUtil::roleCast(CQBaseModelRole::Type),
    CQModelUtil::roleCast(CQBaseModelRole::BaseType),
    CQModelUtil::roleCast(CQBaseModelRole::TypeValues),
    CQModelUtil::roleCast(CQBaseModelRole::Min),
    CQModelUtil::roleCast(CQBaseModelRole::Max),
    CQModelUtil::roleCast(CQBaseModelRole::Key),
    CQModelUtil::roleCast(CQBaseModelRole::Sorted),
    CQModelUtil::roleCast(CQBaseModelRole::SortOrder),
    CQModelUtil::roleCast(CQBaseModelRole::HeaderType),
    CQModelUtil::roleCast(CQBaseModelRole::HeaderTypeValues)
  }};

  auto *model = this->model().data();
  if (! model) return;

  // copy horizontal header data
  for (const auto &role : hroles) {
    auto var = model->headerData(c1, Qt::Horizontal, role);

    if (var.isValid())
      toModel->setHeaderData(c2, Qt::Horizontal, var, role);
  }
}
