//
// Created by kgaft on 5/23/23.
//

#pragma once

#include <External/ImGUI/imgui.h>
#include <stdexcept>



class UIBase{
    friend class UIManager;
private:

    const char* windowTitle = nullptr;
protected:
    UIBase(const char* windowTitle) : windowTitle(windowTitle){

    }

    virtual void drawCall() = 0;

private:
    void update(){
        if(windowTitle==nullptr){
            throw std::runtime_error("failed to draw window!");
        }
        ImGui::Begin(windowTitle);
        drawCall();
        ImGui::End();
    }
};
