#include "tesql.h"
#include "tegui.h"

#include "sqlite/sqlite3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <vector>
#include <map>
#include <cstring>
#include <sstream>
#include <string>
#include <filesystem>

sqlite3 *db;
std::map<unsigned int, std::string> errors;
std::map<unsigned int, bool> indexes;
std::vector<int> sql_cb_return;

void recorrect_trie();

void draw_db_gui(sqlite3 *db)
{
    ImVec2 xy = ImGui::GetWindowSize();

    if (ImGui::BeginListBox("##Errors", ImVec2(-FLT_MIN, 25 * ImGui::GetTextLineHeightWithSpacing())))
    {
        for (const auto &e : errors)
        {   
            if (ImGui::Selectable(e.second.c_str(), indexes[e.first]))
            {
                if (!ImGui::GetIO().KeyCtrl)
                {
                    for (auto &i : indexes)
                    {
                        i.second = false;
                    }
                }                      
                indexes[e.first] ^= 1;
            }                
        }
        ImGui::EndListBox();
    }

    if (ImGui::Button("Delete the Entry/Entries", ImVec2(xy.x / 2, 25)))
    {
        for (const auto &i : indexes)
        {
            if (i.second)
            {
                std::stringstream dsql;
                dsql << "delete from Error_List_Table where id=" << i.first << " returning *;";
                int rc;
                char *err = nullptr;
                rc = sqlite3_exec(db, dsql.str().c_str(), &sql_dml_callback, (void *)&sql_cb_return, &err);
            }        
        }
        for (const auto r : sql_cb_return)
        {
            auto it = errors.find(r);
            if (it != errors.cend())
            {
                errors.erase(it);
                teostream.emplace_back(teochar(tteos::info) + it->second + " successfully deleted.\n");
            }
            else
                teostream.emplace_back(teochar(tteos::error) + it->second + " couldn't deleted. There is no such a entry.\n");              

            auto it2 = indexes.find(r);
            if (it2 != indexes.cend())
                indexes.erase(it2);

            recorrect_trie();
        }
        sql_cb_return.clear();
    }    

    ImGui::SameLine();

    if (ImGui::Button("Add an Entry", ImVec2(xy.x / 2, 25)))
    {
        ImGui::OpenPopup("Write the Error");
        ImGui::SetNextWindowSize(ImVec2(750, 100));
    }
    if (ImGui::Button("Update the Database", ImVec2(xy.x, 25)))
    {
        sqlite3_close(db); 

        std::filesystem::remove("Data.db");
        std::filesystem::copy("YOUR SERVER PATH TO DATABASE\\Data.db", "Data.db");

        int rc;
        rc = sqlite3_open("Data.db", &db);

        errors.clear();
        indexes.clear();
        sql_cb_return.clear();
        const char *const qsql = "select * from Error_List_Table";
        char *err = nullptr;

        rc = sqlite3_exec(db, qsql, &sql_init_callback, nullptr, &err);
    }

    static char str1[256] = "";
    if (ImGui::BeginPopupModal("Write the Error", nullptr, 0))
    {
        ImGui::InputTextWithHint("##Input", "Maximum 256 character", str1, IM_ARRAYSIZE(str1));

        ImGui::SameLine();
        if (ImGui::Button("Add to the Database"))
        {
            ImGui::CloseCurrentPopup();

            if (!(strcmp(str1, "") == 0))
            {
                bool found = false;
                for (const auto e: errors)
                {
                    if (std::strcmp(e.second.c_str(), str1) == 0)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    std::stringstream asql;
                    asql << "insert into Error_List_Table(Error_Name) values (\"" << str1 << "\") returning *;";
                    int rc;
                    char *err = nullptr;                    
                    rc = sqlite3_exec(db, asql.str().c_str(), &sql_dml_callback, (void *)&sql_cb_return, &err);

                    errors.emplace(sql_cb_return[0], str1);
                    indexes.emplace(sql_cb_return[0], false);                    
                    std::memset(&str1[0], 0, sizeof(str1));

                    teostream.emplace_back(teochar(tteos::info) + "Entry successfully added.\n");

                    recorrect_trie();
                }
                else
                    teostream.emplace_back(teochar(tteos::error) + std::string(str1) + " entry already exists.\n");                    
            }
            else
                teostream.emplace_back(teochar(tteos::error) + "Empty string cannot be added.\n");                
        }
        ImGui::EndPopup();

    }
}

int sql_dml_callback(void *data, int argc, char **argv, char **col_name)
{
    std::vector<int> *rts = static_cast<std::vector<int>*>(data);    

    for (int i = 0; i < argc; i++)
    {
        if (std::strcmp(col_name[i], "id") == 0)
            rts->emplace_back(std::atoi(argv[i]));
    }
    return 0;
}

int sql_init_callback(void *data, int argc, char **argv, char **col_name)
{
    int id;
    std::string err;
    for (int i = 0; i < argc; i++)
    {
        if (std::strcmp(col_name[i], "id") == 0)
            id = std::atoi(argv[i]);
        else
            err = argv[i];
    }
    errors.emplace(id, err);
    indexes.emplace(id, false);

    return 0;
}