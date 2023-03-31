#include "relative_layout.h"

#include <deque>

#include "layout_manager_base.h"
#include "layout_metadata_impl_macros.h"
#include "layout_params.h"

#include "base/stl_util.h"
#include "ui/views/view.h"

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
}  // namespace

namespace Layout {
class RelativeLayoutParams : public LayoutParams {
 public:
  METADATA_HEADER(RelativeLayoutParams);
  ~RelativeLayoutParams() override = default;

  static RelativeLayoutParams* Get(views::View* v) {
    LayoutParams* layout_params = LayoutParams::Get(v);
    if (layout_params == nullptr)
      return nullptr;

    return static_cast<RelativeLayoutParams*>(layout_params);
  }

  void SetLeftToLeft(int id) { rules[kLayout_Left_Left] = id; }
  int GetLeftToLeft() const { return rules[kLayout_Left_Left]; }

  void SetLeftToRight(int id) { rules[kLayout_Left_Right] = id; }
  int GetLeftToRight() const { return rules[kLayout_Left_Right]; }

  void SetRightToLeft(int id) { rules[kLayout_Right_Left] = id; }
  int GetRightToLeft() const { return rules[kLayout_Right_Left]; }

  void SetRightToRight(int id) { rules[kLayout_Right_Right] = id; }
  int GetRightToRight() const { return rules[kLayout_Right_Right]; }

  void SetTopToTop(int id) { rules[kLayout_Top_Top] = id; }
  int GetTopToTop() const { return rules[kLayout_Top_Top]; }

  void SetTopToBottom(int id) { rules[kLayout_Top_Bottom] = id; }
  int GetTopToBottom() const { return rules[kLayout_Top_Bottom]; }

  void SetBottomToTop(int id) { rules[kLayout_Bottom_Top] = id; }
  int GetBottomToTop() const { return rules[kLayout_Bottom_Top]; }

  void SetBottomToBottom(int id) { rules[kLayout_Bottom_Bottom] = id; }
  int GetBottomToBottom() const { return rules[kLayout_Bottom_Bottom]; }

  explicit operator int*() { return rules; }

 private:
  int rules[8] = {0};
};

struct RelativeTypeConverter : ui::metadata::BaseTypeConverter<true, false> {
  static std::u16string ToString(int value) {
    if (value == -1)
      return u"parent";

    return base::NumberToString16(value);
  }

  static absl::optional<int> FromString(const std::u16string& source_value) {
    if (source_value == u"parent")
      return kLayout_Parent;

    int ret;
    if (base::StringToInt(source_value, &ret))
      return ret;

    return absl::nullopt;
  }

  static ui::metadata::ValidStrings GetValidStrings() {
    return {u"parent", u">=0"};
  }
};

BEGIN_METADATA(RelativeLayoutParams, LayoutParams)
ADD_LAYOUT_PROPERTY_METADATA(int,
                             layout_left_to_left,
                             LeftToLeft,
                             RelativeTypeConverter)
ADD_LAYOUT_PROPERTY_METADATA(int,
                             layout_left_to_right,
                             LeftToRight,
                             RelativeTypeConverter)
ADD_LAYOUT_PROPERTY_METADATA(int,
                             layout_right_to_left,
                             RightToLeft,
                             RelativeTypeConverter)
ADD_LAYOUT_PROPERTY_METADATA(int,
                             layout_right_to_right,
                             RightToRight,
                             RelativeTypeConverter)
ADD_LAYOUT_PROPERTY_METADATA(int,
                             layout_top_to_top,
                             TopToTop,
                             RelativeTypeConverter)
ADD_LAYOUT_PROPERTY_METADATA(int,
                             layout_top_to_bottom,
                             TopToBottom,
                             RelativeTypeConverter)
ADD_LAYOUT_PROPERTY_METADATA(int,
                             layout_bottom_to_top,
                             BottomToTop,
                             RelativeTypeConverter)
ADD_LAYOUT_PROPERTY_METADATA(int,
                             layout_bottom_to_bottom,
                             BottomToBottom,
                             RelativeTypeConverter)
END_METADATA

struct RelativeLayout::DependencyGraph {
  struct Node {
    explicit Node(views::View* view) : view(view) {}

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

