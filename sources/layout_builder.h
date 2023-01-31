#ifndef _LAYOUT_BUILDER_H_
#define _LAYOUT_BUILDER_H_

#include <memory>
#include <string>

namespace views {
class View;
}

/*
// 1. register controls first
REGISTER_LAYOUT_CONTROL(View)
REGISTER_LAYOUT_CONTROL(Button)
...

// 2. create view from resource
std::string xml_resource = "<button .../>";
std::unique_ptr<views::View> view = Layout::CreateView(xml_resource);
*/

namespace Layout {

class ViewBuilder {
 public:
  virtual std::unique_ptr<views::View> Build() = 0;
};

template <typename T>
class ViewBuilderImpl : public ViewBuilder {
 public:
  static_assert(std::is_constructible_v<T>, "class need default constructor");
  static_assert(std::is_base_of_v<views::View, T>,
                "class need derived from View");

  static ViewBuilder* CreateObject() { return new ViewBuilderImpl<T>(); }

 private:
  ViewBuilderImpl() = default;
  std::unique_ptr<views::View> Build() override {
    return std::make_unique<T>();
  }
};

void RegisterViewBuilder(const std::string& key, ViewBuilder* view_builder);

std::unique_ptr<views::View> CreateView(const std::string& content);
}  // namespace Layout

#define _REGISTER_NAMESPACE_CLASS_(class_name, namespace_name) \
  Layout::RegisterViewBuilder(                                 \
      #namespace_name "::" #class_name,                        \
      Layout::ViewBuilderImpl<namespace_name::class_name>::CreateObject());

#define _REGISTER_CLASS_(class_name) \
  Layout::RegisterViewBuilder(       \
      #class_name, Layout::ViewBuilderImpl<class_name>::CreateObject());

#define _GET_LAYOUT_MACRO_NAME(_1, _2, NAME, ...) NAME

#define REGISTER_LAYOUT_CONTROL(class_name, ...)                       \
  _GET_LAYOUT_MACRO_NAME(class_name, ##__VA_ARGS__,                    \
                         _REGISTER_NAMESPACE_CLASS_, _REGISTER_CLASS_) \
  (class_name, ##__VA_ARGS__)

#endif
