#ifndef CQChartsViewPropertiesWidget_H
#define CQChartsViewPropertiesWidget_H

#include <CQChartsWidgetIFace.h>

#include <QFrame>
#include <QPointer>

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

  View *view() const;
  void setView(View *view) override;

  CQChartsPropertyViewTree *propertyTree() const { return propertyTree_; }

 Q_SIGNALS:
  void propertyItemSelected(QObject *obj, const QString &path);

 private Q_SLOTS:
  void filterStateSlot(bool show, bool focus);

 private:
  using ViewP = QPointer<View>;

  ViewP                               view_;
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

 private Q_SLOTS:
  void editKeySlot();
  void writeSlot();

 private:
  using EditKeyDlg = CQChartsEditKeyDlg;

  CQChartsViewPropertiesWidget* propertiesWidget_ { nullptr };

  EditKeyDlg* editKeyDlg_ { nullptr }; //!< edit plot key dialog
};

#endif
