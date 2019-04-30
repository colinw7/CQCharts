#ifndef CQChartsGradientPaletteList_H
#define CQChartsGradientPaletteList_H

#include <QFrame>

class CQChartsTheme;
class CQColorEdit;

class QComboBox;
class QListWidget;
class QListWidgetItem;

class CQChartsGradientPaletteList : public QFrame {
  Q_OBJECT

 public:
  CQChartsGradientPaletteList(QWidget *parent=nullptr);

 public slots:
  void updateThemes();
  void updateLists();
  void updateData();

 private:
  bool getCurrentItem(QListWidgetItem* &item, int &row) const;

  bool getAllItem(QListWidgetItem* &item, int &row) const;

  CQChartsTheme *currentTheme() const;

 signals:
  void palettesChanged();

 private slots:
  void themesComboSlot(int);

  void upSlot   ();
  void downSlot ();
  void leftSlot ();
  void rightSlot();

  void selectColorSlot(const QColor &c);
  void insideColorSlot(const QColor &c);

 private:
  QComboBox*   themesCombo_     { nullptr };
  QListWidget* currentList_     { nullptr };
  QListWidget* allList_         { nullptr };
  CQColorEdit* selectColorEdit_ { nullptr };
  CQColorEdit* insideColorEdit_ { nullptr };
};

#endif
