#include "CollidablePoints.h"
#include "Physika_Core/Utilities/Function1Pt.h"
#include "Physika_Framework/Framework/DeviceContext.h"
#include "Physika_Framework/Framework/MechanicalState.h"
#include "Physika_Framework/Framework/Node.h"
#include "Physika_Framework/Topology/PointSet.h"
#include "Physika_Framework/Mapping/RigidToPoints.h"
#include "Physika_Framework/Mapping/PointsToPoints.h"
#include "Physika_Core/Utilities/Reduction.h"

namespace Physika
{
	IMPLEMENT_CLASS_1(CollidablePoints, TDataType)

	template<typename TDataType>
	CollidablePoints<TDataType>::CollidablePoints()
		: CollidableObject(CollidableObject::POINTSET_TYPE)
		, m_bUniformRaidus(true)
		, m_radius(0.005)
	{
	}
	
	template<typename TDataType>
	CollidablePoints<TDataType>::~CollidablePoints()
	{
	}

	template<typename TDataType>
	void CollidablePoints<TDataType>::setPositions(DeviceArray<Coord>& centers)
	{
	}

	template<typename TDataType>
	void CollidablePoints<TDataType>::setVelocities(DeviceArray<Coord>& vel)
	{
	}

	template<typename TDataType>
	void CollidablePoints<TDataType>::setRadii(DeviceArray<Coord>& radii)
	{

	}

	template<typename TDataType>
	bool Physika::CollidablePoints<TDataType>::initializeImpl()
	{
		Node* parent = getParent();
		if (parent == NULL)
		{
			Log::sendMessage(Log::Error, "Should insert this module into a node!");
			return false;
		}

		PointSet<TDataType>* pSet = dynamic_cast<PointSet<TDataType>*>(parent->getTopologyModule().get());
		if (pSet == NULL)
		{
			Log::sendMessage(Log::Error, "The topology module is not supported!");
			return false;
		}

		auto initPoints = pSet->getPoints();

		m_positions.resize(initPoints->size());
		Function1Pt::copy(m_positions, *initPoints);

		m_velocities.resize(initPoints->size());
		m_velocities.reset();

		auto mstate = getParent()->getMechanicalState();
		auto mType = getParent()->getMechanicalState()->getMaterialType();

		if (mType == MechanicalState::RIGIDBODY)
		{
			auto mapping = std::make_shared<RigidToPoints<TDataType>>();
			auto center = mstate->getField<HostVarField<Coord>>(MechanicalState::position())->getValue();
			auto rotation = mstate->getField<HostVarField<Matrix>>(MechanicalState::rotation())->getValue();

			mapping->initialize(Rigid(center, Quaternion<Real>(rotation)), m_positions);
			m_mapping = mapping;
		}
		else
		{
			auto mapping = std::shared_ptr<PointsToPoints<TDataType>>();
			m_mapping = mapping;
		}
	}


	template<typename TDataType>
	void CollidablePoints<TDataType>::updateCollidableObject()
	{
		auto mstate = getParent()->getMechanicalState();
		auto mType = mstate->getMaterialType();
		if (mType == MechanicalState::RIGIDBODY)
		{
			auto center = mstate->getField<HostVarField<Coord>>(MechanicalState::position())->getValue();
			auto rotation = mstate->getField<HostVarField<Matrix>>(MechanicalState::rotation())->getValue();

			auto pSet = TypeInfo::CastPointerDown<PointSet<TDataType>>(getParent()->getTopologyModule());

			auto mp = std::dynamic_pointer_cast<RigidToPoints<TDataType>>(m_mapping);

			mp->applyTransform(Rigid(center, Quaternion<Real>(rotation)), m_positions);
		}
		else
		{
			std::shared_ptr<Field> pos = mstate->getField(MechanicalState::position());
			std::shared_ptr<DeviceArrayField<Coord>> pBuf = TypeInfo::CastPointerDown<DeviceArrayField<Coord>>(pos);

			std::shared_ptr<Field> vel = mstate->getField(MechanicalState::velocity());
			std::shared_ptr<DeviceArrayField<Coord>> vBuf = TypeInfo::CastPointerDown<DeviceArrayField<Coord>>(vel);

			Function1Pt::copy(m_positions, *(pBuf->getDataPtr()));
			Function1Pt::copy(m_velocities, *(vBuf->getDataPtr()));
		}
	}

	template<typename TDataType>
	void CollidablePoints<TDataType>::updateMechanicalState()
	{
		auto mstate = getParent()->getMechanicalState();
		auto mType = mstate->getMaterialType();
		auto dc = getParent()->getMechanicalState();
		if (mType == MechanicalState::RIGIDBODY)
		{
			auto center = mstate->getField<HostVarField<Coord>>(MechanicalState::position())->getValue();
			auto rotation = mstate->getField<HostVarField<Matrix>>(MechanicalState::rotation())->getValue();
			auto vel = mstate->getField<HostVarField<Coord>>(MechanicalState::velocity())->getValue();
			
			HostArray<Coord> hPos;
			HostArray<Coord> hInitPos;
			DeviceArray<Coord> dInitPos;
			hPos.resize(m_positions.size());
			hInitPos.resize(m_positions.size());
			dInitPos.resize(m_positions.size());

			auto mp = std::dynamic_pointer_cast<RigidToPoints<TDataType>>(m_mapping);
			mp->applyTransform(Rigid(center, Quaternion<Real>(rotation)), dInitPos);

			Real dt = getParent()->getDt();

			Function1Pt::copy(hPos, m_positions);
			Function1Pt::copy(hInitPos, dInitPos);
			Coord displacement(0);
			Coord angularVel(0);
			int nn = 0;
			for (int i = 0; i < hPos.size(); i++)
			{
				Coord r = hInitPos[i] - center;
				if ((hInitPos[i] - hPos[i]).norm() > EPSILON && r.norm() > EPSILON)
				{
					displacement += (hPos[i] - hInitPos[i]);
					Coord vel_i = (hPos[i] - hInitPos[i]) / dt;
					angularVel += r.cross(vel_i) / r.normSquared();
					nn++;
				}
			}
			if (nn > 0)
			{
				displacement /= nn;
				angularVel /= nn;
			}
			
			dc->getField<HostVarField<Coord>>(MechanicalState::position())->setValue(center + displacement);
			dc->getField<HostVarField<Coord>>(MechanicalState::velocity())->setValue(vel + displacement/ dt);
			dc->getField<HostVarField<Coord>>(MechanicalState::angularVelocity())->setValue(angularVel);

			hPos.release();
			hInitPos.release();
			dInitPos.release();
		}
		else
		{
			auto posArr = dc->getField<DeviceArrayField<Coord>>(MechanicalState::position());
			auto velArr = dc->getField<DeviceArrayField<Coord>>(MechanicalState::velocity());

			Function1Pt::copy(*(posArr->getDataPtr()), m_positions);
			Function1Pt::copy(*(velArr->getDataPtr()), m_velocities);
		}
	}
}