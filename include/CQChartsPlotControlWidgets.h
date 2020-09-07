#ifndef CQChartsPlotControlWidgets_H
#define CQChartsPlotControlWidgets_H

#include <CQChartsColumn.h>

#include <QFrame>

class CQChartsPlotControlIFace;
class CQChartsPlot;
class CQChartsView;

class CQDoubleRangeSlider;
class CQIntRangeSlider;
class CQTimeRangeSlider;

class QRadioButton;
class QComboBox;
class QCheckBox;
class QLabel;
class QButtonGroup;
class QGridLayout;

/*!
 * \brief Plot Property Control Container class
 * \ingroup Charts
 */
class CQChartsPlotControlFrame : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool opEqual READ isEqual WRITE setEqual)
  Q_PROPERTY(bool opAnd   READ isAnd   WRITE setAnd  )

 public:
  CQChartsPlotControlFrame(QWidget *parent=nullptr);

  CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *p) { plot_ = p; }

  bool isEqual() const;
  void setEqual(bool b);

  bool isAnd() const;
  void setAnd(bool b);

  QFrame *controlArea() const { return controlArea_; }

  //---

  void clearControls();

  void addIFace(CQChartsPlotControlIFace *iface);

  void addIFaceTerm();

  int numIFaces() const;

  CQChartsPlotControlIFace *iface(int i) const;

 public slots:
  void setPlotControls();

 private slots:
  void controlsChanged();

 private:
  void addPlotControls(CQChartsPlot *plot);

 private:
  using IFaces = std::vector<CQChartsPlotControlIFace *>;

  CQChartsPlot* plot_             { nullptr }; //!< plot
  QCheckBox*    equalCheck_       { nullptr }; //!< equal check
  QCheckBox*    andCheck_         { nullptr }; //!< and check
  QFrame*       controlArea_      { nullptr }; //!< controls area
  QGridLayout*  controlLayout_    { nullptr }; //!< controls layout
  IFaces        ifaces_;                       //!< option interfaces
  QButtonGroup* groupButtonGroup_ { nullptr }; //!< group radio button group
};

//---

/*!
 * \brief Plot Property Control Interface class
 * \ingroup Charts
 */
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

/*!
 * \brief Plot Property Real Control class
 * \ingroup Charts
 */
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

/*!
 * \brief Plot Property Integer Control class
 * \ingroup Charts
 */
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

/*!
 * \brief Plot Property Time Control class
 * \ingroup Charts
 */
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

/*!
 * \brief Plot Property Value Control class
 * \ingroup Charts
 */
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
