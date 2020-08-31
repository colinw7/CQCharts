#ifndef CQChartsColumnCombo_H
#define CQChartsColumnCombo_H

#include <CQChartsColumn.h>
#include <QComboBox>

/*!
 * \brief combo box for CQChartsColumn
 * \ingroup Charts
 */
class CQChartsColumnCombo : public QComboBox {
  Q_OBJECT

  Q_PROPERTY(bool           allowNone READ isAllowNone WRITE setAllowNone)
  Q_PROPERTY(CQChartsColumn column    READ getColumn   WRITE setColumn   )

 public:
  CQChartsColumnCombo(QWidget *parent=nullptr);

  bool isAllowNone() const { return allowNone_; }
  void setAllowNone(bool b);

  CQChartsColumn getColumn() const;
  void setColumn(const CQChartsColumn &column);

  void setModel(QAbstractItemModel *model);

 signals:
  void columnChanged();

 private:
  void updateItems();

 private:
  QAbstractItemModel *model_     { nullptr };
  bool                allowNone_ { true };
};

#endif
