#include <assert.h>
#include "UserMemAlloc.h"
#include "MeshImport.h"
#include "ImportPSK.h"
#include "SystemServices.h"

#ifdef WIN32
#ifdef MESHIMPORTOGRE_EXPORTS
#define MESHIMPORTOGRE_API __declspec(dllexport)
#else
#define MESHIMPORTOGRE_API __declspec(dllimport)
#endif
#else
#define MESHIMPORTOGRE_API
#endif

#pragma warning(disable:4100)

bool doShutdown(void);

extern "C"
{
MESHIMPORTOGRE_API MESHIMPORT::MeshImporter * getInterface(int version_number,SYSTEM_SERVICES::SystemServices *services);
};

namespace MESHIMPORT
{
class MyMeshImportPSK : public MeshImporter
{
public:
  MyMeshImportPSK(void)
  {
  }

  ~MyMeshImportPSK(void)
  {
  }

  bool shutdown(void)
  {
    return doShutdown();
  }

  virtual int              getExtensionCount(void) { return 1; }; // most importers support just one file name extension.

  virtual const char * getExtension(int index)  // report the default file name extension for this mesh type.
  {
    return ".psk";
  }

  virtual const char * getDescription(int index)  // report the default file name extension for this mesh type.
  {
    return "PSK 3d Skeletal Mesh Files";
  }

  virtual bool importMesh(const char *meshName,const void *data,unsigned int dlen,MESHIMPORT::MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  {
    bool ret = false;

    MESHIMPORT::MeshImporter *mi = MESHIMPORT::createMeshImportPSK();

    if ( mi )
    {
      ret = mi->importMesh(meshName,data,dlen,callback,options,appResource);
      MESHIMPORT::releaseMeshImportPSK(mi);
    }

    return ret;
  }



};


enum MeshImportPSKAPI
{
  PPA_TEST,
};

};  // End of Namespace


using namespace MESHIMPORT;


static MyMeshImportPSK *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImporter * getInterfaceMeshImportPSK(int version_number,SYSTEM_SERVICES::SystemServices *services)
#else
MESHIMPORTOGRE_API MeshImporter * getInterface(int version_number,SYSTEM_SERVICES::SystemServices *services)
#endif
{
  if ( services )
  {
    SYSTEM_SERVICES::gSystemServices = services;
  }
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORT_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportPSK);
  }
  return static_cast<MeshImporter *>(gInterface);
};

};  // End of namespace PATHPLANNING

#ifndef PLUGINS_EMBEDDED

using namespace MESHIMPORT;

bool doShutdown(void)
{
  bool ret = false;
  if ( gInterface )
  {
    ret = true;
    MEMALLOC_DELETE(MeshImportPSK,gInterface);
    gInterface = 0;
  }
  return ret;
}

using namespace MESHIMPORT;

#ifdef WIN32

#include <windows.h>

BOOL APIENTRY DllMain( HANDLE ,
                       DWORD  ul_reason_for_call,
                       LPVOID )
{
  int ret = 0;

  switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
      ret = 1;
			break;
		case DLL_THREAD_ATTACH:
      ret = 2;
			break;
		case DLL_THREAD_DETACH:
      ret = 3;
			break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

#endif

#endif