#ifndef CQChartsTest_H
#define CQChartsTest_H

#include <QFrame>

class CQChartsTable;
class CQChartsTree;
class CQGradientPalette;
class CQGradientPaletteControl;

class QAbstractItemModel;
class QStackedWidget;
class QTabWidget;
class QGroupBox;
class QLineEdit;
class QCheckBox;
class QPushButton;
class QGridLayout;

class CQChartsTest : public QFrame {
  Q_OBJECT

 public:
  CQChartsTest();

  void loadCsv (const QString &filename);
  void loadJson(const QString &filename);
  void loadData(const QString &filename);

 private:
  void addPieTab     (QTabWidget *plotTab);
  void addXYTab      (QTabWidget *plotTab);
  void addSunburstTab(QTabWidget *plotTab);
  void addBarChartTab(QTabWidget *plotTab);

  void setTableModel(QAbstractItemModel *model);
  void setTreeModel (QAbstractItemModel *model);

  QLineEdit *addLineEdit(QGridLayout *grid, int &row, const QString &name) const;

  bool lineEditValue(QLineEdit *le, int &i, int defi=0) const;

 private slots:
  void tableColumnClicked(int column);
  void typeOKSlot();

  void pieOKSlot();
  void xyOKSlot();
  void sunburstOKSlot();
  void barChartOKSlot();

 private:
  QAbstractItemModel*       model_             { nullptr };
  QLineEdit*                pieLabelEdit_      { nullptr };
  QLineEdit*                pieDataEdit_       { nullptr };
  QPushButton*              pieOKButton_       { nullptr };
  QLineEdit*                xyXEdit_           { nullptr };
  QLineEdit*                xyYEdit_           { nullptr };
  QCheckBox*                xyBivariateCheck_  { nullptr };
  QPushButton*              xyOKButton_        { nullptr };
  QLineEdit*                sunburstNameEdit_  { nullptr };
  QLineEdit*                sunburstValueEdit_ { nullptr };
  QPushButton*              sunburstOKButton_  { nullptr };
  QLineEdit*                barChartNameEdit_  { nullptr };
  QLineEdit*                barChartValueEdit_ { nullptr };
  QPushButton*              barChartOKButton_  { nullptr };
  QLineEdit*                columnTypeEdit_    { nullptr };
  QStackedWidget*           stack_             { nullptr };
  CQChartsTable*            table_             { nullptr };
  CQChartsTree*             tree_              { nullptr };
  CQGradientPalette*        palettePlot_       { nullptr };
  CQGradientPaletteControl* paletteControl_    { nullptr };
  QGroupBox*                typeGroup_         { nullptr };
  int                       tableColumn_       { 0 };
};

#endif
