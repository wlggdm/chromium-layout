#ifndef _LAYOUT_BUILDER_H_
#define _LAYOUT_BUILDER_H_

#include <memory>
#include <string>

#include "base/functional/bind.h"

namespace views {
class View;
}

/*
// 1. register controls first
REGISTER_LAYOUT_CONTROL(View, views)
REGISTER_LAYOUT_CONTROL(Button, views)
...

// 2. create view from resource
std::string xml_resource = "<button .../>";
std::unique_ptr<views::View> view = Layout::Build(xml_resource);
*/

namespace Layout {

using ViewBuilder = base::RepeatingCallback<std::unique_ptr<views::View>()>;

void RegisterViewBuilder(const std::string& key, ViewBuilder view_builder);

std::unique_ptr<views::View> Build(const std::string& content);

#define _REGISTER_NAMESPACE_CLASS_(class_name, namespace_name)   \
  Layout::RegisterViewBuilder(                                   \
      #namespace_name "::" #class_name,                          \
      base::BindRepeating([]() -> std::unique_ptr<views::View> { \
        return std::make_unique<namespace_name::class_name>();   \
      }))

#define _REGISTER_CLASS_(class_name)                                          \
  Layout::RegisterViewBuilder(                                                \
      #class_name, base::BindRepeating([]() -> std::unique_ptr<views::View> { \
        return std::make_unique<class_name>();                                \
      }))

#define _GET_LAYOUT_MACRO_NAME(_1, _2, NAME, ...) NAME

#define REGISTER_LAYOUT_CONTROL(class_name, ...)                       \
  _GET_LAYOUT_MACRO_NAME(class_name, ##__VA_ARGS__,                    \
                         _REGISTER_NAMESPACE_CLASS_, _REGISTER_CLASS_) \
  (class_name, ##__VA_ARGS__)
}  // namespace Layout

#endif
