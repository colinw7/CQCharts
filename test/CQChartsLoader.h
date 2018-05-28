#ifndef CQChartsLoader_H
#define CQChartsLoader_H

#include <CQChartsCsv.h>
#include <CQChartsTsv.h>
#include <CQChartsJson.h>
#include <CQChartsGnuData.h>

namespace CQChartsLoader {

  CQChartsCsv *loadCsv(CQCharts *charts, const QString &filename, bool commentHeader=false,
                       bool firstLineHeader=false, bool firstColumnHeader=false,
                       const QString &filter="") {
    CQChartsCsv *csv = new CQChartsCsv(charts);

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

  CQChartsTsv *loadTsv(CQCharts *charts, const QString &filename, bool commentHeader=false,
                       bool firstLineHeader=false, bool firstColumnHeader=false,
                       const QString &filter="") {
    CQChartsTsv *tsv = new CQChartsTsv(charts);

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

  CQChartsJson *loadJson(CQCharts *charts, const QString &filename) {
    CQChartsJson *json = new CQChartsJson(charts);

    if (! json->load(filename)) {
      delete json;
      return nullptr;
    }

    return json;
  }

  CQChartsGnuData *loadData(CQCharts *charts, const QString &filename, bool commentHeader=false,
                            bool firstLineHeader=false ) {
    CQChartsGnuData *data = new CQChartsGnuData(charts);

    data->setCommentHeader  (commentHeader);
    data->setFirstLineHeader(firstLineHeader);

    if (! data->load(filename)) {
      delete data;
      return nullptr;
    }

    return data;
  }
};

#endif
