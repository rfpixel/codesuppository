#include "ImportFBX.h"


int sortPolygonMaterials( const void* a, const void* b );


// Process recursively each node of the scene. To avoid recomputing 
// uselessly the global positions, the global position of each 
// node is passed to it's children while browsing the node tree.
void MeshImportFBX::ProcessScene(KFbxNode* subScene)
{
	KFbxNode* pNode = subScene;
	if(subScene == NULL)
		pNode = m_scene->GetRootNode();

    KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
    if (lNodeAttribute)
    {
		if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eMESH)
        {
              AddMeshNode(pNode);
        }
        else if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eNURB)
        {
            // Not supported yet. 
            // Should have been converted into a mesh in function ConvertNurbsAndPatch().
        }
        else if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::ePATCH)
        {
            // Not supported yet. 
            // Should have been converted into a mesh in function ConvertNurbsAndPatch().
        }
    }

	//recurse
    int i, lCount = pNode->GetChildCount();
    for (i = 0; i < lCount; i++)
    {
        ProcessScene(pNode->GetChild(i));
    }
}

struct VertexSkinInfo
{
	int mBone[MAX_BONES];
	float mWeight[MAX_BONES];

	VertexSkinInfo()
	{
		for(int i = 0; i < MAX_BONES; i++)
		{
			mBone[i] = 0;
			mWeight[i] = 0;
		}
	}
};

