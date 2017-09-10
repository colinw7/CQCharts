#ifndef CQChartsProbeBand_H
#define CQChartsProbeBand_H

#include <QString>

class CQChartsView;
class CQChartsPlot;
class QRubberBand;
class QLabel;

class CQChartsProbeBand {
 public:
  CQChartsProbeBand(CQChartsView *view);
 ~CQChartsProbeBand();

  void show(CQChartsPlot *plot, const QString &text, double px, double py1, double py2);
  void hide();

 private:
  CQChartsView* view_  { nullptr };
  QRubberBand*  vband_ { nullptr };
  QRubberBand*  hband_ { nullptr };
  QLabel*       tip_   { nullptr };
};

#endif
