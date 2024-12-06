# ImGui

������һЩ���õ�ImGui��������ڹ���ͼ�ν���Ԫ�أ��ر��Ƕ�����չʾ�����ƺ͵��Ժ��а�����

1. **�������ںͲ�������**

   - `ImGui::Begin("��������");` �� `ImGui::End();`�����ڴ���һ�����ڣ����ڴ����ڰ��������ؼ���
   - `ImGui::BeginChild("�Ӵ�������");` �� `ImGui::EndChild();`���ڴ����ڴ����Ӵ��ڣ�����Ƕ�ײ��֡�
   - `ImGui::SameLine();`�����������Ŀؼ�����ͬһ�С�

2. **�ı��Ͱ�ť**

   - `ImGui::Text("��ʾ���ı�");`����ʾһ���ı���
   - `ImGui::Button("��ť����");`������һ����ť��
   - `ImGui::BulletText("������ı�");`������������ı����������б���ʾ��
   - `ImGui::Selectable("��ѡ����", &isSelected);`������һ����ѡ����б��

3. **����ͻ���ؼ�**

   - `ImGui::InputText("��ǩ", buffer, buffer_size);`�������ı������`buffer`Ϊ�ַ����顣
   - `ImGui::InputInt("��ǩ", &value);`�����������
   - `ImGui::InputFloat("��ǩ", &value);`�������������
   - `ImGui::SliderFloat("��ǩ", &value, min, max);`���������������顣
   - `ImGui::SliderInt("��ǩ", &value, min, max);`�������������顣

4. **��ѡ��͵�ѡ��ť**

   - `ImGui::Checkbox("��ǩ", &isChecked);`����ѡ��
   - `ImGui::RadioButton("��ǩ", &isSelected, value);`����ѡ��ť��`value`�������ֲ�ͬ��ť�顣

5. **�����˵�����Ͽ�**

   - `ImGui::BeginCombo("��ǩ", "��ǰѡ����");` �� `ImGui::EndCombo();`������һ����Ͽ������б���
   - `ImGui::Combo("��ǩ", &current_item, items, items_count);`����ݵ������˵�������`items`���ַ������顣

6. **�������ͻ�ͼ**

   - `ImGui::ProgressBar(fraction);`������һ����������`fraction`Ϊ���ȣ�0.0��1.0����
   - `ImGui::PlotLines("��ǩ", data, data_count);`����������ͼ��
   - `ImGui::PlotHistogram("��ǩ", data, data_count);`��������״ͼ��

7. **������ʾ�͵���**

   - `ImGui::IsItemHovered()`����鵱ǰԪ���Ƿ������ͣ��
   - `ImGui::BeginTooltip();` �� `ImGui::EndTooltip();`������һ����ʾ���ڡ�
   - `ImGui::OpenPopup("��������");` �� `ImGui::BeginPopup("��������");` / `ImGui::EndPopup();`������ʽ���ڡ�

8. **���νṹ�Ϳ��۵�����**

   - `ImGui::TreeNode("�ڵ�����");` �� `ImGui::TreePop();`������һ�����ڵ㣬�ʺ�չʾ��νṹ��
   - `ImGui::CollapsingHeader("��������");`������һ�����۵����⣬���ڷ������ݡ�

��Щ������������ϣ����������ж��ֽ����Ľ��档ImGui�ǳ���������࣬�����ʺ���Ҫ���ٿ����ĵ��Թ��ߺͲ��Ի�����