#include "Gravity.h"
#include "Physika_Framework/Framework/Node.h"
#include "Physika_Core/Utilities/cuda_helper_math.h"
#include "Physika_Framework/Framework/FieldVar.h"
#include "Physika_Framework/Framework/FieldArray.h"

namespace Physika
{
IMPLEMENT_CLASS_1(Gravity, TDataType)

template<typename TDataType>
Gravity<TDataType>::Gravity()
	: ForceModule()
{
	if (Coord::dims() == 3)
		m_gravity = Coord(0, -9.8f, 0);
}

template<typename TDataType>
Gravity<TDataType>::~Gravity()
{
}

template <typename Coord>
__global__ void K_AddGravity(
	DeviceArray<Coord> points,
	Coord force)
{
	int pId = threadIdx.x + (blockIdx.x * blockDim.x);
	if (pId >= points.size()) return;

	points[pId] += force;
}

template<typename TDataType>
bool Gravity<TDataType>::applyForce()
{
	auto mstate = getParent()->getMechanicalState();
	if (mstate->getMaterialType() == MechanicalState::RIGIDBODY)
	{
		auto massField = mstate->getField<HostVarField<Real>>(MechanicalState::mass());
		auto forceField = mstate->getField<HostVarField<Coord>>(MechanicalState::force());
		Coord nForce = forceField->getValue() + m_gravity * massField->getValue();
		forceField->setValue(nForce);
	}
	else
	{
		auto massField = mstate->getField<HostVarField<Real>>(MechanicalState::mass());
		auto forceField = mstate->getField<DeviceArrayField<Coord>>(MechanicalState::force());

		auto oldForce = forceField->getValue();
		Coord deltaF = massField->getValue()*m_gravity;

		uint pDims = cudaGridSize(oldForce.size(), BLOCK_SIZE);
		K_AddGravity << <pDims, BLOCK_SIZE >> > (oldForce, deltaF);
	}

	return true;
}

}


