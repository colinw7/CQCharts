#ifndef CQChartsLineDataEdit_H
#define CQChartsLineDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsLineDataEdit;
class CQChartsPlot;
class CQChartsView;

/*!
 * \brief Line Data line edit
 * \ingroup Charts
 */
class CQChartsLineDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsLineData lineData READ lineData WRITE setLineData)

 public:
  CQChartsLineDataLineEdit(QWidget *parent=nullptr);

  const CQChartsLineData &lineData() const;
  void setLineData(const CQChartsLineData &c);

  void drawPreview(QPainter *painter, const QRect &rect) override;

 signals:
  void lineDataChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateLineData(const CQChartsLineData &c, bool updateText);

  void lineDataToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsLineDataEdit* dataEdit_ { nullptr };
};

//---

#include <CQChartsEditBase.h>

class CQChartsColorLineEdit;
class CQChartsAlphaEdit;
class CQChartsLengthEdit;
class CQChartsLineDashEdit;
class CQChartsLineDataEditPreview;
class CQGroupBox;

/*!
 * \brief Line data edit
 * \ingroup Charts
 */
class CQChartsLineDataEdit : public CQChartsEditBase {
  Q_OBJECT

 public:
  CQChartsLineDataEdit(QWidget *parent=nullptr);

  const CQChartsLineData &data() const { return data_; }
  void setData(const CQChartsLineData &d);

  void setTitle(const QString &title);

  void setPreview(bool b);

 signals:
  void lineDataChanged();

 private:
  void dataToWidgets();

  void connectSlots(bool b);

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                plot_      { nullptr }; //!< parent plot
  CQChartsView*                view_      { nullptr }; //!< parent view
  CQChartsLineData             data_;                  //!< line data
  CQGroupBox*                  groupBox_  { nullptr }; //!< group box
  CQChartsColorLineEdit*       colorEdit_ { nullptr }; //!< color edit
  CQChartsAlphaEdit*           alphaEdit_ { nullptr }; //!< alpha edit
  CQChartsLengthEdit*          widthEdit_ { nullptr }; //!< width edit
  CQChartsLineDashEdit*        dashEdit_  { nullptr }; //!< dash edit
  CQChartsLineDataEditPreview* preview_   { nullptr }; //!< preview widget
  bool                         connected_ { false };   //!< is connected
};

//---

/*!
 * \brief Line data edit preview
 * \ingroup Charts
 */
class CQChartsLineDataEditPreview : public CQChartsEditPreview {
  Q_OBJECT

 public:
  CQChartsLineDataEditPreview(CQChartsLineDataEdit *edit);

  void draw(QPainter *painter) override;

  static void draw(QPainter *painter, const CQChartsLineData &data, const QRect &rect,
                   CQChartsPlot *plot, CQChartsView *view);

 private:
  CQChartsLineDataEdit *edit_ { nullptr };
};

//------

#include <CQChartsPropertyViewEditor.h>

/*!
 * \brief type for CQChartsLineData
 * \ingroup Charts
 */
class CQChartsLineDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   CQChartsPlot *plot, CQChartsView *view) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "line_data"; }
};

//---

/*!
 * \brief editor factory for CQChartsLineData
 * \ingroup Charts
 */
class CQChartsLineDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
