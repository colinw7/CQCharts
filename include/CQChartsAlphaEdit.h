#ifndef CQChartsAlphaEdit_H
#define CQChartsAlphaEdit_H

#include <CQRealSpin.h>

/*!
 * \brief alpha value edit
 */
class CQChartsAlphaEdit : public CQRealSpin {
  Q_OBJECT

 public:
  CQChartsAlphaEdit(QWidget *parent=nullptr);
};

#endif