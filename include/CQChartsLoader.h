#ifndef CQChartsLoader_H
#define CQChartsLoader_H

#include <CQChartsFileType.h>
#include <QVariant>
#include <vector>

class CQCharts;
class CQChartsInputData;
class CQChartsCsvFilterModel;
class CQChartsTsvFilterModel;
class CQChartsJsonFilterModel;
class CQChartsGnuDataFilterModel;

#if defined(CQCharts_USE_TCL)
class CQTcl;
#endif

class QAbstractItemModel;

class CQChartsLoader {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQChartsLoader(CQCharts *charts);

#if defined(CQCharts_USE_TCL)
  void setQtcl(CQTcl *qtcl);
#endif

  QAbstractItemModel *loadFile(const QString &filename, CQChartsFileType type,
                               const CQChartsInputData &inputData, bool &hierarchical);

  CQChartsCsvFilterModel*     loadCsv (const QString &filename, const CQChartsInputData &inputData);
  CQChartsTsvFilterModel*     loadTsv (const QString &filename, const CQChartsInputData &inputData);
  CQChartsJsonFilterModel*    loadJson(const QString &filename, const CQChartsInputData &inputData);
  CQChartsGnuDataFilterModel* loadData(const QString &filename, const CQChartsInputData &inputData);

  QAbstractItemModel *createExprModel(int n);

  QAbstractItemModel *createVarsModel(const CQChartsInputData &inputData);

  QAbstractItemModel *createCorrelationModel(QAbstractItemModel *model);

 private:
  CQCharts* charts_ { nullptr };
#if defined(CQCharts_USE_TCL)
  CQTcl*    qtcl_   { nullptr };
#endif
};

#endif
