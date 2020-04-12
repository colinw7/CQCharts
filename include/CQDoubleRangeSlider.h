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

  double rangeMin() const { return range_.min; }
  void setRangeMin(double r);

  double rangeMax() const { return range_.max; }
  void setRangeMax(double r);

  double sliderMin() const { return slider_.min; }
  void setSliderMin(double r, bool force=false);

  double sliderMax() const { return slider_.max; }
  void setSliderMax(double r, bool force=false);

  double sliderDelta() const { return sliderDelta_; }
  void setSliderDelta(double r) { sliderDelta_ = r; }

  int decimalPlaces() const { return decimalPlaces_; }
  void setDecimalPlaces(int i) { decimalPlaces_ = i; }

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

 private:
  void updateTip();

  QString realToString(double r) const;

  double clampValue(double i) const;

  double deltaValue(double r, int inc) const;

  void pixelToSliderValue(int px, int &ind, bool force=false);
  void pixelSetSliderValue(int px, int ind, bool force=false);

  double valueToPixel(double x) const;
  double pixelToValue(double px) const;

 private:
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
};

#endif
