#include "Base.h"
#include "Importer.h"
#include "Importers/Image/BMP.h"
#include "Str.h"

#ifndef MOK_IMPORTER_REGISTRY_NAME
    #define MOK_IMPORTER_REGISTRY_NAME Importer_Registry
#endif

static IImporter _Importers[] = {
    {STATIC_STR("Bitmap"), STATIC_STR(".bmp"), import_bmp_load},
};

ImporterRegistry MOK_IMPORTER_REGISTRY_NAME = {
    _Importers,
    ARRAY_COUNT(_Importers)
};


#include "Importers/Image/BMP.cpp"