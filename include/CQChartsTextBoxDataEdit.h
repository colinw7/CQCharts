#ifndef CQChartsTextBoxDataEdit_H
#define CQChartsTextBoxDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsTextBoxDataEdit;
class CQChartsPlot;
class CQChartsView;

/*!
 * \brief Text Box Data line edit
 * \ingroup Charts
 */
class CQChartsTextBoxDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsTextBoxData textBoxData READ textBoxData WRITE setTextBoxData)

 public:
  using TextBoxData = CQChartsTextBoxData;

 public:
  CQChartsTextBoxDataLineEdit(QWidget *parent=nullptr);

  const TextBoxData &textBoxData() const;
  void setTextBoxData(const TextBoxData &c);

  void drawPreview(QPainter *painter, const QRect &rect) override;

 Q_SIGNALS:
  void textBoxDataChanged();

 private Q_SLOTS:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateTextBoxData(const TextBoxData &c, bool updateText);

  void textBoxDataToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsTextBoxDataEdit* dataEdit_ { nullptr };
};

//---

#include <CQChartsEditBase.h>

class CQChartsTextDataEdit;
class CQChartsBoxDataEdit;
class CQChartsTextBoxDataEditPreview;

/*!
 * \brief Text Box data edit
 * \ingroup Charts
 */
class CQChartsTextBoxDataEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsTextBoxData data READ data WRITE setData)

 public:
  using Plot        = CQChartsPlot;
  using View        = CQChartsView;
  using TextBoxData = CQChartsTextBoxData;

 public:
  CQChartsTextBoxDataEdit(QWidget *parent=nullptr, bool tabbed=false);

  const TextBoxData &data() const { return data_; }
  void setData(const TextBoxData &d);

  void setPlot(Plot *plot) override;
  void setView(View *view) override;

  void setTitle(const QString &title);

  void setPreview(bool b);

 Q_SIGNALS:
  void textBoxDataChanged();

 private:
  void dataToWidgets();

  void connectSlots(bool b);

 private Q_SLOTS:
  void widgetsToData();

 private:
  bool                            tabbed_   { false };
  TextBoxData                     data_;
  CQChartsTextDataEdit*           textEdit_ { nullptr };
  CQChartsBoxDataEdit*            boxEdit_  { nullptr };
  CQChartsTextBoxDataEditPreview* preview_  { nullptr };
};

//---

/*!
 * \brief Text Box data edit preview
 * \ingroup Charts
 */
class CQChartsTextBoxDataEditPreview : public CQChartsEditPreview {
  Q_OBJECT

 public:
  using TextBoxData = CQChartsTextBoxData;

 public:
  CQChartsTextBoxDataEditPreview(CQChartsTextBoxDataEdit *edit);

  void draw(QPainter *painter) override;

  static void draw(QPainter *painter, const TextBoxData &data, const QRect &rect,
                   Plot *plot, View *view);

 private:
  CQChartsTextBoxDataEdit *edit_ { nullptr };
};

//------

#include <CQChartsPropertyViewEditor.h>

/*!
 * \brief type for CQChartsTextBoxData
 * \ingroup Charts
 */
class CQChartsTextBoxDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  using Plot = CQChartsPlot;
  using View = CQChartsView;

 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   Plot *plot, View *view) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "text_box_data"; }
};

//---

/*!
 * \brief editor factory for CQChartsTextBoxData
 * \ingroup Charts
 */
class CQChartsTextBoxDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

//------

#endif
