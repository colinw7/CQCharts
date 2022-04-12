#ifndef CQChartsViewPropertiesWidget_H
#define CQChartsViewPropertiesWidget_H

#include <CQChartsWidgetIFace.h>

#include <QFrame>

class CQChartsView;
class CQChartsPropertyViewTree;
class CQChartsPropertyViewTreeFilterEdit;
class CQChartsEditKeyDlg;

/*!
 * \brief View properties widget
 * \ingroup Charts
 */
class CQChartsViewPropertiesWidget : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using View = CQChartsView;

 public:
  CQChartsViewPropertiesWidget(View *view=nullptr);

  View *view() const { return view_; }
  void setView(View *view) override;

  CQChartsPropertyViewTree *propertyTree() const { return propertyTree_; }

 signals:
  void propertyItemSelected(QObject *obj, const QString &path);

 private slots:
  void filterStateSlot(bool show, bool focus);

 private:
  View*                               view_         { nullptr };
  CQChartsPropertyViewTree*           propertyTree_ { nullptr };
  CQChartsPropertyViewTreeFilterEdit* filterEdit_   { nullptr };
};

//---

class CQChartsViewPropertiesControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using View = CQChartsView;

 public:
  CQChartsViewPropertiesControl(View *view=nullptr);

  View *view() const;
  void setView(View *view) override;

  CQChartsViewPropertiesWidget *propertiesWidget() const { return propertiesWidget_; }

 private slots:
  void editKeySlot();
  void writeSlot();

 private:
  using EditKeyDlg = CQChartsEditKeyDlg;

  CQChartsViewPropertiesWidget* propertiesWidget_ { nullptr };

  EditKeyDlg* editKeyDlg_ { nullptr }; //!< edit plot key dialog
};

#endif