void MeshImportFBX::AddMeshNode(KFbxNode* pNode)
{
	KFbxVector4* positionValues = NULL;
	KFbxLayerElementArrayTemplate<int>* materialIndices = NULL;
	//KFbxLayerElementArrayTemplate<KFbxVector4>* normalValues = NULL;
	//KFbxLayerElementArrayTemplate<int>* normalIndices = NULL;
	KFbxLayerElementArrayTemplate<KFbxVector2>* uvValues = NULL;    
	KFbxLayerElementArrayTemplate<int>* uvIndices = NULL;    


	KFbxMesh* lMesh = (KFbxMesh*) pNode->GetNodeAttribute();

	const char* meshName = lMesh->GetCurrentTakeNodeName();
	const char* nodeName = pNode->GetName();

	if(lMesh->GetLayerCount() == 0)
		return;
	
	KFbxLayer* lLayer0 = lMesh->GetLayer(0);
	
	int lPolygonCount = lMesh->GetPolygonCount();
	if( lPolygonCount == 0 )
		return;

    int lPolyVertexCount = lMesh->GetPolygonVertexCount();
    if (lPolyVertexCount == 0)
        return;

    int lVertexCount = lMesh->GetControlPointsCount();
    if (lVertexCount == 0)
        return;

	positionValues = lMesh->GetControlPoints();
	lMesh->GetMaterialIndices(&materialIndices);
	
	int materialIndexCount = materialIndices != NULL ? materialIndices->GetCount() : 1;

	//if(!lMesh->GetNormals(&normalValues))
	//{
	//	lMesh->ComputeVertexNormals();
	//	lMesh->GetNormals(&normalValues);
	//}
	//lMesh->GetNormalsIndices(&normalIndices);
	//KFbxLayerElement::EMappingMode normalMapping = lLayer0->GetNormals()->GetMappingMode();
	

    lMesh->GetTextureUV(&uvValues, KFbxLayerElement::eDIFFUSE_TEXTURES); 
    lMesh->GetTextureIndices(&uvIndices, KFbxLayerElement::eDIFFUSE_TEXTURES); 
	KFbxLayerElement::EMappingMode uvMapping = uvIndices ? KFbxLayerElement::eBY_POLYGON_VERTEX : KFbxLayerElement::eBY_CONTROL_POINT;

	std::vector<VertexSkinInfo> meshSkinInfo(lVertexCount);

	mVertexFormat = 0;
	mVertexFormat |= MESHIMPORT::MIVF_POSITION;
	mVertexFormat |= MESHIMPORT::MIVF_BONE_WEIGHTING;
	//if(normalValues != NULL && normalValues->GetCount() > 0)
	mVertexFormat |= MESHIMPORT::MIVF_NORMAL;
	if(uvValues != NULL && uvValues->GetCount() > 0)
		mVertexFormat |= MESHIMPORT::MIVF_TEXEL1;

	KFbxXMatrix bindShapeXform;
	KFbxXMatrix geometryXform = GetGeometry(pNode);
	KFbxXMatrix shapeNodeXform = m_takeInfo ? pNode->GetGlobalFromCurrentTake(m_takeInfo->mLocalTimeSpan.GetStart()) : pNode->GetGlobalFromDefaultTake();

	int lSkinCount= lMesh->GetDeformerCount(KFbxDeformer::eSKIN);
	for(int s = 0; s < lSkinCount; s++)
	{
		KFbxSkin* skin = ((KFbxSkin *)(lMesh->GetDeformer(0, KFbxDeformer::eSKIN)));

		int clusterCount = skin->GetClusterCount();
		if(clusterCount == 0)
			continue;

		for (int c=0; c < clusterCount; c++)
		{
			KFbxCluster* cluster = skin->GetCluster(c);

			KFbxNode* link = cluster->GetLink();
			if (!link)
				continue;

			const char* linkName = link->GetName();

			int  weightCount = cluster->GetControlPointIndicesCount();
			if (weightCount == 0)
				continue;

			int boneIndex = -1;
			for( int b=0; b < meshBones.size() && boneIndex == -1; b++)
			{
				if(meshNodes[b] == link)
					boneIndex = b;
			}
			assert(boneIndex >=0 && boneIndex < meshBones.size());


			if(c==0)
			{
				KFbxXMatrix bindShapeBaseXform;
				cluster->GetTransformMatrix(bindShapeBaseXform);
				
				bindShapeXform = bindShapeBaseXform;//shapeNodeXform * bindShapeBaseXform * geometryXform;

				meshWorldBindShapeXforms[boneIndex] = bindShapeXform;
			}

			int *weightIndices = cluster->GetControlPointIndices();
			double *weightValues = cluster->GetControlPointWeights();
			for( int w = 0; w < weightCount; w++)
			{
				int wIndex = weightIndices[w];
				double wValue = weightValues[w];

				VertexSkinInfo& vertexSkinInfo = meshSkinInfo[wIndex];
				
				//Assign the weight to the vertex
				for(int a = 0; a < MAX_BONES; a++ )
				{
					//Test to see if we want to insert here
					if( wValue <= vertexSkinInfo.mWeight[a])
						continue;//nope, try the next assignment
		
					//shift lower assignments to make room for the new value
					for(int shift = MAX_BONES-1; shift > a; shift-- )
					{
						vertexSkinInfo.mBone[shift]=vertexSkinInfo.mBone[shift-1];
						vertexSkinInfo.mWeight[shift]=vertexSkinInfo.mWeight[shift-1];
					}

					//insert new value
					vertexSkinInfo.mBone[a]=boneIndex;
					vertexSkinInfo.mWeight[a]=wValue;
					break;
				}
			}
		}
	}
	//scale weights to 1.0 total contribution
	for(int v = 0; v < meshSkinInfo.size(); v++ )
	{
		VertexSkinInfo& vertexSkinInfo = meshSkinInfo[v];

		float totalWeight = 0;
		for(int a = 0; a < MAX_BONES; a++ )
			totalWeight += vertexSkinInfo.mWeight[a];

		if ( totalWeight == 0 )
		{
			for(int a = 0; a < MAX_BONES; a++ )
			{
				vertexSkinInfo.mWeight[a] = (a == 0) ? 1 : 0;
			}
		}
		else
		{
			float scaleWeights = 1.0f/totalWeight;
			for(int a = 0; a < MAX_BONES; a++ )
				vertexSkinInfo.mWeight[a] *= scaleWeights;
		}
	}


	std::vector<PolygonMaterial> sortedPolygonMaterials(lPolygonCount);


	assert( materialIndexCount == lPolygonCount || materialIndexCount == 1 );
	if( lPolygonCount < materialIndexCount && materialIndexCount != 1 )
	{
		materialIndexCount = lPolygonCount;
	}

	for( int i = 0; i < lPolygonCount; ++i )
	{
		int idx = i < materialIndexCount ? i : 0;
		sortedPolygonMaterials[i].materialIndex = materialIndices ? materialIndices->GetAt(idx) : -1;
		sortedPolygonMaterials[i].polygonIndex = i;

	}
	qsort( &sortedPolygonMaterials[0], materialIndexCount, sizeof( PolygonMaterial ), sortPolygonMaterials );
	
	
	std::vector<int> partition;
	partition.reserve(lPolygonCount+1);
	partition.push_back(0);
	for( int i = 1; i < lPolygonCount; ++i )
	{
		if( !sortedPolygonMaterials[i].submeshesEqual( sortedPolygonMaterials[partition.back()] ) )
		{
			partition.push_back(i);
		}
	}
	partition.push_back(lPolygonCount);
	
	unsigned submeshCount = (unsigned)partition.size();
	int lNbMat = pNode->GetSrcObjectCount(KFbxSurfaceMaterial::ClassId);
	KString materialNamePrefix = m_fileName + KString( "#" );
	PolygonMaterial* tmat = &sortedPolygonMaterials[0];



	std::vector<MESHIMPORT::MeshVertex> meshVertices(lPolyVertexCount);

	//for( unsigned submeshIndex = 0; submeshIndex < submeshCount; ++submeshIndex )
	//{
	//	KFbxSurfaceMaterial* lMaterial = NULL;
	//	if( tmat->materialIndex >= 0 && tmat->materialIndex < lNbMat )
	//	{
	//		lMaterial = KFbxCast <KFbxSurfaceMaterial>(pNode->GetSrcObject(KFbxSurfaceMaterial::ClassId, tmat->materialIndex));
	//	}
	//	KString materialName = lMaterial ? (materialNamePrefix+lMaterial->GetName()) : KString("");

	//	const int polygonNum = partition[submeshIndex+1]-partition[submeshIndex];


		KFbxSurfaceMaterial* lMaterial = KFbxCast <KFbxSurfaceMaterial>(pNode->GetSrcObject(KFbxSurfaceMaterial::ClassId, tmat->materialIndex));
		KString materialName = lMaterial ? (materialNamePrefix+lMaterial->GetName()) : KString("");

		int lPolygonIndex;
		for (lPolygonIndex = 0; lPolygonIndex < lPolygonCount; lPolygonIndex++)
		{
			int lPolyVertexStart = lMesh->GetPolygonVertexIndex(lPolygonIndex);
			int lPolyVertexIndex = lPolyVertexStart;

			int lPolyLocalVertexCount = lMesh->GetPolygonSize(lPolygonIndex);

			for (int lPolyLocalVertexIndex=0; lPolyLocalVertexIndex < lPolyLocalVertexCount; lPolyLocalVertexIndex++, lPolyVertexIndex++)
			{
				int lVertexIndex = lMesh->GetPolygonVertex(lPolygonIndex, lPolyLocalVertexIndex);

				MESHIMPORT::MeshVertex& meshVertex = meshVertices[lPolyVertexIndex];
				VertexSkinInfo& vertexSkinInfo = meshSkinInfo[lVertexIndex];
				KFbxVector4& sourceVert = positionValues[lVertexIndex];

				KFbxVector4 sourceNorm;
				lMesh->GetPolygonVertexNormal(lPolygonIndex, lPolyLocalVertexIndex, sourceNorm);
				sourceNorm[3] = 0;

				KFbxVector2 sourceUV;
				if(uvValues != NULL)
				{
					if (uvMapping == KFbxLayerElement::eBY_POLYGON_VERTEX)
					{
						int uvIndex = lMesh->GetTextureUVIndex(lPolygonIndex, lPolyLocalVertexIndex);
						sourceUV = uvValues->GetAt(uvIndex);
					}
					else // KFbxLayerElement::eBY_CONTROL_POINT
					{
						sourceUV = uvValues->GetAt(lVertexIndex);
					}
				}

				KFbxVector4 accumVert;
				KFbxVector4 accumNorm;
//#define ACCUMULATE_BIND_SHAPE
#ifdef ACCUMULATE_BIND_SHAPE
				for(int a = 0; a < MAX_BONES; a++ )
				{
					if(vertexSkinInfo.mWeight[a] == 0)
						break;

					KFbxXMatrix& bindXform = meshWorldBindShapeXforms[vertexSkinInfo.mBone[a]];
					float bindWeight = vertexSkinInfo.mWeight[a];

					KFbxVector4 bindVert = bindXform.MultT(sourceVert);
					KFbxVector4 bindNorm = bindXform.MultT(sourceNorm);

					accumVert += bindVert * bindWeight;
					accumNorm += bindNorm * bindWeight;
				}

#else //!ACCUMULATE_BIND_SHAPE
				accumVert = bindShapeXform.MultT(sourceVert);
				accumNorm = bindShapeXform.MultT(sourceNorm);
#endif
				accumNorm[3] = 0;
				accumNorm.Normalize();

				meshVertex.mPos[0] = accumVert[0];
				meshVertex.mPos[1] = accumVert[1];
				meshVertex.mPos[2] = accumVert[2];

				meshVertex.mNormal[0] = accumNorm[0];
				meshVertex.mNormal[1] = accumNorm[1];
				meshVertex.mNormal[2] = accumNorm[2];

				meshVertex.mTexel1[0] = sourceUV[0];
				meshVertex.mTexel1[1] = sourceUV[1];

				meshVertex.mBone[0] = vertexSkinInfo.mBone[0];
				meshVertex.mBone[1] = vertexSkinInfo.mBone[1];
				meshVertex.mBone[2] = vertexSkinInfo.mBone[2];
				meshVertex.mBone[3] = vertexSkinInfo.mBone[3];

				meshVertex.mWeight[0] = vertexSkinInfo.mWeight[0];
				meshVertex.mWeight[1] = vertexSkinInfo.mWeight[1];
				meshVertex.mWeight[2] = vertexSkinInfo.mWeight[2];
				meshVertex.mWeight[3] = vertexSkinInfo.mWeight[3];
			}

			lPolyVertexIndex = lPolyVertexStart;
			for (int lPolyLocalVertexIndex=0; lPolyLocalVertexIndex < lPolyLocalVertexCount-2; lPolyLocalVertexIndex++, lPolyVertexIndex++)
			{
			   m_callback->importTriangle(
				   pNode->GetName(),
				   pNode->GetName(),//materialName, 
					mVertexFormat,
					meshVertices[lPolyVertexStart+0],
					meshVertices[lPolyVertexStart+lPolyLocalVertexIndex+1],
					meshVertices[lPolyVertexStart+lPolyLocalVertexIndex+2]);
			}
		}
	//}  
}


	

