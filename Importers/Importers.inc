#include "Base.h"
#include "Importer.h"
#include "Importers/Image/BMP.h"
#include "Importers/Model/OBJ.h"
#include "Str.h"

/**
 * Change this to define a different name for the default import registry
 */
#ifndef MOK_IMPORTER_REGISTRY_NAME
    #define MOK_IMPORTER_REGISTRY_NAME Importer_Registry
#endif

/**
 * When defining your own static import registry, you can include this
 * in your array to add the default importers
 */
#define MOK_DEFAULT_IMPORTERS \
    {LIT("Bitmap"),         LIT(".bmp"), import_bmp_load}, \
    {LIT("Wavefront OBJ"),  LIT(".obj"), import_obj_load}, \


static IImporter _Importers[] = {
    MOK_DEFAULT_IMPORTERS
};

ImporterRegistry MOK_IMPORTER_REGISTRY_NAME = {
    _Importers,
    ARRAY_COUNT(_Importers)
};


#include "Importers/Image/BMP.cpp"
#include "Importers/Model/OBJ.cpp"