#ifndef CQChartsPlotPropertyEdit_H
#define CQChartsPlotPropertyEdit_H

#include <CQChartsWidgetIFace.h>

#include <QFrame>
#include <QPointer>

class CQChartsPlotPropertyEdit;
class CQChartsPlot;

class QLabel;
class QHBoxLayout;
class QVBoxLayout;

/*!
 * \brief Widget to hold a Group of Plot Property Editors
 * \ingroup Charts
 */
class CQChartsPlotPropertyEditGroup : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsPlotPropertyEditGroup(Plot *plot=nullptr);

  Plot *plot() const;
  void setPlot(Plot *p) override;

  void addEdit(CQChartsPlotPropertyEdit *edit);

  QSize sizeHint() const override;

 public Q_SLOTS:
  void addSlot(const QString &data); // [<label>] <property>

 private:
  using PlotP = QPointer<Plot>;
  using Edits = std::vector<CQChartsPlotPropertyEdit *>;

  PlotP        plot_;
  QVBoxLayout* widgetLayout_ { nullptr };
  QVBoxLayout* layout_       { nullptr };
  Edits        edits_;
};

//------

/*!
 * \brief Plot Property Editor with optional Label
 * \ingroup Charts
 */
class CQChartsPlotPropertyEdit : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

  Q_PROPERTY(QString propertyName READ propertyName WRITE setPropertyName)
  Q_PROPERTY(QString label        READ label        WRITE setLabel)

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsPlotPropertyEdit(Plot *plot=nullptr, const QString &propertyName="");

  Plot *plot() const;
  void setPlot(Plot *p) override;

  const QString &propertyName() const { return propertyName_; }
  void setPropertyName(const QString &s);

  const QString &label() const { return label_; }
  void setLabel(const QString &s);

  QSize sizeHint() const override;

 private:
  void init(bool rebuild=false);

  QWidget *createEditor();

  void setWidgetPlot(QWidget *widget);

 private Q_SLOTS:
  void updateValue();

 private:
  using PlotP = QPointer<Plot>;

  PlotP        plot_;
  QString      propertyName_;
  QString      label_;
  QHBoxLayout* layout_       { nullptr };
  QLabel*      labelWidget_  { nullptr };
  QHBoxLayout* widgetLayout_ { nullptr };
  QWidget*     widget_       { nullptr };
};

#endif
