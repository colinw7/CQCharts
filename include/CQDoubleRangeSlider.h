#ifndef CQDoubleRangeSlider_H
#define CQDoubleRangeSlider_H

#include <CQRangeSlider.h>

// range widget with sub range sliders
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

  void setRangeMinMax(double min, double max, bool resetSlider=true);

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

  void fixSliderValues();

  //---

  void paintEvent(QPaintEvent *) override;

  virtual void drawRangeLabels(QPainter *painter);

  virtual void drawSliderLabels(QPainter *painter);

  //---

  QSize sizeHint() const override;

 signals:
  void rangeChanged(double min, double max);

  void sliderRangeChanging(double min, double max);
  void sliderRangeChanged (double min, double max);

 protected:
  virtual void updateTip();

  void drawText(QPainter *painter, int x, int y, const QString &text);

  QString realToString(double r) const;

  void deltaSliderMin(int d) override {
    setSliderMin(clampValue(deltaValue(getSliderMin(), d)), /*force*/true);
  }

  void deltaSliderMax(int d) override {
    setSliderMax(clampValue(deltaValue(getSliderMax(), d)), /*force*/true);
  }

  void reset() override {
    setSliderMinMax(rangeMin(), rangeMax());
  }

  double clampValue(double i) const;
  double deltaValue(double r, int inc) const;

  void pixelToSliderValue(int px, int &ind, bool force=false) override;
  void pixelSetSliderValue(int px, int ind, bool force=false) override;

  void saveRange() override { save_ = slider_; }

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
};

#endif
