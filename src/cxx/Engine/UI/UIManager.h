//
// Created by kgaft on 5/23/23.
//
#pragma once

#include "UIBase.h"
#include <External/ImGUI/imgui_impl_glfw.h>
#include <External/ImGUI/imgui_impl_vulkan.h>
#include <vector>

class UIManager{
private:
    std::vector<UIBase*> windows;
public:
    void registerWindow(UIBase* window){
        windows.push_back(window);
    }
    void draw(){
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        for (const auto &item: windows){
            item->update();
        }
        ImGui::Render();
    }
    void endDraw(){
        ImGui::EndFrame();
    }

    UIBase* getWindow(int index){
        return windows[index];
    }

};