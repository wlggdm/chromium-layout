#ifndef LAYOUT_METADATA_IMPL_MACROS_H_
#define LAYOUT_METADATA_IMPL_MACROS_H_

#include "ui/base/metadata/metadata_impl_macros.h"

#define METADATA_LAYOUT_PROPERTY_TYPE_INTERNAL(property_type, property_name, \
                                               method_name, ...)             \
  ui::metadata::ObjectPropertyMetaData<                                      \
      TheClass, property_type, decltype(&TheClass::Set##method_name),        \
      &TheClass::Set##method_name,                                           \
      decltype(std::declval<TheClass>().Get##method_name()),                 \
      &TheClass::Get##method_name, ##__VA_ARGS__>

#define ADD_LAYOUT_PROPERTY_METADATA(property_type, property_name, ...)  \
  auto property_name##_prop =                                            \
      std::make_unique<METADATA_LAYOUT_PROPERTY_TYPE_INTERNAL(           \
          property_type, property_name, ##__VA_ARGS__)>(#property_name,  \
                                                        #property_type); \
  AddMemberData(std::move(property_name##_prop));

#endif  // UI_BASE_METADATA_METADATA_IMPL_MACROS_H_
