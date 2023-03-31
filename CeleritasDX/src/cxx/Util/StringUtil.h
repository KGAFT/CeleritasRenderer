#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <cmath>
#include "glm/glm.hpp"
#include <windows.h>
namespace StringUtil{
    void split(std::string& source, std::vector<std::string>& out, char regex){
        out.clear();
        std::string currentString = "";
        for(char element : source){
            if(element!=regex){
                currentString+=element;
            }
            else{
                out.push_back(currentString);
                currentString = "";
            }
        }
        if(currentString.size()>0){
            out.push_back(currentString);
        }
    }

    std::string toLowerCase(std::string& source){
        std::string result = "";
        for(char el : source){
            result+=std::tolower(el);
        }
        return result;
    }

    glm::vec4 stringToVector(std::string& source){
        std::vector<std::string> stringArgs;
        split(source, stringArgs, ' ');
        glm::vec4 result(stof(stringArgs[0]), stof(stringArgs[1]), stof(stringArgs[2]), stof(stringArgs[3]));
        return result;
    }
    glm::vec3 stringToVector3(std::string& source){
        std::vector<std::string> stringArgs;
        split(source, stringArgs, ' ');
        glm::vec3 result(stof(stringArgs[0]), stof(stringArgs[1]), stof(stringArgs[2]));
        return result;
    }
    bool parseBoolean(std::string& source){
        std::string lowerCase = toLowerCase(source);
        if(!std::strcmp(lowerCase.c_str(), "true")){
            return true;
        }
        else{
            return false;
        }
    }
    std::string wcharToString(WCHAR* data){
        std::string res;
        char curChar = data[0];
        unsigned int counter = 1;
        while(curChar!='\0'){
            res+=curChar;
            curChar = data[counter];
            counter++;
        }
        return res;
    }
}