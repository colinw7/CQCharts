#include <CQChartsLoader.h>
#include <CQChartsInputData.h>
#include <CQChartsFileType.h>
#include <CQChartsModelExprMatch.h>
#include <CQChartsFilterModel.h>
#include <CQChartsExprModel.h>
#include <CQChartsColumnType.h>
#include <CQCharts.h>

#include <CQCsvModel.h>
#include <CQTsvModel.h>
#include <CQGnuDataModel.h>
#include <CQJsonModel.h>
#include <CQDataModel.h>

#include <CQPerfMonitor.h>
#if defined(CQCharts_USE_TCL)
#include <CQTclUtil.h>
#endif
#include <CMathCorrelation.h>

CQChartsLoader::
CQChartsLoader(CQCharts *charts) :
 charts_(charts)
{
}

#if defined(CQCharts_USE_TCL)
void
CQChartsLoader::
setQtcl(CQTcl *qtcl)
{
  qtcl_ = qtcl;
}
#endif

QAbstractItemModel *
CQChartsLoader::
loadFile(const QString &filename, CQChartsFileType type, const CQChartsInputData &inputData,
         bool &hierarchical)
{
  CQPerfTrace trace("CQChartsLoader::loadFile");

  hierarchical = false;

  if      (type == CQChartsFileType::CSV) {
    CQChartsFilterModel *csv = loadCsv(filename, inputData);

    if (! csv) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return csv;
  }
  else if (type == CQChartsFileType::TSV) {
    CQChartsFilterModel *tsv = loadTsv(filename, inputData);

    if (! tsv) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return tsv;
  }
  else if (type == CQChartsFileType::JSON) {
    CQChartsFilterModel *json = loadJson(filename, inputData);

    if (! json) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    CQJsonModel *jsonModel = qobject_cast<CQJsonModel *>(json->baseModel());
    assert(jsonModel);

    hierarchical = jsonModel->isHierarchical();

    return json;
  }
  else if (type == CQChartsFileType::DATA) {
    CQChartsFilterModel *data = loadData(filename, inputData);

    if (! data) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return data;
  }
  else if (type == CQChartsFileType::EXPR) {
    CQChartsFilterModel *model = createExprModel(inputData.numRows);

    if (! model) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return model;
  }
  else if (type == CQChartsFileType::VARS) {
    CQChartsFilterModel *model = createVarsModel(inputData);

    if (! model) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return model;
  }
  else {
    charts_->errorMsg("Bad file type specified '" + fileTypeToString(type) + "'");
    return nullptr;
  }
}

CQChartsFilterModel *
CQChartsLoader::
loadCsv(const QString &filename, const CQChartsInputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::loadCsv");

  CQCsvModel *csvModel = new CQCsvModel;

  CQChartsFilterModel *csv = new CQChartsFilterModel(charts_, csvModel);

  csvModel->setCommentHeader    (inputData.commentHeader);
  csvModel->setFirstLineHeader  (inputData.firstLineHeader);
  csvModel->setFirstColumnHeader(inputData.firstColumnHeader);

  if (inputData.separator.length())
    csvModel->setSeparator(inputData.separator[0].toLatin1());

  if (! csvModel->load(filename)) {
    delete csv;
    return nullptr;
  }

  if (inputData.filter.length())
    csv->setSimpleFilter(inputData.filter);

  return csv;
}

CQChartsFilterModel *
CQChartsLoader::
loadTsv(const QString &filename, const CQChartsInputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::loadTsv");

  CQTsvModel *tsvModel = new CQTsvModel;

  CQChartsFilterModel *tsv = new CQChartsFilterModel(charts_, tsvModel);

  tsvModel->setCommentHeader    (inputData.commentHeader);
  tsvModel->setFirstLineHeader  (inputData.firstLineHeader);
  tsvModel->setFirstColumnHeader(inputData.firstColumnHeader);

  if (! tsvModel->load(filename)) {
    delete tsv;
    return nullptr;
  }

  if (inputData.filter.length())
    tsv->setSimpleFilter(inputData.filter);

  return tsv;
}

CQChartsFilterModel *
CQChartsLoader::
loadJson(const QString &filename, const CQChartsInputData &)
{
  CQPerfTrace trace("CQChartsLoader::loadJson");

  CQJsonModel *jsonModel = new CQJsonModel;

  CQChartsFilterModel *json = new CQChartsFilterModel(charts_, jsonModel, /*exprModel*/false);

  if (! jsonModel->load(filename)) {
    delete json;
    return nullptr;
  }

  return json;
}

