#ifndef CQChartsLoader_H
#define CQChartsLoader_H

#include <CQChartsFileType.h>
#include <QVariant>
#include <vector>

class CQCharts;
class CQChartsFilterModel;

struct CQChartsInputData;

#if defined(CQCharts_USE_TCL)
class CQTcl;
#endif

class QAbstractItemModel;

class CQChartsLoader {
 public:
  CQChartsLoader(CQCharts *charts);

#if defined(CQCharts_USE_TCL)
  void setQtcl(CQTcl *qtcl);
#endif

  QAbstractItemModel *loadFile(const QString &filename, CQChartsFileType type,
                               const CQChartsInputData &inputData, bool &hierarchical);

  CQChartsFilterModel* loadCsv (const QString &filename, const CQChartsInputData &inputData);
  CQChartsFilterModel* loadTsv (const QString &filename, const CQChartsInputData &inputData);
  CQChartsFilterModel* loadJson(const QString &filename, const CQChartsInputData &inputData);
  CQChartsFilterModel* loadData(const QString &filename, const CQChartsInputData &inputData);

  CQChartsFilterModel *createExprModel(int n);

  CQChartsFilterModel *createVarsModel(const CQChartsInputData &inputData);

  CQChartsFilterModel *createCorrelationModel(QAbstractItemModel *model, bool flip=false);

 private:
  CQCharts* charts_ { nullptr };
#if defined(CQCharts_USE_TCL)
  CQTcl*    qtcl_   { nullptr };
#endif
};

#endif
