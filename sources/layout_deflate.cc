#include "layout_deflate.h"
#include "layout_attribute.h"
#include "layout_types.h"
#include "layout_params.h"
#include "layout_master.h"

#include <stack>

#include "base/notreached.h"
#include "ui/views/view.h"
#include "expat.h"

namespace {
struct ScopedPtrXMlParser {
  void operator()(XML_Parser parser) const { 
    XML_ParserFree(parser); 
  }
};

using XML_ParserPtr = std::unique_ptr<std::remove_pointer_t<XML_Parser>, ScopedPtrXMlParser>;

}  // namespace

namespace Layout {

struct LayoutDeflate {
  void StartElement(const XML_Char* name, const XML_Char** attrs) {
    static LayoutTypes layout_types;

    //create decoder
    LayoutView* layout_view = LayoutView::GetViewByName(name);
    if (layout_view == nullptr) {
      NOTREACHED();
      return;
    }

    //extract attribute
    layout_types.Reset();
    for (int i = 0; attrs[i]; i += 2)
      layout_types.Add(attrs[i], attrs[i + 1]);
    
    //view = decoder + attribute
    views::View* v = layout_view->Build(layout_types);

    //construct hierarchy
    if (root == nullptr) {
      root = v;
      LayoutMaster* layout_master =
          static_cast<LayoutMaster*>(root->GetLayoutManager());
      layout_master->SetRoot(true);
    } 
    else {
      record.top()->AddChildView(v);

      LayoutParams* layout_params =
          layout_views.top()->BuildLayoutParams(layout_types);

      LayoutParams::Set(v, layout_params);
    }
      

    record.push(v);
    layout_views.push(layout_view);
  }

  void EndElement(const XML_Char* name) { 
    record.pop();
    layout_views.pop();
  }

  void Reset() { 
    root = nullptr;
    while (!record.empty()) {
      record.pop();
      layout_views.pop();
    }
  }

  views::View* root;

  std::stack<views::View*> record;
  std::stack<LayoutView*> layout_views;
};

void StartElement(void* userData, const XML_Char* name, const XML_Char** atts) {
  LayoutDeflate* layout_deflate = reinterpret_cast<LayoutDeflate*>(userData);
  layout_deflate->StartElement(name, atts);
}

void EndElement(void* userData, const XML_Char* name) {
  LayoutDeflate* layout_deflate = reinterpret_cast<LayoutDeflate*>(userData);
  layout_deflate->EndElement(name);
}

views::View* LayoutBuild(const std::string& resource) {
  static LayoutDeflate layout_deflate;

  XML_ParserPtr parser(XML_ParserCreate(NULL));

  XML_SetUserData(parser.get(), &layout_deflate);
  XML_SetElementHandler(parser.get(), StartElement, EndElement);

  layout_deflate.Reset();
  if (XML_Parse(parser.get(), resource.c_str(), resource.size(), 1) ==
      XML_STATUS_ERROR)
    return nullptr;

  return layout_deflate.root;
}

}  // namespace Layout

/*
   LayoutParams     View               LayoutManager
   LayoutParams     |_____View
   LayoutParams     |_____View         LayoutManager
   LayoutParams           |_____View 

   match_parent    exactly
   100             exactly

   wrap_content    wrap_content
   relative        
*/