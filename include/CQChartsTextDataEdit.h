#ifndef CQChartsTextDataEdit_H
#define CQChartsTextDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsTextDataEdit;
class CQChartsPlot;
class CQChartsView;

/*!
 * \brief Text Data line edit
 * \ingroup Charts
 */
class CQChartsTextDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsTextData textData READ textData WRITE setTextData)

 public:
  CQChartsTextDataLineEdit(QWidget *parent=nullptr);

  const CQChartsTextData &textData() const;
  void setTextData(const CQChartsTextData &c);

  void setNoFocus();

  void drawPreview(QPainter *painter, const QRect &rect) override;

 signals:
  void textDataChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateTextData(const CQChartsTextData &c, bool updateText);

  void textDataToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsTextDataEdit* dataEdit_ { nullptr }; //!< text data edit widget
};

//---

#include <CQChartsEditBase.h>

class CQChartsColorLineEdit;
class CQChartsAlphaEdit;
class CQChartsTextDataEditPreview;
class CQChartsFontLineEdit;
class CQAngleSpinBox;
class CQAlignEdit;
class CQGroupBox;
class CQCheckBox;

/*!
 * \brief Text data edit
 * \ingroup Charts
 */
class CQChartsTextDataEdit : public CQChartsEditBase {
  Q_OBJECT

 public:
  CQChartsTextDataEdit(QWidget *parent=nullptr, bool optional=true);

  const CQChartsTextData &data() const { return data_; }
  void setData(const CQChartsTextData &d);

  void setTitle(const QString &title);

  void setPreview(bool b);

  void setNoFocus();

 signals:
  void textDataChanged();

 private:
  void dataToWidgets();

  void connectSlots(bool b);

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                plot_          { nullptr }; //!< parent plot
  CQChartsView*                view_          { nullptr }; //!< parent view
  CQChartsTextData             data_;                      //!< text data
  CQGroupBox*                  groupBox_      { nullptr }; //!< group box
  CQChartsFontLineEdit*        fontEdit_      { nullptr }; //!< font edit
  CQChartsColorLineEdit*       colorEdit_     { nullptr }; //!< color edit
  CQChartsAlphaEdit*           alphaEdit_     { nullptr }; //!< alpha edit
  CQAngleSpinBox*              angleEdit_     { nullptr }; //!< angle edit
  CQCheckBox*                  contrastEdit_  { nullptr }; //!< contrast edit
  CQAlignEdit*                 alignEdit_     { nullptr }; //!< align edit
  CQCheckBox*                  formattedEdit_ { nullptr }; //!< formatted edit
  CQCheckBox*                  scaledEdit_    { nullptr }; //!< scaled edit
  CQCheckBox*                  htmlEdit_      { nullptr }; //!< html edit
  CQChartsTextDataEditPreview* preview_       { nullptr }; //!< preview
  bool                         connected_     { false };   //!< is connected
};

//---

/*!
 * \brief Text data edit preview
 * \ingroup Charts
 */
class CQChartsTextDataEditPreview : public CQChartsEditPreview {
  Q_OBJECT

 public:
  CQChartsTextDataEditPreview(CQChartsTextDataEdit *edit);

  void draw(QPainter *painter) override;

  static void draw(QPainter *painter, const CQChartsTextData &data, const QRect &rect,
                   CQChartsPlot *plot, CQChartsView *view);

 private:
  CQChartsTextDataEdit *edit_ { nullptr };
};

//------

#include <CQChartsPropertyViewEditor.h>

/*!
 * \brief type for CQChartsTextData
 * \ingroup Charts
 */
class CQChartsTextDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   CQChartsPlot *plot, CQChartsView *view) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "text_data"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsTextData
 * \ingroup Charts
 */
class CQChartsTextDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
