#ifndef CQChartsProbeBand_H
#define CQChartsProbeBand_H

#include <QObject>
#include <QString>
#include <QFont>

class CQChartsView;
class CQChartsPlot;
class QRubberBand;
class QLabel;

/*!
 * \brief x/y rubber bands for probe
 * \ingroup Charts
 */
class CQChartsProbeBand : public QObject {
  Q_OBJECT

  Q_PROPERTY(QFont font READ font WRITE setFont)

 public:
  CQChartsProbeBand(CQChartsView *view);
 ~CQChartsProbeBand();

  const QFont &font() const { return font_; }
  void setFont(const QFont &f) { font_ = f; }

  void showVertical  (CQChartsPlot *plot, const QString &text, double px, double py1, double py2);
  void showHorizontal(CQChartsPlot *plot, const QString &text, double px1, double px2, double py);

  void hide();

 private:
  CQChartsView* view_  { nullptr };
  QRubberBand*  vband_ { nullptr };
  QRubberBand*  hband_ { nullptr };
  QLabel*       tip_   { nullptr };
  QFont         font_;
};

#endif
