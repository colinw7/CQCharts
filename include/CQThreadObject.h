#ifndef CQThreadObject_H
#define CQThreadObject_H

#include <QObject>

#include <CHRTime.h>
#include <future>
#include <memory>

class CQThreadObject;

class QTimer;

#define CQThreadManagerInst CQThreadManager::instance()

class CQThreadManager : public QObject {
  Q_OBJECT

 public:
  static CQThreadManager *instance();

  int delay() const { return delay_; }
  void setDelay(int t) { delay_ = t; }

  CQThreadObject *createObject();

  void addObject(CQThreadObject *object);

  void startTimer();

 signals:
  void update();

 private:
  CQThreadManager();

 private slots:
  void updateSlot();

 private:
  using Objects = std::vector<CQThreadObject *>;

  QTimer *timer_ { nullptr };
  int     delay_ { 100 };
  Objects objects_;
};

//---

/*
 * Thread Object
 *
 * States:
 *  Busy     : started
 *  Ready    : ended (not handled)
 *  Finished : ended (handled)
 */
class CQThreadObject {
 public:
  //! create object with optional debug id
  CQThreadObject(const char *id=nullptr);

  //! get/set debug
  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  //! thread is ready (finished calculating result)
  bool isReady() const;

  //! thread is busy (calculating result)
  bool isBusy() const;

  //! thread is ready and has been marked as finished (ready detected)
  bool isFinished() const;

  //! exec thread (set started state and starts thread using async)
  template<class Function, class... Args>
  void exec(Function &&f, Args&&... args) {
    start();

    future_ = std::async(std::launch::async, f, args...);
  }

  //! mark thread ended (should be called in thread when calculation done)
  void end();

  //! wait and finish thread
  void term();

  //! set thread to finished state (called when called detects ready and handles it)
  void finish();

  //! finish thread and return true if it transitioned to finished state
  bool isDone();

 private:
  //! mark thread started (called before actual thread started)
  void start();

 private:
  const char *      id_       { nullptr }; //!< debug id
  bool              debug_    { false };   //!< is debug
  CHRTime           startTime_;            //!< thread start time
  std::future<void> future_;               //!< future for result
  std::atomic<bool> busy_     { false };   //!< is busy
  std::atomic<bool> finished_ { false };   //!< is finished
};

#endif
