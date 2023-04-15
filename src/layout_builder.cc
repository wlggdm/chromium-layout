#include "layout_builder.h"

#include "layout_params.h"

#include <map>
#include <stack>

#include "base/memory/ptr_util.h"
#include "base/strings/utf_string_conversions.h"
#include "third_party/expat/src/expat/lib/expat.h"
#include "ui/views/layout/layout_manager.h"
#include "ui/views/view.h"

namespace Layout {
namespace {
std::map<std::string, ViewBuilder> viewbuilder_map;

struct ScopedPtrXMlParser {
  void operator()(XML_Parser parser) const { XML_ParserFree(parser); }
};

using XML_ParserPtr =
    std::unique_ptr<std::remove_pointer_t<XML_Parser>, ScopedPtrXMlParser>;

std::unique_ptr<views::View> CreateView(const std::string& class_name) {
  auto iter = viewbuilder_map.find(class_name);
  if (iter != viewbuilder_map.end())
    return iter->second.Run();

  return nullptr;
}

struct XMLRecursive {
  void StartElement(const XML_Char* name, const XML_Char** attrs) {
    // 1. build view
    auto* view = CreateView(name).release();
    DCHECK(view);

    // 2. create parent->child relationship
    if (root == nullptr) {
      root = view;
    } else {
      record.top()->AddChildView(view);
    }

    record.push(view);

    // 3. parse property
    auto* meta_data = view->GetClassMetaData();
    LayoutParams* layout_params = LayoutParams::Get(view);

    for (int i = 0; attrs[i]; i += 2) {
      auto* member_metadata = meta_data->FindMemberData(attrs[i]);
      std::u16string value = base::UTF8ToUTF16(attrs[i + 1]);

      if (member_metadata) {
        member_metadata->SetValueAsString(view, value);
      } else if (layout_params) {
        layout_params->SetValueAsString(attrs[i], value);
      }
    }
  }

  void EndElement(const XML_Char* name) { record.pop(); }

  views::View* root = nullptr;
  std::stack<views::View*> record;
};

void StartElement(void* userData, const XML_Char* name, const XML_Char** atts) {
  XMLRecursive* xml_recursive = reinterpret_cast<XMLRecursive*>(userData);
  xml_recursive->StartElement(name, atts);
}

void EndElement(void* userData, const XML_Char* name) {
  XMLRecursive* xml_recursive = reinterpret_cast<XMLRecursive*>(userData);
  xml_recursive->EndElement(name);
}
}  // namespace

void RegisterViewBuilder(const std::string& key, ViewBuilder view_builder) {
  viewbuilder_map[key] = std::move(view_builder);
}

std::unique_ptr<views::View> Build(const std::string& content) {
  XMLRecursive xml_recursive;

  XML_ParserPtr parser(XML_ParserCreate(nullptr));

  XML_SetUserData(parser.get(), &xml_recursive);
  XML_SetElementHandler(parser.get(), StartElement, EndElement);

  if (XML_Parse(parser.get(), content.c_str(), content.size(), 1) ==
      XML_STATUS_ERROR)
    return nullptr;

  return base::WrapUnique(xml_recursive.root);
}
}  // namespace Layout
