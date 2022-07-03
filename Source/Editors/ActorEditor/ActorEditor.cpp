﻿// ActorEditor.cpp : Определяет точку входа для приложения.
//
#include "stdafx.h"
#include <iostream>
#include <string>

// argv[1] - mode
// argv[2] - object
// argv[3] - ogf / omf / skls || object scale
// argv[4] - data
// argv[5] - object scale

extern ECORE_API BOOL g_force16BitTransformQuant;
extern ECORE_API BOOL g_forceFloatTransformQuant;

int iReaderPos = 0;

enum EExportFlags
{
    exf16Bit            = (1<<0),	
    exfNoCompress		= (1<<1),
    exfMakeProgressive	= (1<<2),	
    exfOptimizeSurfaces	= (1<<3),	
    exfDbgWindow    	= (1<<4),	
    exfScaleCenterMass 	= (1<<5),	
    exfHQGeometryPlus   = (1<<6),	
    exfMakeStripify	    = (1<<7),	
};

enum EditorMode
{
    ExportOGF = 0,
    ExportOMF,
    GenerateShape,
    DeleteMotions,
    LoadMotions,
    SaveSklsMotions,
    LoadBones,
    SaveBones,
    ExportOBJ,
    ExportDM,
    SaveObject,
    LoadBoneParts,
    SaveBoneParts,
    ToDefaultBoneParts,
    SaveSklMotions,
};

xr_vector<CEditableObject::ShapeEditType> LoadShapes(char** args, int count)
{
    xr_vector<CEditableObject::ShapeEditType> vec;

    for (int i = 0; i < count; i++)
    {
        CEditableObject::ShapeEditType shape;
        std::string first = args[iReaderPos];
        first.erase(first.find_first_of("-"), first.size());
        shape.bone_id = atoi(first.c_str());

        std::string second = args[iReaderPos];
        second.erase(second.find_last_of("-"), second.size());
        second.erase(0, first.size() + 1);
        shape.bone_type = atoi(second.c_str());

        std::string third = args[iReaderPos];
        third.erase(0, third.find_last_of("-") + 1);
        shape.bone_flags.assign(atoi(third.c_str()));

        vec.push_back(shape);

        iReaderPos++;
    }

    return vec;
}

xr_vector<CEditableObject::SurfaceParams> LoadSurfaces(char** args, int count)
{
    xr_vector<CEditableObject::SurfaceParams> vec;

    u8 reader = 0;
    int counter = 0;
    CEditableObject::SurfaceParams surface;

    for (int i = 0; i < int(count * 4); i++)
    {
        switch (reader)
        {
        case 0:
            surface.flags = atoi(args[iReaderPos]);
            break;
        case 1:
            surface.texture = args[iReaderPos];
            break;
        case 2:
            surface.shader = args[iReaderPos];
            break;
        }

        reader++;

        if (reader >= 3) // Конец считки, записываем сюрфейс
        {
            reader = 0;
            counter++;
            vec.push_back(surface);
        }

        iReaderPos++;

        if (counter > count - 1)
            break;
    }

    return vec;
}

xr_vector<shared_str> LoadAnims(char** args, int count)
{
    xr_vector<shared_str> vec;

    for (int i = 0; i < count; i++)
    {
        vec.push_back(args[iReaderPos]);

        iReaderPos++;
    }

    return vec;
}

