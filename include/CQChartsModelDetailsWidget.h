#ifndef CQChartsModelDetailsWidget_H
#define CQChartsModelDetailsWidget_H

#include <QFrame>
#include <QPointer>

class CQChartsModelDetails;
class CQCharts;

class CQTableWidget;
class QPushButton;
class QLabel;
class QCheckBox;

/*!
 * \brief Model Details Widget
 * \ingroup Charts
 */
class CQChartsModelDetailsWidget : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool flip READ isFlip WRITE setFlip)

 public:
  CQChartsModelDetailsWidget(CQCharts *charts);

  bool isFlip() const { return flip_; }
  void setFlip(bool b);

  void updateDetails(const CQChartsModelDetails *details);

  void setDetails(const CQChartsModelDetails *details, bool invalidate=true);

  void invalidate();

 private slots:
  void invalidateSlot();
  void flipSlot(int state);
  void updateSlot();

 private:
  using ModelDetailsP = QPointer<CQChartsModelDetails>;

  CQCharts*      charts_          { nullptr };
  bool           flip_            { false };
  ModelDetailsP  details_;
  QLabel*        modelLabel_      { nullptr };
  QPushButton*   updateButton_    { nullptr };
  QLabel*        numColumnsLabel_ { nullptr };
  QLabel*        numRowsLabel_    { nullptr };
  QLabel*        hierLabel_       { nullptr };
  QCheckBox*     flipCheck_       { nullptr };
  CQTableWidget* detailsTable_    { nullptr };
};

#endif
