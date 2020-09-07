#ifndef CQChartsLineEdit_H
#define CQChartsLineEdit_H

#include <QLineEdit>

/*!
 * \brief Wrapper around QLineEdit Widget
 * \ingroup Charts
 */
class CQChartsLineEdit : public QLineEdit {
  Q_OBJECT

 public:
  CQChartsLineEdit(QWidget *parent=nullptr);
};

#endif
