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
  using Column = CQChartsColumn;

 public:
  CQChartsColumnCombo(QWidget *parent=nullptr);

  bool isAllowNone() const { return allowNone_; }
  void setAllowNone(bool b);

  Column getColumn() const;
  void setColumn(const Column &column);

  void setModelData(CQChartsModelData *model);

 signals:
  void columnChanged();

 private slots:
  void updateItems();

 private:
  CQChartsModelData *modelData_ { nullptr };
  bool               allowNone_ { true };
};

#endif
