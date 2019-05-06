#ifndef CQChartsGradientPaletteList_H
#define CQChartsGradientPaletteList_H

#include <QFrame>

class CQChartsTheme;
class CQColorEdit;

class QComboBox;
class QListWidget;
class QListWidgetItem;

/*!
 * \brief Widget to control themes and associated lists of palettes
 */
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

  void upTopSlot();
  void upSlot   (bool top=false);
  void downSlot ();
  void leftSlot ();
  void rightSlot();

  void selectColorSlot(const QColor &c);
  void insideColorSlot(const QColor &c);

 private:
  QComboBox*   themesCombo_     { nullptr }; //!< theme combo box
  QListWidget* currentList_     { nullptr }; //!< theme current palettes list
  QListWidget* allList_         { nullptr }; //!< theme unused palettes list
  CQColorEdit* selectColorEdit_ { nullptr }; //!< theme select color edit
  CQColorEdit* insideColorEdit_ { nullptr }; //!< theme inside color edit
};

#endif
