// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/examples/layout_example.h"
#include <type_traits>

#include "base/files/file_util.h"
#include "ui/gfx/canvas.h"
#include "ui/views/background.h"
#include "ui/views/border.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/button/label_button.h"
#include "ui/views/controls/label.h"
#include "ui/views/examples/layout/grit/layout_resources.h"
#include "ui/views/examples/layout/id.h"
#include "ui/views/examples/layout/layout_builder.h"
#include "ui/views/examples/layout/layout_metadata_impl_macros.h"
#include "ui/views/examples/layout/relative_layout.h"
#include "ui/views/view.h"
#include "ui/views/view_utils.h"

class RelativeView : public views::View {
 public:
  METADATA_HEADER(RelativeView);
  RelativeView() {
    SetLayoutManager(std::make_unique<Layout::RelativeLayout>());
  }
};

BEGIN_METADATA(RelativeView, views::View)
END_METADATA

DEFINE_UI_CLASS_PROPERTY_TYPE(SkColor)
DEFINE_UI_CLASS_PROPERTY_KEY(SkColor, kButtonNormal, SK_ColorWHITE)
DEFINE_UI_CLASS_PROPERTY_KEY(SkColor, kButtonHovered, SK_ColorWHITE)
DEFINE_UI_CLASS_PROPERTY_KEY(SkColor, kButtonPressed, SK_ColorWHITE)
DEFINE_UI_CLASS_PROPERTY_KEY(SkColor, kButtonDisabled, SK_ColorWHITE)

namespace {
/*
std::string xml_resource = R"(<RelativeView>
            <views::Label ID="1"
             layout_width="wrap_content"
             layout_left_to_left="parent"
             layout_top_to_top="parent"
             layout_bottom_to_bottom="parent"
             Text="wang"/>
             <views::Label ID="2"
             layout_width="wrap_content"
             layout_height="wrap_content"
             layout_left_to_right="1"
             layout_top_to_top="parent"
             layout_right_to_right="parent"
             layout_bottom_to_bottom="parent"
             Text="wangleigang"/>
  </RelativeView>)";
*/
#if 1
std::string xml_resource = R"(<RelativeView>
  <views::Label ID="1"
   layout_width="wrap_content"
   layout_height="wrap_content"
   layout_left_to_left="parent"
   layout_top_to_top="parent"
   layout_margin="0,10,0,10"
   Text="Icon"
   LineHeight="30"/>
  <views::Label ID="2"
   layout_height="wrap_content"
   layout_left_to_right="1"
   layout_top_to_top="1"
   layout_right_to_left="3"
   Text="Title"
   LineHeight="30"
   HorizontalAlignment="ALIGN_LEFT"/>
   <views::Label ID="3"
   layout_width="wrap_content"
   layout_height="wrap_content"
   layout_top_to_top="1"
   layout_right_to_right="parent"
   layout_margin="0,10,0,0"
   Text="Close"
   LineHeight="30"/>
  <views::Label ID="4"
   layout_width="wrap_content"
   layout_height="wrap_content"
   layout_left_to_left="2"
   layout_top_to_bottom="2"
   layout_bottom_to_top="5"
   Text="message ..."
   LineHeight="30"/>
  <views::Label ID="5"
   layout_width="wrap_content"
   layout_height="wrap_content"
   layout_right_to_right="parent"
   layout_bottom_to_bottom="parent"
   Text="Ok"
   LineHeight="30"/>
</RelativeView>)";

#else
<views::Label ID = "5" layout_width = "wrap_content" layout_height =
     "wrap_content" layout_left_to_right = "1" layout_top_to_bottom =
         "2" layout_right_to_left = "3" layout_bottom_to_top = "4" Text =
             "middle" LineHeight = "30" MultiLine = "true" /> std::string
    xml_resource = R"(<RelativeView>
            <views::Label ID="1" 
             layout_width="wrap_content" 
             layout_left_to_left="parent"
             layout_top_to_top="parent" 
             layout_bottom_to_top="4" 
             Text="left"/>
             <views::Label ID="2" 
             layout_height="wrap_content"
             layout_left_to_right="1"
             layout_top_to_top="parent" 
             layout_right_to_right="parent" 
             Text="top"
             LineHeight="30"
             MultiLine="true"/>
             <views::Label ID="3" 
             layout_width="wrap_content"
             layout_right_to_right="parent"
             layout_top_to_bottom="2" 
             layout_bottom_to_bottom="parent" 
             Text="right"/>
             <views::Label ID="4" 
             layout_height="wrap_content"
             layout_left_to_left="parent"
             layout_right_to_left="3" 
             layout_bottom_to_bottom="parent" 
             Text="bottom"
             LineHeight="30"
             MultiLine="true"/>
  </RelativeView>)";
#endif
}  // namespace