//void MeshImportFBX::AddSkeletonNode(KFbxNode* pNode, KTime& pTime, KFbxXMatrix& pGlobalPosition)
//{
//	KFbxSkeleton* lSkeleton = (KFbxSkeleton*) pNode->GetNodeAttribute();
//	KFbxNode* pParentNode = pNode->GetParent();
//
//	KFbxNodeAttribute* testAttr;
//	KFbxNodeAttribute::EAttributeType testType;
//	if(pParentNode)
//	{
//		testAttr = pParentNode->GetNodeAttribute();
//		if(testAttr)
//			testType = testAttr->GetAttributeType();
//	}
//
//
//	KFbxSkeleton::ESkeletonType skelType = lSkeleton->GetSkeletonType();
//    // We're only intersted if this is a limb node and if 
//    // the parent also has an attribute of type skeleton.
//	
//    if (
//		lSkeleton->GetSkeletonType() == KFbxSkeleton::eLIMB_NODE &&
//        pParentNode
//       )
//	{
//		
//		const char *pName = pNode->GetName();
//		const char *pParentName = pParentNode->GetName();
//
//		// Root Bone
//		bool isRoot = false;
//		if((!pParentNode->GetNodeAttribute()) || (!pParentNode->GetNodeAttribute()->GetAttributeType() == KFbxNodeAttribute::eSKELETON))
//		{
//			isRoot = true;
//			
//		}
//
//		const char * attrName;
//		const char * attrNodeName;
//		KFbxNode* attrNode;
//		
//		MESHIMPORT::MeshBone* bone = new MESHIMPORT::MeshBone();
//
//		BoneRelation *bonePair = new BoneRelation();
//			
//		bonePair->bone = pName;
//		bonePair->parent = pParentName;
//
//		m_boneHierarchy.push_back(bonePair);
//		
//		bone->mName = pName;
//
//
//		fbxDouble3 r = pNode->LclRotation.Get();
//		fbxDouble3 p = pNode->LclTranslation.Get();
//		fbxDouble3 s = pNode->LclScaling.Get();
//	
//		KFbxVector4 preRotate = pNode->PreRotation.Get();
//		KFbxVector4 rot(r[0],r[1],r[2]);
//		KFbxVector4 pos(p[0],p[1],p[2]);
//		KFbxVector4 scl(s[0],s[1],s[2]);
//
//
//
//		KFbxXMatrix lGlobalPosition(pos, rot + preRotate, scl);
//	
//	    addBoneInfo(pName, bone, lGlobalPosition, isRoot);
//
//	}
//}




