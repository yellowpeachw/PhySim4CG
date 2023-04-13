#ifndef RIGIDBODYSYSTEMSIMULATOR_h
#define RIGIDBODYSYSTEMSIMULATOR_h
#include "Simulator.h"
//add your header for your rigid body system, for e.g.,
//#include "rigidBodySystem.h" 

#define TESTCASEUSEDTORUNTEST 2

class RigidBodySystemSimulator:public Simulator{
public:
	// Construtors
	RigidBodySystemSimulator();
	
	// Functions
	const char* getMethodCasesStr();
	const char * getTestCasesStr();
	void initUI(DrawingUtilitiesClass * DUC);
	void reset();
	void drawFrame(ID3D11DeviceContext* pd3dImmediateContext);
	void notifyCaseChanged(int testCase);
	void notifyMethodChanged(int methodCase);
	void externalForcesCalculations(float timeElapsed);
	void simulateTimestep(float timeStep);
	void onClick(int x, int y);
	void onMouse(int x, int y);
	void changeMethod() {
		usingEuler = (usingEuler) ? false : true;
		if (usingEuler) printf("current method is Euler\n");
		else printf("current method is Midpoint\n");
	};

	// ExtraFunctions
	int getNumberOfRigidBodies();
	Vec3 getPositionOfRigidBody(int i);
	Vec3 getLinearVelocityOfRigidBody(int i);
	Vec3 getAngularVelocityOfRigidBody(int i);
	void applyForceOnBody(int i, Vec3 loc, Vec3 force);
	void addRigidBody(Vec3 position, Vec3 size, int mass);
	void setOrientationOf(int i,Quat orientation);
	void setVelocityOf(int i, Vec3 velocity);

	void initDemo();

private:
	// Attributes
	// add your RigidBodySystem data members, for e.g.,
	// RigidBodySystem * m_pRigidBodySystem; 
	Vec3 m_externalForce;

	// UI Attributes
	Point2D m_mouse;
	Point2D m_trackmouse;
	Point2D m_oldtrackmouse;

	struct Rect {
		Vec3 center;
		Vec3 shape;
		Quat q;
		Vec3 omega;
		Vec3 velocity;
		Mat4 inertia;
		float mass;
		void initInertia() {
			inertia.initScaling(1.0 / 12 * mass * (shape.y * shape.y + shape.z * shape.z), 1.0 / 12 * mass * (shape.x * shape.x + shape.z * shape.z), 1.0 / 12 * mass * (shape.y * shape.y + shape.x * shape.x));
		}
	};
	std::vector<Rect> rects;

	void addExternalForces(float timeStep);
	void updatePosition(float timeStep);
	bool print;
};
#endif