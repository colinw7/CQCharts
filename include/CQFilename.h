#ifndef CQFilename_H
#define CQFilename_H

#include <QFrame>

class QLineEdit;
class QToolButton;

class CQFilename : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool    save    READ isSave  WRITE setSave   )
  Q_PROPERTY(QString pattern READ pattern WRITE setPattern)

 public:
  CQFilename(QWidget *parent=0);

  bool isSave() const { return save_; }
  void setSave(bool b) { save_ = b; }

  const QString &pattern() const { return pattern_; }
  void setPattern(const QString &v) { pattern_ = v; }

  QString name() const;
  void setName(const QString &name);

 signals:
  void filenameChanged(const QString &filename);

 private slots:
  void acceptSlot();
  void fileSlot();

 private:
  bool         save_    { false };
  QString      pattern_;
  QLineEdit   *edit_    { nullptr };
  QToolButton *button_  { nullptr };
};

#endif
