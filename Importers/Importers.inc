#include "Base.h"
#include "Importer.h"
#include "Str.h"

#ifndef MOK_INCLUDE_DEFAULT_IMPORTERS
    #define MOK_INCLUDE_DEFAULT_IMPORTERS 1
#endif

#ifndef MOK_DEFINE_DEFAULT_IMPORTERS
    #define MOK_DEFINE_DEFAULT_IMPORTERS 1
#endif

#if MOK_INCLUDE_DEFAULT_IMPORTERS
#include "Importers/Image/BMP.h"
#include "Importers/Model/OBJ.h"

#if OS_MSWINDOWS
#include "Importers/Image/WindowsImagingComponent.h"
#endif

#endif

#if MOK_DEFINE_DEFAULT_IMPORTERS
/**
 * Change this to define a different name for the default import registry
 */
#ifndef MOK_IMPORTER_REGISTRY_NAME
    #define MOK_IMPORTER_REGISTRY_NAME Importer_Registry
#endif
#endif

/**
 * When defining your own static import registry, you can include this
 * in your array to add the default importers
 */
#define MOK_DEFAULT_IMPORTERS \
    {LIT("Bitmap"),                     LIT(".bmp"),                       import_bmp_load}, \
    {LIT("Wavefront OBJ"),              LIT(".obj"),                       import_obj_load}, \
    {LIT("Windows Imaging Component"),  LIT(".bmp|.png|.tiff|.jpeg|.jpg"), import_wic_load}, \

#if MOK_DEFINE_DEFAULT_IMPORTERS
static IImporter _Importers[] = {
    MOK_DEFAULT_IMPORTERS
};

ImporterRegistry MOK_IMPORTER_REGISTRY_NAME = {
    _Importers,
    ARRAY_COUNT(_Importers)
};
#endif

#if MOK_INCLUDE_DEFAULT_IMPORTERS
#include "Importers/Image/BMP.cpp"
#include "Importers/Model/OBJ.cpp"

#if OS_MSWINDOWS
#include "Importers/Image/WindowsImagingComponent.cpp"
#endif
#endif