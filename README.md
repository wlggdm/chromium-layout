# relative layout

relative layout for chromium ui layout

# using layout

* First you need write a layout xml file like this

```c
<?xml version="1.0" encoding="utf-8"?>
<RelativeView>
     <Button id="@+id/top"
          layout_width="0"
          layout_height="100"
          layout_left_to_right="@id/left"
          layout_right_to_right="parent"
          layout_top_to_top="parent"
          Text="top"/>
     <Button id="@+id/right"
          layout_width="100"
          layout_top_to_bottom="@id/top"
          layout_right_to_right="parent"
          layout_bottom_to_bottom="parent"
          Text="right"/>
     <Button id="@+id/bottom"
          layout_height="100"
          layout_left_to_left="parent"
          layout_right_to_left="@id/right"
          layout_bottom_to_bottom="parent"
          Text="bottom"/>
     <Button id="@+id/left"
          layout_width="100"
          layout_left_to_left="parent"
          layout_top_to_top="parent"
          layout_bottom_to_top="@id/bottom"
          Text="left"/>   
     <Button id="@+id/middle"
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

* Second load xml resource to initial views
```c
SetUseDefaultFillLayout(true);
ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
View* v = Layout::CreateView(rb.LoadDataResourceString(IDR_LAYOUT_1_XML));
AddChildView(v);
```
* Third you can use the generated id file to get view
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
sources/res/1.xml

![image](https://user-images.githubusercontent.com/11361001/137422245-15cac355-97e6-49a8-925c-24af6fd93285.png)

# Requirements
* chromium 90+

# Authors
* wlggdm(wlggdm@163.com)
