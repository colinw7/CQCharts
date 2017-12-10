#ifndef CQChartsJson_H
#define CQChartsJson_H

#include <CQChartsColumn.h>
#include <CQChartsModelFilter.h>

class CQCharts;
class CQChartsColumnType;
class CQJsonModel;

class CQChartsJson : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsJson(CQCharts *charts);
 ~CQChartsJson();

  CQJsonModel *jsonModel() const { return jsonModel_; }

  bool load(const QString &filename);

  bool isHierarchical() const;

  //---

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  //---

  QString columnType(int col) const;
  bool setColumnType(int col, const QString &type);

 private:
  using Columns = std::vector<CQChartsColumn>;

  CQCharts*    charts_   { nullptr };
  CQJsonModel* jsonModel_ { nullptr };
  Columns      columns_;
};

#endif
