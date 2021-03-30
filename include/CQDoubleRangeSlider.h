#ifndef CQDoubleRangeSlider_H
#define CQDoubleRangeSlider_H

#include <CQRangeSlider.h>

class CQDoubleRangeSlider : public CQRangeSlider {
  Q_OBJECT

  Q_PROPERTY(double rangeMin      READ rangeMin      WRITE setRangeMin )
  Q_PROPERTY(double rangeMax      READ rangeMax      WRITE setRangeMax )
  Q_PROPERTY(double sliderMin     READ sliderMin     WRITE setSliderMin)
  Q_PROPERTY(double sliderMax     READ sliderMax     WRITE setSliderMax)
  Q_PROPERTY(double sliderDelta   READ sliderDelta   WRITE setSliderDelta)
  Q_PROPERTY(int    decimalPlaces READ decimalPlaces WRITE setDecimalPlaces)

 public:
  CQDoubleRangeSlider(QWidget *parent=nullptr);

  //---

  //! get/set range extent min/max
  double rangeMin() const { return range_.min; }
  void setRangeMin(double r);

  double rangeMax() const { return range_.max; }
  void setRangeMax(double r);

  void setRangeMinMax(double min, double max);

  //---

  //! get/set slider min/max (in extent)
  double sliderMin() const { return slider_.min; }
  void setSliderMin(double r, bool force=false);

  double sliderMax() const { return slider_.max; }
  void setSliderMax(double r, bool force=false);

  void setSliderMinMax(double min, double max, bool force=false);

  //---

  //! get/set delta (click delta)
  double sliderDelta() const { return sliderDelta_; }
  void setSliderDelta(double r) { sliderDelta_ = r; }

  //! get/set label decimal places
  int decimalPlaces() const { return decimalPlaces_; }
  void setDecimalPlaces(int i) { decimalPlaces_ = i; }

  //---

  //! set/clear linear gradient
  void setLinearGradient(const QLinearGradient &lg);
  void clearLinearGradient();

  //---

  void mousePressEvent  (QMouseEvent *) override;
  void mouseMoveEvent   (QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;

  void keyPressEvent(QKeyEvent *e) override;

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 signals:
  void rangeChanged(double min, double max);

  void sliderRangeChanging(double min, double max);
  void sliderRangeChanged (double min, double max);

 protected:
  void updateTip();

  QString realToString(double r) const;

  double clampValue(double i) const;

  double deltaValue(double r, int inc) const;

  void pixelToSliderValue(int px, int &ind, bool force=false);
  void pixelSetSliderValue(int px, int ind, bool force=false);

  double valueToPixel(double x) const override;
  double pixelToValue(double px) const override;

  double getSliderMin() const override { return sliderMin(); }
  double getSliderMax() const override { return sliderMax(); }

 protected:
  struct Range {
    double min { 0.0 };
    double max { 1.0 };

    Range() = default;

    Range(double min, double max) :
     min(min), max(max) {
    }
  };

  Range range_  { 0.0, 1.0 };
  Range slider_ { 0.0, 1.0 };
  Range save_;

  double sliderDelta_   { 0.01 };
  int    decimalPlaces_ { 3 };

  QLinearGradient lg_;
  bool            lgSet_ { false };
};

#endif
