//
// Created by kgaft on 5/23/23.
//
#pragma once

#include "UIBase.h"
#include "../Pipelines/GameAssebmlyPipeline.h"
class PBRSetupWindow : public UIBase{
public:
    PBRSetupWindow(LightConfiguration& lightConfig) : UIBase("PBR Setting"), lightConfig(lightConfig){

    }
private:
    LightConfiguration& lightConfig;
protected:
    void drawCall() override {
        ImGui::SliderFloat("Emissive intens", &lightConfig.emissiveIntensity, 0, 100);
        ImGui::SliderFloat("Emissive shininess", &lightConfig.emissiveShininess, 0, 100);
        ImGui::SliderFloat("Gamma correct", &lightConfig.gammaCorrect, 0, 1);
        ImGui::SliderFloat("Ambiend intensity", &lightConfig.ambientIntensity, 0, 1);
    }
};