#include "tesql.h"
#include "tegui.h"
#include "telogic.h"

#include "aho_corasick/aho_corasick.hpp"
#include "sqlite/sqlite3.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <thread>
#include <utility>
#include <vector>
#include <map>
#include <unordered_map>
#include <array>
#include <thread>
#include <mutex>
#include <algorithm>


const unsigned int selection_length = 4;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{    
    //ST CONF.
    for (int i = 0; i < selection_length; i++)
    {
        path_map[i] = std::make_pair("", "");
    }


    //TREE CONF.
    int rc;    

    rc = sqlite3_open("Data.db", &db);

    //FIRST INIT.
    if(rc) 
    {      
        //HATA
    }
    else 
    {
        //OKEY
        const char *const qsql = "select * from Error_List_Table";
        char *err = nullptr;

        rc = sqlite3_exec(db, qsql, &sql_init_callback, nullptr, &err);
        if( rc != SQLITE_OK )
        {      
            //HATA
            sqlite3_free(err);
        }    
        else
        {
      
        }
    }        

    //GUI
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    const auto width = mode->width;
    const auto height = mode->height;
    GLFWwindow* window = glfwCreateWindow(width, height, "SBSEEExtreme", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;  

    glViewport(0, 0, width, height);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    bool mockbb = false;
    ImGuiWindowFlags window_flags = 0;
    
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    style.WindowBorderSize = 1;
    style.FrameBorderSize = 1;
    style.PopupBorderSize = 1;
    style.WindowPadding = ImVec2(20.0f, 20.0f);
    style.FramePadding = ImVec2(10.0f, 5.0f);
    style.ItemSpacing = ImVec2(20.0f, 5.0f);
    style.WindowRounding = 12.0f;
    style.ChildRounding = 12.0f;
    style.FrameRounding = 12.0f;
    style.PopupRounding = 12.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = 0;
    style.SelectableTextAlign = ImVec2(0.5f, 0.5f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    bool first_time = true;
    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        if (first_time) [[unlikely]]
        {
            ImGui::OpenPopup("SBSTripleE");            
            ImGui::SetNextWindowSize(ImVec2(300, 135));
        }
        else
        {
            ImGui::Begin("-__-", &mockbb, window_flags);

            draw_file_gui();

            ImGui::End();


            ImGui::Begin("Errors", &mockbb, window_flags);

            draw_db_gui(db);

            ImGui::End();


            ImGui::Begin("Output", &mockbb, window_flags);

            draw_st_output();

            ImGui::End();
        }
        
        if (ImGui::BeginPopupModal("SBSTripleE", nullptr, 0))
        {
            ImGui::Text("Created and designed by Resat Albay");
            ImGui::Text("Contact: resatalbay@yahoo.com");
            first_time = false;

            if (ImGui::Button("Go to the Program"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::Render();
        

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    sqlite3_close(db);
    return 0;
}