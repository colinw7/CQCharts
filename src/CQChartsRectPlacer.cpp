#include <CQChartsRectPlacer.h>
#include <cmath>
#include <iostream>

CQChartsRectPlacer::
CQChartsRectPlacer()
{
}

void
CQChartsRectPlacer::
addRect(RectData *rectData)
{
  rectDatas_.push_back(rectData);
}

void
CQChartsRectPlacer::
addRectValues(RectData *rectData)
{
  grid_.addRectValues(rectData->rect());
}

void
CQChartsRectPlacer::
place()
{
  int n = 0;

  while (n < 32) {
    stepPlace();

    if (! moveAllOut())
      break;

    ++n;
  }
}

bool
CQChartsRectPlacer::
stepPlace()
{
  struct MoveRect {
    RectData *rectData { nullptr };
    Rect      rect;

    MoveRect(RectData *rectData, const Rect &rect) :
     rectData(rectData), rect(rect) {
    }
  };

  using MoveRects = std::vector<MoveRect>;

  //---

  int n = 0;

  while (n < 32) {
    // add all rects to grid
    clearGrid();

    for (const auto &rectData : rectDatas_) {
      addRectValues(rectData);

      grid_.addRect(rectData);
    }

    MoveRects moveRects;

    // move each rect to empty space
    for (const auto &rectData : rectDatas_) {
      if (isDebug())
        std::cerr << "Place " << rectData->name() << "\n";

      //---

      // get touching (hit) rects
      Rect dirRect = rectData->rect();

      RectDatas hitRects;

      getTouchingRects(rectData, dirRect, hitRects);

      if (hitRects.empty())
        continue;

      //---

      // get range of hit rects
      HitRect hitRect;

      calcHitRectRange(hitRects, hitRect, /*min*/true);

      //---

      // generate list of rects for each side
      Rects dirRects;

      addDirRects(dirRect, dirRects, hitRect);

      //---

      // check rects for empty (miss) and hit rects
      Rects     emptyRects;
      RectDatas hitRects1;

      getEmptyRects(rectData, dirRects, emptyRects, hitRects1);

      if (emptyRects.empty()) { // no empty rects so skip
        if (isDebug())
          std::cerr << "Placement failed " << rectData->name() << "\n";
        continue;
      }

      //---

      // check rects in empty space for best match
      Rect erect;

      getBestDirRect(rectData, emptyRects, erect);

      if (isDebug())
        std::cerr << " MOVE TO: " << erect << "\n";

      moveRects.push_back(MoveRect(rectData, erect));
    }

    if (moveRects.empty())
      break;

    // get best move rect
    const MoveRect *moveRect = nullptr;
    double          moveDist = -1;

    for (const auto &moveRect1 : moveRects) {
      const Rect &rect1 = moveRect1.rectData->rect();
      const Rect &rect2 = moveRect1.rect;

      double moveDist1 = std::hypot(rect1.xmid() - rect2.xmid(), rect1.ymid() - rect2.ymid());

      if (! moveRect || moveDist1 < moveDist) {
        moveDist = moveDist1;
        moveRect = &moveRect1;
      }
    }

    // move best rect
    moveRect->rectData->setRect(moveRect->rect);

    //---

    ++n;
  }

  return true;
}

void
CQChartsRectPlacer::
getEmptyRects(RectData *rectData, const Rects &dirRects, Rects &emptyRects, RectDatas &hitRects)
{
  for (const auto &dirRect1 : dirRects) {
    RectDatas hitRects1;

    getTouchingRects(rectData, dirRect1, hitRects1);

    RectDatas hitRects2;

    for (const auto &hitData1 : hitRects1) {
      if (hitData1 == rectData)
        continue;

      hitRects2.push_back(hitData1);
    }

    int nh = hitRects2.size();

    if (nh == 0) {
      if (isDebug())
        std::cerr << " MISS: " << dirRect1 << "\n";

      emptyRects.push_back(dirRect1);
    }
    else {
      if (isDebug())
        std::cerr << " HIT : " << dirRect1 << " (#" << nh << ")\n";

      for (const auto &hitRect2 : hitRects2)
        hitRects.push_back(hitRect2);
    }
  }
}

void
CQChartsRectPlacer::
getTouchingRects(RectData *rectData, const Rect &dirRect, RectDatas &hitRects)
{
  RectDatas hitRects1;

  grid_.getTouchingRects(dirRect, hitRects1);

  for (const auto &hitRect1 : hitRects1) {
    if (hitRect1 == rectData)
      continue;

    hitRects.push_back(hitRect1);
  }
}

void
CQChartsRectPlacer::
getBestDirRect(RectData *rectData, const Rects &dirRects, Rect &erect)
{
  erect = Rect();

  double d = -1;

  for (const auto &erect1 : dirRects) {
    double d1 = std::hypot(rectData->rect().xmid() - erect1.xmid(),
                           rectData->rect().ymid() - erect1.ymid());

    if (d < 0 || d1 < d) {
      d     = d1;
      erect = erect1;
    }
  }
}

