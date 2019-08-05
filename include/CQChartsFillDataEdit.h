#ifndef CQChartsFillDataEdit_H
#define CQChartsFillDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsFillDataEdit;
class CQChartsPlot;
class CQChartsView;

/*!
 * \brief Fill Data line edit
 * \ingroup Charts
 */
class CQChartsFillDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsFillData fillData READ fillData WRITE setFillData)

 public:
  CQChartsFillDataLineEdit(QWidget *parent=nullptr);

  const CQChartsFillData &fillData() const;
  void setFillData(const CQChartsFillData &c);

  void drawPreview(QPainter *painter, const QRect &rect) override;

 signals:
  void fillDataChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateFillData(const CQChartsFillData &c, bool updateText);

  void fillDataToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsFillDataEdit* dataEdit_ { nullptr };
};

//---

#include <CQChartsEditBase.h>

class CQChartsColorLineEdit;
class CQChartsAlphaEdit;
class CQChartsFillPatternEdit;
class CQChartsFillDataEditPreview;
class CQGroupBox;

/*!
 * \brief Fill data edit
 * \ingroup Charts
 */
class CQChartsFillDataEdit : public CQChartsEditBase {
  Q_OBJECT

 public:
  CQChartsFillDataEdit(QWidget *parent=nullptr);

  const CQChartsFillData &data() const { return data_; }
  void setData(const CQChartsFillData &d);

  void setTitle(const QString &title);

  void setPreview(bool b);

 signals:
  void fillDataChanged();

 private:
  void dataToWidgets();

  void connectSlots(bool b);

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                plot_        { nullptr }; //!< parent plot
  CQChartsView*                view_        { nullptr }; //!< parent view
  CQChartsFillData             data_;                    //!< fill data
  CQGroupBox*                  groupBox_    { nullptr }; //!< group box
  CQChartsColorLineEdit*       colorEdit_   { nullptr }; //!< color edit
  CQChartsAlphaEdit*           alphaEdit_   { nullptr }; //!< alpha edit
  CQChartsFillPatternEdit*     patternEdit_ { nullptr }; //!< pattern edit
  CQChartsFillDataEditPreview* preview_     { nullptr }; //!< preview widget
  bool                         connected_   { false };   //!< is connected
};

//---

/*!
 * \brief Fill data edit preview
 * \ingroup Charts
 */
class CQChartsFillDataEditPreview : public CQChartsEditPreview {
  Q_OBJECT

 public:
  CQChartsFillDataEditPreview(CQChartsFillDataEdit *edit);

  void draw(QPainter *painter) override;

  static void draw(QPainter *painter, const CQChartsFillData &data, const QRect &rect,
                   CQChartsPlot *plot, CQChartsView *view);

 private:
  CQChartsFillDataEdit *edit_ { nullptr };
};

//------

#include <CQChartsPropertyViewEditor.h>

/*!
 * \brief type for CQChartsFillData
 * \ingroup Charts
 */
class CQChartsFillDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   CQChartsPlot *plot, CQChartsView *view) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "fill_data"; }
};

//---

/*!
 * \brief editor factory for CQChartsFillData
 * \ingroup Charts
 */
class CQChartsFillDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

//------

#endif
