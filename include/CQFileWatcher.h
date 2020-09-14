#ifndef CQFileWatcher_H
#define CQFileWatcher_H

#include <QObject>
#include <set>

class QFileSystemWatcher;
class QTimer;

class CQFileWatcher : public QObject {
  Q_OBJECT

 public:
  CQFileWatcher();

  void addFile(const QString &filename);
  void removeFile(const QString &filename);

 signals:
  void dirChanged(const QString &);
  void fileChanged(const QString &);

 private slots:
  void dirChangedSlot(const QString &);
  void fileChangedSlot(const QString &);

  void timerSlot();

 private:
  using FilePaths = std::set<QString>;

  FilePaths           filePaths_;
  QFileSystemWatcher* watcher_ { nullptr };
  QTimer*             timer_   { nullptr };
  mutable QString     timerFilePath_;
};

#endif
