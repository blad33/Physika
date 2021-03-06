#include "Physika_Framework/Framework/Node.h"
#include "CollidableSDF.h"

namespace Physika {

	IMPLEMENT_CLASS_1(CollidableSDF, TDataType)

	template<typename TDataType>
	CollidableSDF<TDataType>::CollidableSDF()
		: CollidableObject(CollidableObject::SIGNED_DISTANCE_TYPE)
		, m_sdf(nullptr)
	{
	}


	template<typename TDataType>
	bool CollidableSDF<TDataType>::initializeImpl()
	{
		if (m_sdf == nullptr)
		{
			Coord lo(0.0f);
			Coord hi(1.0f);
			std::shared_ptr<DistanceField3D<DataType3f>> box = std::make_shared<DistanceField3D<DataType3f>>();
			box->SetSpace(lo - 0.025f, hi + 0.025f, 105, 105, 105);
			box->DistanceFieldToBox(lo, hi, true);
			this->setSDF(box);
		}

		return true;
	}
}