namespace views::examples {

class ButtonBackground : public views::Background {
 public:
  ~ButtonBackground() override = default;
  void Paint(gfx::Canvas* canvas, views::View* view) const override {
    views::Button* btn = views::AsViewClass<views::Button>(view);
    if (btn == nullptr)
      return;

    gfx::Rect bounds = view->GetLocalBounds();

    views::Button::ButtonState state = btn->GetState();
    SkColor color = SK_ColorWHITE;
    switch (state) {
      case views::Button::ButtonState::STATE_NORMAL: {
        color = view->GetProperty(kButtonNormal);
        break;
      }
      case views::Button::ButtonState::STATE_HOVERED: {
        color = view->GetProperty(kButtonHovered);
        break;
      }
      case views::Button::ButtonState::STATE_PRESSED: {
        color = view->GetProperty(kButtonPressed);
        break;
      }
      case views::Button::ButtonState::STATE_DISABLED: {
        color = view->GetProperty(kButtonDisabled);
        break;
      }
      default:
        break;
    }

    canvas->FillRect(bounds, color);

    //border
    bounds.Inset(1);
    canvas->DrawRect(gfx::RectF(bounds), SK_ColorBLACK);
  }

  //尽管label可以设置文字颜色4态，这里演示多态的情况
  void OnButtonStateChanged(views::LabelButton* button) {
    views::Button::ButtonState state = button->GetState();
    switch (state) {
      case views::Button::ButtonState::STATE_NORMAL: {
        button->SetEnabledTextColors(SK_ColorBLACK);
        break;
      }
      case views::Button::ButtonState::STATE_HOVERED: {
        button->SetEnabledTextColors(SK_ColorRED);
        break;
      }
      case views::Button::ButtonState::STATE_PRESSED: {
        button->SetEnabledTextColors(SK_ColorWHITE);
        break;
      }
      case views::Button::ButtonState::STATE_DISABLED: {
        button->SetEnabledTextColors(SK_ColorWHITE);
        break;
      }
      default:
        break;
    }
  }

  static void Install(views::View* view) {
    view->SetBackground(base::WrapUnique(new ButtonBackground(view)));
  }

  static void SetButtonStateColor(views::View* view,
                                  SkColor normal,
                                  SkColor hovered,
                                  SkColor pressed,
                                  SkColor disabled) {
    view->SetProperty(kButtonNormal, normal);
    view->SetProperty(kButtonHovered, hovered);
    view->SetProperty(kButtonPressed, pressed);
    view->SetProperty(kButtonDisabled, disabled);
  }

 private:
  ButtonBackground(views::View* view) {
    views::LabelButton* button = views::AsViewClass<views::LabelButton>(view);
    if (button == nullptr)
      return;

    state_changed_subscription_ = button->AddStateChangedCallback(
        base::BindRepeating(&ButtonBackground::OnButtonStateChanged,
                            base::Unretained(this), button));
  }

  base::CallbackListSubscription state_changed_subscription_;
};

LayoutExample::LayoutExample() : ExampleBase("Layout") {
  REGISTER_LAYOUT_CONTROL(RelativeView);
  REGISTER_LAYOUT_CONTROL(View, views);
  REGISTER_LAYOUT_CONTROL(Label, views);
  REGISTER_LAYOUT_CONTROL(LabelButton, views);
}

LayoutExample::~LayoutExample() = default;

void LayoutExample::CreateExampleView(View* container) {
  container->SetUseDefaultFillLayout(true);

  std::string content;
  // base::FilePath
  // file_path(L"D:\\chromium\\src\\out\\debug\\gen\\ui\\views\\examples\\layout\\1.xml");
  // base::ReadFileToString(file_path, &content);
  ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
  content = rb.LoadDataResourceString(IDR_LAYOUT_1_XML);

  auto relative_view = Layout::Build(content);  // xml_resource

#if 1  // 1.xml
  views::View* views[4];
  views[0] = relative_view->GetViewByID(R::id::Left);
  views[1] = relative_view->GetViewByID(R::id::Top);
  views[2] = relative_view->GetViewByID(R::id::Right);
  views[3] = relative_view->GetViewByID(R::id::Bottom);

  for (views::View* view : views) {
    ButtonBackground::Install(view);
    ButtonBackground::SetButtonStateColor(view, SK_ColorWHITE, SK_ColorGREEN,
                                          SK_ColorRED, SK_ColorWHITE);
  }
#else  // 2.xml
  views::View* views[2];
  views[0] = relative_view->GetViewByID(R::id::Close);
  views[1] = relative_view->GetViewByID(R::id::Ok);

  for (views::View* view : views) {
    ButtonBackground::Install(view);
    ButtonBackground::SetButtonStateColor(view, SK_ColorWHITE, SK_ColorGREEN,
                                          SK_ColorRED, SK_ColorWHITE);
  }
#endif

  container->AddChildView(std::move(relative_view));
}

}  // namespace views::examples
