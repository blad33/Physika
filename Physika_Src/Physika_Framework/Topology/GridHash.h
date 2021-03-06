#pragma once
#include "Physika_Core/DataTypes.h"
#include "Physika_Core/Utilities/cuda_utilities.h"
#include "Physika_Core/Cuda_Array/Array.h"
#include "Physika_Framework/Topology/NeighborList.h"

namespace Physika{

	#define INVALID -1
	#define BUCKETS 8
	#define CAPACITY 16

	template<typename TDataType>
	class GridHash
	{
	public:
		typedef typename TDataType::Real Real;
		typedef typename TDataType::Coord Coord;

		GridHash();
		~GridHash();

		void setSpace(Real _h, Coord _lo, Coord _hi);

		void construct(DeviceArray<Coord>& pos);

		void clear();

		void release();

		GPU_FUNC inline int getIndex(int i, int j, int k)
		{
			if (i < 0 || i >= nx) return INVALID;
			if (j < 0 || j >= ny) return INVALID;
			if (k < 0 || k >= nz) return INVALID;

			return i + j*nx + k*nx*ny;
		}

		GPU_FUNC inline int getIndex(Coord pos)
		{
			int i = floor((pos[0] - lo[0]) / ds);
			int j = floor((pos[1] - lo[1]) / ds);
			int k = floor((pos[2] - lo[2]) / ds);

			return getIndex(i, j, k);
		}

		GPU_FUNC inline int3 getIndex3(Coord pos)
		{
			int i = floor((pos[0] - lo[0]) / ds);
			int j = floor((pos[1] - lo[1]) / ds);
			int k = floor((pos[2] - lo[2]) / ds);

			return make_int3(i, j, k);
		}

		GPU_FUNC inline int getCounter(int gId) { return counter[gId]; }

		GPU_FUNC inline int getParticleId(int gId, int n) { return ids[gId*npMax + n]; }

	public:
		int num;
		int nx, ny, nz;

		Real ds;

		Coord lo;
		Coord hi;

		int npMax;		//maximum particle number for each cell

		int* ids;
		int* counter;
	};

#ifdef PRECISION_FLOAT
	template class GridHash<DataType3f>;
#else
	template class GridHash<DataType3d>;
#endif
}
