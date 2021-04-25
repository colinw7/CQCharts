#ifndef CQChartsModelDetailsWidget_H
#define CQChartsModelDetailsWidget_H

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
class CQChartsModelDetailsWidget : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool flip READ isFlip WRITE setFlip)

 public:
  CQChartsModelDetailsWidget(CQCharts *charts);

  bool isFlip() const;
  void setFlip(bool b);

  void setModelData(CQChartsModelData *modelData, bool invalidate=true);

  void invalidate();

 private slots:
  void invalidateSlot();
  void flipSlot(int state);
  void updateSlot();

 private:
  using ModelDataP = QPointer<CQChartsModelData>;

  CQCharts*                  charts_          { nullptr };
  ModelDataP                 modelData_;
  QLabel*                    modelLabel_      { nullptr };
  QPushButton*               updateButton_    { nullptr };
  CQLabel*                   numColumnsLabel_ { nullptr };
  CQLabel*                   numRowsLabel_    { nullptr };
  QLabel*                    hierLabel_       { nullptr };
  QCheckBox*                 flipCheck_       { nullptr };
  CQChartsModelDetailsTable* detailsTable_    { nullptr };
};

#endif
