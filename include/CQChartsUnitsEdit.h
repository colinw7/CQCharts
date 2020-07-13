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
  using Units = CQChartsUnits;

 public:
  CQChartsUnitsEdit(QWidget *parent=nullptr);

  const Units &units() const;
  void setUnits(const Units &units);

  QSize sizeHint() const override;

 signals:
  void unitsChanged();

 private:
  void updateTip();

 private slots:
  void indexChanged();

 private:
  Units units_ { Units::NONE };
};

#endif
