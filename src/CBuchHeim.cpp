#include <CBuchHeim.h>
#include <algorithm>
#include <set>
#include <cassert>

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
    auto *subtree = const_cast<Tree *>(tree->child(i));

    auto child = std::make_shared<DrawTree>(subtree, this, depth + 1, i + 1);

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

// return leftmost child or thread
DrawTree *
DrawTree::
left()
{
  if (thread()) return thread();

  if (! children().empty())
    return child(0);

  return nullptr;
}

// return rightmost child or thread
DrawTree *
DrawTree::
right()
{
  if (thread()) return thread();

  if (! children().empty())
    return child(numChildren() - 1);

  return nullptr;
}

// get previous node (in parent's children)
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

// get first node (in parent's children)
DrawTree *
DrawTree::
get_lmost_sibling()
{
#if 1
  // result cached in lmost_sibling_

  if (! lmost_sibling_ && parent_ && this != parent_->child(0))
    lmost_sibling_ = parent_->child(0);

  return lmost_sibling_;
#else
  DrawTree *lmost_sibling = nullptr;

  if (parent_ && this != parent_->child(0))
    lmost_sibling = parent_->child(0);

  return lmost_sibling;
#endif
}

//---

void
DrawTree::
knuthPlace()
{
  (void) knuthPlace1(this, 0);
}

int
DrawTree::
knuthPlace1(DrawTree *tree, int i)
{
  if (! tree)
    return i;

  int nc = tree->numChildren();

  auto *l = (nc > 0 ? tree->child(0) : nullptr);
  auto *r = (nc > 1 ? tree->child(1) : nullptr);

  auto i1 = knuthPlace1(l, i);

  tree->x_ = i1;

  return knuthPlace1(r, i1 + 1);
}

//---

void
DrawTree::
wsPlace()
{
  std::map<int, int> nexts;

  wsPlace1(this, 0, nexts);
}

void
DrawTree::
wsPlace1(DrawTree *tree, int depth, std::map<int, int> &nexts)
{
  auto p = nexts.find(depth);

  int next;

  if (p != nexts.end()) {
    next = (*p).second;

    ++nexts[depth];
  }
  else {
    next = 0;

    nexts[depth] = 1;
  }

  tree->x_ = next;
  tree->y_ = depth;

  for (auto &c : tree->children())
    wsPlace1(c.get(), depth + 1, nexts);
}

//---

void
DrawTree::
wsPlaceCentered()
{
  std::map<int, int> nexts;
  std::map<int, int> offset;

  wsPlaceCentered1(this, 0, nexts, offset);

  wsPlaceCenteredAddMods(this, 0);
}

void
DrawTree::
wsPlaceCentered1(DrawTree *tree, int depth, std::map<int, int> &nexts, std::map<int, int> &offset)
{
  for (auto &c : tree->children())
    wsPlaceCentered1(c.get(), depth + 1, nexts, offset);

  tree->y_ = depth;

  int nc = tree->numChildren();

  int place;

  if (nc == 0) {
    place    = nexts[depth];
    tree->x_ = place;
  }
  else if (nc == 1) {
    place = int(tree->child(0)->x_) - 1;
  }
  else {
    place = int(tree->child(0)->x_ + tree->child(1)->x_)/2;
  }

  offset[depth] = std::max(offset[depth], nexts[depth] - place);

  if (nc > 0)
    tree->x_ = place + offset[depth];

  nexts[depth] += 2;

  tree->offset_ = offset[depth];
}

void
DrawTree::
wsPlaceCenteredAddMods(DrawTree *tree, int modsum)
{
  tree->x_ += modsum;

  modsum += tree->offset_;

  for (auto &t : tree->children())
    wsPlaceCenteredAddMods(t.get(), modsum);
}

//---

void
DrawTree::
place()
{
  auto *dt = first_walk(this);

  auto min = second_walk(dt);

  if (min < 0) {
#if 0
    third_walk(dt, -min);
#endif
  }
}

