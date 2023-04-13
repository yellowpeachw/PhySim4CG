#include "RigidBodySystemSimulator.h"

// Construtors
RigidBodySystemSimulator::RigidBodySystemSimulator(){
	m_iTestCase = 0;
	initDemo();
}

// Functions
const char* RigidBodySystemSimulator::getTestCasesStr(){
	return "demo1, demo2, demo3, demo4";
}

const char* RigidBodySystemSimulator::getMethodCasesStr() {
	return "RigidBody";
}

void RigidBodySystemSimulator::initUI(DrawingUtilitiesClass* DUC){
	this->DUC = DUC;
}

void RigidBodySystemSimulator::reset(){
	m_mouse.x = m_mouse.y = 0;
	m_trackmouse.x = m_trackmouse.y = 0;
	m_oldtrackmouse.x = m_oldtrackmouse.y = 0;
}

void RigidBodySystemSimulator::drawFrame(ID3D11DeviceContext* pd3dImmediateContext){
	for (const auto& rect: rects)
	{
		DUC->setUpLighting(Vec3(0, 0, 0), 0.4 * Vec3(1, 1, 1), 2000.0, Vec3(0.5, 0.5, 0.5));
		Mat4 translatMat;
		translatMat.initTranslation(rect.center.x, rect.center.y, rect.center.z);
		Mat4 rotMat = rect.q.getRotMat();
		Mat4 scaleMat;
		scaleMat.initScaling(rect.shape.x, rect.shape.y, rect.shape.z);
		GamePhysics::Mat4 worldMat = scaleMat * rotMat * translatMat;
		DUC->drawRigidBody(worldMat);
	}
}

void RigidBodySystemSimulator::notifyMethodChanged(int methodCase){}
void RigidBodySystemSimulator::notifyCaseChanged(int testCase){
	m_iTestCase = testCase;
	switch (m_iTestCase)
	{
	case 0:
		cout << "A simple one-step test !\n";
		break;
	case 1:
		cout << "Simple single body simulation !\n";
		break;
	case 2:
		cout << "Two-rigid-body collision scene !\n";
		break;
	default:
		cout << "Complex simulation !\n";
		break;
	}
	initDemo();
}

void RigidBodySystemSimulator::initDemo() {
	rects.clear();
	Rect sample;
	print = 1;
	switch (m_iTestCase)
	{
	case 0:
		{
			sample.center = Vec3(0, 0, 0);
			sample.shape = Vec3(1, 0.6, 0.5);
			sample.q = Quat(0, 0, sin(M_PI/4), cos(M_PI / 4));
			//sample.q = Quat(0, 0, 0, 1);
			sample.velocity = Vec3(0, 0, 0);
			sample.omega = Vec3(0, 0, 0);
			sample.mass = 2;
			sample.initInertia();
		}
		rects.push_back(sample);
		break;
	default:
		break;
	}
}

void RigidBodySystemSimulator::externalForcesCalculations(float timeElapsed){}

void RigidBodySystemSimulator::simulateTimestep(float timeStep){
	if ((m_iTestCase == 0) && (!print)) return;
	addExternalForces(timeStep);
	updatePosition(timeStep);
	if ((m_iTestCase == 0) && (print)) 
	{
		printf("linear velocity = (%f, %f, %f)\n", rects[0].velocity.x, rects[0].velocity.y, rects[0].velocity.z);
		printf("angular velocity = (%f, %f, %f)\n", rects[0].omega.x, rects[0].omega.y, rects[0].omega.z);
		Vec3 test_pos(-0.3, -0.5, -0.25);
		Vec3 test_vel = rects[0].velocity + cross(rects[0].omega, test_pos - rects[0].center);
		printf("velocity of point (-0.3, -0.5, -0.25) = (%f, %f, %f)\n", test_vel.x, test_vel.y, test_vel.z);
		print = 0;
	}
}

void RigidBodySystemSimulator::addExternalForces(float timeStep) {
	Vec3 force_pos(0.3, 0.5, 0.25);
	Vec3 force(1, 1, 0);
	for (auto& rect : rects) {
		// calculate rotMat and Inertia
		Mat4 rotMat = rect.q.getRotMat();
		Mat4 rotMatTrans = rotMat;
		rotMatTrans.transpose();
		Mat4 curInertia = rotMat * rect.inertia * rotMatTrans;
		// update omega
		Vec3 r = rotMat * (force_pos - rect.center);
		Vec3 tau = cross(r, force);
		rect.omega += curInertia.inverse() * tau * timeStep;
		// update velocity
		rect.velocity += force / rect.mass * timeStep;
	}
}

void RigidBodySystemSimulator::updatePosition(float timeStep) {
	for (auto& rect : rects) {
		// update q
		rect.q = rect.q + Quat(0.5 * timeStep * rect.omega.x, 0.5 * timeStep * rect.omega.y, 0.5 * timeStep * rect.omega.z, 0.0) * rect.q;
		rect.q = rect.q.unit();
		// update position
		rect.center += rect.velocity * timeStep;
	}
}

void RigidBodySystemSimulator::onClick(int x, int y){
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}

void RigidBodySystemSimulator::onMouse(int x, int y){
	m_oldtrackmouse.x = x;
	m_oldtrackmouse.y = y;
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}

// ExtraFunctions
int RigidBodySystemSimulator::getNumberOfRigidBodies(){
	return 0;
}
Vec3 RigidBodySystemSimulator::getPositionOfRigidBody(int i){
	return Vec3(0, 0, 0);
}
Vec3 RigidBodySystemSimulator::getLinearVelocityOfRigidBody(int i){
	return Vec3(0, 0, 0);
}
Vec3 RigidBodySystemSimulator::getAngularVelocityOfRigidBody(int i){
	return Vec3(0, 0, 0);
}
void RigidBodySystemSimulator::applyForceOnBody(int i, Vec3 loc, Vec3 force){}
void RigidBodySystemSimulator::addRigidBody(Vec3 position, Vec3 size, int mass){}
void RigidBodySystemSimulator::setOrientationOf(int i, Quat orientation){}
void RigidBodySystemSimulator::setVelocityOf(int i, Vec3 velocity){}