#include "relative_layout.h"

#include "layout_attribute.h"
#include "layout_params.h"
#include "layout_types.h"
#include "property_manager.h"

#include "ui/views/view.h"

#include <deque>
#include "base/stl_util.h"

namespace {
constexpr int VALUE_NOT_SET = std::numeric_limits<int>::min();

constexpr int kLayout_Parent = -1;

constexpr int kLayout_Left_Left = 0;
constexpr int kLayout_Left_Right = 1;
constexpr int kLayout_Right_Left = 2;
constexpr int kLayout_Right_Right = 3;
constexpr int kLayout_Top_Top = 4;
constexpr int kLayout_Top_Bottom = 5;
constexpr int kLayout_Bottom_Top = 6;
constexpr int kLayout_Bottom_Bottom = 7;

constexpr int kRulesHorizontal[] = {kLayout_Left_Left, kLayout_Left_Right,
                                    kLayout_Right_Left, kLayout_Right_Right};

constexpr int kRulesVertical[] = {kLayout_Top_Top, kLayout_Top_Bottom,
                                  kLayout_Bottom_Top, kLayout_Bottom_Bottom};

LAYOUT_PROPERTY(layout_left_left, kGroup_Relative, kLayout_Left_Left)
LAYOUT_PROPERTY(layout_left_right, kGroup_Relative, kLayout_Left_Right)
LAYOUT_PROPERTY(layout_right_left, kGroup_Relative, kLayout_Right_Left)
LAYOUT_PROPERTY(layout_right_right, kGroup_Relative, kLayout_Right_Right)
LAYOUT_PROPERTY(layout_top_top, kGroup_Relative, kLayout_Top_Top)
LAYOUT_PROPERTY(layout_top_bottom, kGroup_Relative, kLayout_Top_Bottom)
LAYOUT_PROPERTY(layout_bottom_top, kGroup_Relative, kLayout_Bottom_Top)
LAYOUT_PROPERTY(layout_bottom_bottom, kGroup_Relative, kLayout_Bottom_Bottom)
}  // namespace

namespace Layout {
class RelativeLayout::RelativeLayoutParams : public LayoutParams {
 public:
  int rules[8] = { 0 };
  RelativeLayoutParams(const LayoutTypes& types) : LayoutParams(types) {
    for (int i = 0; i < 8; ++i)
      rules[i] = 0;

    size_t count = types.GetCount();
    for (size_t i = 0; i < count; ++i) {
      types.SetIndex(i);
      if (types.GetGroup() != kGroup_Relative)
        continue;

      int attr = types.GetType();
      switch (attr) {
        case kLayout_Left_Left: {
          rules[kLayout_Left_Left] = types.GetId();
          break;
        }
        case kLayout_Left_Right: {
          rules[kLayout_Left_Right] = types.GetId();
          break;
        }
        case kLayout_Right_Left: {
          rules[kLayout_Right_Left] = types.GetId();
          break;
        }
        case kLayout_Right_Right: {
          rules[kLayout_Right_Right] = types.GetId();
          break;
        }
        case kLayout_Top_Top: {
          rules[kLayout_Top_Top] = types.GetId();
          break;
        }
        case kLayout_Top_Bottom: {
          rules[kLayout_Top_Bottom] = types.GetId();
          break;
        }
        case kLayout_Bottom_Top: {
          rules[kLayout_Bottom_Top] = types.GetId();
          break;
        }
        case kLayout_Bottom_Bottom: {
          rules[kLayout_Bottom_Bottom] = types.GetId();
          break;
        }
      }
    }
  }

  static RelativeLayoutParams* Get(views::View* v) {
    LayoutParams* layout_params = LayoutParams::Get(v);
    if (layout_params == nullptr)
      return nullptr;

    return static_cast<RelativeLayoutParams*>(layout_params);
  }
};

LayoutParams* LayoutRelative::BuildLayoutParams(const LayoutTypes& types) {
  return new RelativeLayout::RelativeLayoutParams(types);
}

struct RelativeLayout::DependencyGraph {
  struct Node {
    Node(views::View* view) : view(view) {}

    void Clear() {
      dependents.clear();
      dependencies.clear();
    }

    views::View* view;
    // depend on this
    std::map<Node*, DependencyGraph*> dependents;
    // depend on other
    std::map<int, Node*> dependencies;
  };

  void Clear() {
    for (auto* node : nodes)
      delete node;

    nodes.clear();
    key_nodes.clear();
    roots.clear();
  }

  void Add(views::View* view) {
    int id = view->GetID();
    Node* node = new Node(view);
    if (id > 0)
      key_nodes.insert({id, node});

    nodes.push_back(node);
  }