void MeshImportFBX::ConvertNurbsAndPatchRecursive(KFbxSdkManager* pSdkManager,  KFbxScene* pScene, KFbxNode* pNode)
{
    KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
    if (lNodeAttribute)
    {
		KFbxNodeAttribute::EAttributeType attributeType = lNodeAttribute->GetAttributeType();
		if(attributeType == KFbxNodeAttribute::eNURBS_SURFACE)
		{
			KFbxGeometryConverter lConverter(pSdkManager);
			lConverter.ConvertNurbsSurfaceToNurbInPlace(pNode);
		}
	}
	
	lNodeAttribute = pNode->GetNodeAttribute();
	if(lNodeAttribute)
	{
		KFbxNodeAttribute::EAttributeType attributeType = lNodeAttribute->GetAttributeType();
        if (attributeType == KFbxNodeAttribute::eNURB ||
            attributeType == KFbxNodeAttribute::ePATCH)
        {
			KFbxGeometryConverter lConverter(pSdkManager);
            lConverter.TriangulateInPlace(pNode);
        }
    }

    int i, lCount = pNode->GetChildCount();

    for (i = 0; i < lCount; i++)
    {
        ConvertNurbsAndPatchRecursive(pSdkManager, pScene, pNode->GetChild(i));
    }
}


void MeshImportFBX::ConvertNurbsAndPatch(KFbxSdkManager* pSdkManager, KFbxScene* pScene)
{
    ConvertNurbsAndPatchRecursive(pSdkManager, pScene, pScene->GetRootNode());
}





