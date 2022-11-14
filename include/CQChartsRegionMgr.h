#ifndef CQChartsRegionMgr_H
#define CQChartsRegionMgr_H

#include <CQChartsGeom.h>
#include <CQIconButton.h>

#include <QPointer>
#include <set>

class CQChartsView;

class CQChartsRegionButton : public CQIconButton {
  Q_OBJECT

 public:
  enum class Mode {
    RECT,
    POINT
  };

 public:
  using View = CQChartsView;

 public:
  CQChartsRegionButton(QWidget *parent=nullptr);
 ~CQChartsRegionButton();

  void setView(View *view);

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &v) { mode_ = v; }

  virtual void setRect (const CQChartsGeom::BBox  &) { }
  virtual void setPoint(const CQChartsGeom::Point &) { }

  void hideEvent(QHideEvent *) override;

 Q_SIGNALS:
  void rectRegionSet (const CQChartsGeom::BBox  &bbox);
  void pointRegionSet(const CQChartsGeom::Point &p);

  void hidden();

 private Q_SLOTS:
  void rectRegionSetSlot (const CQChartsGeom::BBox  &bbox);
  void pointRegionSetSlot(const CQChartsGeom::Point &p);

 private:
  using ViewP = QPointer<View>;

  ViewP view_;
  Mode  mode_ { Mode::POINT };
};

//---

class CQChartsRegionMgr : public QObject {
  Q_OBJECT

 public:
  using View = CQChartsView;

 public:
  CQChartsRegionMgr(View *view);
 ~CQChartsRegionMgr();

 private:
  friend class CQChartsRegionButton;

  void addButton   (CQChartsRegionButton *button);
  void removeButton(CQChartsRegionButton *button);

  void buttonChecked(CQChartsRegionButton *button, bool b);

  void activateEdit(QWidget *w);

 private Q_SLOTS:
  void buttonHideSlot();

  void regionSlot(bool);

 private:
  using ViewP     = QPointer<View>;
  using ButtonP   = QPointer<CQChartsRegionButton>;
  using ButtonSet = std::set<ButtonP>;

  ViewP     view_;
  ButtonSet buttonSet_;
};

#endif
