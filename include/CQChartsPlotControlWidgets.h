#ifndef CQChartsPlotControlWidgets_H
#define CQChartsPlotControlWidgets_H

#include <CQChartsColumn.h>

#include <QFrame>

class CQChartsPlot;
class QRadioButton;
class CQDoubleRangeSlider;
class CQIntRangeSlider;
class CQTimeRangeSlider;

class QLabel;
class QComboBox;

class CQChartsPlotControlIFace : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn column READ column WRITE setColumn)

 public:
  CQChartsPlotControlIFace(QWidget *parent=nullptr);
  CQChartsPlotControlIFace(CQChartsPlot *plot, const CQChartsColumn &column);

  const CQChartsPlot *plot() const { return plot_; }
  virtual void setPlot(CQChartsPlot *p) { plot_ = p; }

  const CQChartsColumn &column() const { return column_; }
  virtual void setColumn(const CQChartsColumn &c) { column_ = c; }

  QRadioButton *radio() const { return radio_; }

  virtual void connectValueChanged(QObject *obj, const char *slotName) = 0;

  virtual QString filterStr(const QString &cmpStr) const = 0;

 protected:
  CQChartsPlot*  plot_  { nullptr };
  CQChartsColumn column_;
  QLabel*        label_ { nullptr };
  QRadioButton*  radio_ { nullptr };
};

//---

class CQChartsPlotRealControl : public CQChartsPlotControlIFace {
  Q_OBJECT

 public:
  CQChartsPlotRealControl(QWidget *parent=nullptr);
  CQChartsPlotRealControl(CQChartsPlot *plot, const CQChartsColumn &column);

  void setPlot(CQChartsPlot *p) override {
    CQChartsPlotControlIFace::setPlot(p); init(); }
  void setColumn(const CQChartsColumn &c) override {
    CQChartsPlotControlIFace::setColumn(c); init(); }

  void connectValueChanged(QObject *obj, const char *slotName) override;

  QString filterStr(const QString &) const override;

 signals:
  void valueChanged();

 private:
  void init();

 private:
  CQDoubleRangeSlider* slider_ { nullptr };
};

//---

class CQChartsPlotIntControl : public CQChartsPlotControlIFace {
  Q_OBJECT

 public:
  CQChartsPlotIntControl(QWidget *parent=nullptr);
  CQChartsPlotIntControl(CQChartsPlot *plot, const CQChartsColumn &column);

  void setPlot(CQChartsPlot *p) override {
    CQChartsPlotControlIFace::setPlot(p); init(); }
  void setColumn(const CQChartsColumn &c) override {
    CQChartsPlotControlIFace::setColumn(c); init(); }

  void connectValueChanged(QObject *obj, const char *slotName) override;

  QString filterStr(const QString &) const override;

 signals:
  void valueChanged();

 private:
  void init();

 private:
  CQIntRangeSlider *slider_ { nullptr };
};

//---

class CQChartsPlotTimeControl : public CQChartsPlotControlIFace {
  Q_OBJECT

 public:
  CQChartsPlotTimeControl(QWidget *parent=nullptr);
  CQChartsPlotTimeControl(CQChartsPlot *plot, const CQChartsColumn &column);

  void setPlot(CQChartsPlot *p) override {
    CQChartsPlotControlIFace::setPlot(p); init(); }
  void setColumn(const CQChartsColumn &c) override {
    CQChartsPlotControlIFace::setColumn(c); init(); }

  void connectValueChanged(QObject *obj, const char *slotName) override;

  QString filterStr(const QString &) const override;

 signals:
  void valueChanged();

 private:
  void init();

 private:
  CQTimeRangeSlider *slider_ { nullptr };
};

//---

class CQChartsPlotValueControl : public CQChartsPlotControlIFace {
  Q_OBJECT

 public:
  CQChartsPlotValueControl(QWidget *parent=nullptr);
  CQChartsPlotValueControl(CQChartsPlot *plot, const CQChartsColumn &column);

  void setPlot(CQChartsPlot *p) override {
    CQChartsPlotControlIFace::setPlot(p); init(); }
  void setColumn(const CQChartsColumn &c) override {
    CQChartsPlotControlIFace::setColumn(c); init(); }

  void connectValueChanged(QObject *obj, const char *slotName) override;

  QString filterStr(const QString &cmpStr) const override;

 signals:
  void valueChanged();

 private:
  void init();

 private:
  QComboBox *combo_ { nullptr };
};

#endif