/*

void MeshImportFBX::LoadTexture(KFbxTexture* pTexture, KArrayTemplate<VSTexture*>& pTextureArray)
{
    // First find if the texture is already loaded
    int i, lCount = pTextureArray.GetCount();

    for (i = 0; i < lCount; i++)
    {
        if (pTextureArray[i]->mRefTexture == pTexture) return;
    }

    // Right now, only Targa textures are loaded by this sample
    KString lFileName = pTexture->GetFileName();

    if (lFileName.Right(3).Upper() == "TGA")
    {
        tga_image lTGAImage;

        if (tga_read(&lTGAImage, lFileName.Buffer()) == TGA_NOERR)
        {
            // Make sure the image is left to right
            if (tga_is_right_to_left(&lTGAImage)) tga_flip_horiz(&lTGAImage);

            // Make sure the image is bottom to top
            if (tga_is_top_to_bottom(&lTGAImage)) tga_flip_vert(&lTGAImage);

            // Make the image BGR 24
            tga_convert_depth(&lTGAImage, 24);

            VSTexture* lTexture = new VSTexture;

            lTexture->mW = lTGAImage.width;
            lTexture->mH = lTGAImage.height;
            lTexture->mRefTexture = pTexture;
            lTexture->mImageData  = new unsigned char[lTGAImage.width*lTGAImage.height*lTGAImage.pixel_depth/8];
            memcpy(lTexture->mImageData, lTGAImage.image_data, lTGAImage.width*lTGAImage.height*lTGAImage.pixel_depth/8);

            tga_free_buffers(&lTGAImage);

            pTextureArray.Add(lTexture);
        }
    }
}
*/

/*
void MeshImportFBX::LoadSupportedTexturesRecursive(KFbxNode* pNode, KArrayTemplate<VSTexture*>& pTextureArray)
{
    if (pNode)
    {
        int i, lCount;
        KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

        if (lNodeAttribute)
        {
            KFbxLayerContainer* lLayerContainer = NULL;

            switch (lNodeAttribute->GetAttributeType())
            {
            case KFbxNodeAttribute::eNURB:
                lLayerContainer = pNode->GetNurb();
                break;

            case KFbxNodeAttribute::ePATCH:
                lLayerContainer = pNode->GetPatch();
                break;

            case KFbxNodeAttribute::eMESH:
                lLayerContainer = pNode->GetMesh();
                break;
            }

            if (lLayerContainer){
                int lMaterialIndex;
                int lTextureIndex;
                KFbxProperty lProperty;
                int lNbTex;
                KFbxTexture* lTexture = NULL; 
                KFbxSurfaceMaterial *lMaterial = NULL;
                int lNbMat = pNode->GetSrcObjectCount(KFbxSurfaceMaterial::ClassId);
                for (lMaterialIndex = 0; lMaterialIndex < lNbMat; lMaterialIndex++){
                    lMaterial = KFbxCast <KFbxSurfaceMaterial>(pNode->GetSrcObject(KFbxSurfaceMaterial::ClassId, lMaterialIndex));
                    if(lMaterial){                                                          
                        lProperty = lMaterial->FindProperty(KFbxSurfaceMaterial::sDiffuse);
                        if(lProperty.IsValid()){
                            lNbTex = lProperty.GetSrcObjectCount(KFbxTexture::ClassId);
                            for (lTextureIndex = 0; lTextureIndex < lNbTex; lTextureIndex++){
                                lTexture = KFbxCast <KFbxTexture> (lProperty.GetSrcObject(KFbxTexture::ClassId, lTextureIndex)); 
                                if(lTexture)
                                    LoadTexture(lTexture, pTextureArray);
                            }
                        }
                    }
                }
            } 

        }

        lCount = pNode->GetChildCount();

        for (i = 0; i < lCount; i++)
        {
            LoadSupportedTexturesRecursive(pNode->GetChild(i), pTextureArray);
        }
    }
*/

/*
void MeshImportFBX::LoadSupportedTextures(KFbxScene* pScene, KArrayTemplate<VSTexture*>& pTextureArray)
{
    pTextureArray.Clear();

    LoadSupportedTexturesRecursive(pScene->GetRootNode(), pTextureArray);
}*/


inline void MeshImportFBX::GetBindPoseMatrix(KFbxCluster* lCluster, KFbxXMatrix& bindPose)
{
	 // Get the link initial global position and the link current global position.
     lCluster->GetTransformLinkMatrix(bindPose);
}

inline void MeshImportFBX::GetBindShapeMatrix( KFbxCluster* lCluster, KFbxXMatrix& bindShape)
{   
			lCluster->GetTransformMatrix(bindShape);       
}


