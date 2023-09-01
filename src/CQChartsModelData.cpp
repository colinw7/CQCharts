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

#include <CQBucketModel.h>
#include <CQCollapseModel.h>
#include <CQCsvModel.h>
#include <CQGnuDataModel.h>
#include <CQHierSepModel.h>
#include <CQJsonModel.h>
#include <CQFoldedModel.h>
#include <CQPivotModel.h>
#include <CQSummaryModel.h>
#include <CQTransposeModel.h>
#include <CQTsvModel.h>

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
     h2("CSV").
     p("CSV (Command Separated Values) uses rows of comma separated values to define "
       "the model data. An optional header can be specified using the first row or the "
       "first commented line (using #).").
     p("CSV can also have meta data in comments to provide extra data");
  }
  else if (type == "TSV") {
    return CQChartsHtml().
     h2("TSV").
     p("TSV (Tabbed Separated Values) uses rows of tab separated values to define "
       "the model data. An optional header can be specified using the first row or the "
       "first commented line (using #).");
  }
  else if (type == "Json") {
    return CQChartsHtml().
     h2("Json").
     p("Json (Javascript object notation) provides a structured, hierarchical or flat "
       "data structure for the model values.");
  }
  else if (type == "Data") {
    return CQChartsHtml().
     h2("Data").
     p("Data models are GNU Plot like data files where rows of space separated values to define "
       "the model data.");
  }
  else if (type == "Expr") {
    return CQChartsHtml().
     h2("Expr").
     p("Expr models derived from an existing model and add extra columns using tcl expressions "
       "to define how the new columns are generated from existing values.");
  }
  else if (type == "Vars") {
    return CQChartsHtml().
     h2("Vars").
     p("Vars models use a list of tcl variables to defined the columns or rows of the model.");
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

  delete bucketModelData_   .modelData;
//delete collapseModelData_ .modelData;
//delete foldedModelData_   .modelData;
  delete hierSepModelData_  .modelData;
  delete pivotModelData_    .modelData;
//delete subsetModelaData_  .modelData;
  delete summaryModelData_  .modelData;
  delete transposeModelData_.modelData;

  delete fileWatcher_;
}

