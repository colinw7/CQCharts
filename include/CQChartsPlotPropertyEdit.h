#ifndef CQChartsPlotPropertyEdit_H
#define CQChartsPlotPropertyEdit_H

#include <QFrame>

class CQChartsPlot;

class QHBoxLayout;

class CQChartsPlotPropertyEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString propertyName READ propertyName WRITE setPropertyName)

 public:
  CQChartsPlotPropertyEdit(CQChartsPlot *plot=nullptr, const QString &propertyName="");

  const CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *p);

  const QString &propertyName() const { return propertyName_; }
  void setPropertyName(const QString &s);

 private:
  void init();

  QWidget *createEditor();

 private slots:
  void updateValue();

 private:
  CQChartsPlot* plot_   { nullptr };
  QString       propertyName_;
  QHBoxLayout*  layout_ { nullptr };
  QWidget*      widget_ { nullptr };
};

#endif
