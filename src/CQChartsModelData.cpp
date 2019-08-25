#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelUtil.h>
#include <CQChartsFilterModel.h>
#include <CQChartsVarsModel.h>
#include <CQChartsExprDataModel.h>
#include <CQCharts.h>
#include <CQChartsHtml.h>

#include <CQSummaryModel.h>
#include <CQCsvModel.h>
#include <CQTsvModel.h>
#include <CQJsonModel.h>
#include <CQGnuDataModel.h>
#include <CQTclUtil.h>

#ifdef CQCHARTS_FOLDED_MODEL
#include <CQFoldedModel.h>
#endif

#include <QSortFilterProxyModel>
#include <QItemSelectionModel>

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
   p("csv format also supports specially formated comments to support meta data "
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
  connectModel();
}

CQChartsModelData::
~CQChartsModelData()
{
  delete details_;
  delete summaryModelData_;
}

QString
CQChartsModelData::
id() const
{
  if (name().length())
    return name();

  return QString("%1").arg(ind());
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
connectModel()
{
  if (! model_.data())
    return;

  connect(model_.data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
          this, SLOT(modelDataChangedSlot(const QModelIndex &, const QModelIndex &)));
  connect(model_.data(), SIGNAL(layoutChanged()),
          this, SLOT(modelLayoutChangedSlot()));
  connect(model_.data(), SIGNAL(modelReset()),
          this, SLOT(modelResetSlot()));

  connect(model_.data(), SIGNAL(rowsInserted(QModelIndex,int,int)),
          this, SLOT(modelRowsInsertedSlot()));
  connect(model_.data(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
          this, SLOT(modelRowsRemovedSlot()));
  connect(model_.data(), SIGNAL(columnsInserted(QModelIndex,int,int)),
          this, SLOT(modelColumnsInsertedSlot()));
  connect(model_.data(), SIGNAL(columnsRemoved(QModelIndex,int,int)),
          this, SLOT(modelColumnsRemovedSlot()));
}

void
CQChartsModelData::
disconnectModel()
{
  if (! model_.data())
    return;

  disconnect(model_.data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
             this, SLOT(modelDataChangedSlot(const QModelIndex &, const QModelIndex &)));
  disconnect(model_.data(), SIGNAL(layoutChanged()),
             this, SLOT(modelLayoutChangedSlot()));
  disconnect(model_.data(), SIGNAL(modelReset()),
             this, SLOT(modelResetSlot()));

  disconnect(model_.data(), SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SLOT(modelRowsInsertedSlot()));
  disconnect(model_.data(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
             this, SLOT(modelRowsRemovedSlot()));
  disconnect(model_.data(), SIGNAL(columnsInserted(QModelIndex,int,int)),
             this, SLOT(modelColumnsInsertedSlot()));
  disconnect(model_.data(), SIGNAL(columnsRemoved(QModelIndex,int,int)),
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

  if (details_)
    details_->reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelLayoutChangedSlot()
{
  if (details_)
    details_->reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelResetSlot()
{
  if (details_)
    details_->reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelRowsInsertedSlot()
{
  if (details_)
    details_->reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelRowsRemovedSlot()
{
  if (details_)
    details_->reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelColumnsInsertedSlot()
{
  if (details_)
    details_->reset();

  emit modelChanged();
}

void
CQChartsModelData::
modelColumnsRemovedSlot()
{
  if (details_)
    details_->reset();

  emit modelChanged();
}

CQChartsModelData::ModelP
CQChartsModelData::
currentModel() const
{
#ifdef CQCHARTS_FOLDED_MODEL
  if (! foldedModels_.empty())
    return foldProxyModel_;
#endif

  return model_;
}

//---

void
CQChartsModelData::
addSelectionModel(QItemSelectionModel *model)
{
  connect(model, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this, SLOT(selectionSlot()));

  selectionModels_.push_back(SelectionModelP(model));
}

void
CQChartsModelData::
removeSelectionModel(QItemSelectionModel *model)
{
  int i   = 0;
  int len = selectionModels_.size();

  for ( ; i < len; ++i) {
    if (selectionModels_[i] == model)
      break;
  }

  if (i >= len)
    return;

  disconnect(model, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
             this, SLOT(selectionSlot()));

  ++i;

  for ( ; i < len; ++i)
    selectionModels_[i - i] = selectionModels_[i];

  selectionModels_.pop_back();
}

void
CQChartsModelData::
select(const QItemSelection &sel)
{
  for (auto &sm : selectionModels_) {
    disconnect(sm, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
               this, SLOT(selectionSlot()));

    sm->select(sel, QItemSelectionModel::ClearAndSelect);

    connect(sm, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(selectionSlot()));
  }
}

void
CQChartsModelData::
selectionSlot()
{
  QItemSelectionModel *sm = qobject_cast<QItemSelectionModel *>(sender());
  assert(sm);

  emit selectionChanged(sm);
}

//---

#ifdef CQCHARTS_FOLDED_MODEL
void
CQChartsModelData::
foldModel(const FoldData &foldData)
{
  foldClear(/*notify*/false);

  //---

  CQChartsModelDetails *details = this->details();

  using FoldDatas = std::vector<CQFoldData>;

  FoldDatas foldDatas;

  QStringList columnStrs = foldData.columnsStr.split(",", QString::SkipEmptyParts);

  for (int i = 0; i < columnStrs.length(); ++i) {
    QStringList columnSubStrs = columnStrs[i].split(":", QString::SkipEmptyParts);

    if (columnSubStrs.length() == 0)
      continue;

    //---

    // get column
    QString columnStr = columnSubStrs[0];

    CQChartsColumn column;

    if (! CQChartsModelUtil::stringToColumn(model_.data(), columnStr, column)) {
      bool ok;

      int icolumn = CQChartsUtil::toInt(columnStr, ok);

      if (! ok)
        continue;

      column = CQChartsColumn(icolumn);
    }

    //---

    // get type, count and delta
    CQFoldData::Type type = CQFoldData::Type::STRING;

    int    count    = foldData.count;
    double delta    = foldData.delta;
    bool   integral = false;

    CQFoldData columnFoldData(column.column());

    if (columnSubStrs.length() > 1) {
      int i = 1;

      // <column>:<type>:<delta>
      if (columnSubStrs.length() > 2) {
        // get type
        QString typeStr = columnSubStrs[1];

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

        QString deltaStr = columnSubStrs[2];

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

        QString deltaStr = columnSubStrs[1];

        delta = CQChartsUtil::toReal(columnSubStrs[i], ok);

        if (! ok)
          continue;
      }
    }
    else {
      // get type from column
      const CQChartsModelColumnDetails *columnDetails = details->columnDetails(column);

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

    foldDatas.push_back(columnFoldData);
  }

  //---

  ModelP modelp = this->model();

  // create folded models
  for (const auto &foldData : foldDatas) {
    QAbstractItemModel *model = modelp.data();

    CQFoldedModel *foldedModel = new CQFoldedModel(model, foldData);

    modelp = ModelP(foldedModel);

    foldedModels_.push_back(modelp);
  }

  //---

  if (! foldedModels_.empty()) {
    // add sort/filter proxy if needed
    QAbstractItemModel *model = modelp.data();

    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model);

    if (! proxyModel) {
      QSortFilterProxyModel *foldProxyModel = new QSortFilterProxyModel;

      foldProxyModel->setObjectName("foldProxyModel");

      foldProxyModel->setSortRole(static_cast<int>(Qt::EditRole));

      foldProxyModel->setSourceModel(model);

      modelp = ModelP(foldProxyModel);
    }

    foldProxyModel_ = modelp;
  }

  //---

  delete details_;

  details_ = nullptr;

  //---

  emit currentModelChanged();
}

void
CQChartsModelData::
foldClear(bool notify)
{
  foldedModels_.clear();

  foldProxyModel_ = ModelP();

  if (notify) {
    delete details_;

    details_ = nullptr;

    //---

    emit currentModelChanged();
  }
}

CQChartsModelData::FoldedModels
CQChartsModelData::
foldedModels() const
{
  if (! foldProxyModel_)
    return FoldedModels();

  QSortFilterProxyModel *proxyModel =
    qobject_cast<QSortFilterProxyModel *>(foldProxyModel_.data());
  if (! proxyModel) return FoldedModels();

  CQFoldedModel *foldedModel = qobject_cast<CQFoldedModel *>(proxyModel);

  if (! foldedModel) {
    QAbstractItemModel *sourceModel = proxyModel->sourceModel();

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

CQSummaryModel *
CQChartsModelData::
addSummaryModel()
{
  if (! summaryModel_) {
    summaryModel_ = new CQSummaryModel(model_.data());

    summaryModelP_ = ModelP(summaryModel_);

    summaryModelData_ = new CQChartsModelData(charts_, summaryModelP_);
  }

  return summaryModel_;
}

void
CQChartsModelData::
write(std::ostream &os, const QString &varName) const
{
  QAbstractItemModel *model = model_.data();
  if (! model) return;

  if (varName != "")
    os << "set " << varName.toStdString();
  else
    os << "set model";

  os << " [load_charts_model";

  const CQChartsExprModel  *exprModel = CQChartsModelUtil::getExprModel(model);
  const CQDataModel        *dataModel = CQChartsModelUtil::getDataModel(model);
  const QAbstractItemModel *baseModel = CQChartsModelUtil::getBaseModel(model);

  const CQChartsVarsModel *varsModel = dynamic_cast<const CQChartsVarsModel *>(dataModel);

  const CQChartsExprDataModel *exprDataModel =
    dynamic_cast<const CQChartsExprDataModel *>(dataModel);

  const CQCsvModel     *csvModel  = dynamic_cast<const CQCsvModel     *>(dataModel);
  const CQTsvModel     *tsvModel  = dynamic_cast<const CQTsvModel     *>(dataModel);
  const CQGnuDataModel *gnuModel  = dynamic_cast<const CQGnuDataModel *>(dataModel);
  const CQJsonModel    *jsonModel = dynamic_cast<const CQJsonModel    *>(baseModel);

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
    QStringList varNames = varsModel->varNames();

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
    QString name = model->objectName();

    os << " -model {" << name.toStdString() << "}";
  }

  if (name() != "") {
    os << " -name {" << name().toStdString() << "}";
  }

  //---

  const CQChartsModelDetails *details = this->details();

  int nc = details->numColumns();

  int numExtra = 0;

  if (exprModel)
    numExtra = exprModel->numExtraColumns();

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  for (int i = 0; i < nc - numExtra; ++i) {
    CQChartsColumn column(i);

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    if (! columnTypeMgr->getModelColumnType(model, column, columnType, columnBaseType, nameValues))
      continue;

    if (columnType != CQBaseModelType::STRING || ! nameValues.nameValues().empty()) {
      const CQChartsColumnType *columnTypePtr = columnTypeMgr->getType(columnType);

      os << " -column_type {{";

      os << "{" << i << " " << columnTypePtr->name().toStdString() << "}";

      for (const auto &nv : nameValues.nameValues()) {
        const QString  &name = nv.first;
        const QVariant &var  = nv.second;

        if (! var.isValid())
          continue;

        const CQChartsColumnTypeParam *param = columnTypePtr->getParam(name);

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

      CQBaseModelType    columnType;
      CQBaseModelType    columnBaseType;
      CQChartsNameValues nameValues;

      if (! columnTypeMgr->getModelColumnType(model, column, columnType, columnBaseType,
                                              nameValues))
        continue;

      if (columnType != CQBaseModelType::STRING || ! nameValues.nameValues().empty()) {
        const CQChartsColumnType *columnTypePtr = columnTypeMgr->getType(columnType);

        os << " -type {";

        os << "{" << columnTypePtr->name().toStdString() << "}";

        for (const auto &nv : nameValues.nameValues()) {
          const QString  &name = nv.first;
          const QVariant &var  = nv.second;

          if (! var.isValid())
            continue;

          const CQChartsColumnTypeParam *param = columnTypePtr->getParam(name);

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
}

QAbstractItemModel *
CQChartsModelData::
copy()
{
  QAbstractItemModel *model = model_.data();

  CQChartsModelDetails *details = this->details();

  int nc = details->numColumns();
  int nr = details->numRows();

  std::vector<int> hroles = {{
    static_cast<int>(Qt::DisplayRole),
    static_cast<int>(CQBaseModelRole::Type),
    static_cast<int>(CQBaseModelRole::BaseType),
    static_cast<int>(CQBaseModelRole::TypeValues),
    static_cast<int>(CQBaseModelRole::Min),
    static_cast<int>(CQBaseModelRole::Max),
    static_cast<int>(CQBaseModelRole::Key),
    static_cast<int>(CQBaseModelRole::Sorted),
    static_cast<int>(CQBaseModelRole::SortOrder)
  }};

  std::vector<int> vroles;
  vroles.push_back(static_cast<int>(Qt::DisplayRole)); // clang complains {{}} with single value

  CQDataModel *dataModel = new CQDataModel(nc, nr);

  // copy horizontal header data
  for (int c = 0; c < nc; ++c) {
    for (const auto &role : hroles) {
      QVariant var = model->headerData(c, Qt::Horizontal, role);

      if (var.isValid())
        dataModel->setHeaderData(c, Qt::Horizontal, var, role);
    }
  }

  // copy vertical header data
  for (int r = 0; r < nr; ++r) {
    for (const auto &role : vroles) {
      QVariant var = model->headerData(r, Qt::Vertical, role);

      if (var.isValid())
        dataModel->setHeaderData(r, Qt::Vertical, var, role);
    }
  }

  // copy model data
  for (int r = 0; r < nr; ++r) {
    for (int c = 0; c < nc; ++c) {
      QModelIndex ind1 = model    ->index(r, c);
      QModelIndex ind2 = dataModel->index(r, c);

      QVariant var = model->data(ind1);

      if (var.isValid())
        dataModel->setData(ind2, var);
    }
  }

  // create model
  CQChartsFilterModel *filterModel = new CQChartsFilterModel(charts_, dataModel);

  return filterModel;
}
