#ifndef CQChartsPropertyViewTreeFilterEdit_H
#define CQChartsPropertyViewTreeFilterEdit_H

#include <QFrame>

class CQChartsPropertyViewTree;
class CQChartsFilterEdit;

/*!
 * \brief properties view tree filter edit widget
 * \ingroup Charts
 */
class CQChartsPropertyViewTreeFilterEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsPropertyViewTreeFilterEdit(CQChartsPropertyViewTree *tree);

 private Q_SLOTS:
  void replaceFilterSlot(const QString &text);
  void addFilterSlot(const QString &text);

  void replaceSearchSlot(const QString &text);
  void addSearchSlot(const QString &text);

  void hideFilterSlot();

 private:
  CQChartsPropertyViewTree* tree_       { nullptr };
  CQChartsFilterEdit*       filterEdit_ { nullptr };
};

#endif
