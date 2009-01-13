#ifndef BOUNDINGVOLUME_H

#define BOUNDINGVOLUME_H

/*----------------------------------------------------------------------
		Copyright (c) 2004 Open Dynamics Framework Group
					www.physicstools.org
		All rights reserved.

		Redistribution and use in source and binary forms, with or without modification, are permitted provided
		that the following conditions are met:

		Redistributions of source code must retain the above copyright notice, this list of conditions
		and the following disclaimer.

		Redistributions in binary form must reproduce the above copyright notice,
		this list of conditions and the following disclaimer in the documentation
		and/or other materials provided with the distribution.

		Neither the name of the Open Dynamics Framework Group nor the names of its contributors may
		be used to endorse or promote products derived from this software without specific prior written permission.

		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES,
		INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
		DISCLAIMED. IN NO EVENT SHALL THE INTEL OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
		EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
		LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
		IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
		THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------*/


#include "viewtest.h"
#include "UserMemAlloc.h"

#include <float.h>
#include <assert.h>


#include <vector>

enum BoundingVolumeFlag
{
	BV_LEAF  = (1<<0),
	BV_XAXIS = (1<<1),
	BV_YAXIS = (1<<2),
	BV_ZAXIS = (1<<3),
	BV_LEFT  = (1<<4),
	BV_RIGHT = (1<<5),
	BV_NODE  = (1<<6)
};


class BRect3d
{
public:

	bool Intersect(const BRect3d &test) const;
	void GetCenter(float *center) const;

	void InitMinMax(void);
	void MinMax(const BRect3d &b);
	void SetMin(const float *m);
	void SetMax(const float *m);
	void SetMin(float x,float y,float z);
	void SetMax(float x,float y,float z);
	float GetDistance2(void) const;


	float mMin[3];
	float mMax[3];
};


class BoundingVolume
{
public:
	BoundingVolume(void)
	{
		mVolumeFrameNo = 0xFFFFFFFF;
	}

	virtual void GetBound(BRect3d &bound) = 0;

	virtual void Render(FrustumInterface * /* iface */,ViewTest /* state */)
  {
  }

	unsigned int GetVolumeFrameNo(void) const { return mVolumeFrameNo; };
	void SetVolumeFrameNo(unsigned int fno) { mVolumeFrameNo = fno; };

private:
	unsigned int          mVolumeFrameNo;
};


class BoundingVolumeSystem;

typedef USER_STL::vector< BoundingVolume * > BoundingVolumeVector;

class BoundingVolumeAABV
{
public:
	BoundingVolumeAABV(void)
	{
		mFlags = 0;
		mLeft = 0;
		mRight = 0;
	}

	~BoundingVolumeAABV(void);

	void Add(BoundingVolume *item)
	{
		mList.push_back(item);
	}

	void SetBound(const BRect3d &bound)
	{
		mBound = bound;
	};

	void SetBoundingVolumeFlag(BoundingVolumeFlag flag)   { mFlags|=flag; };
	void ClearBoundingVolumeFlag(BoundingVolumeFlag flag) { mFlags&=~flag; };

	bool HasBoundingVolumeFlag(BoundingVolumeFlag flag) const
	{
		if ( mFlags & flag ) return true;
		return false;
	};

	void Split(BoundingVolumeSystem *system,unsigned int depth);


	bool Intersect(const BRect3d &bound) const
	{
		return mBound.Intersect(bound);
	};

	void Render(ViewTest state,FrustumInterface *iface,unsigned int fno);

	void AddBoundingVolume(BoundingVolume *bv)
	{
		BRect3d b;
		bv->GetBound(b);
		mBound.MinMax(b);
		mList.push_back(bv);
	}

private:

	void BuildTwo(BoundingVolumeSystem *aabv,unsigned int axis,const float *midpoint);

	void SplitRect(unsigned int axis,
								 const BRect3d &source,
								 BRect3d       &b1,
								 BRect3d       &b2,
								 const float *midpoint);

	unsigned int             mFlags;
	BRect3d                   mBound;
	BoundingVolumeVector     mList;
	BoundingVolumeAABV      *mLeft;
	BoundingVolumeAABV      *mRight;

};

class BoundingVolumeSystem
{
public:

	BoundingVolumeSystem(unsigned int minleaf=2,         // if less than 2 items in a leaf node, stop splitting.
											 unsigned int maxdepth=12,       // don't recurse more than 12 ply deep
											 float minaxis=0.01f);      // if the bounding volume is less than 2 meters, don't recurse any further.

	~BoundingVolumeSystem(void);

	BoundingVolumeAABV * GetBoundingVolumeAABV(const BRect3d &bound,const BoundingVolumeVector &list);

	void Render(FrustumInterface *iface); // render relative to this frustum interface

	void Add(BoundingVolume *item)
	{
		mRoot->AddBoundingVolume(item);
	}

	void Build(void);

private:
	unsigned int                  mMinLeaf;
	unsigned int                  mMaxDepth;
	float                         mMinAxis;
	bool                          mBuilt;
	unsigned int                  mVolumeFrameNo;
	BoundingVolumeAABV           *mRoot;
};

#endif