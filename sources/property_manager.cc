#include "property_manager.h"

namespace {
constexpr int kGroupShift = 5;
constexpr int kGroupMask = 7 << kGroupShift;

int MakeGroup(uint8_t group, uint8_t index) {
  return (group << kGroupShift) | index;
}


std::map<std::string, uint8_t>& PropertyMap(){
  static std::map<std::string, uint8_t> property_map;
  return property_map;
}

std::map<std::string, int>& PropertyValues() {
  static std::map<std::string, int> property_values;
  return property_values;
}
}  // namespace

namespace Layout {

uint8_t LayoutGroup(uint8_t params) {
  return params >> kGroupShift;
}

uint8_t LayoutIndex(uint8_t params) {
  return params & ~kGroupMask;
}

uint8_t FindProperty(const std::string& property) {
  auto& property_map = PropertyMap();
  auto iter = property_map.find(property);
  if (iter == property_map.end())
    return kGroup_Invalid;

  return iter->second;
}

bool FindValue(const std::string& property, int* value) {
  auto& property_values = PropertyValues();
  auto iter = property_values.find(property);
  if (iter == property_values.end())
    return false;

  *value = iter->second;
  return true;
}

PropertyCollect::PropertyCollect(std::string key,
                                 uint8_t group,
                                 uint8_t index) {
  PropertyMap()[key] = MakeGroup(group, index);
}

 PropertyValue::PropertyValue(std::string key, int value) {
  PropertyValues()[key] = value;
 }

}  // namespace Layout