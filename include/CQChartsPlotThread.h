#ifndef CQChartsPlotThread_H
#define CQChartsPlotThread_H

#include <CQChartsPlot.h>
#include <CHRTime.h>
#include <future>
#include <memory>

//! Plot Thread Data
class CQChartsPlotThread {
 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsPlotThread(Plot *plot, const char *id) :
   plot_(plot), id_(id) {
  }

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  bool isReady() const { return ! isBusy(); }
  bool isBusy() const { return busy_.load(); }

  template<class Function>
  void exec(Function &&f) {
    start();

    future_ = std::async(std::launch::async, f, plot_);
  }

  void term() {
    (void) future_.get(); // sync

    assert(! future_.valid());

    finish();
  }

  void start() {
    if (debug_ && id_) {
      std::cerr << "Start: " << plot_->id().toStdString() << " : " << id_ << "\n";

      startTime_ = CHRTime::getTime();
    }

    busy_.store(true);
  }

  void end() {
    busy_.store(false);

    if (debug_ && id_) {
      CHRTime dt = startTime_.diffTime();

      std::cerr << "Elapsed: " << plot_->id().toStdString() << " : " << id_ << " " <<
                   dt.getMSecs() << "\n";
    }
  }

  void finish() {
    if (debug_ && id_) {
      CHRTime dt = startTime_.diffTime();

      std::cerr << "Finish: " << plot_->id().toStdString() << " : " << id_ << " " <<
                   dt.getMSecs() << "\n";
    }
  }

 private:
  Plot*             plot_  { nullptr };
  const char *      id_    { nullptr };
  bool              debug_ { false };
  CHRTime           startTime_;
  std::future<void> future_;
  std::atomic<bool> busy_  { false };
};

#endif
