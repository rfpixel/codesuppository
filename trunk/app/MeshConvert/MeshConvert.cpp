#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <direct.h>

#include "MeshImport.h"

#pragma warning(disable:4996 4100)

char * findLastDot(char *scan)
{
  char *lastDot = 0;
  while ( *scan )
  {
    if  (*scan == '.' ) lastDot = scan;
    scan++;
  }
  return lastDot;
}

char *stristr(const char *str,const char *key)       // case insensitive str str
{
	assert( strlen(str) < 2048 );
	assert( strlen(key) < 2048 );
	char istr[2048];
	char ikey[2048];
	strncpy(istr,str,2048);
	strncpy(ikey,key,2048);
	strlwr(istr);
	strlwr(ikey);

	char *foo = strstr(istr,ikey);
	if ( foo )
	{
		unsigned int loc = (unsigned int)(foo - istr);
		foo = (char *)str+loc;
	}

	return foo;
}

class MyMeshImportApplicationResource : public MESHIMPORT::MeshImportApplicationResource
{
public:
	virtual void * getApplicationResource(const char *base_name,const char *resource_name,unsigned int &len) 
	{
		void *ret = 0;
		len = 0;

		FILE *fph = fopen(resource_name,"rb");
		if ( fph )
		{
			fseek(fph,0L,SEEK_END);
			len = ftell(fph);
			fseek(fph,0L,SEEK_SET);
			if ( len > 0 )
			{
  			  ret = malloc(len);
			  fread(ret,len,1,fph);
			}
			fclose(fph);
		}
		return ret;
	}

	virtual void   releaseApplicationResource(void *mem) 
	{
		free(mem);
	}
};


