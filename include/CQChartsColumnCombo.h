#ifndef CQChartsColumnCombo_H
#define CQChartsColumnCombo_H

#include <CQChartsColumn.h>
#include <QComboBox>

class CQChartsModelData;

/*!
 * \brief combo box for CQChartsColumn
 * \ingroup Charts
 */
class CQChartsColumnCombo : public QComboBox {
  Q_OBJECT

  Q_PROPERTY(bool           allowNone READ isAllowNone WRITE setAllowNone)
  Q_PROPERTY(CQChartsColumn column    READ getColumn   WRITE setColumn   )

 public:
  using ModelData = CQChartsModelData;
  using Column    = CQChartsColumn;

 public:
  CQChartsColumnCombo(QWidget *parent=nullptr);

  //! get set allow no column
  bool isAllowNone() const { return allowNone_; }
  void setAllowNone(bool b);

  //! get/set current column
  Column getColumn() const;
  void setColumn(const Column &column);

  //! get/set model data
  const ModelData *modelData() const { return modelData_; }
  void setModelData(const ModelData *model);

  //! convenience routine to set model and column
  void setModelColumn(ModelData *modelData, const Column &column);

 signals:
  //! emitted when column changed
  void columnChanged();

 private slots:
  void updateItems();

 private:
  void connectSlots(bool b);

 private:
  const ModelData *modelData_ { nullptr };
  bool             allowNone_ { true };
};

#endif
