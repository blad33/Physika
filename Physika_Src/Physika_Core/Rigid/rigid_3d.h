#pragma once
#include <iostream>
#include "rigid_base.h"
#include "Vectors/vector_3d.h"
#include "Matrices/matrix_3x3.h"
#include "Quaternion/quaternion.h"

namespace Physika {
	template <typename Scalar>
	class Rigid<Scalar, 3>
	{
	public:
		typedef typename Vector<Scalar, 3> TranslationDOF;
		typedef typename Vector<Scalar, 3> RotationDOF;

		COMM_FUNC Rigid()
			: m_p(0)
			, m_quat(Quaternion<Scalar>::Identity())
		{};

		COMM_FUNC Rigid(Vector<Scalar, 3> p, Quaternion<Scalar> quat)
			: m_p(p)
			, m_quat(quat)
		{};

		COMM_FUNC ~Rigid() {};

		COMM_FUNC Vector<Scalar, 3> getCenter() { return m_p; }

		COMM_FUNC SquareMatrix<Scalar, 3> getRotationMatrix()
		{
			return m_quat.get3x3Matrix();
		}

		COMM_FUNC Quaternion<Scalar> getOrientation() { return m_quat; }

	private:
		Vector<Scalar, 3> m_p;
		Quaternion<Scalar> m_quat;
	};

	template class Rigid<float, 3>;
	template class Rigid<double, 3>;

	typedef Rigid<float, 3> Rigid3f;
	typedef Rigid<double, 3> Rigid3d;
}  //end of namespace Physika
