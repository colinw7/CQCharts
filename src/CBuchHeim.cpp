#include <CBuchHeim.h>
#include <algorithm>

namespace CBuchHeim {

Tree::
Tree(const std::string &name) :
 name_(name)
{
}

Tree::
~Tree()
{
}

Tree *
Tree::
addChild(TreeP tree)
{
  tree->parent_ = this;

  children_.push_back(tree);

  return this;
}

//---

DrawTree::
DrawTree(Tree *tree, DrawTree *parent, int depth, int number)
{
  x_ = -1.0;
  y_ = depth;

  tree_ = tree;

  for (int i = 0; i < tree->numChildren(); ++i) {
    auto *subtree = tree->child(i);

    auto child = std::make_shared<DrawTree>(const_cast<Tree *>(subtree), this, depth + 1, i + 1);

    addChild(child);
  }

  parent_   = parent;
  thread_   = nullptr;
  offset_   = 0;
  ancestor_ = this;
  change_   = 0;
  shift_    = 0;

  lmost_sibling_ = nullptr;

  // this is the number of the node in its group of siblings 1..n
  number_ = number;
}

DrawTree::
~DrawTree()
{
}

DrawTree *
DrawTree::
addChild(DrawTreeP tree)
{
  tree->parent_ = this;

  children_.push_back(tree);

  return this;
}

//---

DrawTree *
DrawTree::
left()
{
  if (thread_) return thread_;

  if (! children().empty())
    return child(0);

  return nullptr;
}

DrawTree *
DrawTree::
right()
{
  if (thread_) return thread_;

  if (! children().empty())
    return child(numChildren() - 1);

  return nullptr;
}

DrawTree *
DrawTree::
left_brother()
{
  DrawTree *n = nullptr;

  if (parent_) {
    for (const auto &node : parent_->children()) {
      if (node.get() == this)
        return n;

      n = node.get();
    }
  }

  return n;
}

DrawTree *
DrawTree::
get_lmost_sibling()
{
  if (! lmost_sibling_ && parent_ && this != parent_->child(0))
    lmost_sibling_ = parent_->child(0);

  return lmost_sibling_;
}

void
DrawTree::
place()
{
  auto *dt = first_walk(this);

  auto min = second_walk(dt);

  if (min < 0)
    third_walk(dt, -min);
}

DrawTree *
DrawTree::
first_walk(DrawTree *v, double distance)
{
  if (v->children().empty()) {
    if (v->get_lmost_sibling())
      v->x_ = v->left_brother()->x() + distance;
    else
      v->x_ = 0.0;
  }
  else {
    auto *default_ancestor = v->child(0);

    for (const auto &w : v->children()) {
      first_walk(w.get());

      default_ancestor = apportion(w.get(), default_ancestor, distance);
    }

    //print "finished v =", v->tree, "children";
    execute_shifts(v);

    auto midpoint = (v->child(0)->x() + v->child(v->numChildren() - 1)->x())/2.0;

    //auto *ell = v->child(0);
    //auto *arr = v->child(v->numChildren() - 1);

    auto *w = v->left_brother();

    if (w) {
      v->x_      = w->x() + distance;
      v->offset_ = v->x() - midpoint;
    }
    else {
      v->x_ = midpoint;
    }
  }

  return v;
}

DrawTree *
DrawTree::
apportion(DrawTree *v, DrawTree *default_ancestor, double distance)
{
  auto *w = v->left_brother();
  if (w) {
    // in buchheim notation:
    //   i == inner; o == outer; r == right; l == left; r = +; l = -
    auto *vir = v;
    auto *vor = v;
    auto *vil = w;
    auto *vol = v->get_lmost_sibling();
    auto  sir = v->offset_;
    auto  sor = v->offset_;
    auto  sil = vil->offset_;
    auto  sol = vol->offset_;

    while (vil->right() && vir->left()) {
      vil = vil->right();
      vir = vir->left();
      vol = vol->left();
      vor = vor->right();

      vor->ancestor_ = v;

      double shift = (vil->x() + sil) - (vir->x() + sir) + distance;

      if (shift > 0) {
        auto *a = ancestor(vil, v, default_ancestor);
        move_subtree(a, v, shift);
        sir = sir + shift;
        sor = sor + shift;
      }

      sil += vil->offset_;
      sir += vir->offset_;
      sol += vol->offset_;
      sor += vor->offset_;
    }

    if (vil->right() && ! vor->right()) {
      vor->thread_ = vil->right();
      vor->offset_ += sil - sor;
    }
    else {
      if (vir->left() && ! vol->left()) {
        vol->thread_ = vir->left();
        vol->offset_ += sir - sol;
      }

      default_ancestor = v;
    }
  }

  return default_ancestor;
}

void
DrawTree::
move_subtree(DrawTree *wl, DrawTree *wr, int shift)
{
  int subtrees = wr->number_ - wl->number_;

  //print wl->tree, "is conflicted with", wr->tree, 'moving', subtrees, 'shift', shift
  // print wl, wr, wr->number, wl->number, shift, subtrees, shift/subtrees

  wr->change_ -= shift/subtrees;
  wr->shift_  += shift;
  wl->change_ += shift/subtrees;
  wr->x_      += shift;
  wr->offset_ += shift;
}

void
DrawTree::
execute_shifts(DrawTree *v)
{
  int shift  = 0;
  int change = 0;

  for (auto pc = v->children().rbegin(); pc != v->children().rend(); ++pc) {
    const auto &w = *pc;

    //print "shift:", w->name, shift, w->change_
    w->x_      += shift;
    w->offset_ += shift;
    change     += w->change_;
    shift      += w->shift_ + change;
  }
}

DrawTree *
DrawTree::
ancestor(DrawTree *vil, DrawTree *v, DrawTree *default_ancestor)
{
  // the relevant text is at the bottom of page 7 of
  // Improving Walker's Algorithm to Run in Linear Time" by Buchheim et al, (2002)
  // http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.16.8757&rep=rep1&type=pdf
  for (const auto &child : v->parent_->children()) {
    if (child.get() == vil->ancestor_)
      return vil->ancestor_;
  }

#if 0
  if (std::find(v->parent_->children().begin(), v->parent_->children().end(), vil->ancestor_) !=
      v->parent_->children().end()) {
    return vil->ancestor_;
  }
#endif

  return default_ancestor;
}

double
DrawTree::
second_walk(DrawTree *v, int m, int depth, double min)
{
  v->x_ += m;
  v->y_ = depth;

  if (min < -9000 || v->x() < min) {
    min = v->x();
  }

  for (const auto &w : v->children()) {
    min = second_walk(w.get(), m + v->offset_, depth + 1, min);
  }

  return min;
}

void
DrawTree::
third_walk(DrawTree *tree, int n)
{
  tree->x_ += n;

  for (const auto &c : tree->children()) {
    third_walk(c.get(), n);
  }
}

void
DrawTree::
normalize(bool equalScale)
{
  double xmin = 9999.0, ymin = 9999.0, xmax = -9999.0, ymax = -9999.0;

  sizeTree(this, 0, xmin, ymin, xmax, ymax);

  if (equalScale) {
    auto xmid = (xmin + xmax)/2.0;
    auto ymid = (ymin + ymax)/2.0;

    auto s = std::max(xmax - xmin, ymax - ymin);

    xmin = xmid - s/2.0;
    xmax = xmid + s/2.0;
    ymin = ymid - s/2.0;
    ymax = ymid + s/2.0;
  }

  moveScale(this, 0, xmin, ymin, xmax, ymax);
}

void
DrawTree::
sizeTree(DrawTree *root, int depth, double &xmin, double &ymin, double &xmax, double &ymax)
{
  double x = root->x();
  double y = depth;
  double r = root->r();

  double x1 = x - r*1.5;
  double y1 = y - r*1.5;
  double x2 = x + r*1.5;
  double y2 = y + r*1.5;

  xmin = std::min(xmin, x1);
  ymin = std::min(ymin, y1);
  xmax = std::max(xmax, x2);
  ymax = std::max(ymax, y2);

  for (const auto &child : root->children())
    sizeTree(child.get(), depth + 1, xmin, ymin, xmax, ymax);
}

void
DrawTree::
moveScale(DrawTree *root, int depth, double xmin, double ymin, double xmax, double ymax)
{
  auto lerp = [](double x, double low, double high) {
    return low + (high - low)*x;
  };

  auto norm =[](double x, double low, double high) {
    if (high != low)
      return (x - low)/(high - low);
    else
      return low;
  };

  auto map = [&](double value, double low1, double high1, double low2, double high2) {
    return lerp(norm(value, low1, high1), low2, high2);
  };

  auto mapX = [&](double x) {
    return map(x, xmin, xmax, 0.0, 1.0);
  };

  auto mapY = [&](double y) {
    return map(y, ymin, ymax, 0.0, 1.0);
  };

  double x = root->x();
  double y = depth;
  double r = root->r();

  double x1 = mapX(x - r);
  double y1 = mapY(y - r);
  double x2 = mapX(x + r);
  double y2 = mapY(y + r);

  double s = std::min(x2 - x1, y2 - y1);

  double xc = (x1 + x2)/2.0;
  double yc = (y1 + y2)/2.0;

  root->setX1(xc - s/2.0);
  root->setY1(yc - s/2.0);
  root->setX2(xc + s/2.0);
  root->setY2(yc + s/2.0);

  for (const auto &child : root->children())
    moveScale(child.get(), depth + 1, xmin, ymin, xmax, ymax);
}

}
