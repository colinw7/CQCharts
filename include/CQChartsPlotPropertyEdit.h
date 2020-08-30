#ifndef CQChartsPlotPropertyEdit_H
#define CQChartsPlotPropertyEdit_H

#include <QFrame>

class CQChartsPlotPropertyEdit;
class CQChartsPlot;

class QLabel;
class QHBoxLayout;
class QVBoxLayout;

class CQChartsPlotPropertyEditGroup : public QFrame {
  Q_OBJECT

 public:
  CQChartsPlotPropertyEditGroup(CQChartsPlot *plot=nullptr);

  const CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *p);

  void addEdit(CQChartsPlotPropertyEdit *edit);

  QSize sizeHint() const override;

 public slots:
  void addSlot(const QString &data); // [<label>] <property>

 private:
  using Edits = std::vector<CQChartsPlotPropertyEdit *>;

  CQChartsPlot* plot_         { nullptr };
  QVBoxLayout*  widgetLayout_ { nullptr };
  QVBoxLayout*  layout_       { nullptr };
  Edits         edits_;
};

//------

class CQChartsPlotPropertyEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString propertyName READ propertyName WRITE setPropertyName)
  Q_PROPERTY(QString label        READ label        WRITE setLabel)

 public:
  CQChartsPlotPropertyEdit(CQChartsPlot *plot=nullptr, const QString &propertyName="");

  const CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *p);

  const QString &propertyName() const { return propertyName_; }
  void setPropertyName(const QString &s);

  const QString &label() const { return label_; }
  void setLabel(const QString &s);

  QSize sizeHint() const override;

 private:
  void init(bool rebuild=false);

  QWidget *createEditor();

  void setWidgetPlot(QWidget *widget);

 private slots:
  void updateValue();

 private:
  CQChartsPlot* plot_         { nullptr };
  QString       propertyName_;
  QString       label_;
  QHBoxLayout*  layout_       { nullptr };
  QLabel*       labelWidget_  { nullptr };
  QHBoxLayout*  widgetLayout_ { nullptr };
  QWidget*      widget_       { nullptr };
};

#endif