int main(int argc, char** argv)
{
    Core._initialize("Actor", ELogCallback,1, "",true, (atoi(argv[4]) & exfDbgWindow));

    std::cout << "[Arg debugger]" << std::endl;
    std::cout << "Arg count: " << argc << std::endl;
    int size = 0;
    for (int i = 0; i < argc; i++)
    {
        shared_str arg = argv[i];
        size += arg.size();
    }
    std::cout << "Arg size: " << size << std::endl;

    // Program params
    int ret_code = 0;
    int mode = atoi(argv[1]);
    shared_str object_path = argv[2];
    shared_str second_file_path = argv[3];
    int flags = atoi(argv[4]);
    float scale = atof(argv[5]);
    int shapes_count = atoi(argv[6]);
    int surfaces_count = atoi(argv[7]);
    int loaded_skls_count = atoi(argv[8]);
    iReaderPos = 9;
    xr_vector<CEditableObject::ShapeEditType> pShapes = LoadShapes(argv, shapes_count);
    xr_vector<CEditableObject::SurfaceParams> pSurfaces = LoadSurfaces(argv, surfaces_count);
    xr_vector<shared_str> pLoadedAnims = LoadAnims(argv, loaded_skls_count);
    // End of program params

    Tools = xr_new<CActorTools>();
    ATools = (CActorTools*)Tools;

    std::cout << "Import object" << std::endl;
    ATools->LoadScale(object_path.c_str(), scale, (flags & exfScaleCenterMass));
    ATools->CurrentObject()->ChangeSurfaceFlags(pSurfaces);
    ATools->CurrentObject()->ChangeBoneShapeTypes(pShapes);

    if (!ATools->BonePartsExist() && mode != 9)
    {
        ATools->ToDefaultBoneParts();
        std::cout << "Can't find bone parts, reset to default." << std::endl;
    }

    if (flags & exf16Bit)
        g_force16BitTransformQuant = true;
    else if (flags & exfNoCompress)
        g_forceFloatTransformQuant = true;

    if (flags & exfMakeProgressive)
        ATools->CurrentObject()->m_objectFlags.set(CEditableObject::eoProgressive, TRUE);

    if (flags & exfMakeStripify)
        ATools->CurrentObject()->m_objectFlags.set(CEditableObject::eoStripify, TRUE);

    if (flags & exfOptimizeSurfaces)
        ATools->CurrentObject()->m_objectFlags.set(CEditableObject::eoOptimizeSurf, TRUE);

    if (flags & exfHQGeometryPlus)
        ATools->CurrentObject()->m_objectFlags.set(CEditableObject::eoHQExportPlus, TRUE);

    switch (mode)
    {
        case ExportOGF:
        {
            std::cout << "Export OGF" << std::endl;
            if (!ATools->ExportOGF(second_file_path.c_str()))
                ret_code = -1;
        }break;
        case ExportOMF:
        {
            std::cout << "Export OMF" << std::endl;
            if (!ATools->ExportOMF(second_file_path.c_str()))
                ret_code = -1;
        }break;
        case GenerateShape:
        {
            if (!ATools->CurrentObject()->GenerateBoneShape(false) || !ATools->Save(object_path.c_str()))
                ret_code = -1;
        }break;
        case DeleteMotions:
        {
            ATools->CurrentObject()->ClearSMotions();
            if (!ATools->Save(object_path.c_str()))
                ret_code = -1;
        }break;
        case LoadMotions:
        {
            bool res = true;
            for (int i = 0; i < pLoadedAnims.size(); i++)
            {
                if (!ATools->AppendMotion(pLoadedAnims[i].c_str()))
                    res = false;
            }
            if (res)
                res = ATools->Save(object_path.c_str());

            if (!res)
                ret_code = -1;
        }break;
        case SaveSklsMotions:
        {
            if (!ATools->SaveMotions(second_file_path.c_str(), false))
                ret_code = -1;
        }break;
        case LoadBones:
        {
            if (!ATools->LoadBoneData(second_file_path.c_str()) || !ATools->Save(object_path.c_str()))
                ret_code = -1;
        }break;
        case SaveBones:
        {
            if (!ATools->SaveBoneData(second_file_path.c_str()))
                ret_code = -1;
        }break;
        case ExportOBJ:
        {
            if (!ATools->ExportOBJ(second_file_path.c_str()))
                ret_code = -1;
        }break;
        case ExportDM:
        {
            if (!ATools->ExportDM(second_file_path.c_str()))
                ret_code = -1;
        }break;
        case SaveObject:
        {
            if (!ATools->Save(object_path.c_str()))
                ret_code = -1;
        }break;
        case LoadBoneParts:
        {
            if (!ATools->LoadBoneParts(second_file_path.c_str()) || !ATools->Save(object_path.c_str()))
                ret_code = -1;
        }break;
        case SaveBoneParts:
        {
            if (!ATools->SaveBoneParts(second_file_path.c_str()))
                ret_code = -1;
        }break;
        case ToDefaultBoneParts:
        {
            if (!ATools->ToDefaultBoneParts() || !ATools->Save(object_path.c_str()))
                ret_code = -1;
        }break;
        case SaveSklMotions:
        {
            if (!ATools->SaveMotions(second_file_path.c_str(), true))
                ret_code = -1;
        }break;
    }

    Core._destroy();

    if (flags & exfDbgWindow)
        system("pause");

    return ret_code;
}
