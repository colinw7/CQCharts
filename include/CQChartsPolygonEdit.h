#ifndef CQChartsPolygonEdit_H
#define CQChartsPolygonEdit_H

#include <CQChartsPolygon.h>
#include <CQChartsLineEditBase.h>

class CQChartsPolygonEdit;

class CQChartsPolygonLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsPolygon polygon READ polygon WRITE setPolygon)

 public:
  CQChartsPolygonLineEdit(QWidget *parent=nullptr);

  const CQChartsPolygon &polygon() const;
  void setPolygon(const CQChartsPolygon &pos);

  void updateMenu();

  void drawPreview(QPainter *painter, const QRect &rect);

 signals:
  void polygonChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updatePolygon(const CQChartsPolygon &polygon, bool updateText);

  void polygonToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsPolygonEdit* dataEdit_ { nullptr };
};

//---

#include <CQChartsEditBase.h>

class CQChartsUnitsEdit;
class CQPoint2DEdit;
class QScrollArea;

class CQChartsPolygonEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsPolygon polygon READ polygon WRITE setPolygon)

 public:
  CQChartsPolygonEdit(QWidget *parent=nullptr);

  const CQChartsPolygon &polygon() const;
  void setPolygon(const CQChartsPolygon &pos);

  QSize sizeHint() const;

  void calcSizes(QSize &scrollSize, QSize &pointsSize, QSize &fullSize) const;

 signals:
  void polygonChanged();

 private slots:
  void widgetsToPolygon();

  void updatePointEdits();

  void updateState();

  void unitsChanged();

  void addSlot();
  void removeSlot();

  void pointSlot();

 private:
  void polygonToWidgets();

 private:
  using PointEdits = std::vector<CQPoint2DEdit *>;

  CQChartsPolygon    polygon_;
  CQChartsUnitsEdit* unitsEdit_    { nullptr };
  QFrame*            controlFrame_ { nullptr };
  QScrollArea*       scrollArea_   { nullptr };
  QFrame*            pointsFrame_  { nullptr };
  PointEdits         pointEdits_;
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsPolygon
class CQChartsPolygonPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsPolygonPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//------

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsPolygon
class CQChartsPolygonPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsPolygonPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
