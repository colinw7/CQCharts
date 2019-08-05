#ifndef CQChartsModelDetailsWidget_H
#define CQChartsModelDetailsWidget_H

#include <QFrame>
#include <QPointer>

class CQChartsModelDetails;
class CQCharts;

class CQTableWidget;
class QTextEdit;
class QPushButton;

/*!
 * \brief Model Details Widget
 * \ingroup Charts
 */
class CQChartsModelDetailsWidget : public QFrame {
  Q_OBJECT

 public:
  CQChartsModelDetailsWidget(CQCharts *charts);

  void updateDetails(const CQChartsModelDetails *details);

  void setDetails(const CQChartsModelDetails *details);

  void invalidate();

 private slots:
  void invalidateSlot();
  void updateSlot();

 private:
  using ModelDetailsP = QPointer<CQChartsModelDetails>;

  CQCharts*      charts_       { nullptr };
  ModelDetailsP  details_;
  QPushButton*   updateButton_ { nullptr };
  QTextEdit*     detailsText_  { nullptr };
  CQTableWidget* detailsTable_ { nullptr };
};

#endif