// Deform the vertex array with the links contained in the mesh.
void MeshImportFBX::ApplyVertexTransform(KFbxMesh* pMesh, MESHIMPORT::MeshVertex* pVertexArray)
{
	
	int lVertexCount = pMesh->GetControlPointsCount();

	KFbxXMatrix lVertexTransformMatrix;

	int* lBoneCount = new int[lVertexCount];
	memset(lBoneCount, 0, lVertexCount * sizeof(int));

    double* lBoneWeight = new double[lVertexCount];
    memset(lBoneWeight, 0, lVertexCount * sizeof(double));

    int lSkinCount=pMesh->GetDeformerCount(KFbxDeformer::eSKIN);
    int clusterCount = ((KFbxSkin *)(pMesh->GetDeformer(0, KFbxDeformer::eSKIN)))->GetClusterCount();

	if(clusterCount == 0)
		return;


	for(int i = 0; i < lSkinCount; i ++)
	{
		for (int j=0; j< clusterCount; ++j)
		{
			

			KFbxCluster* lCluster =((KFbxSkin *) pMesh->GetDeformer(i, KFbxDeformer::eSKIN))->GetCluster(j);
			if (!lCluster->GetLink())
				continue;

				
			GetBindShapeMatrix(lCluster, lVertexTransformMatrix);

			char* clusterName = const_cast<char *>(lCluster->GetName());

			MESHIMPORT::MeshBone bone;

			ClusterBoneMap* clusterMap = new ClusterBoneMap();
			clusterMap->clusterID = j;
			clusterMap->pMesh = pMesh;
			clusterMap->clusterName = clusterName;

			AddClusterInfo(clusterMap);

			 int k;
			 int lVertexIndexCount = lCluster->GetControlPointIndicesCount();

			 for (k = 0; k < lVertexIndexCount; ++k) 
			 {            
				 int lIndex = lCluster->GetControlPointIndices()[k];
				 double lWeight = lCluster->GetControlPointWeights()[k];

				 int count = lBoneCount[lIndex];

				 float* lSrcVertex = &pVertexArray[lIndex].mPos[0];
				 KFbxVector4 srcVec((double)lSrcVertex[0],(double)lSrcVertex[1],(double)lSrcVertex[2]);
				 KFbxVector4 dstVec;

				 dstVec = lVertexTransformMatrix.MultT(srcVec);
	           
				 pVertexArray[lIndex].mPos[0] = (float)dstVec.mData[0];
				 pVertexArray[lIndex].mPos[1] = (float)dstVec.mData[1];
				 pVertexArray[lIndex].mPos[2] = (float)dstVec.mData[2];

				 if((lWeight == 0.0) || (count >= MAX_BONES))
					  continue;

				  pVertexArray[lIndex].mBone[count] = j;
				  pVertexArray[lIndex].mWeight[count] = (float)lWeight;

				  lBoneCount[lIndex] += 1;

			 }
		}

		

	}



	delete [] lBoneWeight;
	delete [] lBoneCount;
}


// Scale all the elements of a matrix.
inline void MeshImportFBX::MatrixScale(KFbxXMatrix& pMatrix, double pValue)
{
    int i,j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pMatrix[i][j] *= pValue;
        }
    }
}

// Add a value to all the elements in the diagonal of the matrix.
inline void MeshImportFBX::MatrixAddToDiagonal(KFbxXMatrix& pMatrix, double pValue)
{
    pMatrix[0][0] += pValue;
    pMatrix[1][1] += pValue;
    pMatrix[2][2] += pValue;
    pMatrix[3][3] += pValue;
}

// Sum two matrices element by element.
inline void MeshImportFBX::MatrixAdd(KFbxXMatrix& pDstMatrix, KFbxXMatrix& pSrcMatrix)
{
    int i,j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pDstMatrix[i][j] += pSrcMatrix[i][j];
        }
    }
}

  

