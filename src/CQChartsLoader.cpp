#include <CQChartsLoader.h>
#include <CQChartsInputData.h>
#include <CQChartsFileType.h>
#include <CQChartsModelExprMatch.h>
#include <CQChartsFilterModel.h>
#include <CQChartsExprModel.h>
#include <CQChartsVarsModel.h>
#include <CQChartsTclModel.h>
#include <CQChartsCorrelationModel.h>
#include <CQChartsExprDataModel.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnType.h>
#include <CQCharts.h>

#include <CQCsvModel.h>
#include <CQTsvModel.h>
#include <CQGnuDataModel.h>
#include <CQJsonModel.h>

#include <CQPerfMonitor.h>
#include <CQTclUtil.h>
#include <CMathCorrelation.h>

CQChartsLoader::
CQChartsLoader(CQCharts *charts) :
 charts_(charts)
{
}

void
CQChartsLoader::
setQtcl(CQTcl *qtcl)
{
  qtcl_ = qtcl;
}

QAbstractItemModel *
CQChartsLoader::
loadFile(const QString &filename, CQChartsFileType type, const InputData &inputData,
         bool &hierarchical)
{
  CQPerfTrace trace("CQChartsLoader::loadFile");

  hierarchical = false;

  if      (type == CQChartsFileType::CSV) {
    auto *csv = loadCsv(filename, inputData);

    if (! csv) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return csv;
  }
  else if (type == CQChartsFileType::TSV) {
    auto *tsv = loadTsv(filename, inputData);

    if (! tsv) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return tsv;
  }
  else if (type == CQChartsFileType::JSON) {
    auto *json = loadJson(filename, inputData);

    if (! json) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    auto *jsonModel = qobject_cast<CQJsonModel *>(json->baseModel());
    assert(jsonModel);

    hierarchical = jsonModel->isHierarchical();

    return json;
  }
  else if (type == CQChartsFileType::DATA) {
    auto *data = loadData(filename, inputData);

    if (! data) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return data;
  }
  else if (type == CQChartsFileType::EXPR) {
    auto *model = createExprModel(inputData.numRows);

    if (! model) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return model;
  }
  else if (type == CQChartsFileType::VARS) {
    auto *model = createVarsModel(inputData);

    if (! model) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return model;
  }
  else if (type == CQChartsFileType::TCL) {
    auto *model = createTclModel(inputData);

    if (! model) {
      charts_->errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return model;
  }
  else {
    charts_->errorMsg("Bad file type specified '" +
                      CQChartsFileTypeUtil::fileTypeToString(type) + "'");
    return nullptr;
  }
}

CQChartsFilterModel *
CQChartsLoader::
loadCsv(const QString &filename, const InputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::loadCsv");

  auto *csvModel = new CQCsvModel;

  auto *csv = new CQChartsFilterModel(charts_, csvModel);

  csvModel->setCommentHeader    (inputData.commentHeader);
  csvModel->setFirstLineHeader  (inputData.firstLineHeader);
  csvModel->setFirstColumnHeader(inputData.firstColumnHeader);

  if (inputData.separator.length())
    csvModel->setSeparator(inputData.separator[0].toLatin1());

  if (inputData.maxRows > 0)
    csvModel->setMaxRows(inputData.maxRows);

  if (inputData.columns.length() > 0)
    csvModel->setColumns(inputData.columns);

  if (! csvModel->load(filename)) {
    delete csv;
    return nullptr;
  }

  //---

  setFilter(csv, inputData);

  return csv;
}

CQChartsFilterModel *
CQChartsLoader::
loadTsv(const QString &filename, const InputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::loadTsv");

  auto *tsvModel = new CQTsvModel;

  auto *tsv = new CQChartsFilterModel(charts_, tsvModel);

  tsvModel->setCommentHeader    (inputData.commentHeader);
  tsvModel->setFirstLineHeader  (inputData.firstLineHeader);
  tsvModel->setFirstColumnHeader(inputData.firstColumnHeader);

  if (inputData.columns.length() > 0)
    tsvModel->setColumns(inputData.columns);

  if (! tsvModel->load(filename)) {
    delete tsv;
    return nullptr;
  }

  //---

  setFilter(tsv, inputData);

  return tsv;
}

CQChartsFilterModel *
CQChartsLoader::
loadJson(const QString &filename, const InputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::loadJson");

  auto *jsonModel = new CQJsonModel;

  auto *json = new CQChartsFilterModel(charts_, jsonModel, /*exprModel*/false);

  if (! jsonModel->load(filename)) {
    delete json;
    return nullptr;
  }

  //---

  setFilter(json, inputData);

  return json;
}

CQChartsFilterModel *
CQChartsLoader::
loadData(const QString &filename, const InputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::loadData");

  auto *dataModel = new CQGnuDataModel;

  auto *data = new CQChartsFilterModel(charts_, dataModel);

  dataModel->setCommentHeader    (inputData.commentHeader);
  dataModel->setFirstLineHeader  (inputData.firstLineHeader);
  dataModel->setFirstColumnHeader(inputData.firstColumnHeader);

  if (inputData.columns.length() > 0)
    dataModel->setColumns(inputData.columns);

  if (! dataModel->load(filename)) {
    delete data;
    return nullptr;
  }

  //---

  setFilter(data, inputData);

  return data;
}

CQChartsFilterModel *
CQChartsLoader::
createExprModel(int n)
{
  CQPerfTrace trace("CQChartsLoader::createExprModel");

  auto *dataModel = new CQChartsExprDataModel(n);

  QModelIndex parent;

  for (int r = 0; r < n; ++r) {
    QModelIndex ind = dataModel->index(r, 0, parent);

    dataModel->setData(ind, QVariant(r));
  }

  auto *data = new CQChartsFilterModel(charts_, dataModel);

  //---

  //setFilter(data, inputData);

  return data;
}

CQChartsFilterModel *
CQChartsLoader::
createVarsModel(const InputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::createVarsModel");

  using ColumnValues = std::vector<QVariant>;
  using VarColumns   = std::vector<ColumnValues>;

  VarColumns varColumns;

  //---

  QStringList varNames;

  for (const auto &var : inputData.vars)
    varNames << var.toString();

  //---

  int nr = -1;

  int nv = varNames.length();

  if (nv == 1) {
    QString varName = varNames[0];

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
      using IndColumnValues = std::map<int, ColumnValues>;

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
        auto &columnValues = i.second;

        nr = std::max(nr, int(columnValues.size()));
      }

      for (auto &i : indColumnValues) {
        auto &columnValues = i.second;

        while (int(columnValues.size()) < nr)
          columnValues.push_back("");
      }

      for (auto &i : indColumnValues) {
        auto &columnValues = i.second;

        varColumns.push_back(columnValues);
      }
    }
  }
  else {
    for (int i = 0; i < nv; ++i) {
      QString varName = varNames[i];

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

  int ic = 0, ir = 0;

  if (inputData.firstColumnHeader) ++ic;
  if (inputData.firstLineHeader  ) ++ir;

  //---

  int nc = varColumns.size();

  int nc1 = nc - ic;
  int nr1 = nr - ir;

  if (nc1 < 0 || nr1 < 0)
    return nullptr;

  auto *varsModel = new CQChartsVarsModel(nc1, nr1);

  auto *filterModel = new CQChartsFilterModel(charts_, varsModel);

  QModelIndex parent;

  if (inputData.firstColumnHeader) {
    const auto &columnValues = varColumns[0];

    for (int r = ir; r < nr; ++r)
      varsModel->setHeaderData(r - ir, Qt::Vertical, columnValues[r]);
  }

  if (inputData.firstLineHeader) {
    for (int c = ic; c < nc; ++c) {
      const auto &columnValues = varColumns[c];

      varsModel->setHeaderData(c - ic, Qt::Horizontal, columnValues[0]);
    }
  }

  for (int c = ic; c < nc; ++c) {
    const auto &columnValues = varColumns[c];

    for (int r = ir; r < nr; ++r) {
      QModelIndex ind = varsModel->index(r - ir, c - ic, parent);

      varsModel->setData(ind, columnValues[r]);
    }
  }

  varsModel->setTranspose        (inputData.transpose);
  varsModel->setFirstColumnHeader(inputData.firstColumnHeader);
  varsModel->setFirstLineHeader  (inputData.firstLineHeader);

  varsModel->setVarNames(varNames);

  //---

  //setFilter(filterModel, inputData);

  return filterModel;
}

CQChartsFilterModel *
CQChartsLoader::
createTclModel(const InputData &inputData)
{
  CQPerfTrace trace("CQChartsLoader::createTclModel");

  int nr = 0, nc = 0;

  using ColumnStrs = std::vector<QStringList>;

  ColumnStrs columnStrs;

  if (! inputData.transpose) {
    // var per column
    for (const auto &var : inputData.vars) {
      QStringList strs = var.toStringList();

      nr = std::max(nr, strs.length());
    }

    nc = inputData.vars.size();

    for (const auto &var : inputData.vars) {
      QStringList strs = var.toStringList();

      while (strs.length() < nr)
        strs << "";

      columnStrs.push_back(strs);
    }
  }
  else {
    // var per row
    for (const auto &var : inputData.vars) {
      QStringList strs = var.toStringList();

      nc = std::max(nc, strs.length());
    }

    nr = inputData.vars.size();

    columnStrs.resize(nc);

    for (const auto &var : inputData.vars) {
      QStringList strs = var.toStringList();

      while (strs.length() < nc)
        strs << "";

      for (int i = 0; i < nc; ++i)
        columnStrs[i].push_back(strs[i]);
    }
  }

  //---

  int ic = 0, ir = 0;

  if (inputData.firstColumnHeader) ++ic;
  if (inputData.firstLineHeader  ) ++ir;

  int nc1 = nc - ic;
  int nr1 = nr - ir;

  if (nc1 < 0 || nr1 < 0)
    return nullptr;

  //---

  auto *tclModel = new CQChartsTclModel(nc1, nr1);

  auto *filterModel = new CQChartsFilterModel(charts_, tclModel);

  QModelIndex parent;

  for (int c = ic; c < nc; ++c) {
    const auto &strs = columnStrs[c];

    for (int r = ir; r < nr; ++r) {
      QModelIndex ind = tclModel->index(r - ir, c - ic, parent);

      tclModel->setData(ind, strs[r]);
    }
  }

  //---

  if (inputData.firstColumnHeader) {
    const auto &strs = columnStrs[0];

    for (int r = ir; r < nr; ++r)
      tclModel->setHeaderData(r - ir, Qt::Vertical, strs[r]);
  }

  if (inputData.firstLineHeader) {
    for (int c = ic; c < nc; ++c) {
      const auto &strs = columnStrs[c];

      tclModel->setHeaderData(c - ic, Qt::Horizontal, strs[0]);
    }
  }

  //---

  //setFilter(filterModel, inputData);

  return filterModel;
}

CQChartsFilterModel *
CQChartsLoader::
createCorrelationModel(QAbstractItemModel *model, bool flip)
{
  CQPerfTrace trace("CQChartsLoader::createCorrelationModel");

  int nr = model->rowCount   ();
  int nc = model->columnCount();

  int nv = 0;

  auto *columnTypeMgr = charts_->columnTypeMgr();

  using ColumnValues = std::vector<CMathCorrelation::Values>;
  using ColumnNames  = std::vector<QString>;
  using RMinMax      = CQChartsGeom::RMinMax;
  using ColumnMinMax = std::vector<RMinMax>;

  ColumnValues columnValues;
  ColumnNames  columnNames;
  ColumnMinMax columnMinMax;

  using ColumnSet = std::set<CQChartsColumn>;

  ColumnSet columnSet;

  if (! flip) {
    for (int ic = 0; ic < nc; ++ic) {
      CQChartsColumn c(ic);

      CQChartsModelTypeData typeData;

      if (! columnTypeMgr->getModelColumnType(model, c, typeData))
        typeData.type = CQBaseModelType::STRING;

      if (typeData.type == CQBaseModelType::INTEGER || typeData.type == CQBaseModelType::REAL)
        columnSet.insert(c);
    }

    //---

    int nc1 = columnSet.size();

    nv = nc1;

    columnValues.resize(nv);
    columnNames .resize(nv);
    columnMinMax.resize(nv);

    //---

    int ic = 0;

    for (const auto &c : columnSet) {
      bool ok;

      columnNames[ic] =
        CQChartsModelUtil::modelHeaderString(model, c.column(), Qt::Horizontal, ok);

      auto &values = columnValues[ic];
      auto &minMax = columnMinMax[ic];

      values.resize(nr);

      for (int ir = 0; ir < nr; ++ir) {
        QModelIndex ind = model->index(ir, c.column(), QModelIndex());

        bool ok;

        double v = CQChartsModelUtil::modelReal(model, ind, ok);

        values[ir] = v;

        minMax.add(v);
      }

      ++ic;
    }
  }
  else {
    nv = nr;

    columnValues.resize(nv);
    columnNames .resize(nv);
    columnMinMax.resize(nv);

    //---

    for (int ir = 0; ir < nr; ++ir) {
      bool ok;

      columnNames[ir] = CQChartsModelUtil::modelHeaderString(model, ir, Qt::Vertical, ok);

      auto &values = columnValues[ir];
      auto &minMax = columnMinMax[ir];

      values.resize(nc);

      for (int ic = 0; ic < nc; ++ic) {
        QModelIndex ind = model->index(ir, ic, QModelIndex());

        bool ok;

        double v = CQChartsModelUtil::modelReal(model, ind, ok);

        values[ic] = v;

        minMax.add(v);
      }
    }
  }

  //---

  // calc off diagonal values
  using ColumnReal          = std::map<int, double>;
  using ColumnColumnReal    = std::map<int, ColumnReal>;
  using ColumnPoints        = std::map<int, CQChartsCorrelationModel::Points>;
  using ColumnColumnPoints  = std::map<int, ColumnPoints>;
  using DevData             = std::pair<double, double>;
  using ColumnDevData       = std::map<int, DevData>;
  using ColumnColumnDevData = std::map<int, ColumnDevData>;
  using ColumnDensity       = std::map<int, CQChartsDensity *>;
  using Point               = CQChartsGeom::Point;

  ColumnReal          columnSumSq;
  ColumnColumnReal    columnsCorr;
  ColumnColumnPoints  columnsPoints;
  ColumnColumnDevData columnsDevData;
  ColumnDensity       columnDensity;

  for (int ic1 = 0; ic1 < nv; ++ic1) {
    CQChartsColumn c1(ic1);

    auto &values1 = columnValues[ic1];

    for (int ic2 = ic1; ic2 < nv; ++ic2) {
      if (ic1 != ic2) {
        CQChartsColumn c2(ic2);

        auto &values2 = columnValues[ic2];

        double corr = CMathCorrelation::calc(values1, values2);

        columnsCorr[ic1][ic2] = corr;
        columnsCorr[ic2][ic1] = corr;

        columnSumSq[ic1] += corr*corr;
        columnSumSq[ic2] += corr*corr;

        CQChartsCorrelationModel::Points points1, points2;

        assert(values1.size() == values2.size());

        int nv1 = values1.size();

        for (int j = 0; j < nv1; ++j) {
          points1.push_back(Point(values1[j], values2[j]));
          points2.push_back(Point(values2[j], values1[j]));
        }

        columnsPoints[ic1][ic2] = points1;
        columnsPoints[ic2][ic1] = points2;

        double stddev1 = CMathCorrelation::stddev(values1);
        double stddev2 = CMathCorrelation::stddev(values2);

        columnsDevData[ic1][ic2] = DevData(stddev1, stddev2);
        columnsDevData[ic2][ic1] = DevData(stddev2, stddev1);
      }
      else {
        CQChartsCorrelationModel::Points points1;

        int nv1 = values1.size();

        CQChartsDensity::XVals xvals;

        for (int j = 0; j < nv1; ++j)
          xvals.push_back(values1[j]);

        if (! columnDensity[ic1])
          columnDensity[ic1] = new CQChartsDensity;

        columnDensity[ic1]->setXVals(xvals);
      }
    }
  }

  //---

  // sort by sum of squares
  using ColumnNums = std::vector<int>;
  using SumColumns = std::map<double, ColumnNums>;

  SumColumns sumColumns;

  for (const auto &pc : columnSumSq) {
    sumColumns[pc.second].push_back(pc.first);
  }

  ColumnNums sortedColumns;

  for (const auto &ps : sumColumns)
    for (const auto &c : ps.second)
      sortedColumns.push_back(c);

  assert(int(sortedColumns.size()) == nv);

  //---

  // create model
  auto *correlationModel = new CQChartsCorrelationModel(nv);

  auto *filterModel = new CQChartsFilterModel(charts_, correlationModel);

  for (int ic = 0; ic < nv; ++ic) {
    CQChartsColumn c(ic);

    (void) columnTypeMgr->setModelColumnType(correlationModel, c, CQBaseModelType::REAL);
  }

  //---

  // set header values and diagonal values
  for (int ic = 0; ic < nv; ++ic) {
    int ics = sortedColumns[ic];

    CQChartsColumn c(ic);

    QString columnName = columnNames[ics];

    CQChartsModelUtil::setModelHeaderValue(correlationModel, ic, Qt::Horizontal,
                                           columnName, Qt::DisplayRole);
    CQChartsModelUtil::setModelHeaderValue(correlationModel, ic, Qt::Vertical  ,
                                           columnName, Qt::DisplayRole);

    CQChartsModelUtil::setModelValue(correlationModel, ic, c, 1.0);
  }

  //---

  // set off diagonal values
  for (int ic1 = 0; ic1 < nv; ++ic1) {
    int ic1s = sortedColumns[ic1];

    CQChartsColumn c1(ic1);

    auto &minMax = columnMinMax[ic1s];

    correlationModel->setMinMax(ic1, minMax);

    for (int ic2 = ic1; ic2 < nv; ++ic2) {
      if (ic1 != ic2) {
        int ic2s = sortedColumns[ic2];

        CQChartsColumn c2(ic2);

        double corr = columnsCorr[ic1s][ic2s];

        CQChartsModelUtil::setModelValue(correlationModel, ic1, c2, corr);
        CQChartsModelUtil::setModelValue(correlationModel, ic2, c1, corr);

        correlationModel->setPoints(ic1, ic2, columnsPoints[ic1s][ic2s]);
        correlationModel->setPoints(ic2, ic1, columnsPoints[ic2s][ic1s]);

        correlationModel->setDevData(ic1, ic2,
          columnsDevData[ic1s][ic2s].first, columnsDevData[ic1s][ic2s].second);
        correlationModel->setDevData(ic2, ic1,
          columnsDevData[ic2s][ic1s].first, columnsDevData[ic2s][ic1s].second);
      }
      else {
        CQChartsModelUtil::setModelValue(correlationModel, ic1, c1, 1.0);
        CQChartsModelUtil::setModelValue(correlationModel, ic1, c1, 1.0); // Dup OK

        correlationModel->setDensity(ic1, columnDensity[ic1s]);

        correlationModel->setDevData(ic1, ic1, 0.0, 0.0);
      }
    }
  }

  //---

  //setFilter(filterModel, inputData);

  return filterModel;
}

void
CQChartsLoader::
setFilter(ModelFilter *model, const InputData &inputData)
{
  if (! inputData.filter.length())
    return;

  if      (inputData.filterType == CQChartsModelFilterData::Type::EXPRESSION)
    model->setExpressionFilter(inputData.filter);
  else if (inputData.filterType == CQChartsModelFilterData::Type::REGEXP)
    model->setRegExpFilter(inputData.filter);
  else if (inputData.filterType == CQChartsModelFilterData::Type::WILDCARD)
    model->setWildcardFilter(inputData.filter);
  else if (inputData.filterType == CQChartsModelFilterData::Type::SIMPLE)
    model->setSimpleFilter(inputData.filter);
}
