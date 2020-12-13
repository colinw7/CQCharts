#ifndef CQChartsProbeBand_H
#define CQChartsProbeBand_H

#include <QLabel>
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
  enum class LabelPos {
    MIN,
    MAX,
    VALUE
  };

  using View = CQChartsView;
  using Plot = CQChartsPlot;

 public:
  CQChartsProbeBand(View *view);
 ~CQChartsProbeBand();

  //---

  const QFont &font() const { return font_; }
  void setFont(const QFont &f) { font_ = f; }

  const LabelPos &labelPos() const { return labelPos_; }
  void setLabelPos(const LabelPos &v) { labelPos_ = v; }

  //---

  void showVertical  (Plot *plot, const QString &text,
                      double px, double py1, double py2, double py3);
  void showHorizontal(Plot *plot, const QString &text,
                      double px1, double px2, double px3, double py);

  void hide();

  bool isVisible() const { return visible_; }

  //---

  QRect labelRect() const;

  void moveLabel(int dx, int dy);

  //---

  double pos() const { return pos_; }

  double value() const { return value_; }

  const Qt::Orientation &orientation() const { return orientation_; }

 private:
  View*           view_        { nullptr };
  QRubberBand*    vband_       { nullptr };
  QRubberBand*    hband_       { nullptr };
  QLabel*         tip_         { nullptr };
  QFont           font_;
  LabelPos        labelPos_    { LabelPos::VALUE };
  int             tickLen_     { 8 };
  Qt::Orientation orientation_ { Qt::Vertical };
  double          pos_         { 0.0 };
  double          value_       { 0.0 };
  bool            visible_     { false };
};

//---

class CQChartsProbeLabel : public QLabel {
  Q_OBJECT

 public:
  CQChartsProbeLabel(CQChartsProbeBand *band);
 ~CQChartsProbeLabel();

 private:
  CQChartsProbeBand* band_ { nullptr };
};

#endif