void MeshImportFBX::ImportAnimation()
{
	KArrayTemplate<KString*> takeNames;
	m_scene->FillTakeNameArray(takeNames);
	int takeCount = takeNames.GetCount();
	if(takeCount == 0)
		return;

	m_takeName = NULL;
	m_takeInfo = NULL;
	for(int t = 0; t < takeCount && m_takeInfo == NULL; t++ )
	{
		m_takeName = takeNames[t];
		m_scene->SetCurrentTake( m_takeName->Buffer() );
		m_takeInfo =  m_scene->GetTakeInfo( *m_takeName );
	}

	KTime start = 0;
	KTime stop = 0;

    if (m_takeInfo)
    {
        start = m_takeInfo->mLocalTimeSpan.GetStart();
        stop = m_takeInfo->mLocalTimeSpan.GetStop();
    }
    else
    {
		// Take the time line value
		KTimeSpan lTimeLine;
		m_scene->GetGlobalTimeSettings().GetTimelineDefautTimeSpan(lTimeLine);

        start = lTimeLine.GetStart();
        stop  = lTimeLine.GetStop();
    }

	KTimeSpan span(start, stop);
	float dTime = 0.02f;// Magic timestep from clothing tool NX_TIME_STEP
	int numFrames = (int)(span.GetDuration().GetSecondDouble()/dTime);
	float duration = (float)numFrames * dTime;

	meshWorldAnimXforms.resize(meshBones.size());
	meshTrackPtrs.resize(meshBones.size() );
	meshTracks.resize(meshBones.size() );
	meshPoses.resize(meshBones.size() * numFrames);

	meshAnimation.mName = m_takeName->Buffer();
	meshAnimation.mTracks = &meshTrackPtrs[0];
	meshAnimation.mTrackCount = (int)meshBones.size();
	meshAnimation.mFrameCount = numFrames;
	meshAnimation.mDuration = duration;
	meshAnimation.mDtime = dTime * 200.0f;//compensate for fricken magic number in SkeletalAnim.cpp

	for(int b=0;b<meshBones.size();b++)
	{
		KFbxNode* node = meshNodes[b];
		MESHIMPORT::MeshBone& bone = meshBones[b];
		MESHIMPORT::MeshAnimTrack& track = meshTracks[b];
		meshTrackPtrs[b] = &track;

		track.SetName(bone.GetName());
		track.mPose = &meshPoses[numFrames*b];
		track.mFrameCount = numFrames;
		track.mDuration = duration;
		track.mDtime = dTime * 200.0f;//compensate for fricken magic number in SkeletalAnim.cpp
	}

	KTime keyTime = start;
	KTime stepTime;
	stepTime.SetSecondDouble(dTime);

	for(int f=0;f<numFrames; f++, keyTime+=stepTime)
	{
		//Get Globals
		for(int b=0;b<meshBones.size();b++)
		{
			KFbxNode* node = meshNodes[b];
			meshWorldAnimXforms[b] = node->GetGlobalFromCurrentTake(keyTime);
		}

		//Convert to locals
		for(int b=0;b<meshBones.size();b++)
		{
			MESHIMPORT::MeshBone& bone = meshBones[b];
			MESHIMPORT::MeshAnimTrack& track = meshTracks[b];
		
			KFbxXMatrix localAnimXform; 
			if(bone.mParentIndex == -1)
				localAnimXform = meshWorldAnimXforms[b];
			else
				localAnimXform = meshWorldAnimXforms[bone.mParentIndex].Inverse() * meshWorldAnimXforms[b];

			float localAnimXformF[16];
			for(int x = 0; x < 16; x++ )
			{
				localAnimXformF[x] = ((double*)localAnimXform)[x];
			}

			float t[3];
			float r[4];
			float s[3];
			fmi_decomposeTransform(localAnimXformF, t, r, s);

			track.SetPose(f, t, r, s);
		}
	}

	m_callback->importAnimation(meshAnimation);
}


bool MeshImportFBX::importSkeletonRecursive(
	KFbxNode* node, 
	int parentBone, 
	int& boneAllocator)
{
	bool result = true;

	int boneIndex = boneAllocator;
	boneAllocator++;

	//reserved so this is efficient
	meshBones.resize(boneIndex+1);
	meshNodes.resize(boneIndex+1);

	meshNodes[boneIndex] = node;

	const char* nodeName = meshStrings.Get(node->GetName()).Get();
	MESHIMPORT::MeshBone& milBone = meshBones[boneIndex];
	milBone.SetName(nodeName);
	milBone.mParentIndex = parentBone;

	int childCount = node->GetChildCount();
	for(int n = 0; n < childCount; n++)
	{
		KFbxNode* child = node->GetChild(n);

		result = result && importSkeletonRecursive(child, boneIndex, boneAllocator);
	}

	return result;
}

void MeshImportFBX::ImportSkeleton()
{
	bool result = true;
	
	
	meshSkeleton.SetName("skeleton");

	
	meshNodes.reserve(m_scene->GetNodeCount());
	meshBones.reserve(m_scene->GetNodeCount());

	int boneAllocator = 0;
	result = importSkeletonRecursive(m_scene->GetRootNode(), -1, boneAllocator);


	meshSkeleton.SetBones((int)meshBones.size(), &meshBones[0]);
	meshWorldBindPoseXforms.resize(meshBones.size());
	meshWorldBindShapeXforms.resize(meshBones.size());


	for( int n = 0; n < meshBones.size(); n++)
	{
		KFbxNode* node = m_scene->GetNode(n);
	    KFbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
		if (!nodeAttribute)
			continue;

		if (nodeAttribute->GetAttributeType() != KFbxNodeAttribute::eMESH)
			continue;
    
		KFbxMesh* mesh = (KFbxMesh*) nodeAttribute;
	
		int lSkinCount=mesh->GetDeformerCount(KFbxDeformer::eSKIN);
		if(lSkinCount == 0)
			continue;

		for(int s = 0; s < lSkinCount; s++)
		{
			KFbxSkin* skin = ((KFbxSkin *)(mesh->GetDeformer(0, KFbxDeformer::eSKIN)));

			int clusterCount = skin->GetClusterCount();
			if(clusterCount == 0)
				continue;

			for (int c=0; c < clusterCount; c++)
			{
				KFbxCluster* cluster = skin->GetCluster(c);

				KFbxNode* link = cluster->GetLink();
				if (!link)
					continue;

				const char* linkName = link->GetName();

				bool boneFound = false;
				int b;
				for(b = 0; b < meshBones.size(); b++)
				{
					if(link == meshNodes[b])//0==strcmp(linkName, meshBones[b].GetName()))
					{
						boneFound = true;
						break;
					}
				}
				if(!boneFound)
					continue;

				cluster->GetTransformLinkMatrix(meshWorldBindPoseXforms[b]);
				//DH:  Removed because we need to update it later anyways if it is bound to two different meshes
				//cluster->GetTransformMatrix(meshWorldBindShapeXforms[b]);
			}
		}
	}

	for(int b = 0; b < meshBones.size(); b++)
	{
		MESHIMPORT::MeshBone& bone = meshBones[b];
		KFbxXMatrix& worldBindPose = meshWorldBindPoseXforms[b];
		KFbxXMatrix localBindPose;
		if(bone.mParentIndex == -1)
			localBindPose = worldBindPose;
		else
		{
			KFbxXMatrix& parentWorldBindPose = meshWorldBindPoseXforms[bone.mParentIndex];

			localBindPose = parentWorldBindPose.Inverse() * worldBindPose;
		}

		float localBindPoseF[16];
		for(int x = 0; x < 16; x++ )
		{
			localBindPoseF[x] = ((double*)localBindPose)[x];
		}

		float t[3];
		float r[4];
		float s[3];
		fmi_decomposeTransform(localBindPoseF, t, r, s);

		bone.mPosition[0] = t[0];
		bone.mPosition[1] = t[1];
		bone.mPosition[2] = t[2];
		bone.mOrientation[0] = r[0];
		bone.mOrientation[1] = r[1];
		bone.mOrientation[2] = r[2];
		bone.mOrientation[3] = r[3];
		bone.mScale[0] = s[0];
		bone.mScale[1] = s[1];
		bone.mScale[2] = s[2];
	}

	m_callback->importSkeleton(meshSkeleton);
}



