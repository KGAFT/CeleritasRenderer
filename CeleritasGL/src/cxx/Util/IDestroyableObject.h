//
// Created by Daniil on 30.03.2023.
//
#pragma once

class IDestroyableObject{
protected:
    bool destroyed;
public:
    virtual void destroy(){
        destroyed = true;
    }
    ~IDestroyableObject(){
        if(!destroyed){
            destroy();

        }
    }
};