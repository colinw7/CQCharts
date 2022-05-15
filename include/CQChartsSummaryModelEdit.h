#ifndef CQChartsSummaryModelEdit_H
#define CQChartsSummaryModelEdit_H

#include <QFrame>

class CQChartsModelData;
class CQChartsColumnCombo;

class CQIntegerSpin;

class QComboBox;
class QStackedWidget;
class QCheckBox;

class CQChartsSummaryModelEdit : public QFrame {
  Q_OBJECT

 public:
  using ModelData = CQChartsModelData;

 public:
  CQChartsSummaryModelEdit(QWidget *parent);

  const ModelData *modelData() const { return modelData_; }
  void setModelData(ModelData *modelData);

 private:
  void connectSlots(bool b);

 private slots:
  void enabledSlot();
  void typeSlot();

  void updateModelFromWidgets();
  void updateWidgetsFromModel();

 private:
  ModelData* modelData_ { nullptr };

  QCheckBox*           enabledCheck_    { nullptr }; //!< enabled checkbox
  CQIntegerSpin*       maxRowsSpin_     { nullptr }; //!< max rows
  QComboBox*           typeCombo_       { nullptr }; //!< type combo
  QStackedWidget*      typeStack_       { nullptr }; //!< type stack
  CQChartsColumnCombo* sortedColEdit_   { nullptr }; //!< sorted column edit
  QCheckBox*           sortOrderCheck_  { nullptr }; //!< sort increasing checkbox
  CQIntegerSpin*       pageSizeEdit_    { nullptr }; //!< page size edit
  CQIntegerSpin*       currentPageEdit_ { nullptr }; //!< current page edit
};

#endif