  void GetSortedView(std::vector<views::View*>& views,
                     const int rules[],
                     size_t size) {
    auto root = FindRoots(rules, size);
    size_t index = 0;
    Node* node = nullptr;
    while (!root.empty()) {
      node = root.back();
      root.pop_back();

      views[index++] = node->view;
      int key = node->view->GetID();

      for (auto& [depent, second] : node->dependents) {
        auto& dependencies = depent->dependencies;
        auto iter = dependencies.find(key);
        if (iter != dependencies.end())
          dependencies.erase(iter);

        if (dependencies.empty())
          root.push_back(depent);
      }
    }

    if (index < views.size()) {
      // assert(false);
    }
  }

  std::deque<Node*> FindRoots(const int rules_filter[], size_t size) {
    for (auto* node : nodes)
      node->Clear();

    for (auto* node : nodes) {
      RelativeLayoutParams* layout_params =
          RelativeLayoutParams::Get(node->view);

      int* rules = layout_params->rules;
      for (size_t i = 0; i < size; ++i) {
        int rule = rules[rules_filter[i]];
        if (rule > 0) {
          auto iter = key_nodes.find(rule);
          if (iter == key_nodes.end())
            continue;

          Node* dependency = iter->second;
          if (dependency == node)
            continue;

          dependency->dependents.insert({node, this});
          node->dependencies.insert({rule, dependency});
        }
      }
    }

    roots.clear();
    for (auto* node : nodes) {
      if (node->dependencies.empty())
        roots.push_back(node);
    }

    return roots;
  }

