#ifndef CQChartsModelDetailsWidget_H
#define CQChartsModelDetailsWidget_H

#include <CQChartsWidgetIFace.h>

#include <QFrame>
#include <QPointer>

class CQChartsModelData;
class CQChartsModelDetailsTable;
class CQCharts;
class CQLabel;

class QPushButton;
class QLabel;
class QCheckBox;

/*!
 * \brief Model Details Widget
 * \ingroup Charts
 */
class CQChartsModelDetailsWidget : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

  Q_PROPERTY(bool flip READ isFlip WRITE setFlip)

 public:
  using ModelData    = CQChartsModelData;
  using DetailsTable = CQChartsModelDetailsTable;

 public:
  CQChartsModelDetailsWidget(CQCharts *charts=nullptr);

  //! get/set flip
  bool isFlip() const;
  void setFlip(bool b);

  void setCharts(CQCharts *charts) override;

  void setModelData(ModelData *modelData) override;

  void invalidateModelData(ModelData *modelData, bool invalidate=true);

  void invalidate();

 private Q_SLOTS:
  void invalidateSlot();
  void flipSlot(int state);
  void updateSlot();

 private:
  using ModelDataP = QPointer<ModelData>;

  CQCharts*     charts_          { nullptr };
  ModelDataP    modelData_;
  QLabel*       modelLabel_      { nullptr };
  QPushButton*  updateButton_    { nullptr };
  CQLabel*      numColumnsLabel_ { nullptr };
  CQLabel*      numRowsLabel_    { nullptr };
  QLabel*       hierLabel_       { nullptr };
  QCheckBox*    flipCheck_       { nullptr };
  DetailsTable* detailsTable_    { nullptr };
};

#endif
