#include <iostream>
#include <memory>
#include <cuda_runtime_api.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Physika_GUI/GlutGUI/GLApp.h"

#include "Physika_Framework/Framework/SceneGraph.h"
#include "Physika_Framework/Topology/PointSet.h"
#include "Physika_Framework/Framework/Log.h"

#include "Physika_Render/PointRenderModule.h"

#include "Physika_Dynamics/ParticleSystem/ParticleFluid.h"
#include "Physika_Dynamics/ParticleSystem/Peridynamics.h"

#include "Physika_Framework/Collision/CollidableSDF.h"
#include "Physika_Framework/Collision/CollidablePoints.h"
#include "Physika_Framework/Collision/CollisionSDF.h"
#include "Physika_Dynamics/RigidBody/RigidBody.h"
#include "Physika_Framework/Framework/Gravity.h"
#include "Physika_Dynamics/ParticleSystem/FixedPoints.h"
#include "Physika_Framework/Collision/CollisionPoints.h"

using namespace std;
using namespace Physika;

#define DEMO_1
//#define DEMO_2

#ifdef DEMO_1
void RecieveLogMessage(const Log::Message& m)
{
	switch (m.type)
	{
	case Log::Info:
		cout << ">>>: " << m.text << endl; break;
	case Log::Warning:
		cout << "???: " << m.text << endl; break;
	case Log::Error:
		cout << "!!!: " << m.text << endl; break;
	case Log::User:
		cout << ">>>: " << m.text << endl; break;
	default: break;
	}
}

void CreateScene()
{
	SceneGraph& scene = SceneGraph::getInstance();

	std::shared_ptr<Node> root = scene.createNewScene<Node>("root");
	auto collidable1 = std::make_shared<CollidableSDF<DataType3f>>();
	root->setCollidableObject(collidable1);

	// 	std::shared_ptr<Node> c1 = root->createChild<Node>("child1");
	// 	auto* pSet1 = new PointSet<Vector3f>();
	// 	c1->setTopologyModule(pSet1);

	//create child node 1
	std::shared_ptr<Node> c1 = root->createChild<Node>("child1");

	auto pSet = std::make_shared<PointSet<DataType3f>>();
	c1->setTopologyModule(pSet);

	auto pS1 = std::make_shared<ParticleFluid<DataType3f>>();
	//auto pS1 = std::make_shared<Peridynamics<DataType3f>>();
	//	auto pS1 = std::make_shared<RigidBody<DataType3f>>();
	c1->setNumericalModel(pS1);

	auto render = std::make_shared<PointRenderModule>();
	//	render->setVisible(false);
	c1->addVisualModule(render);

	auto cPoints = std::make_shared<CollidablePoints<DataType3f>>();
	c1->setCollidableObject(cPoints);

	auto gravity = std::make_shared<Gravity<DataType3f>>();
	c1->addForceModule(gravity);

	//create child node 2
	std::shared_ptr<Node> c2 = root->createChild<Node>("child2");

	auto pSet2 = std::make_shared<PointSet<DataType3f>>();
	std::vector<DataType3f::Coord> positions;
	for (float x = 0.65; x < 0.75; x += 0.005f) {
		for (float y = 0.2; y < 0.3; y += 0.005f) {
			for (float z = 0.45; z < 0.55; z += 0.005f) {
				positions.push_back(DataType3f::Coord(DataType3f::Real(x), DataType3f::Real(y), DataType3f::Real(z)));
			}
		}
	}
	pSet2->setPoints(positions);
	c2->setTopologyModule(pSet2);

	auto fixed = std::make_shared<FixedPoints<DataType3f>>();
	fixed->addPoint(0);
	fixed->addPoint(1);
	fixed->addPoint(2);
	c2->addConstraintModule(fixed);

	//	auto pS2 = std::make_shared<ParticleSystem<DataType3f>>();
	auto pS2 = std::make_shared<Peridynamics<DataType3f>>();
	//	auto pS2 = std::make_shared<RigidBody<DataType3f>>();
	c2->setNumericalModel(pS2);

	auto render2 = std::make_shared<PointRenderModule>();
	//	render->setVisible(false);
	c2->addVisualModule(render2);

	auto cPoints2 = std::make_shared<CollidablePoints<DataType3f>>();
	c2->setCollidableObject(cPoints2);

	auto gravity2 = std::make_shared<Gravity<DataType3f>>();
	c2->addForceModule(gravity2);

	//create child node 3
	std::shared_ptr<Node> c3 = root->createChild<Node>("child2");

	auto pSet3 = std::make_shared<PointSet<DataType3f>>();
	std::vector<DataType3f::Coord> positions3;
	for (float x = 0.25; x < 0.35; x += 0.005f) {
		for (float y = 0.2; y < 0.3; y += 0.005f) {
			for (float z = 0.45; z < 0.55; z += 0.005f) {
				positions3.push_back(DataType3f::Coord(DataType3f::Real(x), DataType3f::Real(y), DataType3f::Real(z)));
			}
		}
	}
	pSet3->setPoints(positions3);
	c3->setTopologyModule(pSet3);

	//	auto pS2 = std::make_shared<ParticleSystem<DataType3f>>();
	//auto pS3 = std::make_shared<Peridynamics<DataType3f>>();
	auto pS3 = std::make_shared<RigidBody<DataType3f>>();
	c3->setNumericalModel(pS3);

	auto render3 = std::make_shared<PointRenderModule>();
	//	render->setVisible(false);
	c3->addVisualModule(render3);

	auto cPoints3 = std::make_shared<CollidablePoints<DataType3f>>();
	c3->setCollidableObject(cPoints3);

	auto gravity3 = std::make_shared<Gravity<DataType3f>>();
	c3->addForceModule(gravity3);

	auto cModel = std::make_shared<CollisionSDF<DataType3f>>();
	cModel->setCollidableSDF(collidable1);
	cModel->addCollidableObject(cPoints);
	cModel->addCollidableObject(cPoints2);
	cModel->addCollidableObject(cPoints3);
	root->addCollisionModel(cModel);

	auto pModel = std::make_shared<CollisionPoints<DataType3f>>();
	pModel->addCollidableObject(cPoints);
	pModel->addCollidableObject(cPoints2);
	pModel->addCollidableObject(cPoints3);
	root->addCollisionModel(pModel);
}

int main()
{
	CreateScene();

	Log::setOutput("console_log.txt");
	Log::setLevel(Log::Info);
	Log::setUserReceiver(&RecieveLogMessage);
	Log::sendMessage(Log::Info, "Simulation begin");

	GLApp window;
	window.createWindow(1024, 768);

	window.mainLoop();

	Log::sendMessage(Log::Info, "Simulation end!");
	return 0;
}

#endif

