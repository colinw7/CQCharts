#ifndef CQChartsModelChooser_H
#define CQChartsModelChooser_H

#include <QFrame>

class CQCharts;
class QComboBox;

class CQChartsModelChooser : public QFrame {
  Q_OBJECT

  Q_PROPERTY(int currentModelInd READ currentModelInd)

 public:
  CQChartsModelChooser(CQCharts *charts);

  int currentModelInd() const;

 signals:
  void currentModelChanged();

 private:
  void connectSlots(bool b);

  void updateModels();

 private slots:
  void setCurrentModel(int ind);

 private:
  CQCharts*  charts_ { nullptr };
  QComboBox* combo_  { nullptr };
};

#endif
