#ifndef GAME_MAIN_HPP
#define GAME_MAIN_HPP

#include "vulkan/vulkan_core.h"
#include <project_setup.hpp>
#include <data_types.hpp>
#include "game_model.hpp"

#include <iostream>
//number of asteroids, checkpoints and powerups
#define ASTEROIDS   15
#define CHECKPOINTS 7
#define POWERUPS    9

std::ostream& operator<<(std::ostream& stream, glm::vec3& vec);

class GameMain : public BaseProject {
protected:
    // Used to sotre Aspect ratio
    float Ar;

    // Define the layout for a new descriptor set
    DescriptorSetLayout
        DSLSun,
        DSLEarth,
        DSLUniverse,
        DSLSPaceShip,
        DSLTorus,
        DSLAsteroids,
        DSLCrystal,
        DSLText,
        DSLPToonLight;

    // Define a new of type Vertex Descriptor
    VertexDescriptor
        VUniverse,
        VSpaceShip,
        VAsteroids,
        VEarth,
        VTorus,
        VUV,
        VNorm,
        VNormUV,
        VNormTanUV,
        VText,
        VSun;
        

    // Create a new custom pipeline
    Pipeline
        PPlain,
        PMesh,
        PTorus,
        PAsteroids,
        PSun,
        PEarth,
        PCrystal,
        PText;

    // Objects to keep model data, be sure to use the proper
    // Vertex descriptor (the one which match the model
    //   data structure)
    // You can check them in
    // src/lib/data_types.hpp
    Model<VertexUV>
        MUniverse,
        MSun; 
    Model<VertexNormUV>
        MMesh;
    Model<VertexNormTanUV>
        MAsteroids;
    Model<VertexNorm>
        MCrystal;
    Model<VertexTorus>
        MTorus;
      Model<VertexEarth>
        MEarth;      
    Model<VertexText>
        MText,
        MBoost;

    // Objects to keep texture data
    // When creating a new one, be sure to update
    // src/game/game_main.cpp:6
    Texture
        TUniverse,
        TMesh,
        TSun,
        T,
        TEarth,
        TAsteroids,
        TTorus,
        TAsteroidsNormMap,
        TText,
        TToon,
        TBoost;
    
    // Create a new descriptor set for your pipeline
    // Remember to update
    // src/game/game_main.cpp:7
    DescriptorSet
        DSSunLight,
        DSSun,
        DSEarth,
        DSUniverse,
        DSMesh,
        DSTorus,
        DSAsteroids[ASTEROIDS],
        DSCrystal[POWERUPS],
        DSText,
        DSPToonLight,
        DSBoost;

    // Uniform Blocks Objects are data passed to the GPU
    // Create a new object to pass data to the GPU
    // Be sure to use the right tipe or to define your own if
    // It does not exist, you can define them at
    // src/lib/data_types.hpp

    // UBO for sun pointlight
    GlobalUniformBlockPointLight
        guboPLSun,
        guboPLCrystal;
    
    // UBO for elements whiich only need a model and a texture
    PlainUniformBlock
        uboSun,
        uboUniverse;
    
    // UBO for meshes (elements which interact with light)
    MeshUniformBlock
        uboTorus,
        uboEarth,
        uboMesh,
        uboCrystal;
    
    TextUniformBlock
        uboText,
        uboBoost;
    // Define matrices statically used by the program
    // EG: matricess to properly scale the sun or the universe
    // You can initialize them at
    // src/game/loader.cpp
    glm::mat4
        I = glm::mat4(1),   // Since we use it a lot
        USun, //for the sun scaling
        UEarth, //for the planet Earth scaling  
        Uast,
        UGWM;

    void setWindowParameters();
    void onWindowResize(int w, int h);

    void localInit();
    void pipelinesAndDescriptorSetsInit();
    void pipelinesAndDescriptorSetsCleanup();
    void localCleanup();
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage);

    void updateUniformBuffer(uint32_t currentImage);

    void gameLogic(GameModel& game);

    void drawScreen(GameModel& game, uint32_t currentImage);
};

#endif//GAME_MAIN_HPP