void
CQChartsRectPlacer::
addDirRects(const Rect &dirRect, Rects &dirRects, const HitRect &hitRect)
{
  auto addRect = [&](const Rect &dirRect) {
    if (clipRect_.isSet() && ! clipRect_.inside(dirRect))
       return false;

    dirRects.push_back(dirRect);

    return true;
  };

  double hitXMin = hitRect.xmin;
  double hitYMin = hitRect.ymin;
  double hitXMax = hitRect.xmax;
  double hitYMax = hitRect.ymax;

  auto lrect = grid_.lrect(dirRect, hitXMin);
  addRect(lrect);

  auto rrect = grid_.rrect(dirRect, hitXMax);
  addRect(rrect);

  auto brect = grid_.brect(dirRect, hitYMin);
  addRect(brect);

  auto trect = grid_.trect(dirRect, hitYMax);
  addRect(trect);

  //---

  auto tlrect = grid_.tlrect(dirRect, hitXMin, hitYMax);
  addRect(tlrect);

  auto trrect = grid_.trrect(dirRect, hitXMax, hitYMax);
  addRect(trrect);

  auto blrect = grid_.blrect(dirRect, hitXMin, hitYMin);
  addRect(blrect);

  auto brrect = grid_.brrect(dirRect, hitXMax, hitYMin);
  addRect(brrect);
}

void
CQChartsRectPlacer::
calcHitRectRange(const RectDatas &hitRects, HitRect &hitRect, bool min, double tol)
{
  hitRect.xmin = hitRects[0]->rect().xmin();
  hitRect.ymin = hitRects[0]->rect().ymin();
  hitRect.xmax = hitRects[0]->rect().xmax();
  hitRect.ymax = hitRects[0]->rect().ymax();

  int nh = hitRects.size();

  for (int ih = 1; ih < nh; ++ih) {
    if (min) {
      hitRect.xmin = std::max(hitRect.xmin, hitRects[ih]->rect().xmin());
      hitRect.ymin = std::max(hitRect.ymin, hitRects[ih]->rect().ymin());
      hitRect.xmax = std::min(hitRect.xmax, hitRects[ih]->rect().xmax());
      hitRect.ymax = std::min(hitRect.ymax, hitRects[ih]->rect().ymax());
    }
    else {
      hitRect.xmin = std::min(hitRect.xmin, hitRects[ih]->rect().xmin());
      hitRect.ymin = std::min(hitRect.ymin, hitRects[ih]->rect().ymin());
      hitRect.xmax = std::max(hitRect.xmax, hitRects[ih]->rect().xmax());
      hitRect.ymax = std::max(hitRect.ymax, hitRects[ih]->rect().ymax());
    }
  }

  if (! min) {
    hitRect.xmin -= tol;
    hitRect.ymin -= tol;
    hitRect.xmax += tol;
    hitRect.ymax += tol;
  }
}

bool
CQChartsRectPlacer::
moveAllOut()
{
  bool moved = false;

  for (auto &rectData : rectDatas_) {
    if (moveOut(rectData))
      moved = true;
  }

  return moved;
}

bool
CQChartsRectPlacer::
moveOut(RectData *rectData)
{
  grid_.clear();

  for (auto &rectData1 : rectDatas_)
    grid_.addRect(rectData1);

  //---

  if (! rectData) {
    RectData *moveRectData = nullptr;
    int       numMove      = 0;

    for (auto &rectData1 : rectDatas_) {
      Rect dirRect1 = rectData1->rect();

      RectDatas hitRects;

      getTouchingRects(rectData1, dirRect1, hitRects);

      int numMove1 = hitRects.size();

      if (! numMove1)
        continue;

      if (! moveRectData || numMove1 > numMove) {
        moveRectData = rectData1;
        numMove      = numMove1;
      }
    }

    rectData = moveRectData;
  }

  if (! rectData)
    return false;

  //---

  if (isDebug())
    std::cerr << "Move Out " << rectData->name() << "\n";

  //---

  Rect dirRect = rectData->rect();

  RectDatas hitRects;

  getTouchingRects(rectData, dirRect, hitRects);

  if (hitRects.empty())
    return false;

  //---

  double xmin = rectData->rect().xmin();
  double ymin = rectData->rect().ymin();
  double xmax = rectData->rect().xmax();
  double ymax = rectData->rect().ymax();

  double w = rectData->rect().width ();
  double h = rectData->rect().height();

  HitRect hitRect;

  calcHitRectRange(hitRects, hitRect, /*min*/false, grid_.tol());

  //--

  // create dir rects (outside overlap region)
  Rects dirRects;

  auto addRect = [&](const Rect &rect) {
    if (clipRect_.isSet() && ! clipRect_.inside(rect))
      return false;

    dirRects.push_back(rect);

    return true;
  };

  addRect(Rect(hitRect.xmin - w, ymin, hitRect.xmin    , ymax));
  addRect(Rect(hitRect.xmax    , ymin, hitRect.xmax + w, ymax));
  addRect(Rect(xmin, hitRect.ymin - h, xmax, hitRect.ymin    ));
  addRect(Rect(xmin, hitRect.ymax    , xmax, hitRect.ymax + h));

  // get best dir rect (least move distance)
  Rect bestRect;

  getBestDirRect(rectData, dirRects, bestRect);

  // move best rect
  rectData->setRect(bestRect);

  return true;
}