int sortPolygonMaterials( const void* a, const void* b )
{
	const PolygonMaterial& tmA = *(const PolygonMaterial*)a;
	const PolygonMaterial& tmB = *(const PolygonMaterial*)b;

	int diff = tmA.materialIndex - tmB.materialIndex;
	if( diff )
	{
		return diff;
	}

	return tmA.polygonIndex - tmB.polygonIndex;
}




KFbxXMatrix MeshImportFBX::GetGeometry(KFbxNode* pNode) {
    KFbxVector4 lT, lR, lS;
    KFbxXMatrix lGeometry;

    lT = pNode->GetGeometricTranslation(KFbxNode::eSOURCE_SET);
    lR = pNode->GetGeometricRotation(KFbxNode::eSOURCE_SET);
    lS = pNode->GetGeometricScaling(KFbxNode::eSOURCE_SET);

    lGeometry.SetT(lT);
    lGeometry.SetR(lR);
    lGeometry.SetS(lS);

    return lGeometry;
}



// Get the global position.
// Do not take in account the geometric transform.
KFbxXMatrix MeshImportFBX::GetGlobalPosition(KFbxNode* pNode, KTime& pTime, KFbxXMatrix& pParentGlobalPosition)
{
    // Ideally this function would use parent global position and local position to
    // compute the global position.
    // Unfortunately the equation 
    //    lGlobalPosition = pParentGlobalPosition * lLocalPosition
    // does not hold when inheritance type is other than "Parent" (RSrs). To compute
    // the parent rotation and scaling is tricky in the RrSs and Rrs cases.
    // This is why GetGlobalFromCurrentTake() is used: it always computes the right
    // global position.

    return pNode->GetGlobalFromCurrentTake(pTime);
}


// Get the global position of the node for the current pose.
// If the specified node is not part of the pose, get its
// global position at the current time.
KFbxXMatrix MeshImportFBX::GetGlobalPosition(KFbxNode* pNode, KTime& pTime, KFbxPose* pPose, KFbxXMatrix* pParentGlobalPosition)
{
    KFbxXMatrix lGlobalPosition;
    bool        lPositionFound = false;

    if (pPose)
    {
        int lNodeIndex = pPose->Find(pNode);

        if (lNodeIndex > -1)
        {
            // The bind pose is always a global matrix.
            // If we have a rest pose, we need to check if it is
            // stored in global or local space.
            if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
            {
                lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
            }
            else
            {
                // We have a local matrix, we need to convert it to
                // a global space matrix.
                KFbxXMatrix lParentGlobalPosition;

                if (pParentGlobalPosition)
                {
                    lParentGlobalPosition = *pParentGlobalPosition;
                }
                else
                {
                    if (pNode->GetParent())
                    {
                        lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
                    }
                }

                KFbxXMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
                lGlobalPosition = lParentGlobalPosition * lLocalPosition;
            }

            lPositionFound = true;
        }
    }

    if (!lPositionFound)
    {
        // There is no pose entry for that node, get the current global position instead
        lGlobalPosition = GetGlobalPosition(pNode, pTime, *pParentGlobalPosition);
    }

    return lGlobalPosition;
}

// Get the matrix of the given pose
KFbxXMatrix MeshImportFBX::GetPoseMatrix(KFbxPose* pPose, int pNodeIndex)
{
    KFbxXMatrix lPoseMatrix;
    KFbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

    memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

    return lPoseMatrix;
}







