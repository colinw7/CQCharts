#ifndef CQChartsLoader_H
#define CQChartsLoader_H

#include <CQChartsFileType.h>
#include <QVariant>
#include <vector>

class CQCharts;
class CQChartsInputData;
class CQChartsCsv;
class CQChartsTsv;
class CQChartsJson;
class CQChartsGnuData;
class CQTcl;

class QAbstractItemModel;

class CQChartsLoader {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQChartsLoader(CQCharts *charts);

  void setQtcl(CQTcl *qtcl);

  QAbstractItemModel *loadFile(const QString &filename, CQChartsFileType type,
                               const CQChartsInputData &inputData, bool &hierarchical);

  CQChartsCsv *loadCsv(const QString &filename, bool commentHeader=false,
                       bool firstLineHeader=false, bool firstColumnHeader=false,
                       const QString &filter="");

  CQChartsTsv *loadTsv(const QString &filename, bool commentHeader=false,
                       bool firstLineHeader=false, bool firstColumnHeader=false,
                       const QString &filter="");

  CQChartsJson *loadJson(const QString &filename);

  CQChartsGnuData *loadData(const QString &filename, bool commentHeader=false,
                            bool firstLineHeader=false );

  QAbstractItemModel *createExprModel(int n);

  QAbstractItemModel *createVarsModel(const Vars &vars);

  void errorMsg(const QString &msg) const;

 private:
  CQCharts* charts_ { nullptr };
  CQTcl*    qtcl_   { nullptr };
};

#endif