bool
CQChartsModelData::
isInd(int ind) const
{
  if (ind == this->ind())
    return true;

  int proxyInd;

  if (bucketModelData_.model) {
    if (charts_->getModelInd(bucketModelData_.model, proxyInd) && ind == proxyInd)
      return true;
  }

  if (hierSepModelData_.model) {
    if (charts_->getModelInd(hierSepModelData_.model, proxyInd) && ind == proxyInd)
      return true;
  }

  if (pivotModelData_.model) {
    if (charts_->getModelInd(pivotModelData_.model, proxyInd) && ind == proxyInd)
      return true;
  }

  if (summaryModelData_.model) {
    if (charts_->getModelInd(summaryModelData_.model, proxyInd) && ind == proxyInd)
      return true;
  }

  if (transposeModelData_.model) {
    if (charts_->getModelInd(transposeModelData_.model, proxyInd) && ind == proxyInd)
      return true;
  }

  return false;
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

bool
CQChartsModelData::
isHierarchical() const
{
  auto *details = this->details();

  if (details)
    return details->isHierarchical();

  auto *model = this->model().data();

  return CQChartsModelUtil::isHierarchical(model);
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

  Q_EMIT currentColumnChanged(i);
}

void
CQChartsModelData::
connectModel(bool b)
{
  if (! model().data())
    return;

  CQUtil::connectDisconnect(b,
    model().data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
    this, SLOT(modelDataChangedSlot(const QModelIndex &, const QModelIndex &)));
  CQUtil::connectDisconnect(b,
    model().data(), SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
    this, SLOT(modelHeaderDataChangedSlot(Qt::Orientation, int, int)));

  CQUtil::connectDisconnect(b,
    model().data(), SIGNAL(layoutChanged()), this, SLOT(modelLayoutChangedSlot()));
  CQUtil::connectDisconnect(b,
    model().data(), SIGNAL(modelReset()), this, SLOT(modelResetSlot()));

  CQUtil::connectDisconnect(b,
    model().data(), SIGNAL(rowsInserted(QModelIndex, int, int)),
    this, SLOT(modelRowsInsertedSlot()));
  CQUtil::connectDisconnect(b,
    model().data(), SIGNAL(rowsRemoved(QModelIndex, int, int)),
    this, SLOT(modelRowsRemovedSlot()));
  CQUtil::connectDisconnect(b,
    model().data(), SIGNAL(columnsInserted(QModelIndex, int, int)),
    this, SLOT(modelColumnsInsertedSlot()));
  CQUtil::connectDisconnect(b,
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
modelHeaderDataChangedSlot(Qt::Orientation /*orient*/, int /*start*/, int /*end*/)
{
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
currentModel(bool proxy) const
{
#ifdef CQCHARTS_FOLDED_MODEL
  if (! foldedModels_.empty())
    return foldProxyModel_;

  if (hierSepModel_)
    return hierSepModel_;
#endif

  if (proxy && isBucketEnabled() && bucketModelData_.model)
    return bucketModelData_.modelP;

  if (proxy && isHierSepEnabled() && hierSepModelData_.model)
    return hierSepModelData_.modelP;

  if (proxy && isPivotEnabled() && pivotModelData_.model)
    return pivotModelData_.modelP;

  if (proxy && isSummaryEnabled() && summaryModelData_.model)
    return summaryModelData_.modelP;

  if (proxy && isTransposeEnabled() && transposeModelData_.model)
    return transposeModelData_.modelP;

  return model_;
}

//---

void
CQChartsModelData::
startUpdate()
{
  updating_ = true;

  modelChanged_ = false;
  dataChanged_  = false;
}

void
CQChartsModelData::
endUpdate()
{
  updating_ = false;

  if (modelChanged_)
    emitModelChanged();

  if (dataChanged_)
    emitDataChanged();
}

void
CQChartsModelData::
emitDeleted()
{
  Q_EMIT deleted();
}

void
CQChartsModelData::
emitModelChanged()
{
  if (updating_)
    modelChanged_ = true;
  else
    Q_EMIT modelChanged();
}

void
CQChartsModelData::
emitDataChanged()
{
  if (updating_)
    dataChanged_ = true;
  else
    Q_EMIT dataChanged();
}

//---

void
CQChartsModelData::
addSelectionModel(QItemSelectionModel *sm)
{
  sm->select(sel_, QItemSelectionModel::ClearAndSelect);

  connect(sm, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this, SLOT(selectionSlot()));

  selectionModels_.emplace_back(sm);
}

void
CQChartsModelData::
removeSelectionModel(QItemSelectionModel *sm)
{
  size_t i   = 0;
  auto   len = selectionModels_.size();

  for ( ; i < len; ++i) {
    if (selectionModels_[size_t(i)] == sm)
      break;
  }

  if (i >= len)
    return;

  disconnect(sm, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
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
  sel_ = sel;

  for (auto &sm : selectionModels_) {
    if (! sm) continue;

    CQUtil::AutoDisconnect autoDisconnect(
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

  sel_ = sm->selection();

  Q_EMIT selectionChanged(sm);
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

    auto columnStrs = foldData.columnsStr.split(",", Qt::SkipEmptyParts);

    for (int i = 0; i < columnStrs.length(); ++i) {
      auto columnSubStrs = columnStrs[i].split(":", Qt::SkipEmptyParts);

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

    Q_EMIT currentModelChanged();
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

    Q_EMIT currentModelChanged();
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

    Q_EMIT currentModelChanged();
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
setBucketEnabled(bool b)
{
  if (b != bucketModelData_.enabled) {
    bucketModelData_.enabled = b;

    emitDataChanged();

    if (bucketModelData_.model)
      Q_EMIT currentModelChanged();
  }
}

CQBucketModel *
CQChartsModelData::
addBucketModel()
{
  if (! bucketModelData_.model) {
    bucketModelData_.model     = new CQBucketModel(model().data());
    bucketModelData_.modelP    = ModelP(bucketModelData_.model);
    bucketModelData_.modelData = new CQChartsModelData(charts_, bucketModelData_.modelP);

    int ind;
    charts_->assignModelInd(bucketModelData_.model, ind);

    updatePropertyModel();
  }

  return bucketModelData_.model;
}

//------

void
CQChartsModelData::
setHierSepEnabled(bool b)
{
  if (b != hierSepModelData_.enabled) {
    hierSepModelData_.enabled = b;

    emitDataChanged();

    if (hierSepModelData_.model)
      Q_EMIT currentModelChanged();
  }
}

CQHierSepModel *
CQChartsModelData::
addHierSepModel()
{
  if (! hierSepModelData_.model) {
    hierSepModelData_.model     = new CQHierSepModel(model().data());
    hierSepModelData_.modelP    = ModelP(hierSepModelData_.model);
    hierSepModelData_.modelData = new CQChartsModelData(charts_, hierSepModelData_.modelP);

    int ind;
    charts_->assignModelInd(hierSepModelData_.model, ind);

    updatePropertyModel();
  }

  return hierSepModelData_.model;
}

//------

void
CQChartsModelData::
setTransposeEnabled(bool b)
{
  if (b != transposeModelData_.enabled) {
    transposeModelData_.enabled = b;

    emitDataChanged();

    if (transposeModelData_.model)
      Q_EMIT currentModelChanged();
  }
}

CQTransposeModel *
CQChartsModelData::
addTransposeModel()
{
  if (! transposeModelData_.model) {
    transposeModelData_.model     = new CQTransposeModel(model().data());
    transposeModelData_.modelP    = ModelP(transposeModelData_.model);
    transposeModelData_.modelData = new CQChartsModelData(charts_, transposeModelData_.modelP);

    int ind;
    charts_->assignModelInd(transposeModelData_.model, ind);

    updatePropertyModel();
  }

  return transposeModelData_.model;
}

//------

void
CQChartsModelData::
setPivotEnabled(bool b)
{
  if (b != pivotModelData_.enabled) {
    pivotModelData_.enabled = b;

    emitDataChanged();

    if (pivotModelData_.model)
      Q_EMIT currentModelChanged();
  }
}

CQPivotModel *
CQChartsModelData::
addPivotModel()
{
  if (! pivotModelData_.model) {
    pivotModelData_.model     = new CQPivotModel(model().data());
    pivotModelData_.modelP    = ModelP(pivotModelData_.model);
    pivotModelData_.modelData = new CQChartsModelData(charts_, pivotModelData_.modelP);

    int ind;
    charts_->assignModelInd(pivotModelData_.model, ind);

    updatePropertyModel();
  }

  return pivotModelData_.model;
}

//------

void
CQChartsModelData::
setSummaryEnabled(bool b)
{
  if (b != summaryModelData_.enabled) {
    summaryModelData_.enabled = b;

    emitDataChanged();

    if (summaryModelData_.model)
      Q_EMIT currentModelChanged();
  }
}

CQSummaryModel *
CQChartsModelData::
addSummaryModel()
{
  if (! summaryModelData_.model) {
    summaryModelData_.model     = new CQSummaryModel(model().data());
    summaryModelData_.modelP    = ModelP(summaryModelData_.model);
    summaryModelData_.modelData = new CQChartsModelData(charts_, summaryModelData_.modelP);

    int ind;
    charts_->assignModelInd(summaryModelData_.model, ind);

    updatePropertyModel();
  }

  return summaryModelData_.model;
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

  //--

  auto *pivotModel = pivotModelData_.model;

  if (! pivotModel)
    pivotModel = qobject_cast<CQPivotModel *>(baseModel);

  if (pivotModel)
    names["pivot"][pivotModel] << "valueType" << "includeTotals";

  //--

  // hier sep model
  auto *hierSepModel = hierSepModelData_.model;

  if (! hierSepModel)
    hierSepModel = CQChartsModelUtil::getHierSepModel(model.data());

  if (hierSepModel)
    names["hierSep"][hierSepModel] <<
      NameAlias("foldSeparator", "separator") << "foldColumn" << "propagateValue";

  //--

  // summary model
  auto *summaryModel = summaryModelData_.model;

  if (! summaryModel)
    summaryModel = qobject_cast<CQSummaryModel *>(model.data());

  if (summaryModel)
    names["summary"][summaryModel] << "mode" << "maxRows" << "sortColumn" << "sortRole" <<
                                      "sortOrder" << "pageSize" << "currentPage";

  //--

  // bucket model
  auto *bucketModel = bucketModelData_.model;

  if (! bucketModel)
    bucketModel = qobject_cast<CQBucketModel *>(model.data());

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

  // csv model (read only ?)
  auto *csvModel = qobject_cast<CQCsvModel *>(absModel);

  if (csvModel)
    names["csv"][csvModel] << "commentHeader" << "firstLineHeader" <<
                              "firstColumnHeader" << "separator";

  // tsv model (read only ?)
  auto *tsvModel = qobject_cast<CQTsvModel *>(absModel);

  if (tsvModel)
    names["tsv"][tsvModel] << "commentHeader" << "firstLineHeader" << "firstColumnHeader";

  // gnu model (read only ?)
  auto *gnuModel = qobject_cast<CQGnuDataModel *>(absModel);

  if (gnuModel)
    names["gnu"][gnuModel] << "commentHeader" << "firstLineHeader" << "firstColumnHeader" <<
                              "commentChars" << "missingStr" << "separator" << "parseStrings" <<
                              "setBlankLines" << "subSetBlankLines" << "keepQuotes";

  // jso model
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

  //---

  if      (csvModel || tsvModel || jsonModel || gnuModel) {
  }
  else if (varsModel) {
    os << "# NOTE: definition of tcl variable not saved\n";
  }
  else if (exprDataModel) {
    os << "# NOTE: expr model values not saved\n";
  }
  else {
    os << "# NOTE: model values not saved\n";
  }

  //---

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

    auto header = CQChartsModelUtil::columnToString(model, c, ok);

    QStringList writtenNames;

    auto writeMetaColumnData = [&](const QString &name, const QString &value, bool write) {
      writtenNames << name;

      if (write)
        fs << "#  column," << header.toStdString() << "," <<
              name.toStdString() << "," << value.toStdString() << "\n";
    };

    auto writeMetaColumnNameValue = [&](const QString &name) {
      QString value;

      bool write = columnDetails->columnNameValue(name, value);

      writeMetaColumnData(name, value, write);
    };

    //---

    auto typeName = columnDetails->typeName();

    writeMetaColumnData("type", typeName, true);

    //--

    const auto &nullValue = columnDetails->nullValue();

    writeMetaColumnData("null_value", nullValue, nullValue.length());

    //--

    writeMetaColumnData("key", "1", columnDetails->isKey());

    //--

    writeMetaColumnNameValue("preferred_width");

    writeMetaColumnNameValue("named_values");
    writeMetaColumnNameValue("named_colors");
    writeMetaColumnNameValue("named_images");

    //--

    const auto &drawColor = columnDetails->tableDrawColor();

    writeMetaColumnData("draw_color", drawColor.toString(), drawColor.isValid());

    //--

    auto tableDrawType = columnDetails->tableDrawType();

    QString drawTypeStr;

    if      (tableDrawType == CQChartsModelColumnDetails::TableDrawType::HEATMAP)
      drawTypeStr = "heatmap";
    else if (tableDrawType == CQChartsModelColumnDetails::TableDrawType::BARCHART)
      drawTypeStr = "barchart";

    writeMetaColumnData("draw_type", drawTypeStr, drawTypeStr.length());

    //--

    auto tableDrawStops = columnDetails->tableDrawStops();

    writeMetaColumnData("draw_stops", tableDrawStops.toString(), tableDrawStops.isValid());

    //--

    auto type = columnDetails->type();

    if      (type == CQBaseModelType::REAL) {
      writeMetaColumnNameValue("format");
      writeMetaColumnNameValue("iformat");
      writeMetaColumnNameValue("oformat");
      writeMetaColumnNameValue("format_scale");
      writeMetaColumnNameValue("min");
      writeMetaColumnNameValue("max");
      writeMetaColumnNameValue("sum");
      writeMetaColumnNameValue("decreasing");
      writeMetaColumnNameValue("bad_value");
    }
    else if (type == CQBaseModelType::INTEGER) {
      writeMetaColumnNameValue("format");
      writeMetaColumnNameValue("iformat");
      writeMetaColumnNameValue("oformat");
      writeMetaColumnNameValue("min");
      writeMetaColumnNameValue("max");
      writeMetaColumnNameValue("sum");
    }
    else if (type == CQBaseModelType::TIME) {
      writeMetaColumnNameValue("format");
      writeMetaColumnNameValue("iformat");
      writeMetaColumnNameValue("oformat");
    }
    else if (type == CQBaseModelType::COLOR) {
      writeMetaColumnNameValue("palette");
      writeMetaColumnNameValue("mapped");
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

    auto *columnType = columnDetails->columnType();

    for (const auto &param : columnType->paramNames()) {
      if (writtenNames.indexOf(param) < 0)
        std::cerr << "Parameter '" << param.toStdString() << "' not written\n";
    }
  }

  fs << "#END_META_DATA\n";

  //---

  // write header
  for (int ic = 0; ic < nc; ++ic) {
    CQChartsColumn c(ic);

    bool ok;

    auto header = CQChartsModelUtil::columnToString(model, c, ok);

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
            const auto *timeType = columnDetails->columnTypeT<CQChartsColumnTimeType>();
            assert(timeType);

            auto fmt = timeType->getIFormat(columnDetails->nameValues());

            if (fmt.trimmed() != "") {
              var = CQChartsUtil::timeToString(fmt, r);

              converted = true;
            }
          }
        }
      }

      if (! converted)
        var = modelIndData(model, ind);

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
copy(CopyData &copyData)
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
    // set column name variables
    for (int ic = 0; ic < nc; ++ic) {
      CQChartsColumn c(ic);

      bool ok;

      auto name = CQChartsModelUtil::columnToString(model_.data(), c, ok);

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

      CQChartsExprTcl::ErrorData errorData;

      errorData.showError = copyData.debug;

      if (expr->evaluateExpression(copyData.filter, value, errorData)) {
        bool ok;
        visible = CQChartsVariant::toBool(value, ok);
      }
      else {
        ++copyData.numErrors;

        for (const auto &msg : errorData.messages)
          copyData.rowErrors[r].push_back(msg);
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

    auto var = CQChartsModelUtil::columnToString(model, c, ok);

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
join(CQChartsModelData *rModelData, const Columns &lColumns, const Columns &rColumns,
     JoinType /*joinType*/)
{
  if (lColumns.empty() || rColumns.empty())
    return nullptr;

  //--

  // get model details
  auto *ldetails = this->details();
  auto *rdetails = rModelData->details();

  int lnc = ldetails->numColumns();
  int lnr = ldetails->numRows();

  int rnc = rdetails->numColumns();
  int rnr = rdetails->numRows();

  //---

  // get models
  auto *lModel = this->model().data();
  if (! lModel) return nullptr;

  auto *rModel = rModelData->model().data();
  if (! rModel) return nullptr;

  //---

  // get set of columns to join (by index)
  using ColumnSet = std::set<int>;

  ColumnSet lColumnSet, rColumnSet;

  for (const auto &lColumn : lColumns) {
    int ilColumn = lColumn.column();

    if (ilColumn < 0 || ilColumn >= lnc)
      return nullptr;

    lColumnSet.insert(ilColumn);
  }

  for (const auto &rColumn : rColumns) {
    int irColumn = rColumn.column();

    if (irColumn < 0 || irColumn >= rnc)
      return nullptr;

    rColumnSet.insert(irColumn);
  }

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

  // build map of left columns variant/name to row
  using Rows        = std::vector<int>;
  using Variants    = std::vector<QVariant>;
  using VariantRows = std::map<QVariant, Rows>;

  Variants    lVariants;
  VariantRows lVariantRows, rVariantRows;

  for (int r = 0; r < lnr; ++r) {
    QVariantList vars;

    for (const auto &ilColumn : lColumnSet) {
      auto ind1 = lModel->index(r, ilColumn);
      auto var  = modelIndData(lModel, ind1);

      vars.push_back(var);
    }

    auto var = encodeVariantList(vars);

    auto pv = lVariantRows.find(var);

    if (pv == lVariantRows.end())
      lVariants.push_back(var);

    lVariantRows[var].push_back(r);
  }

  for (int r = 0; r < rnr; ++r) {
    QVariantList vars;

    for (const auto &irColumn : rColumnSet) {
      auto ind1 = rModel->index(r, irColumn);
      auto var  = modelIndData(rModel, ind1);

      vars.push_back(var);
    }

    auto var = encodeVariantList(vars);

    rVariantRows[var].push_back(r);
  }

  //---

  int nc = 0;

  using RowVariants = std::vector<QVariantList>;

  RowVariants rowVariants;

  for (const auto &lvar : lVariants) {
    auto pl = lVariantRows.find(lvar);
    assert(pl != lVariantRows.end());

    const auto &lkey  = (*pl).first;
    const auto &lrows = (*pl).second;

    auto pr = rVariantRows.find(lkey);

    if (pr != rVariantRows.end()) {
      const auto &rrows = (*pr).second;

      for (const auto &lr : lrows) {
        for (const auto &rr : rrows) {
          QVariantList vars;

          for (int lc = 0; lc < lnc; ++lc) {
            auto ind1 = lModel->index(lr, lc);
            auto var  = modelIndData(lModel, ind1);

            vars.push_back(var);
          }

          for (int rc = 0; rc < rnc; ++rc) {
            auto ind1 = rModel->index(rr, rc);
            auto var  = modelIndData(rModel, ind1);

            vars.push_back(var);
          }

          rowVariants.push_back(vars);

          nc = std::max(nc, vars.size());
        }
      }
    }
    else {
      for (const auto &lr : lrows) {
        QVariantList vars;

        for (int lc = 0; lc < lnc; ++lc) {
          auto ind1 = lModel->index(lr, lc);
          auto var  = modelIndData(lModel, ind1);

          vars.push_back(var);
        }

        rowVariants.push_back(vars);

        nc = std::max(nc, vars.size());
      }
    }
  }

  // create new (merged model)
  int nr = int(rowVariants.size());

  auto *dataModel = new CQDataModel(nc, nr);

  int r = 0;

  for (const auto &vars : rowVariants) {
    int c = 0;

    for (const auto &v : vars) {
      auto ind = dataModel->index(r, c);

      dataModel->setData(ind, v);

      ++c;
    }

    ++r;
  }

  //---

  // create model
  auto *filterModel = new CQChartsFilterModel(charts_, dataModel);

  filterModel->setObjectName("joinModel");

  //---

  return filterModel;
}

QAbstractItemModel *
CQChartsModelData::
join(CQChartsModelData *joinModelData, const Columns &joinColumns, JoinType joinType)
{
  if (joinColumns.empty())
    return nullptr;

  //--

  // get model details
  auto *ldetails = this->details();
  auto *rdetails = joinModelData->details();

  int lnc = ldetails->numColumns();
  int lnr = ldetails->numRows();

  int rnc = rdetails->numColumns();
  int rnr = rdetails->numRows();

  //---

  // get models
  auto *lmodel = this->model().data();
  if (! lmodel) return nullptr;

  auto *rmodel = joinModelData->model().data();
  if (! rmodel) return nullptr;

  //---

  // get set of columns to join (by index)
  using ColumnSet = std::set<int>;

  ColumnSet lcolumnSet;

  for (const auto &joinColumn : joinColumns) {
    int ijoinColumn = joinColumn.column();

    if (ijoinColumn < 0 || ijoinColumn >= lnc)
      return nullptr;

    lcolumnSet.insert(ijoinColumn);
  }

  //---

  // get name of join column(s) ...
  bool ok;

  QStringList columnNames;

  for (const auto &joinColumn : joinColumns) {
    auto columnName = CQChartsModelUtil::columnToString(lmodel, joinColumn, ok);

    columnNames.push_back(columnName);
  }

  // ... and find column with matching name in join model
  ColumnSet rcolumnSet;

  for (int ic = 0; ic < rnc; ++ic) {
    CQChartsColumn c(ic);

    bool ok;

    auto joinColumnName = CQChartsModelUtil::columnToString(rmodel, c, ok);

    if (columnNames.contains(joinColumnName))
      rcolumnSet.insert(ic);
  }

  if (rcolumnSet.size() != joinColumns.size())
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

  using Rows          = std::vector<int>;
  using VariantRowMap = std::map<QVariant, Rows>;
  using RowVariantMap = std::map<int, QVariant>;

  // build map of model columns variant/name to row
  VariantRowMap lvariantRowMap;
  RowVariantMap lrowVariantMap;

  for (int r = 0; r < lnr; ++r) {
    QVariantList vars;

    for (const auto &lcolumn : lcolumnSet) {
      auto ind = lmodel->index(r, lcolumn);
      auto var = modelIndData(lmodel, ind);

      vars.push_back(var);
    }

    auto var = encodeVariantList(vars);

    lvariantRowMap[var].push_back(r);

    lrowVariantMap[r] = var;
  }

  // build map of join columns variant/name to row
  VariantRowMap rvariantRowMap;
  RowVariantMap rrowVariantMap;

  for (int r = 0; r < rnr; ++r) {
    QVariantList vars;

    for (const auto &rcolumn : rcolumnSet) {
      auto ind = rmodel->index(r, rcolumn);
      auto var = modelIndData(rmodel, ind);

      vars.push_back(var);
    }

    auto var = encodeVariantList(vars);

    rvariantRowMap[var].push_back(r);

    rrowVariantMap[r] = var;
  }

  std::cerr << "LKeys:";
  for (const auto &pl : lvariantRowMap)
    std::cerr << " \'" << pl.first.toString().toStdString() << "\'";
  std::cerr << "\n";

  std::cerr << "RKeys:";
  for (const auto &pr : rvariantRowMap)
    std::cerr << " \'" << pr.first.toString().toStdString() << "\'";
  std::cerr << "\n";

  //---

  // get join keys
  //  . left : use only keys from left frame; preserve key order.
  //  . right: use only keys from right frame; preserve key order.
  //  . outer: use union of keys from both frames; sort keys lexicographically.
  //  . inner: use intersection of keys from both frames; preserve the order of the left keys.
  //  . cross: creates the cartesian product from both frames, preserves the order of the left keys.

  std::vector<QVariant> joinKeys;

  if      (joinType == JoinType::LEFT) {
    for (const auto &pl : lvariantRowMap) {
      const auto &lvar = pl.first;

      joinKeys.push_back(lvar);
    }
  }
  else if (joinType == JoinType::RIGHT) {
    for (const auto &pr : rvariantRowMap) {
      const auto &rvar = pr.first;

      joinKeys.push_back(rvar);
    }
  }
  else if (joinType == JoinType::OUTER) {
    std::set<QVariant> outerJoinKeys;

    for (const auto &pl : lvariantRowMap) {
      const auto &lvar = pl.first;

      outerJoinKeys.insert(lvar);
    }

    for (const auto &pr : rvariantRowMap) {
      const auto &rvar = pr.first;

      outerJoinKeys.insert(rvar);
    }

    for (const auto &k : outerJoinKeys)
      joinKeys.push_back(k);
  }
  else if (joinType == JoinType::INNER) {
    for (const auto &pl : lvariantRowMap) {
      const auto &lvar = pl.first;

      auto pr = rvariantRowMap.find(lvar);
      if (pr == rvariantRowMap.end()) continue;

      joinKeys.push_back(lvar);
    }
  }

  std::cerr << "JoinKeys:";
  for (const auto &k : joinKeys)
    std::cerr << " \'" << k.toString().toStdString() << "\'";
  std::cerr << "\n";

  //---

  CQDataModel *dataModel = nullptr;

  if (! joinKeys.empty()) {
    using RowVariants = std::vector<QVariantList>;

    RowVariants rowVariants;

    int nc = 0;

    for (const auto &joinKey : joinKeys) {
      QVariantList vars;

      vars.push_back(joinKey);

      const auto &lrows = lvariantRowMap[joinKey];
      const auto &rrows = rvariantRowMap[joinKey];

      auto lrow = lrows.front();
      auto rrow = rrows.front();

      for (int ic = 0; ic < lnc; ++ic) {
        // skip join columns
        if (lcolumnSet.find(ic) != lcolumnSet.end())
          continue;

        auto ind = lmodel->index(lrow, ic);
        auto var = modelIndData(lmodel, ind);

        vars.push_back(var);
      }

      for (int ic = 0; ic < rnc; ++ic) {
        // skip join columns
        if (rcolumnSet.find(ic) != rcolumnSet.end())
          continue;

        auto ind = rmodel->index(rrow, ic);
        auto var = modelIndData(rmodel, ind);

        vars.push_back(var);
      }

      rowVariants.push_back(vars);

      nc = std::max(nc, vars.size());
    }

    // create new (merged model)
    int nr = int(rowVariants.size());

    dataModel = new CQDataModel(nc, nr);

    int r = 0;

    for (const auto &vars : rowVariants) {
      int c = 0;

      for (const auto &v : vars) {
        auto ind = dataModel->index(r, c);

        dataModel->setData(ind, v);

        ++c;
      }

      ++r;
    }
  }
  else {
    // create new (merged model)
    int nlj = int(lcolumnSet.size());
    int nrj = int(rcolumnSet.size());

    dataModel = new CQDataModel(int(lnc - nlj) + (rnc - nrj) + 1, lnr);

    //---

    // copy model data
    for (int r = 0; r < lnr; ++r) {
      int c1 = 0;

      // get join key
      auto joinVar = lrowVariantMap[r];

      //---

      // add join key to joined table
      auto ind1 = dataModel->index(r, c1++);
      dataModel->setData(ind1, joinVar);

      //---

      // add (left) model column values to joined table (skip join columns)
      for (int ic = 0; ic < lnc; ++ic) {
        // skip join columns
        if (lcolumnSet.find(ic) != lcolumnSet.end())
          continue;

        auto ind = lmodel->index(r, ic);
        auto var = modelIndData(lmodel, ind);

        auto ind1 = dataModel->index(r, c1++);
        dataModel->setData(ind1, var);
      }

      //---

      // get matching rows in join model for key
      auto p = rvariantRowMap.find(joinVar);

      if (p == rvariantRowMap.end())
        continue;

      const auto &joinRows = (*p).second;

      int joinRow = joinRows.front();

      //---

      // add join (right) model column values for matching row ((skip join columns)
      for (int ic = 0; ic < rnc; ++ic) {
        // skip join columns
        if (rcolumnSet.find(ic) != rcolumnSet.end())
          continue;

        auto ind = rmodel->index(joinRow, ic);
        auto var = modelIndData(rmodel, ind);

        auto ind1 = dataModel->index(r, c1++);
        dataModel->setData(ind1, var);
      }
    }
  }

  //---

  // copy horizontal header data
  int c1 = 0;

  dataModel->setHeaderData(c1++, Qt::Horizontal, columnNames.join(" "), Qt::DisplayRole);

  auto *lBaseModel = qobject_cast<CQBaseModel *>(CQChartsModelUtil::getBaseModel(lmodel));

  for (int ic = 0; ic < lnc; ++ic) {
    // skip join columns
    if (lcolumnSet.find(ic) != lcolumnSet.end())
      continue;

    if (lBaseModel)
      lBaseModel->copyColumnHeaderRoles(dataModel, ic, c1);
    else {
      auto name = lmodel->headerData(ic, Qt::Horizontal);

      dataModel->setHeaderData(c1, Qt::Horizontal, name, Qt::DisplayRole);
    }

    ++c1;
  }

  auto *rBaseModel = qobject_cast<CQBaseModel *>(CQChartsModelUtil::getBaseModel(rmodel));

  for (int ic = 0; ic < rnc; ++ic) {
    // skip join columns
    if (rcolumnSet.find(ic) != rcolumnSet.end())
      continue;

    if (rBaseModel)
      rBaseModel->copyColumnHeaderRoles(dataModel, ic, c1);
     else {
      auto name = rmodel->headerData(ic, Qt::Horizontal);

      dataModel->setHeaderData(c1, Qt::Horizontal, name, Qt::DisplayRole);
    }

    ++c1;
  }

  //---

  // create model
  auto *filterModel = new CQChartsFilterModel(charts_, dataModel);

  filterModel->setObjectName("joinModel");

  //---

  return filterModel;
}

QAbstractItemModel *
CQChartsModelData::
cross(CQChartsModelData *rModelData)
{
  std::vector<CQChartsModelData *> models;

  models.push_back(this);
  models.push_back(rModelData);

  return cross(models);
}

QAbstractItemModel *
CQChartsModelData::
cross(const std::vector<CQChartsModelData *> &models)
{
  assert(models.size() == 2);

  auto *lModelData = models[0];
  auto *rModelData = models[1];

  // get model details
  auto *ldetails = lModelData->details();
  auto *rdetails = rModelData->details();

  int lnc = ldetails->numColumns();
  int lnr = ldetails->numRows();

  int rnc = rdetails->numColumns();
  int rnr = rdetails->numRows();

  //---

  // get models
  auto *lModel = lModelData->model().data();
  if (! lModel) return nullptr;

  auto *rModel = rModelData->model().data();
  if (! rModel) return nullptr;

  //---

  int nc = 0;

  using RowVariants = std::vector<QVariantList>;

  RowVariants rowVariants;

  for (int lr = 0; lr < lnr; ++lr) {
    QVariantList vars1;

    for (int lc = 0; lc < lnc; ++lc) {
      auto ind1 = lModel->index(lr, lc);
      auto var1 = modelIndData(lModel, ind1);

      vars1.push_back(var1);
    }

    for (int rr = 0; rr < rnr; ++rr) {
      QVariantList vars2 = vars1;

      for (int rc = 0; rc < rnc; ++rc) {
        auto ind2 = rModel->index(rr, rc);
        auto var2 = modelIndData(rModel, ind2);

        vars2.push_back(var2);
      }

      rowVariants.push_back(vars2);

      nc = std::max(nc, vars2.size());
    }
  }

  //---

  // create new (merged model)
  int nr = int(rowVariants.size());

  auto *dataModel = new CQDataModel(nc, nr);

  int r = 0;

  for (const auto &vars : rowVariants) {
    int c = 0;

    for (const auto &v : vars) {
      auto ind = dataModel->index(r, c);

      dataModel->setData(ind, v);

      ++c;
    }

    ++r;
  }

  //---

  // create model
  auto *charts = lModelData->charts();

  auto *filterModel = new CQChartsFilterModel(charts, dataModel);

  filterModel->setObjectName("crossModel");

  //---

  return filterModel;
}

QAbstractItemModel *
CQChartsModelData::
concat(const std::vector<CQChartsModelData *> &modelDatas, const QStringList &keys,
       Qt::Orientation orient)
{
  if (modelDatas.empty())
    return nullptr;

  auto *refModelData = modelDatas[0];

  //---

  bool hasKeys = (keys.length() > 0);

  //---

  int nc = 0, nr = 0;

  if (orient == Qt::Vertical) {
    for (auto *modelData : modelDatas) {
      // get model details
      auto *details = modelData->details();

      int nc1 = details->numColumns();

      nc = std::max(nc, nc1);
    }

    if (hasKeys)
      ++nc;
  }
  else {
    for (auto *modelData : modelDatas) {
      // get model details
      auto *details = modelData->details();

      int nr1 = details->numColumns();

      nr = std::max(nr, nr1);
    }

    if (hasKeys)
      ++nr;
  }

  //---

  using Variants = std::vector<QVariantList>;

  Variants rowVariants, colVariants;

  int nm = 0;

  for (auto *modelData : modelDatas) {
    // get model details
    auto *details = modelData->details();

    int nc1 = details->numColumns();
    int nr1 = details->numRows();

    auto *model = modelData->model().data();

    if (orient == Qt::Vertical) {
      for (int r = 0; r < nr1; ++r) {
        QVariantList vars;

        if (hasKeys)
          vars.push_back(keys.at(nm));

        for (int c = 0; c < nc1; ++c) {
          auto ind1 = model->index(r, c);
          auto var1 = modelIndData(model, ind1);

          vars.push_back(var1);
        }

        rowVariants.push_back(vars);
      }
    }
    else {
      for (int c = 0; c < nc1; ++c) {
        QVariantList vars;

        if (hasKeys)
          vars.push_back(keys.at(nm));

        for (int r = 0; r < nr1; ++r) {
          auto ind1 = model->index(r, c);
          auto var1 = modelIndData(model, ind1);

          vars.push_back(var1);
        }

        colVariants.push_back(vars);
      }
    }

    ++nm;
  }

  //---

  // create new (merged model)
  if (orient == Qt::Vertical)
    nr = int(rowVariants.size());
  else
    nc = int(colVariants.size());

  auto *dataModel = new CQDataModel(nc, nr);

  if (orient == Qt::Vertical) {
    int r = 0;

    for (const auto &vars : rowVariants) {
      int c = 0;

      for (const auto &v : vars) {
        auto ind = dataModel->index(r, c);

        dataModel->setData(ind, v);

        ++c;
      }

      ++r;
    }
  }
  else {
    int c = 0;

    for (const auto &vars : colVariants) {
      int r = 0;

      for (const auto &v : vars) {
        auto ind = dataModel->index(r, c);

        dataModel->setData(ind, v);

        ++r;
      }

      ++c;
    }
  }

  //---

  // create model
  auto *charts = refModelData->charts();

  auto *filterModel = new CQChartsFilterModel(charts, dataModel);

  filterModel->setObjectName("crossModel");

  //---

  // copy horizontal header data
  if (orient == Qt::Vertical) {
    auto *refModel = refModelData->model().data();

    auto *refBaseModel = qobject_cast<CQBaseModel *>(CQChartsModelUtil::getBaseModel(refModel));

    auto *refDetails = refModelData->details();

    int nc1 = refDetails->numColumns();

    for (int c = 0; c < nc1; ++c) {
      if (refBaseModel)
        refBaseModel->copyColumnHeaderRoles(dataModel, c, c);
      else {
        auto name = refModel->headerData(c, Qt::Horizontal);

        dataModel->setHeaderData(c, Qt::Horizontal, name, Qt::DisplayRole);
      }
    }
  }
  else {
    int c1 = 0;

    for (auto *modelData : modelDatas) {
      auto *model = modelData->model().data();

      auto *baseModel = qobject_cast<CQBaseModel *>(CQChartsModelUtil::getBaseModel(model));

      auto *details = modelData->details();

      int nc = details->numColumns();

      for (int c = 0; c < nc; ++c) {
        if (baseModel)
          baseModel->copyColumnHeaderRoles(dataModel, c, c1);
        else {
          auto name = model->headerData(c, Qt::Horizontal);

          dataModel->setHeaderData(c1, Qt::Horizontal, name, Qt::DisplayRole);
        }

        ++c1;
      }
    }
  }

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

    auto columnName = CQChartsModelUtil::columnToString(model, CQChartsColumn(groupColumn), ok);

    for (int r = 0; r < nr; ++r, ++r1) {
      int c1 = 0;

      for (int c = 0; c < nc; ++c) {
        if (groupColumnSet.find(c) != groupColumnSet.end())
          continue;

        auto ind = model->index(r, c);
        auto var = model->data(ind);

        auto ind2 = dataModel->index(r1, c1);
        dataModel->setData(ind2, var);

        ++c1;
      }

      //---

      auto ind3 = dataModel->index(r1, c1); ++c1;

      dataModel->setData(ind3, columnName);

      auto ind4 = model->index(r, groupColumn);
      auto var  = model->data(ind4);

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

bool
CQChartsModelData::
copyHeaderRoles(QAbstractItemModel *toModel) const
{
  auto *model = this->model().data();
  if (! model) return false;

  auto *baseModel = dynamic_cast<CQBaseModel *>(model);
  if (! baseModel) return false;

  baseModel->copyHeaderRoles(toModel);

  return true;
}

bool
CQChartsModelData::
copyColumnHeaderRoles(QAbstractItemModel *toModel, int c1, int c2) const
{
  auto *model = this->model().data();
  if (! model) return false;

  auto *baseModel = dynamic_cast<CQBaseModel *>(model);
  if (! baseModel) return false;

  baseModel->copyColumnHeaderRoles(toModel, c1, c2);

  return true;
}

//------

QVariant
CQChartsModelData::
modelIndData(QAbstractItemModel *model, const QModelIndex &ind)
{
  auto var = model->data(ind, Qt::EditRole);

  if (! var.isValid())
    var = model->data(ind, Qt::DisplayRole);

  return var;
}
