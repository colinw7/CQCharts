#ifndef CQChartsPolygonEdit_H
#define CQChartsPolygonEdit_H

#include <CQChartsPolygon.h>
#include <CQChartsLineEditBase.h>

class CQChartsPolygonEdit;
class CQChartsPolygonEditPointsFrame;

/*!
 * \brief Polygon line edit
 * \ingroup Charts
 */
class CQChartsPolygonLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsPolygon polygon READ polygon WRITE setPolygon)

 public:
  CQChartsPolygonLineEdit(QWidget *parent=nullptr);

  const CQChartsPolygon &polygon() const;
  void setPolygon(const CQChartsPolygon &pos);

  const CQChartsPlot *plot() const;
  void setPlot(CQChartsPlot *plot) override;

  void updateMenu() override;

  void drawPreview(QPainter *painter, const QRect &rect) override;

 signals:
  void polygonChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updatePolygon(const CQChartsPolygon &polygon, bool updateText);

  void polygonToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsPolygonEdit* dataEdit_ { nullptr }; //!< polygon edit
};

//---

#include <CQChartsEditBase.h>

class CQChartsGeomPointEdit;
class CQChartsUnitsEdit;

class QScrollArea;

/*!
 * \brief Polygon edit
 * \ingroup Charts
 */
class CQChartsPolygonEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsPolygon polygon READ polygon WRITE setPolygon)

 public:
  using Units = CQChartsUnits::Type;

 public:
  CQChartsPolygonEdit(QWidget *parent=nullptr);

  const CQChartsPolygon &polygon() const;
  void setPolygon(const CQChartsPolygon &pos);

  const Units &units() const;
  void setUnits(const Units &units);

  const CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *plot) override;

  int numPoints() const;

  QSize sizeHint() const override;

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

  void connectSlots(bool b);

 private:
  using PointEdits = std::vector<CQChartsGeomPointEdit *>;

  CQChartsPolygon                 polygon_;                  //!< polygon
  CQChartsPlot*                   plot_         { nullptr }; //!< associated plot
  CQChartsUnitsEdit*              unitsEdit_    { nullptr }; //!< units edit
  QFrame*                         controlFrame_ { nullptr }; //!< control frame
  QScrollArea*                    scrollArea_   { nullptr }; //!< scroll area
  CQChartsPolygonEditPointsFrame* pointsFrame_  { nullptr }; //!< points frame
  PointEdits                      pointEdits_;               //!< point edits
  bool                            connected_    { false };   //!< is connected
};

//------

/*!
 * \brief Polygon Edit container Widget
 * \ingroup Charts
 */
class CQChartsPolygonEditPointsFrame : public QFrame {
  Q_OBJECT

 public:
  CQChartsPolygonEditPointsFrame(CQChartsPolygonEdit *edit=nullptr);

  void setEdit(CQChartsPolygonEdit *edit) { edit_ = edit; }

  void paintEvent(QPaintEvent *) override;

 private:
  CQChartsPolygonEdit *edit_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsPolygon
 * \ingroup Charts
 */
class CQChartsPolygonPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsPolygonPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "polygon"; }
};

//------

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsPolygon
 * \ingroup Charts
 */
class CQChartsPolygonPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsPolygonPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
