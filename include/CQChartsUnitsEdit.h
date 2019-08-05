#ifndef CQChartsUnitsEdit_H
#define CQChartsUnitsEdit_H

#include <CQChartsTypes.h>
#include <QComboBox>

/*!
 * \brief units edit
 * \ingroup Charts
 */
class CQChartsUnitsEdit : public QComboBox {
  Q_OBJECT

 public:
  CQChartsUnitsEdit(QWidget *parent=nullptr);

  const CQChartsUnits &units() const;
  void setUnits(const CQChartsUnits &units);

  QSize sizeHint() const override;

 signals:
  void unitsChanged();

 private slots:
  void indexChanged();

 private:
  CQChartsUnits units_ { CQChartsUnits::NONE };
};

#endif
