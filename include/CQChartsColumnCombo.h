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

 public:
  CQChartsColumnCombo(QWidget *parent=nullptr);

  void setColumn(const CQChartsColumn &column);
  CQChartsColumn getColumn() const;

  void setModel(QAbstractItemModel *model);

 signals:
  void columnChanged();
};

#endif
