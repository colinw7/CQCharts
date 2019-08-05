#ifndef CQChartsLoader_H
#define CQChartsLoader_H

#include <CQChartsFileType.h>
#include <QVariant>
#include <vector>

class CQCharts;
class CQChartsFilterModel;

struct CQChartsInputData;

class CQTcl;

class QAbstractItemModel;

/*!
 * \brief model loader class
 * \ingroup Charts
 */
class CQChartsLoader {
 public:
  CQChartsLoader(CQCharts *charts);

  void setQtcl(CQTcl *qtcl);

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
  CQTcl*    qtcl_   { nullptr };
};

#endif