      int* rules = static_cast<int*>(*layout_params);
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

void RelativeLayout::SortChildren(const views::View* host) {
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
                        std::size(kRulesHorizontal));
  graph_->GetSortedView(sorted_vertical_children_, kRulesVertical,
                        std::size(kRulesVertical));
}

gfx::Size RelativeLayout::Measure(const views::View* host,
                                  int width_measure_spec,
                                  int height_measure_spec) {
  SortChildren(host);

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

  bool offset_horizontal_axis = false;
  bool offset_vertical_axis = false;

  for (auto* child : sorted_horizontal_children_) {
    RelativeLayoutParams* params = RelativeLayoutParams::Get(child);
    ApplyHorizontalSizeRules(params, my_width);
    MeasureChildHorizontal(child, params);
    PositionChildHorizontal(child, params, my_width, wrap_content_width);

    if (static_cast<int*>(*params)[kLayout_Right_Right] == kLayout_Parent)
      offset_horizontal_axis = true;
  }

  for (auto* child : sorted_vertical_children_) {
    RelativeLayoutParams* params = RelativeLayoutParams::Get(child);
    ApplyVerticalSizeRules(params, my_height);
    MeasureChildVertical(child, params);
    PositionChildVertical(child, params, my_height, wrap_content_height);

    if (static_cast<int*>(*params)[kLayout_Bottom_Bottom] == kLayout_Parent)
      offset_vertical_axis = true;

    if (wrap_content_width)
      width = std::max(width, params->right + params->GetMargin().right());

    if (wrap_content_height)
      height = std::max(height, params->bottom + params->GetMargin().bottom());
  }

  if (width_mode == MeasureSpec::EXACTLY &&
      height_mode == MeasureSpec::EXACTLY) {
    if (wrap_content_width) {
      // if exist align parent right
      if (offset_horizontal_axis) {
        for (auto* child : sorted_horizontal_children_) {
          RelativeLayoutParams* params = RelativeLayoutParams::Get(child);
          int* rules = static_cast<int*>(*params);
          if (rules[kLayout_Right_Right] == kLayout_Parent) {
            int w = params->GetWidth();
            if (w == LayoutParams::WRAP_CONTENT)
              w = params->size.width();
            int right = width - params->GetMargin().right();

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
      // if exist align parent bottom
      if (offset_vertical_axis) {
        for (auto* child : sorted_vertical_children_) {
          RelativeLayoutParams* params = RelativeLayoutParams::Get(child);
          int* rules = static_cast<int*>(*params);
          if (rules[kLayout_Bottom_Bottom] == kLayout_Parent) {
            int h = params->GetHeight();
            if (h == LayoutParams::WRAP_CONTENT)
              h = params->size.height();
            int bottom = height - params->GetMargin().bottom();
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
  }

  return gfx::Size(width, height);
}

void RelativeLayout::ApplyHorizontalSizeRules(RelativeLayoutParams* params,
                                              int my_width) {
  params->left = VALUE_NOT_SET;
  params->right = VALUE_NOT_SET;

  int* rules = static_cast<int*>(*params);
  RelativeLayoutParams* anchor_params = nullptr;

  anchor_params = GetRelatedViewParams(rules[kLayout_Left_Left]);
  if (anchor_params != nullptr) {
    params->left = anchor_params->left - anchor_params->GetMargin().left() +
                   params->GetMargin().left();
  } else if (rules[kLayout_Left_Left] == kLayout_Parent) {
    params->left = params->GetMargin().left();
  }

  anchor_params = GetRelatedViewParams(rules[kLayout_Left_Right]);
  if (anchor_params != nullptr) {
    params->left = anchor_params->right + anchor_params->GetMargin().right() +
                   params->GetMargin().left();
  }

  anchor_params = GetRelatedViewParams(rules[kLayout_Right_Left]);
  if (anchor_params != nullptr) {
    params->right = anchor_params->left - anchor_params->GetMargin().left() -
                    params->GetMargin().right();
  }

  anchor_params = GetRelatedViewParams(rules[kLayout_Right_Right]);
  if (anchor_params != nullptr) {
    params->right = anchor_params->right + anchor_params->GetMargin().right() -
                    params->GetMargin().right();
  } else if (rules[kLayout_Right_Right] == kLayout_Parent) {
    if (my_width >= 0)
      params->right = my_width - params->GetMargin().right();
  }
}

void RelativeLayout::ApplyVerticalSizeRules(RelativeLayoutParams* params,
                                            int my_height) {
  params->top = VALUE_NOT_SET;
  params->bottom = VALUE_NOT_SET;

  int* rules = static_cast<int*>(*params);
  RelativeLayoutParams* anchor_params = nullptr;

  anchor_params = GetRelatedViewParams(rules[kLayout_Top_Top]);
  if (anchor_params != nullptr) {
    params->top = anchor_params->top - anchor_params->GetMargin().top() +
                  params->GetMargin().top();
  } else if (rules[kLayout_Top_Top] == kLayout_Parent) {
    params->top = params->GetMargin().top();
  }

  anchor_params = GetRelatedViewParams(rules[kLayout_Top_Bottom]);
  if (anchor_params != nullptr) {
    params->top = anchor_params->bottom + anchor_params->GetMargin().bottom() +
                  params->GetMargin().top();
  }

  anchor_params = GetRelatedViewParams(rules[kLayout_Bottom_Top]);
  if (anchor_params != nullptr) {
    params->bottom = anchor_params->top - anchor_params->GetMargin().top() -
                     params->GetMargin().bottom();
  }

  anchor_params = GetRelatedViewParams(rules[kLayout_Bottom_Bottom]);
  if (anchor_params != nullptr) {
    params->bottom = anchor_params->bottom +
                     anchor_params->GetMargin().bottom() -
                     params->GetMargin().bottom();
  } else if (rules[kLayout_Bottom_Bottom] == kLayout_Parent) {
    if (my_height >= 0)
      params->bottom = my_height - params->GetMargin().bottom();
  }
}

void RelativeLayout::MeasureChildHorizontal(views::View* child,
                                            RelativeLayoutParams* params) {
  int width = params->GetWidth();
  if (width == LayoutParams::WRAP_CONTENT) {
    width = child->GetPreferredSize().width();
  } else if (width > 0) {
  } else if (params->left != VALUE_NOT_SET && params->right != VALUE_NOT_SET) {
    width = params->right - params->left;
  }

  params->size.set_width(width);
}

void RelativeLayout::MeasureChildVertical(views::View* child,
                                          RelativeLayoutParams* params) {
  int height = params->GetHeight();
  if (height == LayoutParams::WRAP_CONTENT) {
    height = child->GetHeightForWidth(params->size.width());
  } else if (height > 0) {
  } else if (params->top != VALUE_NOT_SET && params->bottom != VALUE_NOT_SET) {
    height = params->bottom - params->top;
  }

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
    if (static_cast<int*>(*params)[kLayout_Right_Right] == kLayout_Parent &&
        !wrap_content) {
      params->right = my_width - params->GetMargin().right();
      params->left = (params->left + params->right - width) / 2;
      params->right = params->left + width;
    }
    params->right = params->left + width;
  } else if (params->left == VALUE_NOT_SET && params->right == VALUE_NOT_SET) {
    params->left = params->GetMargin().left();
    params->right = params->left + width;
  } else if (params->left != VALUE_NOT_SET && params->right != VALUE_NOT_SET) {
    params->left = (params->left + params->right - width) / 2;
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
    if (static_cast<int*>(*params)[kLayout_Bottom_Bottom] == kLayout_Parent &&
        !wrap_content) {
      params->bottom = my_height - params->GetMargin().bottom() - height;
      params->top = (params->top + params->bottom - height) / 2;
    } else
      params->bottom = params->top + height;
  } else if (params->top == VALUE_NOT_SET && params->bottom == VALUE_NOT_SET) {
    params->top = params->GetMargin().top();
    params->bottom = params->top + height;
  } else if (params->top != VALUE_NOT_SET && params->bottom != VALUE_NOT_SET) {
    params->top = (params->top + params->bottom - height) / 2;
    params->bottom = params->top + height;
  }
}

RelativeLayoutParams* RelativeLayout::GetRelatedViewParams(int id) {
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

LayoutParams* RelativeLayout::CreateLayoutParams() {
  return new RelativeLayoutParams;
}
}  // namespace Layout
