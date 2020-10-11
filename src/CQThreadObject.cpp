#include <CQThreadObject.h>

#include <QTimer>

CQThreadManager *
CQThreadManager::
instance()
{
  static CQThreadManager *instance;

  if (! instance)
    instance = new CQThreadManager;

  return instance;
}

CQThreadManager::
CQThreadManager()
{
  timer_ = new QTimer(this);

  connect(timer_, SIGNAL(timeout()), this, SLOT(updateSlot()));
}

CQThreadObject *
CQThreadManager::
createObject()
{
  auto *obj = new CQThreadObject;

  addObject(obj);

  return obj;
}

void
CQThreadManager::
addObject(CQThreadObject *object)
{
  objects_.push_back(object);
}

void
CQThreadManager::
startTimer()
{
  timer_->start(delay());
}

void
CQThreadManager::
updateSlot()
{
  bool update = false;

  for (auto &object : objects_) {
    if (object->isDone())
      update = true;
  }

  if (update)
    emit this->update();
}

//---

CQThreadObject::
CQThreadObject(const char *id) :
 id_(id)
{
}

bool
CQThreadObject::
isReady() const
{
  return ! isBusy();
}

bool
CQThreadObject::
isBusy() const
{
  return busy_.load();
}

void
CQThreadObject::
term()
{
  (void) future_.get(); // sync

  assert(! future_.valid());

  finish();
}

void
CQThreadObject::
start()
{
  assert(! isBusy());

  if (debug_ && id_) {
    std::cerr << "Start: " << id_ << "\n";

    startTime_ = CHRTime::getTime();
  }

  busy_    .store(true);
  finished_.store(false);
}

void
CQThreadObject::
end()
{
  assert(isBusy());

  busy_.store(false);

  if (debug_ && id_) {
    CHRTime dt = startTime_.diffTime();

    std::cerr << "Elapsed: " << id_ << " " << dt.getMSecs() << "\n";
  }
}

// called when not busy first detected
void
CQThreadObject::
finish()
{
  assert(! isBusy());

  finished_.store(true);

  if (debug_ && id_) {
    CHRTime dt = startTime_.diffTime();

    std::cerr << "Finish: " << id_ << " " << dt.getMSecs() << "\n";
  }
}

bool
CQThreadObject::
isFinished() const
{
  return finished_.load();
}

bool
CQThreadObject::
isDone()
{
  if (! isReady())
    return false;

  if (isFinished())
    return false;

  finish();

  return true;
}
