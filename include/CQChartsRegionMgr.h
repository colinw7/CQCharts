#ifndef CQChartsRegionMgr_H
#define CQChartsRegionMgr_H

#include <CQChartsIconButton.h>
#include <CQChartsGeom.h>

#include <QPointer>
#include <set>

class CQChartsView;

class CQChartsRegionButton : public CQChartsIconButton {
  Q_OBJECT

 public:
  enum class Mode {
    RECT,
    POINT
  };

 public:
  CQChartsRegionButton(QWidget *parent=nullptr);
 ~CQChartsRegionButton();

  void setView(CQChartsView *view);

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &v) { mode_ = v; }

  virtual void setRect (const CQChartsGeom::BBox  &) { }
  virtual void setPoint(const CQChartsGeom::Point &) { }

  void hideEvent(QHideEvent *) override;

 signals:
  void rectRegionSet (const CQChartsGeom::BBox  &bbox);
  void pointRegionSet(const CQChartsGeom::Point &p);

  void hidden();

 private slots:
  void rectRegionSetSlot (const CQChartsGeom::BBox  &bbox);
  void pointRegionSetSlot(const CQChartsGeom::Point &p);

 private:
  CQChartsView *view_ { nullptr };
  Mode          mode_ { Mode::POINT };
};

//---

class CQChartsRegionMgr : public QObject {
  Q_OBJECT

 public:
  CQChartsRegionMgr(CQChartsView *view);
 ~CQChartsRegionMgr();

 private:
  friend class CQChartsRegionButton;

  void addButton   (CQChartsRegionButton *button);
  void removeButton(CQChartsRegionButton *button);

  void buttonChecked(CQChartsRegionButton *button, bool b);

  void activateEdit(QWidget *w);

 private slots:
  void buttonHideSlot();

  void regionSlot(bool);

 private:
  using ButtonP   = QPointer<CQChartsRegionButton>;
  using ButtonSet = std::set<ButtonP>;

  CQChartsView* view_ { nullptr };
  ButtonSet     buttonSet_;
};

#endif