CQChartsFilterModel *
CQChartsLoader::
loadData(const QString &filename, const CQChartsInputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::loadData");

  CQGnuDataModel *dataModel = new CQGnuDataModel;

  CQChartsFilterModel *data = new CQChartsFilterModel(charts_, dataModel);

  dataModel->setCommentHeader    (inputData.commentHeader);
  dataModel->setFirstLineHeader  (inputData.firstLineHeader);
  dataModel->setFirstColumnHeader(inputData.firstColumnHeader);

  if (! dataModel->load(filename)) {
    delete data;
    return nullptr;
  }

  if (inputData.filter.length())
    data->setSimpleFilter(inputData.filter);

  return data;
}

CQChartsFilterModel *
CQChartsLoader::
createExprModel(int n)
{
  CQPerfTrace trace("CQChartsLoader::createExprModel");

  int nc = 1;
  int nr = n;

  CQDataModel *dataModel = new CQDataModel(nc, nr);

  QModelIndex parent;

  for (int r = 0; r < nr; ++r) {
    for (int c = 0; c < nc; ++c) {
      QModelIndex ind = dataModel->index(r, c, parent);

      dataModel->setData(ind, QVariant(r*nc + c));
    }
  }

  CQChartsFilterModel *data = new CQChartsFilterModel(charts_, dataModel);

  return data;
}

CQChartsFilterModel *
CQChartsLoader::
createVarsModel(const CQChartsInputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::createVarsModel");

#if defined(CQCharts_USE_TCL)
  using ColumnValues = std::vector<QVariant>;
  using VarColumns   = std::vector<ColumnValues>;

  VarColumns varColumns;

  int nv = inputData.vars.size();

  int nr = -1;

  if (nv == 1) {
    QString varName = inputData.vars[0].toString();

    ColumnValues columnValues;

    if (qtcl_)
      columnValues = qtcl_->getListVar(varName);

    if (! inputData.transpose) {
      int nc = columnValues.size();

      for (int c = 0; c < nc; ++c) {
        ColumnValues columnValues1;

        if (columnValues[c].type() == QVariant::List) {
          QList<QVariant> rowVars = columnValues[c].toList();

          for (int i = 0; i < rowVars.length(); ++i)
            columnValues1.push_back(rowVars[i]);
        }
        else {
          columnValues1.push_back(columnValues[c]);
        }

        int nv1 = columnValues1.size();

        if (nr < 0)
          nr = nv1;
        else
          nr = std::min(nr, nv1);

        varColumns.push_back(columnValues1);
      }
    }
    else {
      using IndColumnValues = std::map<int,ColumnValues>;

      IndColumnValues indColumnValues;

      nr = columnValues.size();

      for (int r = 0; r < nr; ++r) {
        if (columnValues[r].type() == QVariant::List) {
          QList<QVariant> columnVars = columnValues[r].toList();

          for (int c = 0; c < columnVars.length(); ++c)
            indColumnValues[c].push_back(columnVars[c]);
        }
        else {
          indColumnValues[0].push_back(columnValues[r]);
        }
      }

      nr = 0;

      for (auto &i : indColumnValues) {
        ColumnValues &columnValues = i.second;

        nr = std::max(nr, int(columnValues.size()));
      }

      for (auto &i : indColumnValues) {
        ColumnValues &columnValues = i.second;

        while (int(columnValues.size()) < nr)
          columnValues.push_back("");
      }

      for (auto &i : indColumnValues) {
        ColumnValues &columnValues = i.second;

        varColumns.push_back(columnValues);
      }
    }
  }
  else {
    for (int i = 0; i < nv; ++i) {
      QString varName = inputData.vars[i].toString();

      ColumnValues columnValues;

      if (qtcl_)
        columnValues = qtcl_->getListVar(varName);

      int nv1 = columnValues.size();

      if (nr < 0)
        nr = nv1;
      else
        nr = std::min(nr, nv1);

      varColumns.push_back(columnValues);
    }
  }

  if (nr < 0)
    return nullptr;

  //---

  int ic = 0;
  int ir = 0;

  if (inputData.firstColumnHeader) ++ic;
  if (inputData.firstLineHeader  ) ++ir;

  //---

  int nc = varColumns.size();

  CQDataModel *dataModel = new CQDataModel(nc - ic, nr - ir);

  CQChartsFilterModel *filterModel = new CQChartsFilterModel(charts_, dataModel);

  QModelIndex parent;

  if (inputData.firstColumnHeader) {
    const ColumnValues &columnValues = varColumns[0];

    for (int r = ir; r < nr; ++r)
      dataModel->setHeaderData(r - ir, Qt::Vertical, columnValues[r]);
  }

  if (inputData.firstLineHeader) {
    for (int c = ic; c < nc; ++c) {
      const ColumnValues &columnValues = varColumns[c];

      dataModel->setHeaderData(c - ic, Qt::Horizontal, columnValues[0]);
    }
  }

  for (int c = ic; c < nc; ++c) {
    const ColumnValues &columnValues = varColumns[c];

    for (int r = ir; r < nr; ++r) {
      QModelIndex ind = dataModel->index(r - ir, c - ic, parent);

      dataModel->setData(ind, columnValues[r]);
    }
  }

  return filterModel;
#else
  int nc = inputData.vars.size();
  int nr = 100;

  CQDataModel *dataModel = new CQDataModel(nc, nr);

  for (int r = 0; r < nr; ++r) {
    for (int c = 0; c < nc; ++c) {
      QModelIndex ind = dataModel->index(r, c, parent);

      dataModel->setData(ind, QVariant(r*nc + c));
    }
  }

  CQChartsFilterModel *model = new CQChartsFilterModel(charts_, dataModel);

  return model;
#endif
}

