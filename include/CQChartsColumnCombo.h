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

  Q_PROPERTY(bool           allowNone   READ isAllowNone   WRITE setAllowNone  )
  Q_PROPERTY(CQChartsColumn column      READ getColumn     WRITE setColumn     )
  Q_PROPERTY(bool           numericOnly READ isNumericOnly WRITE setNumericOnly)
  Q_PROPERTY(bool           proxy       READ isProxy       WRITE setProxy      )

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

  //! get/set numeric only
  bool isNumericOnly() const { return numericOnly_; }
  void setNumericOnly(bool b);

  bool isProxy() const { return proxy_; }
  void setProxy(bool b);

  //! get/set model data
  const ModelData *modelData() const { return modelData_; }
  void setModelData(ModelData *model);

  //! convenience routine to set model and column
  void setModelColumn(ModelData *modelData, const Column &column);

 Q_SIGNALS:
  //! emitted when column changed
  void columnChanged();

 private Q_SLOTS:
  void resetModelData();

  void updateItems();

 private:
  void connectSlots(bool b);

 private:
  const ModelData *modelData_   { nullptr };
  bool             allowNone_   { true };
  bool             numericOnly_ { false };
  bool             proxy_       { true };
};

#endif