  std::vector<Node*> nodes;
  std::map<int, Node*> key_nodes;
  std::deque<Node*> roots;
};

RelativeLayout::RelativeLayout() : graph_(new DependencyGraph) {}

RelativeLayout::~RelativeLayout() = default;

void RelativeLayout::SortChildren(views::View* host) {
  if (!dirty_hierarchy)
    return;

  dirty_hierarchy = false;

  size_t count = host->children().size();
  sorted_horizontal_children_.resize(count);
  sorted_vertical_children_.resize(count);

  graph_->Clear();
  for (auto* child : host->children())
    graph_->Add(child);

  graph_->GetSortedView(sorted_horizontal_children_, kRulesHorizontal,
                        base::size(kRulesHorizontal));
  graph_->GetSortedView(sorted_vertical_children_, kRulesVertical,
                        base::size(kRulesVertical));
}

void RelativeLayout::Measure(views::View* host,
                             int width_measure_spec,
                             int height_measure_spec) {
  SortChildren(host);

  int my_width = -1;
  int my_height = -1;
  int width = 0;
  int height = 0;

  padding_ = host->GetInsets();

  const int width_mode = MeasureSpec::Mode(width_measure_spec);
  const int height_mode = MeasureSpec::Mode(height_measure_spec);
  const int width_size = MeasureSpec::Size(width_measure_spec);
  const int height_size = MeasureSpec::Size(height_measure_spec);

  if (width_mode != MeasureSpec::UNSPECIFIED)
    my_width = width_size;

  if (height_mode != MeasureSpec::UNSPECIFIED)
    my_height = height_size;

  if (width_mode == MeasureSpec::EXACTLY)
    width = my_width;

  if (height_mode == MeasureSpec::EXACTLY)
    height = my_height;

  const bool wrap_content_width = width_mode != MeasureSpec::EXACTLY;
  const bool wrap_content_height = height_mode != MeasureSpec::EXACTLY;

  bool offsetHorizontalAxis = false;
  bool offsetVerticalAxis = false;

  for (auto* child : sorted_horizontal_children_) {
    RelativeLayoutParams* params = RelativeLayoutParams::Get(child);
    // calculate relative coordinate
    ApplyHorizontalSizeRules(params, my_width);
    // measure child width
    MeasureChildHorizontal(child, params, my_width, my_height);
    // position view with relative coordinate and width
    PositionChildHorizontal(child, params, my_width, wrap_content_width);

    if (params->rules[kLayout_Right_Right] == kLayout_Parent)
      offsetHorizontalAxis = true;
  }

  for (auto* child : sorted_vertical_children_) {
    RelativeLayoutParams* params = RelativeLayoutParams::Get(child);
    ApplyVerticalSizeRules(params, my_height);
    MeasureChild(child, params, my_width, my_height);
    PositionChildVertical(child, params, my_height, wrap_content_height);

    if (params->rules[kLayout_Right_Right] == kLayout_Parent)
      offsetVerticalAxis = true;

    // if wrap content, statistic max width & height
    if (wrap_content_width)
      width = std::max(width, params->right + params->margin.right());

    if (wrap_content_height)
      height = std::max(height, params->bottom + params->margin.bottom());
  }

  if (wrap_content_width) {
    width += padding_.right();

    //if exist align parent right
    if (offsetHorizontalAxis) {
      for (auto* child : sorted_horizontal_children_) {
        RelativeLayoutParams* params = RelativeLayoutParams::Get(child);
        int* rules = params->rules;
        if (rules[kLayout_Right_Right] == kLayout_Parent) {
          int w = params->width;
          if (w == LayoutParams::WRAP_CONTENT)
             w = params->size.width();
          int right = width - padding_.right() - params->margin.right();

          if (params->left != VALUE_NOT_SET) {
            params->right = right;
            if (w != 0) {
               params->left = (params->left + params->right - w) / 2;
               params->right = params->left + w;
            }       
          } else {
            params->right = right;
            params->left = params->right - params->size.width();
          }
        }
      }
    }
  }

  if (wrap_content_height) {
    height += padding_.bottom();

    //if exist align parent bottom
    if (offsetVerticalAxis) {
      for (auto* child : sorted_vertical_children_) {
        RelativeLayoutParams* params = RelativeLayoutParams::Get(child);
        int* rules = params->rules;
        if (rules[kLayout_Bottom_Bottom] == kLayout_Parent) {
          int h = params->height;
          if (h == LayoutParams::WRAP_CONTENT)
             h = params->size.height();
          int bottom = height - padding_.bottom() - params->margin.bottom();
          if (params->top != VALUE_NOT_SET) {
            params->bottom = bottom;
            if (h != 0) {
               params->top = (params->top + params->bottom - h) / 2;
               params->bottom = params->top + h;
            }
          } else {
            params->bottom = bottom;
            params->top = params->bottom - params->size.height();
          }
        }
      }
    }
  }

  LayoutParams* layout_params = LayoutParams::Get(host);
  if (layout_params)
    layout_params->SetDimension(width, height);
}

void RelativeLayout::ApplyHorizontalSizeRules(RelativeLayoutParams* params,
                                              int my_width) {
  RelativeLayoutParams* anchor_params = nullptr;

  int* rules = params->rules;

  params->left = VALUE_NOT_SET;
  params->right = VALUE_NOT_SET;

  anchor_params = GetRelatedViewParams(rules[kLayout_Left_Left]);
  if (anchor_params != nullptr) {
    params->left = anchor_params->left - anchor_params->margin.left() +
                   params->margin.left();
  } else if (rules[kLayout_Left_Left] == kLayout_Parent) {
      params->left = padding_.left() + params->margin.left();
  }

  anchor_params = GetRelatedViewParams(rules[kLayout_Left_Right]);
  if (anchor_params != nullptr) {
    params->left = anchor_params->right + anchor_params->margin.right() +
                   params->margin.left();
  }

  anchor_params = GetRelatedViewParams(rules[kLayout_Right_Left]);
  if (anchor_params != nullptr) {
    params->right = anchor_params->left - anchor_params->margin.left() -
                    params->margin.right();
  }

  anchor_params = GetRelatedViewParams(rules[kLayout_Right_Right]);
  if (anchor_params != nullptr) {
    params->right = anchor_params->right + anchor_params->margin.right() -
                    params->margin.right();
  } else if (rules[kLayout_Right_Right] == kLayout_Parent) {
    if (my_width >= 0)
      params->right = my_width - padding_.right() - params->margin.right();
  }
}

void RelativeLayout::ApplyVerticalSizeRules(RelativeLayoutParams* params,
                                            int my_height) {
  RelativeLayoutParams* anchor_params = nullptr;

  int* rules = params->rules;

  params->top = VALUE_NOT_SET;
  params->bottom = VALUE_NOT_SET;

  anchor_params = GetRelatedViewParams(rules[kLayout_Top_Top]);
  if (anchor_params != nullptr) {
    params->top =
        anchor_params->top - anchor_params->margin.top() + params->margin.top();
  } else if (rules[kLayout_Top_Top] == kLayout_Parent) {
      params->top = padding_.top() + params->margin.top();
  }

  anchor_params = GetRelatedViewParams(rules[kLayout_Top_Bottom]);
  if (anchor_params != nullptr) {
    params->top = anchor_params->bottom + anchor_params->margin.bottom() +
                  params->margin.top();
  }

  anchor_params = GetRelatedViewParams(rules[kLayout_Bottom_Top]);
  if (anchor_params != nullptr) {
    params->bottom = anchor_params->top - anchor_params->margin.top() -
                     params->margin.bottom();
  }

  anchor_params = static_cast<RelativeLayoutParams*>(
      GetRelatedViewParams(rules[kLayout_Bottom_Bottom]));
  if (anchor_params != nullptr) {
    params->bottom = anchor_params->bottom + anchor_params->margin.bottom() -
                     params->margin.bottom();
  } else if (rules[kLayout_Bottom_Bottom] == kLayout_Parent) {
    if (my_height >= 0)
      params->bottom = my_height - padding_.bottom() - params->margin.bottom();
  }
}

void RelativeLayout::MeasureChildHorizontal(views::View* child,
                                            RelativeLayoutParams* params,
                                            int my_width,
                                            int my_height) {
  int child_width_measure_spec = GetChildMeasureSpec(
      params->left, params->right, params->width, params->margin.left(),
      params->margin.right(), padding_.left(), padding_.right(), my_width);

  int child_height_measure_spec;
  if (my_height < 0) {
    if (params->height >= 0)
      child_height_measure_spec =
          MeasureSpec::Make(params->height, MeasureSpec::EXACTLY);
    else
      child_height_measure_spec =
          MeasureSpec::Make(0, MeasureSpec::UNSPECIFIED);
  } else {
    int max_height =
        std::max(0, my_height - padding_.top() - padding_.bottom() -
                        params->margin.top() - params->margin.bottom());

    int height_mode;
    if (params->height == LayoutParams::MATCH_PARENT)
      height_mode = MeasureSpec::EXACTLY;
    else
      height_mode = MeasureSpec::AT_MOST;

    child_height_measure_spec = MeasureSpec::Make(max_height, height_mode);
  }

  MeasureChild(child, child_width_measure_spec, child_height_measure_spec);
}

void RelativeLayout::MeasureChild(views::View* child,
                                  RelativeLayoutParams* params,
                                  int my_width,
                                  int my_height) {
  int child_width_measure_spec = GetChildMeasureSpec(
      params->left, params->right, params->width, params->margin.left(),
      params->margin.right(), padding_.left(), padding_.right(), my_width);

  int child_height_measure_spec = GetChildMeasureSpec(
      params->top, params->bottom, params->height, params->margin.top(),
      params->margin.bottom(), padding_.top(), padding_.bottom(), my_height);

  MeasureChild(child, child_width_measure_spec, child_height_measure_spec);
}

void RelativeLayout::MeasureChild(views::View* child,
                                  int width_measure_spec,
                                  int height_measure_spec) {
  LayoutMaster* layout_manager =
      static_cast<LayoutMaster*>(child->GetLayoutManager());
  if (layout_manager) {
    layout_manager->Measure(child, width_measure_spec, height_measure_spec);
    return;
  }

  LayoutParams* params = LayoutParams::Get(child);
  gfx::Size size = child->GetPreferredSize();

  int my_width = -1;
  int my_height = -1;
  int width = 0;
  int height = 0;

  const int width_mode = MeasureSpec::Mode(width_measure_spec);
  const int height_mode = MeasureSpec::Mode(height_measure_spec);
  const int width_size = MeasureSpec::Size(width_measure_spec);
  const int height_size = MeasureSpec::Size(height_measure_spec);

  if (width_mode != MeasureSpec::UNSPECIFIED)
    my_width = width_size;

  if (height_mode != MeasureSpec::UNSPECIFIED)
    my_height = height_size;

  if (width_mode == MeasureSpec::EXACTLY)
    width = my_width;

  if (height_mode == MeasureSpec::EXACTLY)
    height = my_height;

  const bool wrap_content_width = width_mode != MeasureSpec::EXACTLY;
  const bool wrap_content_height = height_mode != MeasureSpec::EXACTLY;

  if (params->width > 0)
    params->size.set_width(params->width);
  else if (params->width == LayoutParams::WRAP_CONTENT || wrap_content_width)
    params->size.set_width(size.width());
  else
    params->size.set_width(width);

  if (params->height > 0)
    params->size.set_height(params->height);
  else if (params->height == LayoutParams::WRAP_CONTENT || wrap_content_height)
    params->size.set_height(size.height());
  else
    params->size.set_height(height);
}

void RelativeLayout::PositionChildHorizontal(views::View* view,
                                             RelativeLayoutParams* params,
                                             int my_width,
                                             bool wrap_content) {
  int width = params->size.width();
  if (params->left == VALUE_NOT_SET && params->right != VALUE_NOT_SET) {
    params->left = params->right - width;
  } else if (params->left != VALUE_NOT_SET && params->right == VALUE_NOT_SET) {
    if (params->rules[kLayout_Right_Right] == kLayout_Parent && !wrap_content) {
      params->right =
          my_width - padding_.right() - params->margin.right();
      params->left = (params->left + params->right - width)/2;
      params->right = params->left + width;
    }
    params->right = params->left + width;
  } else if (params->left == VALUE_NOT_SET && params->right == VALUE_NOT_SET) {
      params->left = padding_.left() + params->margin.left();
      params->right = params->left + width;
  } else if (params->left != VALUE_NOT_SET && params->right != VALUE_NOT_SET){
    params->left = (params->left+params->right-width)/2;
    params->right = params->left + width;
  }
}

void RelativeLayout::PositionChildVertical(views::View* view,
                                           RelativeLayoutParams* params,
                                           int my_height,
                                           bool wrap_content) {
  int height = params->size.height();
  if (params->top == VALUE_NOT_SET && params->bottom != VALUE_NOT_SET) {
    params->top = params->bottom - height;
  } else if (params->top != VALUE_NOT_SET && params->bottom == VALUE_NOT_SET) {
    if (params->rules[kLayout_Bottom_Bottom] == kLayout_Parent &&
        !wrap_content) {
      params->bottom =
          my_height - padding_.bottom() - params->margin.bottom() - height;
      params->top = (params->top + params->bottom - height)/2;
    } else
      params->bottom = params->top + height;
  } else if (params->top == VALUE_NOT_SET && params->bottom == VALUE_NOT_SET) {
      params->top = padding_.top() + params->margin.top();
      params->bottom = params->top + height;
  } else if (params->top != VALUE_NOT_SET && params->bottom != VALUE_NOT_SET){
    params->top = (params->top+params->bottom-height)/2;
    params->bottom = params->top + height;
  }
}

int RelativeLayout::GetChildMeasureSpec(int child_start,
                                        int child_end,
                                        int child_size,
                                        int start_margin,
                                        int end_margin,
                                        int start_padding,
                                        int end_padding,
                                        int my_size) {
  int child_spec_mode = 0;
  int child_spec_size = 0;

  const bool unspecified = my_size < 0;
  if (unspecified) {
    if (child_size == LayoutParams::WRAP_CONTENT){
      child_spec_mode = MeasureSpec::UNSPECIFIED;
      child_spec_size = 0;
    } else if (child_size > 0) {
      child_spec_size = child_size;
      child_spec_mode = MeasureSpec::EXACTLY;
    } else if (child_start != VALUE_NOT_SET && child_end != VALUE_NOT_SET) {
      child_spec_size = std::max(0, child_end - child_start);
      child_spec_mode = MeasureSpec::EXACTLY;
    }  else {
      child_spec_size = 0;
      child_spec_mode = MeasureSpec::UNSPECIFIED;
    }

    return MeasureSpec::Make(child_spec_size, child_spec_mode);
  }

  int temp_start = child_start;
  int temp_end = child_end;
  if (temp_start == VALUE_NOT_SET)
    temp_start = start_padding + start_margin;

  if (temp_end == VALUE_NOT_SET)
    temp_end = my_size - end_padding - end_margin;

  int max_available = temp_end - temp_start;
  if (child_size == LayoutParams::WRAP_CONTENT){
      child_spec_mode = MeasureSpec::UNSPECIFIED;
      child_spec_size = 0;
  } else if (child_size == LayoutParams::MATCH_PARENT) {
      child_spec_mode = MeasureSpec::EXACTLY;
      child_spec_size = std::max(max_available, 0);
  } else if (child_size >= 0) {
      child_spec_mode = MeasureSpec::EXACTLY;
 
      if (max_available >= 0 && child_size == 0)
        child_spec_size = std::max(max_available, child_size);
      else
        child_spec_size = child_size;
  } else if (child_start != VALUE_NOT_SET && child_end != VALUE_NOT_SET) {
    child_spec_size = std::max(0, max_available);
    child_spec_mode = MeasureSpec::EXACTLY;
  } 

  return MeasureSpec::Make(child_spec_size, child_spec_mode);
}

RelativeLayout::RelativeLayoutParams* RelativeLayout::GetRelatedViewParams(int id) {
  views::View* v = GetRelatedView(id);
  if (v != nullptr)
    return RelativeLayoutParams::Get(v);

  return nullptr;
}

views::View* RelativeLayout::GetRelatedView(int id) {
  if (id != 0) {
    auto iter = graph_->key_nodes.find(id);
    if (iter == graph_->key_nodes.end())
      return nullptr;

    views::View* v = iter->second->view;

    return v;
  }

  return nullptr;
}

}  // namespace Layout
