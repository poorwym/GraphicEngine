# ImGui

以下是一些常用的ImGui命令，适用于构建图形界面元素，特别是对数据展示、控制和调试很有帮助：

1. **基础窗口和布局命令**

   - `ImGui::Begin("窗口名称");` 和 `ImGui::End();`：用于创建一个窗口，并在窗口内包含其他控件。
   - `ImGui::BeginChild("子窗口名称");` 和 `ImGui::EndChild();`：在窗口内创建子窗口，用于嵌套布局。
   - `ImGui::SameLine();`：将接下来的控件放在同一行。

2. **文本和按钮**

   - `ImGui::Text("显示的文本");`：显示一行文本。
   - `ImGui::Button("按钮名称");`：创建一个按钮。
   - `ImGui::BulletText("带点的文本");`：创建带点的文本，常用于列表显示。
   - `ImGui::Selectable("可选择项", &isSelected);`：创建一个可选择的列表项。

3. **输入和滑块控件**

   - `ImGui::InputText("标签", buffer, buffer_size);`：创建文本输入框，`buffer`为字符数组。
   - `ImGui::InputInt("标签", &value);`：整数输入框。
   - `ImGui::InputFloat("标签", &value);`：浮点数输入框。
   - `ImGui::SliderFloat("标签", &value, min, max);`：创建浮点数滑块。
   - `ImGui::SliderInt("标签", &value, min, max);`：创建整数滑块。

4. **复选框和单选按钮**

   - `ImGui::Checkbox("标签", &isChecked);`：复选框。
   - `ImGui::RadioButton("标签", &isSelected, value);`：单选按钮，`value`用来区分不同按钮组。

5. **下拉菜单和组合框**

   - `ImGui::BeginCombo("标签", "当前选择项");` 和 `ImGui::EndCombo();`：创建一个组合框（下拉列表）。
   - `ImGui::Combo("标签", &current_item, items, items_count);`：快捷的下拉菜单，其中`items`是字符串数组。

6. **进度条和绘图**

   - `ImGui::ProgressBar(fraction);`：创建一个进度条，`fraction`为进度（0.0到1.0）。
   - `ImGui::PlotLines("标签", data, data_count);`：绘制折线图。
   - `ImGui::PlotHistogram("标签", data, data_count);`：绘制柱状图。

7. **工具提示和弹窗**

   - `ImGui::IsItemHovered()`：检查当前元素是否被鼠标悬停。
   - `ImGui::BeginTooltip();` 和 `ImGui::EndTooltip();`：创建一个提示窗口。
   - `ImGui::OpenPopup("弹窗名称");` 和 `ImGui::BeginPopup("弹窗名称");` / `ImGui::EndPopup();`：弹出式窗口。

8. **树形结构和可折叠内容**

   - `ImGui::TreeNode("节点名称");` 和 `ImGui::TreePop();`：创建一个树节点，适合展示层次结构。
   - `ImGui::CollapsingHeader("标题名称");`：创建一个可折叠标题，用于分组内容。

这些命令可以灵活组合，构建出具有多种交互的界面。ImGui非常轻量、简洁，尤其适合需要快速开发的调试工具和测试环境。