void main(int argc,const char **argv)
{
    if ( argc < 2 )
    {
        printf("Usage: MeshConvert <name> (options)\r\n");
        printf("\r\n");
        printf("-r x y z   : Rotates the mesh by this euler rotation. 90 0 0 rotates a Y-up axis mesh to be Z-up.\r\n");
        printf("-s scale   : Scales the input mesh by this amount.  50 to go from metric to UE3 and 0.02 to go from UE3 to metric.\r\n");
        printf("-f format  : Output formats are OBJ, EZM, XML (Ogre3d), PSK\r\n");
    }
    else
    {
        bool rotate = false;
        bool scale  = false;
        float meshScale = 1;
        float rot[3];

        MESHIMPORT::MeshSerializeFormat inputFormat = MESHIMPORT::MSF_EZMESH;
        MESHIMPORT::MeshSerializeFormat outputFormat = MESHIMPORT::MSF_EZMESH;

        for (int i=2; i<argc; i++)
        {
            const char *key = argv[i];
            if ( strcmp(key,"-r") == 0 )
            {
                if ( (i+3) < argc )
                {
                    const char *x = argv[i+1];
                    const char *y = argv[i+2];
                    const char *z = argv[i+3];
                    rot[0] = (float) atof( x );
                    rot[1] = (float) atof( y );
                    rot[2] = (float) atof( z );
                    printf("Rotating input mesh by euler(%0.2f,%0.2f,%0.2f)\r\n", rot[0], rot[1], rot[2] );
                    rotate = true;
                    i+=3;
                }
                else
                {
                    printf("Missing rotation arguments.  Expect euler X Y Z in degrees.\r\n");
                }
            }
            else if ( strcmp(key,"-s") == 0 )
            {
                if ( (i+1) < argc )
                {
                    i++;
                    const char *value = argv[i];
                    meshScale = (float)atof(value);
                    if ( meshScale > 0.0001f )
                    {
                        printf("Mesh scale set to %0.4f\r\n", meshScale );
                        scale = true;
                    }
                    else
                    {
                        printf("Invalid mesh scale specified. %s\r\n", value );
                    }
                }
                else
                {
                    printf("Missing scale value after -s\r\n");
                }
            }
            else if ( strcmp(key,"-f") == 0 )
            {
                if ( (i+1) < argc )
                {
                    i++;
                    const char *value = argv[i];
                    if ( stricmp(value,"ezm") == 0 )
                    {
                        outputFormat = MESHIMPORT::MSF_EZMESH;
                        printf("Setting output format to EZ-MESH\r\n");
                    }
                    else if ( stricmp(value,"obj") == 0 )
                    {
                        outputFormat = MESHIMPORT::MSF_WAVEFRONT;
                        printf("Setting output format to Wavefront OBJ\r\n");
                    }
                    else if ( stricmp(value,"xml") == 0 )
                    {
                        outputFormat = MESHIMPORT::MSF_OGRE3D;
                        printf("Setting output format to Ogre3d\r\n");
                    }
                    else if ( stricmp(value,"psk") == 0 )
                    {
                        outputFormat = MESHIMPORT::MSF_PSK;
                        printf("Setting output format to UE3 PSK\r\n");
                    }
                    else
                    {
                        printf("Unknown output format %s, defaulting to EZM\r\n", value );
                    }
                }
                else
                {
                    printf("Missing output format type after -f\r\n");
                }
            }
        }


		char dirname[256];
		strcpy(dirname,argv[0]);
		int len = strlen(dirname);
		char *scan = &dirname[len-1];
		while ( len )
		{
			if ( *scan == '\\' )
			{
				*scan = 0;
				break;
			}
			scan--;
			len--;
		}

		MESHIMPORT::MeshImport *meshImport = loadMeshImporters(dirname,0); // loads the mesh import library (dll) and all available importers from the same directory.
		if ( meshImport )
		{
			printf("Succesfully loaded the MeshImport DLL from directory: %s\r\n", dirname );

			MyMeshImportApplicationResource miar;
			meshImport->setMeshImportApplicationResource(&miar);

			char fname[512];
			strncpy(fname,argv[1],512);
            strlwr(fname);

            if ( strstr(fname,".ezm") )
            {
                inputFormat = MESHIMPORT::MSF_EZMESH;
            }
            else if ( strstr(fname,".mesh.xml") )
            {
                inputFormat = MESHIMPORT::MSF_OGRE3D;
            }
            else if ( strstr(fname,".obj") )
            {
                inputFormat = MESHIMPORT::MSF_WAVEFRONT;
            }
            else if ( strstr(fname,".psk") )
            {
                inputFormat = MESHIMPORT::MSF_PSK;
            }
            else if ( strstr(fname,".fbx") )
            {
				inputFormat = MESHIMPORT::MSF_FBX;
            }
			strncpy(fname,argv[1],512);
			FILE *fph = fopen(fname,"rb");
			if ( fph )
			{
				fseek(fph,0L,SEEK_END);
				unsigned int len = ftell(fph);
				fseek(fph,0L,SEEK_SET);
				if ( len )
				{
					char *mem = (char *)malloc(len);
					fread(mem,len,1,fph);
					MESHIMPORT::MeshSystemContainer *msc = meshImport->createMeshSystemContainer(fname,mem,len,0);
					if ( msc )
					{
						printf("Succesfully loaded input mesh: %s\r\n", fname );
                        if ( scale )
                        {
                            printf("Scaling input mesh by %0.4f\r\n", meshScale );
                            meshImport->scale(msc,meshScale);
                        }
                        if ( rotate )
                        {
                            printf("Rotating input mesh by euler angles (%0.2f,%0.2f,%0.2f)\r\n", rot[0], rot[1], rot[2] );
                            meshImport->rotate(msc,rot[0],rot[1],rot[2]);
                        }

                        char *lastDot = findLastDot(fname);
                        if ( lastDot )
                        {
                            *lastDot = 0;

                            if ( inputFormat == outputFormat )
                            {
                                strcat(fname,"_COPY");
                            }
                            switch ( outputFormat )
                            {
                              case MESHIMPORT::MSF_EZMESH:
                                strcat(fname,".ezm");
                                break;
                              case MESHIMPORT::MSF_WAVEFRONT:
                                strcat(fname,".obj");
                                break;
                              case MESHIMPORT::MSF_OGRE3D:
                                strcat(fname,".xml");
                                break;
                              case MESHIMPORT::MSF_PSK:
                                strcat(fname,".psk");
                                break;
                            }

                            MESHIMPORT::MeshSerialize ms(outputFormat);
							MESHIMPORT::MeshSystem *msystem = meshImport->getMeshSystem(msc);
                            meshImport->serializeMeshSystem(msystem,ms);

                            if ( ms.mBaseData )
                            {
                                FILE *fph = fopen(fname,"wb");
                                if ( fph )
                                {
                                  printf("Saving output file %s which is %d bytes in size.\r\n", fname, ms.mBaseLen );
                                  fwrite(ms.mBaseData,ms.mBaseLen,1,fph);
                                  fclose(fph);
                                }
                                else
                                {
                                    printf("Failed to open file '%s' for write access.\r\n", fname );
                                }

								char *exname = fname;

                                if ( ms.mExtendedData )
                                {
                                  switch ( outputFormat )
                                  {
                                    case MESHIMPORT::MSF_EZMESH:
                                      assert(0);
                                      exname = 0;
                                      break;
                                    case MESHIMPORT::MSF_WAVEFRONT:
                                      {
                                        char *lastDot = findLastDot(exname);
										assert(lastDot);
										if ( lastDot )
										{
  										  *lastDot = 0;
                                          strcat(exname,".mtl");
										}
										else
										{
											exname = 0;
										}
                                      }
                                      break;
                                    case MESHIMPORT::MSF_OGRE3D:
                                      {
                                        char *scan = stristr(exname,".mesh.xml");
                                        if ( scan )
                                        {
                                            *scan = 0;
                                            strcat(exname,".skeleton.xml");
                                        }
                                        else
                                        {
                                            exname = 0;
                                            assert(0);
                                        }
                                      }
                                      break;
                                    case MESHIMPORT::MSF_PSK:
                                      {
                                        char *lastDot = findLastDot(exname);
										if ( lastDot )
										{
										  *lastDot = 0;
                                          strcat(exname,".psa");
										}
										else
										{
											assert(0);
											exname = 0;
										}
                                      }
                                      break;
                                  }
                                  if ( exname )
                                  {
                                    FILE *fpex = fopen(exname,"wb");
                                    if ( fpex )
                                    {
                                        printf("Saving extended data %s length %d\r\n", exname, ms.mExtendedLen );
                                        fwrite(ms.mExtendedData, ms.mExtendedLen, 1, fpex);
                                        fclose(fpex);
                                    }
                                    else
                                    {
                                        printf("Failed to open extended data file %s for write access.\r\n", exname );
                                    }
                                  }
                                  else
                                  {
                                    printf("Failed to save extended data.\r\n");
                                  }
                                }
                            }
                            else
                            {
                                printf("Failed to serialize the mesh system.\r\n");
                            }
                            meshImport->releaseSerializeMemory(ms);
                        }
  						meshImport->releaseMeshSystemContainer(msc);
					}
					else
					{
						printf("Failed to import mesh %s\r\n", fname);
					}
				}
				else
				{
					printf("File %s is empty.\r\n", fname );
				}
				fclose(fph);
			}
			else
			{
				printf("Failed to open file '%s'\r\n", fname );
			}
		}
		else
		{
			printf("Failed to load MeshImport.dll from directory: %s\r\n", dirname );
		}
    }
}
