#include "Importer.h"
#include "Importers/Image/BMP.h"
#include "Str.h"

#ifndef MOK_IMPORTER_REGISTRY_NAME
    #define MOK_IMPORTER_REGISTRY_NAME Importer_Registry
#endif

ImporterRegistry MOK_IMPORTER_REGISTRY_NAME = {
    {
        {STATIC_STR("Bitmap"), STATIC_STR(".bmp"), import_bmp_load},
    },
    1
};

#include "Importers/Image/BMP.cpp"