#include <CFireworks.h>
#include <CParticle2D.h>
#include <CConfig.h>
#include <COSRand.h>

CFireworks::
CFireworks()
{
  config_ = new CConfig("CFireworks");

  time_step_ = 0.01;

  injectData_.ticks          = 50;
  injectData_.x_velocity_min = -25;
  injectData_.x_velocity_max =  25;
  injectData_.y_velocity_min = 20;
  injectData_.y_velocity_max = 100;

  explode_ticks_   = 400;
  explosion_ticks_ = 200;

  config_->getValue("time_step"            , "", &time_step_);
  config_->getValue("inject_ticks"         , "", &injectData_.ticks);
  config_->getValue("inject_x_velocity_min", "", &injectData_.x_velocity_min);
  config_->getValue("inject_x_velocity_max", "", &injectData_.x_velocity_max);
  config_->getValue("inject_y_velocity_min", "", &injectData_.y_velocity_min);
  config_->getValue("inject_y_velocity_max", "", &injectData_.y_velocity_max);
  config_->getValue("explode_ticks"        , "", &explode_ticks_);
  config_->getValue("explosion_ticks"      , "", &explosion_ticks_);

  particleSystem_.setGravity(9.8);
}

void
CFireworks::
draw(int w, int h)
{
  w_ = w;
  h_ = h;

  clear(CRGBA(0, 0, 0));

  setForeground(CRGBA(1, 1, 1));

  drawParticles();
}

void
CFireworks::
drawParticles()
{
  updateCurrentParticles();

  updateParticles(/*draw*/true);
}

void
CFireworks::
updateParticles(bool draw)
{
  const auto &particles = currentParticles();

  for (auto *particle : particles) {
    particle->incAge();

    int age = int(particle->getAge());

    if (age == explode_ticks_) {
      particle->setDead();

      explodeParticle(particle);

      continue;
    }

    if (age > explode_ticks_ + explosion_ticks_) {
      particle->setDead();

      continue;
    }

    if (age > explode_ticks_)
      particle->setColor((particle->getColor() - CRGBA(0.005, 0.005, 0.005, 0.0)).clamp());

    if (draw)
      drawParticle(particle);
  }
}

void
CFireworks::
updateCurrentParticles()
{
  currentParticles_.clear();

  const auto &particles = particleSystem_.getParticles();

  for (auto *particle : particles) {
    if (! particle->isDead())
      currentParticles_.push_back(particle);
  }
}

void
CFireworks::
drawParticle(CParticle2D *particle)
{
  auto p = particle->getPosition();

  int x =      int(p.getX());
  int y = h_ - int(p.getY());

  if (y < 0) {
    particle->setDead();
    return;
  }

  setForeground(particle->getColor());

  drawPoint(x + 1, y    );
  drawPoint(x    , y + 1);
  drawPoint(x - 1, y    );
  drawPoint(x    , y - 1);
  drawPoint(x    , y    );
}

void
CFireworks::
explodeParticle(CParticle2D *particle)
{
  CParticle2D *particles[8];

  auto p = particle->getPosition();

  CRGBA rgb(0.5 + COSRand::randIn(0.0, 0.5),
            0.5 + COSRand::randIn(0.0, 0.5),
            0.5 + COSRand::randIn(0.0, 0.5));

  // create explosion particles
  for (uint i = 0; i < 8; ++i) {
    particles[i] = particleSystem_.addParticle();

    particles[i]->setMass    (1);
    particles[i]->setPosition(p);
    particles[i]->setAge     (uint(explode_ticks_));
    particles[i]->setColor   (rgb);
  }

  // set velocity in random directions
  particles[0]->setVelocity( 20 + COSRand::randIn(-2, 2),   0 + COSRand::randIn(-2, 2));
  particles[1]->setVelocity( 15 + COSRand::randIn(-2, 2),  15 + COSRand::randIn(-2, 2));
  particles[2]->setVelocity(  0 + COSRand::randIn(-2, 2),  20 + COSRand::randIn(-2, 2));
  particles[3]->setVelocity(-15 + COSRand::randIn(-2, 2),  15 + COSRand::randIn(-2, 2));
  particles[4]->setVelocity(-20 + COSRand::randIn(-2, 2),   0 + COSRand::randIn(-2, 2));
  particles[5]->setVelocity(-15 + COSRand::randIn(-2, 2), -15 + COSRand::randIn(-2, 2));
  particles[6]->setVelocity(  0 + COSRand::randIn(-2, 2), -20 + COSRand::randIn(-2, 2));
  particles[7]->setVelocity( 15 + COSRand::randIn(-2, 2), -15 + COSRand::randIn(-2, 2));
}

void
CFireworks::
step()
{
  ++tick_count_;

  injectParticle();

  stepParticles();
}

void
CFireworks::
injectParticle()
{
  if (inject_count_ == 0) {
    auto *particle = particleSystem_.addParticle();

    particle->setMass(1);

    particle->setPosition(0.5*w_, 0);

    particle->setVelocity(COSRand::randIn(injectData_.x_velocity_min, injectData_.x_velocity_max),
                          COSRand::randIn(injectData_.y_velocity_min, injectData_.y_velocity_max));

    particle->setColor(CRGBA(0.5+COSRand::randIn(0.0, 0.5),
                             0.5+COSRand::randIn(0.0, 0.5),
                             0.5+COSRand::randIn(0.0, 0.5)));
  }

  ++inject_count_;

  if (inject_count_ > injectData_.ticks)
    inject_count_ = 0;
}

void
CFireworks::
stepParticles()
{
  auto &particles = particleSystem_.getParticles();

  for (auto *particle : particles)
    particle->step(time_step_);
}

size_t
CFireworks::
numParticles() const
{
  return particleSystem_.numParticles();
}

CParticle2D *
CFireworks::
particle(size_t i) const
{
  return particleSystem_.particle(i);
}
