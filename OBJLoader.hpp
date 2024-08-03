/*
    Created by LuckyIntel from github.

    Version 0.1

    Vertices, normals, texture coords and indices are implemented.
    WARNING : This code here handles everything about OBJ.EXCEPT 
    triangulating.I haven't implemented triangulating yet so you 
    have to triangulate the model before you export in your 3d 
    modelling app such as blender or anything else.However, i am 
    thinking of adding an triangulater in the next version.

    TODO LIST (For V0.2) :
     1. Add more face types
     2. Create a triangulater
*/

#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP
#include <iostream>
#include <cstring>
#include <fstream>
#include <iterator>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>

struct ModelARGS
{
    std::vector<float> vertices; // v/vn/vt
    std::vector<unsigned int> indices; // f
    std::vector<std::string> mtlPath; // only map_Kd for now.
    std::vector<std::string> textures; // Textures
    glm::vec3 color = glm::vec3(0.0f); // Color
};

class ImportOBJ
{
    private:
        void parseFile(std::string path);
        std::vector<ModelARGS> datas;
    public:
        ImportOBJ(std::string path) {parseFile(path);};
        std::vector<ModelARGS> getContents();
        void dump();
};

void ImportOBJ::parseFile(std::string path)
{
    std::vector<std::string> fileContent;
    std::ifstream OBJfile(path);
    std::string reader;

    while (std::getline(OBJfile, reader))
    {
        fileContent.insert(fileContent.end(), reader);
    };

    //std::cout << "Parsing OBJ file\n";
    ModelARGS margs;
    char lastObject[1024];
    char mtlFile[1024];
    //int vCount = 0, fCount = 0;
    std::vector<std::vector<float>> v, vn, vt;
    for (std::string elements : fileContent)
    {
        std::vector<std::string>::iterator it = std::find(fileContent.begin(), fileContent.end(), elements.data());
        if (elements[0] == 'o')
        {
            if (std::strcmp(lastObject, "") == 0)
            {
                for (int i = 0; i < v.size(); i++)
                {
                    margs.vertices.insert(margs.vertices.end(), v[i].begin(), v[i].end());
                    if (vn.size() > 0) margs.vertices.insert(margs.vertices.end(), vn[i].begin(), vn[i].end());
                    else
                    {
                        std::vector<float> emptyvn = {0.0f, 0.0f, 0.0f};
                        margs.vertices.insert(margs.vertices.end(), emptyvn.begin(), emptyvn.end());
                    };
                    if (vt.size() > 0) margs.vertices.insert(margs.vertices.end(), vt[i].begin(), vt[i].end());
                    else
                    {
                        std::vector<float> emptyvt = {0.0f, 0.0f};
                        margs.vertices.insert(margs.vertices.end(), emptyvt.begin(), emptyvt.end()); 
                    };
                };
                datas.push_back(margs);
                ModelARGS margs;
                v.clear();
                vn.clear();
                vt.clear();
                v.shrink_to_fit();
                vn.shrink_to_fit();
                vt.shrink_to_fit();
            };
            sscanf(elements.c_str(), "o %s", &lastObject);
            //std::cout << "object is " << lastObject << "\n";
        }
        else if (elements[0] == 'm')
        {
            //std::cout << "found mttlib\n";
            sscanf(elements.c_str(), "mtllib %s", &mtlFile);
        }
        else if (elements[0] == 'u')
        {
            //std::cout << "found usemtl\n";
            char mtu[1024];
            sscanf(elements.c_str(), "usemtl %s", &mtu);
            margs.mtlPath.push_back(mtu);
        }
        else if (elements[0] == 'v' && elements[1] != 't')
        {
            float v1, v2, v3;
            //std::cout << "found v/vn\n";
            //vCount++;
            if (elements[1] != 'n') 
            {
                sscanf(elements.c_str(), "v %f %f %f", &v1, &v2, &v3);
                v.push_back({v1, v2, v3});
            }
            else 
            {
                sscanf(elements.c_str(), "vn %f %f %f", &v1, &v2, &v3);
                vn.push_back({v1, v2, v3});
            };
        }
        else if (elements[0] == 'v' && elements[1] == 't')
        {
            float v1, v2;
            //std::cout << "found vt\n";
            //vCount++;
            sscanf(elements.c_str(), "vt %f %f", &v1, &v2);
            vt.push_back({v1, v2});
        }
        else if (elements[0] == 'f')
        {
            unsigned int vf1, vf2, vf3;
            //std::cout << "found f\n";
            //fCount++;
            if (elements.find("//") != std::string::npos)
            {
                sscanf(elements.c_str(), 
                "f %i//%*i %i//%*i %i//%*i", 
                &vf1, &vf2, &vf3);
                //std::cout << "found //\n";
            }
            else
            {
                sscanf(elements.c_str(), 
                "f %i/%*i/%*i %i/%*i/%*i %i/%*i/%*i", 
                &vf1, &vf2, &vf3);
                //std::cout << "found /\n";
            };

            margs.indices.push_back(vf1 - 1);
            margs.indices.push_back(vf2 - 1);
            margs.indices.push_back(vf3 - 1);
        };
        if (it - fileContent.begin() == fileContent.size() - 1)
        {
            for (int i = 0; i < v.size(); i++)
            {
                margs.vertices.insert(margs.vertices.end(), v[i].begin(), v[i].end());
                if (vn.size() > 0) margs.vertices.insert(margs.vertices.end(), vn[i].begin(), vn[i].end());
                else
                {
                    std::vector<float> emptyvn = {0.0f, 0.0f, 0.0f};
                    margs.vertices.insert(margs.vertices.end(), emptyvn.begin(), emptyvn.end());
                };
                if (vt.size() > 0) margs.vertices.insert(margs.vertices.end(), vt[i].begin(), vt[i].end());
                else
                {
                    std::vector<float> emptyvt = {0.0f, 0.0f};
                    margs.vertices.insert(margs.vertices.end(), emptyvt.begin(), emptyvt.end()); 
                };
            };
            datas.push_back(margs);
            v.clear();
            vn.clear();
            vt.clear();
            v.shrink_to_fit();
            vn.shrink_to_fit();
            vt.shrink_to_fit();
            //std::cout << "EOL\n";
        };
    };
    //std::cout << vCount << " | " << fCount << "\n";
    
    if (path.find_last_of("/") != std::string::npos)
    {
        path = path.replace(path.find(path.substr(path.find_last_of("/") + 1)), path.size(), reinterpret_cast<const char*>(mtlFile));
    } else path = mtlFile;

    std::ifstream MTLFile(path);
    fileContent.clear();
    fileContent.shrink_to_fit();
    while (std::getline(MTLFile, reader))
    {
        fileContent.insert(fileContent.end(), reader);
    };

    //std::cout << "Parsing MTL file\n";
    char currentMtrl[1024];
    char imagePath[1024];
    int currentObject;
    std::vector<std::string> used_textures;
    for (std::string elements : fileContent)
    {
        //std::vector<std::string>::iterator it = std::find(fileContent.begin(), fileContent.end(), elements.data());
        if (elements[0] == 'n')
        {
            sscanf(elements.c_str(), "newmtl %s", &currentMtrl);
            for (int i = 0; i < datas.size(); i++)
            {
                for (int j = 0; j < datas[i].mtlPath.size(); j++)
                {
                    if (datas[i].mtlPath[j] != currentMtrl) continue;
                    currentObject = i;
                };
            };
        }
        else if (elements.find("Kd") == 0)
        {
            //std::cout << "Found diffuse color\n";
            float r, g, b;
            sscanf(elements.c_str(), "Kd %f %f %f", &r, &g, &b);
            datas[currentObject].color = glm::vec3(r, g, b);
        }
        else if (elements[0] == 'm' && elements.find("Kd") == 4)
        {
            //std::cout << "Found diffuse map\n";
            bool used = false;
            sscanf(elements.c_str(), "map_Kd %s", &imagePath);
            std::string texturePath(imagePath);
            if (texturePath.find_last_of("/") == std::string::npos)
            {
                texturePath = path.replace(path.find(path.substr(path.find_last_of("/") + 1)), path.size(), reinterpret_cast<const char*>(imagePath));
            } else texturePath = imagePath;

            for (int i = 0; i < used_textures.size(); i++)
            {
                if (std::strcmp(used_textures[i].c_str(), texturePath.c_str()) == 0)
                {
                    used = true;
                    datas[currentObject].textures.push_back(used_textures[i]);
                    break;
                };
            };

            if (!used)
            {
                used_textures.push_back(texturePath);
                datas[currentObject].textures.push_back(texturePath);
            };
        };
        /*
        if (it - fileContent.begin() == fileContent.size() - 1)
        {
            //std::cout << "EOL\n";
        };
        */
    };
    //std::cout << "Finished\n";
    fileContent.clear();
    used_textures.clear();
    fileContent.shrink_to_fit();
    used_textures.shrink_to_fit();
};

std::vector<ModelARGS> ImportOBJ::getContents()
{
    //std::cout << datas[0].textures[0] << "\n";
    return datas;
};

void ImportOBJ::dump()
{
    datas.clear();
    datas.shrink_to_fit(); // datas.capacity() is now 0 this frees us some memory
};

#endif