CQChartsFilterModel *
CQChartsLoader::
createCorrelationModel(QAbstractItemModel *model, bool flip)
{
  CQPerfTrace trace("CQChartsLoader::createCorrelationModel");

  int nr = model->rowCount   ();
  int nc = model->columnCount();

  int nv = (! flip ? nc : nr);

  using ColumnValues = std::vector<CMathCorrelation::Values>;
  using ColumnNames  = std::vector<QString>;

  ColumnValues columnValues;
  ColumnNames  columnNames;

  columnValues.resize(nv);
  columnNames .resize(nv);

  if (! flip) {
    for (int c = 0; c < nc; ++c) {
      bool ok;

      columnNames[c] = CQChartsUtil::modelHeaderString(model, c, Qt::Horizontal, ok);

      CMathCorrelation::Values &values = columnValues[c];

      values.resize(nr);

      for (int r = 0; r < nr; ++r) {
        QModelIndex ind = model->index(r, c, QModelIndex());

        bool ok;

        double v = CQChartsUtil::modelReal(model, ind, ok);

        values[r] = v;
      }
    }
  }
  else {
    for (int r = 0; r < nr; ++r) {
      bool ok;

      columnNames[r] = CQChartsUtil::modelHeaderString(model, r, Qt::Vertical, ok);

      CMathCorrelation::Values &values = columnValues[r];

      values.resize(nc);

      for (int c = 0; c < nc; ++c) {
        QModelIndex ind = model->index(r, c, QModelIndex());

        bool ok;

        double v = CQChartsUtil::modelReal(model, ind, ok);

        values[c] = v;
      }
    }
  }

  //---

  CQDataModel *dataModel = new CQDataModel(nv, nv);

  CQChartsFilterModel *filterModel = new CQChartsFilterModel(charts_, dataModel);

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  for (int c = 0; c < nv; ++c)
    (void) columnTypeMgr->setModelColumnType(dataModel, c, CQBaseModel::Type::REAL);

  for (int c = 0; c < nv; ++c) {
    CQChartsUtil::setModelHeaderValue(dataModel, c, Qt::Horizontal,
                                      columnNames[c], Qt::DisplayRole);
    CQChartsUtil::setModelHeaderValue(dataModel, c, Qt::Vertical  ,
                                      columnNames[c], Qt::DisplayRole);

    CQChartsUtil::setModelValue(dataModel, c, c, 1.0);
  }

  for (int c1 = 0; c1 < nv; ++c1) {
    for (int c2 = c1; c2 < nv; ++c2) {
      double corr = CMathCorrelation::calc(columnValues[c1], columnValues[c2]);

      CQChartsUtil::setModelValue(dataModel, c1, c2, corr);
      CQChartsUtil::setModelValue(dataModel, c2, c1, corr);
    }
  }

  return filterModel;
}
