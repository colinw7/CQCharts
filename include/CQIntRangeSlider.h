#ifndef CQIntRangeSlider_H
#define CQIntRangeSlider_H

#include <CQRangeSlider.h>

class CQIntRangeSlider : public CQRangeSlider {
  Q_OBJECT

  Q_PROPERTY(int rangeMin    READ rangeMin    WRITE setRangeMin )
  Q_PROPERTY(int rangeMax    READ rangeMax    WRITE setRangeMax )
  Q_PROPERTY(int sliderMin   READ sliderMin   WRITE setSliderMin)
  Q_PROPERTY(int sliderMax   READ sliderMax   WRITE setSliderMax)
  Q_PROPERTY(int sliderDelta READ sliderDelta WRITE setSliderDelta)

 public:
  CQIntRangeSlider(QWidget *parent=nullptr);

  //---

  //! get/set range extent min/max
  int rangeMin() const { return range_.min; }
  void setRangeMin(int i);

  int rangeMax() const { return range_.max; }
  void setRangeMax(int i);

  void setRangeMinMax(int min, int max);

  //---

  //! get/set slider min/max (in extent)
  int sliderMin() const { return slider_.min; }
  void setSliderMin(int i, bool force=false);

  int sliderMax() const { return slider_.max; }
  void setSliderMax(int i, bool force=false);

  void setSliderMinMax(int min, int max, bool force=false);

  //---

  //! get/set delta (click delta)
  int sliderDelta() const { return sliderDelta_; }
  void setSliderDelta(int i) { sliderDelta_ = i; }

  //---

  void fixSliderValues();

  //---

  void paintEvent(QPaintEvent *) override;

  //---

  virtual void drawRangeLabels(QPainter *painter);

  virtual void drawSliderLabels(QPainter *painter);

  //---

  QSize sizeHint() const override;

 signals:
  void rangeChanged(int min, int max);

  void sliderRangeChanging(int min, int max);
  void sliderRangeChanged (int min, int max);

 protected:
  virtual void updateTip();

  QString intToString(int i) const;

  void deltaSliderMin(int d) override {
    setSliderMin(clampValue(deltaValue(int(getSliderMin()), d)), /*force*/true);
  }

  void deltaSliderMax(int d) override {
    setSliderMax(clampValue(deltaValue(int(getSliderMax()), d)), /*force*/true);
  }

  void reset() override {
    setSliderMinMax(rangeMin(), rangeMax());
  }

  int clampValue(int i) const;
  int deltaValue(int i, int inc) const;

  void pixelToSliderValue(int px, int &ind, bool force=false) override;
  void pixelSetSliderValue(int px, int ind, bool force=false) override;

  void saveRange() override { save_ = slider_; }

  double valueToPixel(double x) const override;
  double pixelToValue(double px) const override;

  double getSliderMin() const override { return sliderMin(); }
  double getSliderMax() const override { return sliderMax(); }

 protected:
  struct Range {
    int min {   0 };
    int max { 100 };

    Range() = default;

    Range(int min, int max) :
     min(min), max(max) {
    }
  };

  Range range_  { 0, 100 };
  Range slider_ { 0, 100 };
  Range save_;

  int sliderDelta_ { 1 };
};

#endif