DrawTree *
DrawTree::
first_walk(DrawTree *v, double distance)
{
  // leaf node placed to right of previous sibling
  if (v->children().empty()) {
    if (v->get_lmost_sibling())
      v->x_ = v->left_brother()->x() + distance;
    else
      v->x_ = 0.0;
  }
  else {
    auto *default_ancestor = v->child(0);

    for (auto &w : v->children()) {
      first_walk(w.get());

      default_ancestor = apportion(w.get(), default_ancestor, distance);
    }

    //std::cout << "finished v = " << v->name() << " children\n";

    execute_shifts(v);

    auto *ell = v->child(0);
    auto *arr = v->child(v->numChildren() - 1);

    auto midpoint = (ell->x() + arr->x())/2.0;

    auto *w = v->left_brother();

    if (w) {
      v->x_      = w->x() + distance;
      v->offset_ = int(v->x() - midpoint);
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
  if (! w) return default_ancestor;

  // in buchheim notation:
  //   i == inner; o == outer; r == right; l == left; r = +; l = -
  auto *vir = v;
  auto *vor = v;
  auto *vil = w;
  auto *vol = v->get_lmost_sibling();

  auto sir = v->offset_;
  auto sor = v->offset_;
  auto sil = vil->offset_;
  auto sol = vol->offset_;

  while (vil->right() && vir->left()) {
    vil = vil->right();
    vir = vir->left();
    vol = vol->left();
    vor = vor->right();

    vor->ancestor_ = v;

    double shift = (vil->x() + sil) - (vir->x() + sir) + distance;

    if (shift > 0) {
      auto *a = ancestor(vil, v, default_ancestor);

      move_subtree(a, v, int(shift));

      sir += int(shift);
      sor += int(shift);
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

  //std::cerr << "apportion " << v->hierName() << " = " << default_ancestor->hierName() << "\n";

  return default_ancestor;
}

void
DrawTree::
move_subtree(DrawTree *wl, DrawTree *wr, int shift)
{
  //std::cerr << "move_subtree " << wl->hierName() << " " << wr->hierName() << " " << shift << "\n";

  int subtrees = wr->number() - wl->number();

  //std::cout << wl->name() << " is conflicted with " << wr->name() <<
  //             " moving " << subtrees << " shift " << shift << "\n";
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

    //std::cout << "shift : " << w->name() << " " << shift << " " << w->change_ << "\n";

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
  for (auto &child : v->parent_->children()) {
    if (child.get() == vil->ancestor()) {
      //std::cerr << "ancestor " << vil->hierName() << ", " << v->hierName() <<
      //             " = " << vil->ancestor()->hierName() << "\n";
      return vil->ancestor();
    }
  }

#if 0
  if (std::find(v->parent_->children().begin(), v->parent_->children().end(), vil->ancestor()) !=
      v->parent_->children().end()) {
    return vil->ancestor();
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

  if (min < -9000 || v->x() < min)
    min = v->x();

  for (auto &w : v->children())
    min = second_walk(w.get(), m + v->offset_, depth + 1, min);

  return min;
}

// shift so min is zero
void
DrawTree::
third_walk(DrawTree *tree, int n)
{
  tree->x_ += n;

  for (auto &c : tree->children())
    third_walk(c.get(), n);
}

void
DrawTree::
normalize(double r, bool equalScale)
{
  s_r = r;

  normalize(equalScale);

  s_r = -1.0;
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
  double r = (s_r >= 0.0 ? s_r : root->r());

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

  auto norm = [](double x, double low, double high) {
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

  double x1, y1, x2, y2;

  if (s_r >= 0.0) {
    x1 = mapX(x) - s_r;
    y1 = mapY(y) - s_r;
    x2 = mapX(x) + s_r;
    y2 = mapY(y) + s_r;
  }
  else {
    double r = root->r();

    x1 = mapX(x - r);
    y1 = mapY(y - r);
    x2 = mapX(x + r);
    y2 = mapY(y + r);
  }

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

void
DrawTree::
checkOverlaps() const
{
  PositionMap positionMap;

  checkOverlaps1(const_cast<DrawTree *>(this), positionMap);

  for (const auto &px : positionMap) {
    //int x = px.first;

    for (const auto &py : px.second) {
      //int y = py.first;

      auto n = py.second.size();
      if (n <= 1) continue;

      std::cerr << "Overlaps:";

      for (const auto &pt : py.second) {
        std::cerr << " " << pt->hierName();
      }

      std::cerr << "\n";
    }
  }
}

void
DrawTree::
fixOverlaps()
{
  bool changed = true;

  while (changed) {
    changed = false;

    PositionMap positionMap;

    checkOverlaps1(const_cast<DrawTree *>(this), positionMap);

    for (const auto &px : positionMap) {
      //int x = px.first;

      for (const auto &py : px.second) {
        //int y = py.first;

        auto n = py.second.size();
        if (n <= 1) continue;

        auto *cp = commonParent(py.second[0], py.second[1]);

      //auto *tree1 = py.second[0];
        auto *ltree = py.second[1];

        auto *lparent = ltree->parent();

        while (lparent != cp && lparent->parent() != cp) {
          ltree   = lparent;
          lparent = lparent->parent();
        }

        //std::cerr << "Move tree " << ltree->hierName() <<
        //             " in parent " << lparent->hierName() << "\n";

        if (lparent)
          moveChildren(lparent, ltree);

        changed = true;

        break;
      }
    }
  }
}

void
DrawTree::
moveChildren(const DrawTree *parent, const DrawTree *startChild)
{
  bool inside = false;

  for (auto &c : parent->children()) {
    if (! inside && (! startChild || c.get() == startChild))
      inside = true;

    if (inside)
      ++c.get()->x_;

    moveChildren(c.get(), nullptr);
  }
}

void
DrawTree::
checkOverlaps1(DrawTree *root, PositionMap &positionMap) const
{
  positionMap[int(root->x())][int(root->y())].push_back(root);

  for (const auto &child : root->children())
    checkOverlaps1(child.get(), positionMap);
}

DrawTree *
DrawTree::
commonParent(DrawTree *tree1, DrawTree *tree2) const
{
  std::set<DrawTree *> parents;
  std::set<DrawTree *> commonParents;

  auto addParents = [&](DrawTree *tree) {
    auto *parent = tree->parent();

    while (parent) {
      auto p = parents.find(parent);

      if (p != parents.end())
        commonParents.insert(parent);
      else
        parents.insert(parent);

      parent = parent->parent();
    }
  };

  addParents(tree1);
  addParents(tree2);

  DrawTree *commonParent = nullptr;

  for (auto *parent : commonParents) {
    if (! commonParent || parent->y() > commonParent->y())
      commonParent = parent;
  }

  //if (commonParent)
  //  std::cerr << "commonParent " << commonParent->hierName() << "\n";

  return commonParent;
}

}
