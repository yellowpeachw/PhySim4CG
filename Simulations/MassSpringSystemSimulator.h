#ifndef MASSSPRINGSYSTEMSIMULATOR_h
#define MASSSPRINGSYSTEMSIMULATOR_h
#include "Simulator.h"
#include "stdio.h"

// Do Not Change
#define EULER 0
#define LEAPFROG 1
#define MIDPOINT 2
// Do Not Change

class MassSpringSystemSimulator:public Simulator{
public:
	// Construtors
	MassSpringSystemSimulator();
	
	// UI Functions
	const char * getTestCasesStr();
	const char* getMethodCasesStr();
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
		usingEuler = (usingEuler)? false: true;
		if (usingEuler) printf("current method is Euler\n");
		else printf("current method is Midpoint\n");
	};

	// Specific Functions
	void setMass(float mass);
	void setStiffness(float stiffness);
	void setDampingFactor(float damping);
	int addMassPoint(Vec3 position, Vec3 Velocity, bool isFixed);
	void addSpring(int masspoint1, int masspoint2, float initialLength);
	int getNumberOfMassPoints();
	int getNumberOfSprings();
	Vec3 getPositionOfMassPoint(int index);
	Vec3 getVelocityOfMassPoint(int index);
	void applyExternalForce(Vec3 force);
	void calculateSpringForce(const std::vector<Vec3>& positions);
	void calculateSpringForceWithDamping(const std::vector<Vec3>& positions, const std::vector<Vec3>& velocities);
	void addGravity();
	
	// Do Not Change
	void setIntegrator(int integrator) {
		m_iIntegrator = integrator;
	}

	// init function
	void initNaieve();
	void initComplex();

private:
	// Data Attributes
	float m_fMass;
	float m_fStiffness;
	float m_fDamping;
	int m_iIntegrator;
	float m_flength;
	float m_ballSize;
	Vec3 gravity;

	int m_size_n;
	int m_size_m;
	Vec3 m_startPos;

	// UI Attributes
	Vec3 m_externalForce;
	Point2D m_mouse;
	Point2D m_trackmouse;
	Point2D m_oldtrackmouse;

	// test a ball fall
	int m_numOfBall;
	std::vector<Vec3> m_ballPos;
	std::vector<Vec3> m_ballVec;
	std::vector<Vec3> m_ballForce;
	std::vector<std::vector<int>> m_relation;
	std::vector<bool> m_isFixed;
	inline bool is_inside(int idx_n, int idx_m) {
		return ((idx_n >= 0) && (idx_n < m_size_n) && (idx_m >= 0) && (idx_m < m_size_m));
	}
	inline int idx2ord(int idx_n, int idx_m) {
		return idx_n * m_size_n + idx_m;
	}
	void ord2idx(int ord, int& idx_n, int& idx_m) {
		idx_n = ord / m_size_m;
		idx_m = ord % m_size_m;
	}

	// for print
	bool m_print;
};
#endif