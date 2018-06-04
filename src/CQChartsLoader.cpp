#include <CQChartsLoader.h>
#include <CQChartsInputData.h>
#include <CQChartsFileType.h>
#include <CQChartsCsv.h>
#include <CQChartsTsv.h>
#include <CQChartsJson.h>
#include <CQChartsGnuData.h>
#include <CQChartsDataModel.h>
#include <CQChartsExprDataModel.h>
#include <CQChartsModelExprMatch.h>
#include <CQExprModel.h>
#include <CQDataModel.h>
#ifdef CQ_CHARTS_CEIL
#include <CQChartsCeilUtil.h>
#endif
#include <CQCharts.h>
#include <CQTclUtil.h>

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
loadFile(const QString &filename, CQChartsFileType type, const CQChartsInputData &inputData,
         bool &hierarchical)
{
  hierarchical = false;

  QAbstractItemModel *model = nullptr;

  if      (type == CQChartsFileType::CSV) {
    CQChartsCsv *csv = loadCsv(filename,
                               inputData.commentHeader,
                               inputData.firstLineHeader,
                               inputData.firstColumnHeader,
                               inputData.filter);

    if (! csv) {
      errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    if      (charts_->parserType() == CQCharts::ParserType::CEIL) {
      csv->exprModel()->setExprType(CQExprModel           ::ExprType::EXPR);
      csv->exprMatch()->setExprType(CQChartsModelExprMatch::ExprType::EXPR);
    }
    else if (charts_->parserType() == CQCharts::ParserType::TCL) {
      csv->exprModel()->setExprType(CQExprModel           ::ExprType::TCL);
      csv->exprMatch()->setExprType(CQChartsModelExprMatch::ExprType::TCL);
    }

    return csv;
  }
  else if (type == CQChartsFileType::TSV) {
    CQChartsTsv *tsv = loadTsv(filename,
                               inputData.commentHeader,
                               inputData.firstLineHeader,
                               inputData.firstColumnHeader,
                               inputData.filter);

    if (! tsv) {
      errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    if      (charts_->parserType() == CQCharts::ParserType::CEIL) {
      tsv->exprModel()->setExprType(CQExprModel           ::ExprType::EXPR);
      tsv->exprMatch()->setExprType(CQChartsModelExprMatch::ExprType::EXPR);
    }
    else if (charts_->parserType() == CQCharts::ParserType::TCL) {
      tsv->exprModel()->setExprType(CQExprModel           ::ExprType::TCL);
      tsv->exprMatch()->setExprType(CQChartsModelExprMatch::ExprType::TCL);
    }

    return tsv;
  }
  else if (type == CQChartsFileType::JSON) {
    CQChartsJson *json = loadJson(filename);

    if (! json) {
      errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    hierarchical = json->isHierarchical();

    return json;
  }
  else if (type == CQChartsFileType::DATA) {
    CQChartsGnuData *data = loadData(filename,
                                     inputData.commentHeader,
                                     inputData.firstLineHeader);

    if (! data) {
      errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }

    return data;
  }
  else if (type == CQChartsFileType::EXPR) {
    model = createExprModel(inputData.numRows);

    if (! model) {
      errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }
  }
  else if (type == CQChartsFileType::VARS) {
    model = createVarsModel(inputData.vars);

    if (! model) {
      errorMsg("Failed to load '" + filename + "'");
      return nullptr;
    }
  }
  else {
    errorMsg("Bad file type specified '" + fileTypeToString(type) + "'");
    return nullptr;
  }

  return model;
}

CQChartsCsv *
CQChartsLoader::
loadCsv(const QString &filename, bool commentHeader, bool firstLineHeader,
        bool firstColumnHeader, const QString &filter)
{
  CQChartsCsv *csv = new CQChartsCsv(charts_);

  csv->setCommentHeader    (commentHeader);
  csv->setFirstLineHeader  (firstLineHeader);
  csv->setFirstColumnHeader(firstColumnHeader);

  if (! csv->load(filename)) {
    delete csv;
    return nullptr;
  }

  if (filter.length())
    csv->setSimpleFilter(filter);

  return csv;
}

CQChartsTsv *
CQChartsLoader::
loadTsv(const QString &filename, bool commentHeader, bool firstLineHeader,
        bool firstColumnHeader, const QString &filter)
{
  CQChartsTsv *tsv = new CQChartsTsv(charts_);

  tsv->setCommentHeader    (commentHeader);
  tsv->setFirstLineHeader  (firstLineHeader);
  tsv->setFirstColumnHeader(firstColumnHeader);

  if (! tsv->load(filename)) {
    delete tsv;
    return nullptr;
  }

  if (filter.length())
    tsv->setSimpleFilter(filter);

  return tsv;
}

CQChartsJson *
CQChartsLoader::
loadJson(const QString &filename)
{
  CQChartsJson *json = new CQChartsJson(charts_);

  if (! json->load(filename)) {
    delete json;
    return nullptr;
  }

  return json;
}

CQChartsGnuData *
CQChartsLoader::
loadData(const QString &filename, bool commentHeader, bool firstLineHeader)
{
  CQChartsGnuData *data = new CQChartsGnuData(charts_);

  data->setCommentHeader  (commentHeader);
  data->setFirstLineHeader(firstLineHeader);

  if (! data->load(filename)) {
    delete data;
    return nullptr;
  }

  return data;
}

QAbstractItemModel *
CQChartsLoader::
createExprModel(int n)
{
  int nc = 1;
  int nr = n;

  CQChartsExprDataModel *data = new CQChartsExprDataModel(charts_, nc, nr);

  return data;
}

QAbstractItemModel *
CQChartsLoader::
createVarsModel(const Vars &vars)
{
#if defined(CQ_CHARTS_CEIL) || defined(CQ_CHARTS_TCL)
  using ColumnValues = std::vector<QVariant>;
  using VarColumns   = std::vector<ColumnValues>;

  VarColumns varColumns;

  int nv = vars.size();

  int nr = -1;

  for (int i = 0; i < nv; ++i) {
    QString varName = vars[i].toString();

    ColumnValues columnValues;

    if      (charts_->parserType() == CQCharts::ParserType::CEIL) {
#ifdef CQ_CHARTS_CEIL
      columnValues = CQChartsCeilUtil::varArrayValue(varName);
#else
      continue;
#endif
    }
    else if (charts_->parserType() == CQCharts::ParserType::TCL) {
#ifdef CQ_CHARTS_TCL
      if (qtcl_)
        columnValues = qtcl_->getListVar(varName);
#else
      continue;
#endif
    }

    int nv1 = columnValues.size();

    if (nr < 0)
      nr = nv1;
    else
      nr = std::min(nr, nv1);

    varColumns.push_back(columnValues);
  }

  if (nr < 0)
    return nullptr;

  int nc = varColumns.size();

  CQChartsDataModel *model = new CQChartsDataModel(charts_, nc, nr);

  CQDataModel *dataModel = model->dataModel();

  QModelIndex parent;

  for (int c = 0; c < nc; ++c) {
    const ColumnValues &columnValues = varColumns[c];

    for (int r = 0; r < nr; ++r) {
      QModelIndex ind = dataModel->index(r, c, parent);

      dataModel->setData(ind, columnValues[r]);
    }
  }

  return model;
#else
  int nc = vars.size();
  int nr = 100;

  CQChartsExprDataModel *model = new CQChartsExprDataModel(charts_, nc, nr);

  return model;
#endif
}

void
CQChartsLoader::
errorMsg(const QString &msg) const
{
  std::cerr << msg.toStdString() << "\n";
}
