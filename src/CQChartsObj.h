#ifndef CQChartsObj_H
#define CQChartsObj_H

#include <QObject>

class CQChartsObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool selected READ isSelected WRITE setSelected)
  Q_PROPERTY(bool inside   READ isInside   WRITE setInside  )

 public:
  CQChartsObj(QObject *parent=0) : QObject(parent) { }

  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; }

  bool isInside() const { return inside_; }
  void setInside(bool b) { inside_ = b; }

 protected:
  bool selected_ { false }; // is selected
  bool inside_   { false }; // is mouse inside
};

#endif
