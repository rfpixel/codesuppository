#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common/snippets/UserMemAlloc.h"
#include "common/snippets/fmem.h"
#include "common/snippets/SendTextMessage.h"
#include "MeshImport/MeshImport.h"
#include "MeshImport/MeshSystem.h"
#include "CodeSuppository.h"

#include "TestBestFitOBB.h"
#include "TestBestFitPlane.h"
#include "TestStanHull.h"
#include "TestInPlaceParser.h"
#include "TestClipper.h"
#include "TestFrustum.h"
#include "TestPlaneTri.h"
#include "TestVertexLookup.h"
#include "TestMapPal.h"
#include "TestMeshVolume.h"
#include "TestDfrac.h"
#include "TestLookAt.h"
#include "TestConvexDecomposition.h"
#include "TestWinMsg.h"
#include "TestWildCard.h"
#include "TestGestalt.h"
#include "TestAsc2Bin.h"
#include "TestFileInterface.h"
#include "TestKeyValueIni.h"
#include "TestTinyXML.h"
#include "TestKdTree.h"
#include "TestMeshCleanup.h"
#include "TestFastAstar.h"
#include "TestSas.h"
#include "TestCompression.h"
#include "TestSplitMesh.h"
#include "TestArrowHead.h"
#include "TestSendMail.h"
#include "TestSendAIM.h"
#include "TestErosion.h"

CodeSuppository *gCodeSuppository=0;

class MyCodeSuppository : public CodeSuppository
{
public:
  MyCodeSuppository(void)
  {
    mMeshSystem = 0;
  }

  ~MyCodeSuppository(void)
  {
    if ( mMeshSystem )
    {
      gMeshImport->releaseMeshSystem(mMeshSystem);
      mMeshSystem = 0;
    }
  }

  void processCommand(CodeSuppositoryCommand command,bool /*state*/,const float * /*data*/)
  {
    switch ( command )
    {
      case CSC_SPLIT_MESH:
        testSplitMesh();
        break;
      case CSC_LOOKAT:
        testLookAt();
        break;
      case CSC_EROSION:
        testErosion();
        break;
      case CSC_SEND_MAIL:
        testSendMail();
        break;
      case CSC_SEND_AIM:
        testSendAIM();
        break;
      case CSC_ARROW_HEAD:
        testArrowHead();
        break;
      case CSC_COMPRESSION:
        testCompression();
        break;
      case CSC_SAS:
        testSas();
        break;
      case CSC_FAST_ASTAR:
        testFastAstar();
        break;
      case CSC_KDTREE:
        testKdTree();
        break;
      case CSC_MESH_CLEANUP:
        testMeshCleanup();
        break;
      case CSC_TINY_XML:
        testTinyXML();
        break;
      case CSC_KEY_VALUE_INI:
        testKeyValueIni();
        break;
      case CSC_FILE_INTERFACE:
        testFileInterface();
        break;
      case CSC_ASC2BIN:
        testAsc2Bin();
        break;
      case CSC_GESTALT:
        testGestalt();
        break;
      case CSC_WILDCARD:
        testWildCard();
        break;
      case CSC_WINMSG:
        testWinMsg();
        break;
      case CSC_CONVEX_DECOMPOSITION:
        testConvexDecomposition();
        break;
      case CSC_DFRAC:
        testDfrac();
        break;
      case CSC_MESH_VOLUME:
        testMeshVolume();
        break;
      case CSC_MAP_PAL:
        testMapPal();
        break;
      case CSC_VERTEX_LOOKUP:
        testVertexLookup();
        break;
      case CSC_PLANE_TRI:
        testPlaneTri();
        break;
      case CSC_FRUSTUM:
        testFrustum();
        break;
      case CSC_CLIPPER:
        testClipper();
        break;
      case CSC_INPARSER:
        testInPlaceParser();
        break;
      case CSC_STAN_HULL:
        testStanHull();
        break;
      case CSC_BEST_FIT_PLANE:
        testBestFitPlane();
        break;
      case CSC_BEST_FIT_OBB:
        testBestFitOBB();
        break;
    }
  }

  void render(float /*dtime*/)
  {
  }

  virtual void importMesh(const char *fname)
  {
    if ( mMeshSystem )
    {
      gMeshImport->releaseMeshSystem(mMeshSystem);
      mMeshSystem = 0;
    }
    unsigned int len;
    unsigned char *data = getLocalFile(fname,len);
    if ( data )
    {
      mMeshSystem = gMeshImport->createMeshSystem(fname,data,len,0);
      if ( mMeshSystem )
      {
        SEND_TEXT_MESSAGE(0,"Successfully imported mesh '%s'\r\n", fname );
      }
    }
    else
    {
      SEND_TEXT_MESSAGE(0,"Failed to load file '%s'\r\n", fname );
    }

  }

private:
  MESHIMPORT::MeshSystem  *mMeshSystem;
};

CodeSuppository * createCodeSuppository(void)
{
  MyCodeSuppository *mcs = MEMALLOC_NEW(MyCodeSuppository);
  return static_cast< CodeSuppository *>(mcs);
}

void              releaseCodeSuppository(CodeSuppository *c)
{
  MyCodeSuppository *mcs = static_cast< MyCodeSuppository *>(c);
  delete mcs;
}
