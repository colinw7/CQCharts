#ifndef CQChartsModelDetailsTable_H
#define CQChartsModelDetailsTable_H

#include <QFrame>

class CQCharts;
class CQChartsModelData;
class CQTableWidget;

class CQChartsModelDetailsTable : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool flip       READ isFlip       WRITE setFlip)
  Q_PROPERTY(bool autoUpdate READ isAutoUpdate WRITE setAutoUpdate)

 public:
  CQChartsModelDetailsTable(CQChartsModelData *modelData=nullptr);

  void setModelData(CQChartsModelData *modelData);

  bool isFlip() const { return flip_; }
  void setFlip(bool b);

  bool isAutoUpdate() const { return autoUpdate_; }
  void setAutoUpdate(bool b) { autoUpdate_ = b; }

  void checkedUpdate(bool force=false);

 private:
  void update();

  void contextMenuEvent(QContextMenuEvent *e) override;

 private slots:
  void modelChangedSlot();
  void modelTypeChangedSlot(int);

  void flipSlot(bool b);

 private:
  CQCharts*          charts_     { nullptr };
  CQChartsModelData* modelData_  { nullptr };
  CQTableWidget*     table_      { nullptr };
  bool               flip_       { false };
  bool               autoUpdate_ { true };
  bool               valid_      { false };
};

#endif
