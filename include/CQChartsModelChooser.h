#ifndef CQChartsModelChooser_H
#define CQChartsModelChooser_H

#include <CQChartsWidgetIFace.h>
#include <QFrame>

class CQCharts;
class QComboBox;

/*!
 * \brief Model Chooser Combo
 * \ingroup Charts
 */
class CQChartsModelChooser : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

  Q_PROPERTY(int currentModelInd READ currentModelInd)

 public:
  CQChartsModelChooser(CQCharts *charts=nullptr);

  void setCharts(CQCharts *charts) override;

  int currentModelInd() const;

 signals:
  void currentModelChanged();

 private:
  void connectSlots(bool b);

 private slots:
  void setCurrentModel(int ind);

  void updateModels();

 private:
  CQCharts*  charts_ { nullptr };
  QComboBox* combo_  { nullptr };
};

#endif
