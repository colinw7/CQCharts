#include <CQFileWatcher.h>
#include <QApplication>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QTimer>
#include <iostream>

CQFileWatcher::
CQFileWatcher()
{
  watcher_ = new QFileSystemWatcher(this);

  connect(watcher_, SIGNAL(directoryChanged(const QString &)),
          this, SLOT(dirChangedSlot(const QString &)));
  connect(watcher_, SIGNAL(fileChanged(const QString &)),
          this, SLOT(fileChangedSlot(const QString &)));

  timer_ = new QTimer(this);

  timer_->setSingleShot(true);

  connect(timer_, SIGNAL(timeout()), this, SLOT(timerSlot()));
}

void
CQFileWatcher::
addFile(const QString &filename)
{
  QFileInfo file(filename);

  auto filePath = file.absoluteFilePath();

  auto p = filePaths_.find(filePath);

  if (p == filePaths_.end()) {
  //watcher_->addPath(file.absolutePath());
    watcher_->addPath(filePath);

    filePaths_.insert(filePath);
  }
}

void
CQFileWatcher::
removeFile(const QString &filename)
{
  QFileInfo file(filename);

  auto filePath = file.absoluteFilePath();

  auto p = filePaths_.find(filePath);

  if (p == filePaths_.end()) {
  //watcher_->addPath(file.absolutePath());
    watcher_->removePath(filePath);

    filePaths_.erase(p);
  }
}

void
CQFileWatcher::
dirChangedSlot(const QString &)
{
  //std::cerr << "Dir Changed: " << path.toStdString() << "\n";

  timer_->start(100);

  //emit dirChanged(path);
}

void
CQFileWatcher::
fileChangedSlot(const QString &path)
{
  //std::cerr << "File Changed: " << path.toStdString() << "\n";

  QFileInfo file(path);

  timerFilePath_ = file.absoluteFilePath();

  timer_->start(100);

  //emit fileChanged(path);
}

void
CQFileWatcher::
timerSlot()
{
  QFileInfo file(timerFilePath_);

  if (file.exists()) {
    auto filePath = file.absoluteFilePath();

    watcher_->addPath(filePath);

    emit fileChanged(filePath);
  }
}
