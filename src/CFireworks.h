#include <CRGBA.h>
#include <CParticle2D.h>

class CConfig;

class CFireworks {
 public:
  using ParticleList = CParticleSystem2D::ParticleList;

  CFireworks();

  virtual ~CFireworks() { }

  SACCESSOR(InjectXVelMin, int, injectData_, x_velocity_min)
  SACCESSOR(InjectYVelMin, int, injectData_, y_velocity_min)
  SACCESSOR(InjectXVelMax, int, injectData_, x_velocity_max)
  SACCESSOR(InjectYVelMax, int, injectData_, y_velocity_max)

//ACCESSOR(InjectTime    , int, inject_time)
  ACCESSOR(ExplodeTicks  , int, explode_ticks)
  ACCESSOR(ExplosionTicks, int, explosion_ticks)

//int drawCount() const { return draw_count_; }

  void draw(int w, int h);

  void drawParticles();

  void drawParticle(CParticle2D *particle);

  void updateParticles(bool draw=false);

  void explodeParticle(CParticle2D *particle);

  void step();

  void injectParticle();

  void stepParticles();

  size_t numParticles() const;

  CParticle2D *particle(size_t i) const;

  const ParticleList &currentParticles() { return currentParticles_; }

  void updateCurrentParticles();

  //---

  // optional draw interface
  virtual void clear(const CRGBA &) { }

  virtual void setForeground(const CRGBA &) { }

  virtual void drawPoint(int, int) { }

  //---

 private:
  CParticleSystem2D particleSystem_;

  CConfig *config_ { nullptr };

  // simulation time step
  double time_step_ { 0.01 };

  // injection data
  struct InjectData {
    int ticks          { 50 };  // time to next particle
    int x_velocity_min { -25 }; // x velocity min/max
    int x_velocity_max { 25 };
    int y_velocity_min { 20 };  // y velocity min/max
    int y_velocity_max { 100 };
  };

  InjectData injectData_;

  // explode data
  int explode_ticks_   { 400 }; // time to explode
  int explosion_ticks_ { 200 }; // explostion time

  // current state
//int inject_time_  { 0 };
  int tick_count_   { 0 };
  int inject_count_ { 0 };

//int draw_count_   { 0 };

  int w_ { 100 };
  int h_ { 100 };

  ParticleList currentParticles_;
};
