#ifndef CQIconCombo_H
#define CQIconCombo_H

#include <QComboBox>

class CQIconComboModel;

class CQIconCombo : public QComboBox {
  Q_OBJECT

 public:
  CQIconCombo(QWidget *parent=0);

  int iconWidth() const { return iconWidth_; }
  void setIconWidth(int i) { iconWidth_ = i; }

  void setMenuTextWidth(int w);
  int menuTextWidth() const { return textWidth_; }

  void addItem(const QIcon &icon, const QString &str, const QVariant &var=QVariant());
  QVariant itemData(int ind) const;

  QSize sizeHint() const;
  QSize minimumSizeHint() const;

  void paintEvent(QPaintEvent *e);

 private slots:
  void updateTip();

 protected:
  void showPopup();

  void calcMenuTextWidth();

 private:
  CQIconComboModel *model_     { 0 };
  int               iconWidth_ { -1 };
  int               textWidth_ { -1 };
};

#endif
