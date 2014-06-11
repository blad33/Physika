/*
 * @file  object_bvh.h
 * @bounding volume hierarchy (BVH) of a collidable object, second level of DT-BVH [Tang et al. 2009]
 * @author Tianxiang Zhang
 * 
 * This file is part of Physika, a versatile physics simulation library.
 * Copyright (C) 2013 Physika Group.
 *
 * This Source Code Form is subject to the terms of the GNU General Public License v2.0. 
 * If a copy of the GPL was not distributed with this file, you can obtain one at:
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#ifndef PHYSIKA_GEOMETRY_BOUNDING_VOLUME_OBJECT_BVH_H_
#define PHYSIKA_GEOMETRY_BOUNDING_VOLUME_OBJECT_BVH_H_

namespace Physika{

template <typename Scalar,int Dim> class Vector;
template <typename Scalar,int Dim> class BVHBase;
template <typename Scalar,int Dim> class CollidableObject;

template <typename Scalar,int Dim>
class ObjectBVH : public BVHBase<Scalar, Dim>
{
public:
	//constructors && deconstructors
	ObjectBVH();
	~ObjectBVH();

	//get & set
	//inline void setRootNode(ObjectBVHNode* root_node);
	//inline ObjectBVHNode* getRootNode();

	//structure maintain
	//void refit();
	void buildFromObject(CollidableObject<Scalar, Dim>* collidable_object);
	//void clean();

	//collision detection
	//bool selfCollide();
	//bool collide(ObjectBVH* target);

	
protected:
	//ObjectBVHNode* root_node_;
	CollidableObject<Scalar, Dim>* collidable_object_;
};

}  //end of namespace Physika

#endif  //PHYSIKA_GEOMETRY_BOUNDING_VOLUME_OBJECT_BVH_H_