#include "tegui.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "OpenXLSX/OpenXLSX/OpenXLSX.hpp"

#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <cstdlib>

std::map<unsigned int, std::pair<std::string, std::string>> path_map;
std::vector<std::string> teostream;

void at_file_logic_clear_single(int id);
void at_file_logic_clear();
void at_file_logic();
void at_logic();

__attribute__((always_inline)) void draw_file_info(const std::pair<std::string, std::string> &file_name, const std::unordered_map<std::string, size_t> &values)
{
    teostream.emplace_back(teochar(tteos::normal) + "Error counts for the file: " + file_name.first + '\n');
    for (const auto &v: values)
    {
        teostream.emplace_back(teochar(tteos::normal) + v.first + ": " + std::to_string(v.second) + '\n');        
    }

    OpenXLSX::XLDocument doc;
    doc.create(file_name.second + ".xlsx");
    auto wks = doc.workbook().worksheet("Sheet1");

    int iii = 1;
    for (const auto &v: values)
    {
        std::string err = "A" + std::to_string(iii);
        std::string cnt = "B" + std::to_string(iii);
        wks.cell(err).value() = v.first;
        wks.cell(cnt).value() = v.second;

        iii++;
    }

    doc.save();
}

void draw_file_gui()
{   
    ImVec2 xy = ImGui::GetWindowSize();

    //OPEN FILE DIALOG    
    if (ImGui::Button("Open File Dialog", ImVec2(xy.x / 2, 25)))
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".*,.txt,.log,.TXT,.LOG", ".", 0);

    //CLEAR SELECTION
    ImGui::SameLine();
    if (ImGui::Button("Clear the Selection", ImVec2(xy.x / 2, 25)))
    {
        
        //CLEAR MAP
        for (auto &p : path_map)
        {
            p.second.first = "";
            p.second.second = "";
        }

        at_file_logic_clear();
    }

    //MAIN LOGIC FUNC.
    if (ImGui::Button("Start the process", ImVec2(xy.x, 25)))
    {
        at_logic();

        ImGui::OpenPopup("Process completed"); 
    }

    if (ImGui::BeginPopupModal("Process completed", nullptr, 0))
    {        
        ImGui::Text("PROCESS COMPLETED.");
        if (ImGui::Button("OKAY"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            //CLEAR MAP
            for (auto &p: path_map)
            {
                p.second.first = "";
                p.second.second = "";
            }

            int iii = 0;
            for (const auto &f : ImGuiFileDialog::Instance()->GetSelection())
            {
                path_map[iii] = std::make_pair(f.first, f.second);
                iii++;
            }

            teostream.emplace_back(teochar(tteos::info) + std::to_string(iii) + " entry successfully selected.\n");
            
            at_file_logic();
        }

        ImGuiFileDialog::Instance()->Close();
    }
    
    //ADD THE SELECTIONS
    static bool mockb = true;
    int ii = 0;
    for (auto &p: path_map)
    {
        ImGui::PushID(p.first);

        if (p.first % 2 == 1)
            ImGui::SameLine();

        if (p.second.first == "" && p.second.second == "")
        {
            ImGui::Selectable("No file.", mockb, 0, ImVec2(xy.x / 2, 50));
        }
        else
        {
            if (ImGui::Selectable(p.second.first.c_str(), mockb, 0, ImVec2(xy.x / 2, 50)))
            {
                p.second.first = "";
                p.second.second = "";

                at_file_logic_clear_single(ii);
            }                
            ii++;
        }

        ImGui::PopID();
    }
}

void draw_st_output()
{
    if (ImGui::BeginListBox("##Output", ImVec2(-FLT_MIN, 25 * ImGui::GetTextLineHeightWithSpacing())))
    {
        for (const auto s: teostream)
        {
            std::string ssttrr = s.substr(1);
            if (s[0] == teochar(tteos::normal)[0])
                ImGui::TextWrapped(ssttrr.c_str());
            else if (s[0] == teochar(tteos::error)[0])
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 0.0f));
                ImGui::TextWrapped(ssttrr.c_str());
                ImGui::PopStyleColor();
            }
            else if (s[0] == teochar(tteos::info)[0])
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.13f, 0.54f, 0.13f, 0.0f));
                ImGui::TextWrapped(ssttrr.c_str());
                ImGui::PopStyleColor();
            }
        }        

        ImGui::EndListBox();
    }
}