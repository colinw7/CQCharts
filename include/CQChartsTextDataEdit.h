#ifndef CQChartsTextDataEdit_H
#define CQChartsTextDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsTextDataEdit;
class CQChartsPlot;
class CQChartsView;

/*!
 * \brief line edit like widget for text data
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
  CQChartsTextDataEdit* dataEdit_ { nullptr }; //! text data edit widget
};

//---

#include <CQChartsEditBase.h>

class CQChartsColorLineEdit;
class CQChartsAlphaEdit;
class CQChartsTextDataEditPreview;
class CQFontEdit;
class CQAngleSpinBox;
class CQAlignEdit;
class CQGroupBox;
class CQCheckBox;

/*!
 * \brief widget to edit text data values
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

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                plot_          { nullptr };
  CQChartsView*                view_          { nullptr };
  CQChartsTextData             data_;
  CQGroupBox*                  groupBox_      { nullptr };
  CQFontEdit*                  fontEdit_      { nullptr };
  CQChartsColorLineEdit*       colorEdit_     { nullptr };
  CQChartsAlphaEdit*           alphaEdit_     { nullptr };
  CQAngleSpinBox*              angleEdit_     { nullptr };
  CQCheckBox*                  contrastEdit_  { nullptr };
  CQAlignEdit*                 alignEdit_     { nullptr };
  CQCheckBox*                  formattedEdit_ { nullptr };
  CQCheckBox*                  scaledEdit_    { nullptr };
  CQCheckBox*                  htmlEdit_      { nullptr };
  CQChartsTextDataEditPreview* preview_       { nullptr };
};

//---

/*!
 * \brief preview widget for text data
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
 */
class CQChartsTextDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   CQChartsPlot *plot, CQChartsView *view) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsTextData
 */
class CQChartsTextDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
