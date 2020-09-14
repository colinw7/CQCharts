#ifndef CQChartsLoader_H
#define CQChartsLoader_H

#include <CQChartsFileType.h>
#include <QVariant>
#include <vector>

class CQCharts;
class CQChartsFilterModel;
class CQChartsModelFilter;

struct CQChartsInputData;

class CQTcl;

class QAbstractItemModel;

/*!
 * \brief model loader class
 * \ingroup Charts
 */
class CQChartsLoader {
 public:
  using InputData   = CQChartsInputData;
  using ModelFilter = CQChartsModelFilter;

 public:
  CQChartsLoader(CQCharts *charts);

  void setQtcl(CQTcl *qtcl);

  QAbstractItemModel *loadFile(const QString &filename, CQChartsFileType type,
                               const InputData &inputData, bool &hierarchical);

  CQChartsFilterModel* loadCsv (const QString &filename, const InputData &inputData);
  CQChartsFilterModel* loadTsv (const QString &filename, const InputData &inputData);
  CQChartsFilterModel* loadJson(const QString &filename, const InputData &inputData);
  CQChartsFilterModel* loadData(const QString &filename, const InputData &inputData);

  CQChartsFilterModel *createExprModel(int n);

  CQChartsFilterModel *createVarsModel(const InputData &inputData);

  CQChartsFilterModel *createTclModel(const InputData &inputData);

  CQChartsFilterModel *createCorrelationModel(QAbstractItemModel *model, bool flip=false);

 private:
  void setFilter(ModelFilter *model, const InputData &inputData);

 private:
  CQCharts* charts_ { nullptr };
  CQTcl*    qtcl_   { nullptr };
};

#endif
