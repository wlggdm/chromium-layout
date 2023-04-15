# relative layout

relative layout for chromium ui layout

# using layout

1. Register view for  layout builder

```c
REGISTER_LAYOUT_CONTROL(Button, views);
REGISTER_LAYOUT_CONTROL(RelativeView);
```

2. Write a layout xml file like this

```c
<?xml version="1.0" encoding="utf-8"?>
<RelativeView>
     <views::Button id="@+id/top"
          layout_width="0"
          layout_height="100"
          layout_left_to_right="@id/left"
          layout_right_to_right="parent"
          layout_top_to_top="parent"
          Text="top"/>
     <views::Button id="@+id/right"
          layout_width="100"
          layout_top_to_bottom="@id/top"
          layout_right_to_right="parent"
          layout_bottom_to_bottom="parent"
          Text="right"/>
     <views::Button id="@+id/bottom"
          layout_height="100"
          layout_left_to_left="parent"
          layout_right_to_left="@id/right"
          layout_bottom_to_bottom="parent"
          Text="bottom"/>
     <views::Button id="@+id/left"
          layout_width="100"
          layout_left_to_left="parent"
          layout_top_to_top="parent"
          layout_bottom_to_top="@id/bottom"
          Text="left"/>   
     <views::Button id="@+id/middle"
          layout_width="100"
          layout_height="100"
          layout_left_to_right="@id/left"
          layout_right_to_left="@id/right"
          layout_top_to_bottom="@id/top"
          layout_bottom_to_top="@id/bottom"
          Text="middle"/>
</RelativeView>
```
you can find a xml example named 1.xml in project folder

3. Load xml resource to build views

```c
ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
std::unique_ptr<View> view = Layout::Build(rb.LoadDataResourceString(IDR_LAYOUT_1_XML));
```

4. Use the generated id file to get view

```c
//layout_id.h
namespace R::id { 

constexpr int Left                      = 0x7F0F0000;
constexpr int Right                     = 0x7F0F0001;
constexpr int Top                       = 0x7F0F0002;
constexpr int Bottom                    = 0x7F0F0003;
constexpr int Middle                    = 0x7F0F0004;

} //namespace R::id
```

# Effect

![image](https://user-images.githubusercontent.com/11361001/137422245-15cac355-97e6-49a8-925c-24af6fd93285.png)

# Requirements
* chromium 90+

# Authors
* wlggdm(wlggdm@163.com)
