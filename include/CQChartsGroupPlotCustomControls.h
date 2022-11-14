#ifndef CQChartsGroupPlotCustomControls_H
#define CQChartsGroupPlotCustomControls_H

#include <CQChartsPlotCustomControls.h>

class CQChartsGroupPlot;
class CQIconRadio;
class CQDoubleRangeSlider;
class CQRealSpin;
class CQIntegerSpin;
class CQLabel;

class QAbstractButton;
class QLineEdit;
class QButtonGroup;

class CQChartsGroupPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsGroupPlotCustomControls(CQCharts *charts, const QString &plotType);

  void setPlot(Plot *plot) override;

  void addGroupColumnWidgets();

  FrameData &groupFrame() { return groupFrame_; }

 protected:
  void connectSlots(bool b) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected Q_SLOTS:
  void groupColumnSlot();

  void bucketRadioGroupSlot(QAbstractButton *);
  void bucketRangeSlot();
  void startBucketSlot();
  void deltaBucketSlot();
  void numBucketsSlot();
  void bucketStopsSlot();

 private:
  CQChartsGroupPlot* groupPlot_ { nullptr };

  FrameData            groupFrame_;
  QButtonGroup*        bucketRadioGroup_  { nullptr };
  CQIconRadio*         fixedBucketRadio_  { nullptr };
  CQIconRadio*         rangeBucketRadio_  { nullptr };
  CQIconRadio*         stopsBucketRadio_  { nullptr };
  CQIconRadio*         uniqueBucketRadio_ { nullptr };
  CQChartsColumnCombo* groupColumnCombo_  { nullptr };
  CQDoubleRangeSlider* bucketRange_       { nullptr };
  CQRealSpin*          startBucketEdit_   { nullptr };
  CQRealSpin*          deltaBucketEdit_   { nullptr };
  CQIntegerSpin*       numBucketsEdit_    { nullptr };
  QLineEdit*           bucketStopsEdit_   { nullptr };
  CQLabel*             uniqueCount_       { nullptr };
  QLabel*              rangeLabel_        { nullptr };
};

#endif
