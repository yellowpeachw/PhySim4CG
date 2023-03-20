#include "MassSpringSystemSimulator.h"

MassSpringSystemSimulator::MassSpringSystemSimulator() {
	m_iTestCase = EULER;
	initNaieve();
};

void MassSpringSystemSimulator::initNaieve() {
	m_print = true;

	m_ballPos.clear();
	m_ballVec.clear();
	m_relation.clear();
	m_ballForce.clear();

	/* init for basic situation */
	m_fMass = 10;
	m_flength = 1;
	m_fStiffness = 40;
	m_ballSize = 0.01f;

	m_ballPos.push_back(Vec3(0, 0, 0));
	m_ballVec.push_back(Vec3(-1, 0, 0));
	m_ballPos.push_back(Vec3(0, 2, 0));
	m_ballVec.push_back(Vec3(1, 0, 0));

	m_numOfBall = m_ballPos.size();

	m_relation.resize(m_numOfBall);
	m_relation[0].push_back(1);
	m_relation[1].push_back(0);

	m_ballForce.resize(m_numOfBall);
}

void MassSpringSystemSimulator::initComplex() {
	m_print = true;

	m_ballPos.clear();
	m_ballVec.clear();
	m_relation.clear();
	m_ballForce.clear();

	/* init for basic situation */
	m_fMass = 10;
	m_flength = 1;
	m_fStiffness = 40;
	m_ballSize = 0.01f;

	m_ballPos.push_back(Vec3(0, 0, 0));
	m_ballVec.push_back(Vec3(-1, 0, 0));
	m_ballPos.push_back(Vec3(0, 2, 0));
	m_ballVec.push_back(Vec3(1, 0, 0));

	m_numOfBall = m_ballPos.size();

	m_relation.resize(m_numOfBall);
	m_relation[0].push_back(1);
	m_relation[1].push_back(0);

	m_ballForce.resize(m_numOfBall);
}

const char* MassSpringSystemSimulator::getTestCasesStr() {
	return "Euler, Leapfrog, Midpoint, Complex";
};

void MassSpringSystemSimulator::initUI(DrawingUtilitiesClass* DUC){
	this->DUC = DUC;
};

void MassSpringSystemSimulator::reset(){
	m_mouse.x = m_mouse.y = 0;
	m_trackmouse.x = m_trackmouse.y = 0;
	m_oldtrackmouse.x = m_oldtrackmouse.y = 0;
};

void MassSpringSystemSimulator::drawFrame(ID3D11DeviceContext* pd3dImmediateContext){
	for (const auto& pos: m_ballPos)
	{
		DUC->setUpLighting(Vec3(), 0.4 * Vec3(1, 1, 1), 100, 0.6 * Vec3(1, 1, 1));
		DUC->drawSphere(pos, Vec3(m_ballSize, m_ballSize, m_ballSize));
	}
	DUC->beginLine();
	for (int i = 0; i < m_relation.size(); i++) {
		for (const auto& idx : m_relation[i]) {
			if (idx <= i) continue;
			DUC->drawLine(m_ballPos[i], Vec3(1, 1, 1), m_ballPos[idx], Vec3(1, 1, 1));
		}
	}
	DUC->endLine();
};

void MassSpringSystemSimulator::notifyCaseChanged(int testCase){
	m_iTestCase = testCase;
	switch (m_iTestCase)
	{
	case EULER:
		cout << "EULER !\n";
		initNaieve();
		break;
	case LEAPFROG:
		cout << "LEAPFROG !\n";
		initNaieve();
		break;
	case MIDPOINT:
		cout << "MIDPOINT !\n";
		initNaieve();
		break;
	case COMPLEX:
		cout << "COMPLEX !\n";
		initComplex();
		break;
	default:
		cout << "Empty Test!\n";
		initNaieve();
		break;
	}
};

void MassSpringSystemSimulator::externalForcesCalculations(float timeElapsed){
	return;
};

void MassSpringSystemSimulator::simulateTimestep(float timeStep){
	std::vector<Vec3> copy_pos;
	switch (m_iTestCase)
	{
	case EULER:
		calculateSpringForce(m_ballPos);
		for (int i = 0; i < m_numOfBall; i++) {
			m_ballPos[i] += m_ballVec[i] * timeStep;
			m_ballVec[i] += m_ballForce[i] / m_fMass * timeStep;
		}
		break;
	case MIDPOINT:
		for (const auto& pos : m_ballPos) copy_pos.push_back(pos);
		calculateSpringForce(m_ballPos);
		/* calculate midpoint position */
		for (int i = 0; i < m_numOfBall; i++) {
			m_ballPos[i] += (m_ballVec[i] + m_ballForce[i] / m_fMass * timeStep / 2) * timeStep;
			// m_ballVec[i] += m_ballForce[i] / m_fMass * timeStep;
		}
		/* calculate midpoint velocity */
		for (int i = 0; i < m_numOfBall; i++) copy_pos[i] += m_ballVec[i] * timeStep / 2;
		calculateSpringForce(copy_pos);
		for (int i = 0; i < m_numOfBall; i++) {
			m_ballVec[i] += m_ballForce[i] / m_fMass * timeStep;
		}
		break;
	case COMPLEX:
		for (const auto& pos : m_ballPos) copy_pos.push_back(pos);
		calculateSpringForce(m_ballPos);
		/* calculate midpoint position */
		for (int i = 0; i < m_numOfBall; i++) {
			m_ballPos[i] += (m_ballVec[i] + m_ballForce[i] / m_fMass * timeStep / 2) * timeStep;
			// m_ballVec[i] += m_ballForce[i] / m_fMass * timeStep;
		}
		/* calculate midpoint velocity */
		for (int i = 0; i < m_numOfBall; i++) copy_pos[i] += m_ballVec[i] * timeStep / 2;
		calculateSpringForce(copy_pos);
		for (int i = 0; i < m_numOfBall; i++) {
			m_ballVec[i] += m_ballForce[i] / m_fMass * timeStep;
		}
		break;
	default:
		break;
	}

	if (m_print)
	{
		printf("Here are the solution after one time step: \n");
		for (int i = 0; i < m_numOfBall; i++) {
			printf("  Info of particle [%d]:\n", i);
			printf("  position = (%f, %f, %f), velocity = (%f, %f, %f)\n", m_ballPos[i].x, m_ballPos[i].y, m_ballPos[i].z, m_ballVec[i].x, m_ballVec[i].y, m_ballVec[i].z);
		}
		m_print = false;
	}
};

void MassSpringSystemSimulator::calculateSpringForce(const std::vector<Vec3>& positions) {
	for (int i = 0; i < m_ballForce.size(); i++) {
		m_ballForce[i] = Vec3(0, 0, 0);
		for (const auto& neighborIdx : m_relation[i]) {
			Vec3 dist = positions[neighborIdx] - positions[i];
			float len = 0;
			for (int j = 0; j < 3; j++) len += dist[j] * dist[j];
			len = sqrt(len);
			//cout << len << endl;
			m_ballForce[i] += m_fStiffness * (len - m_flength) * dist / len;
		}
	}
}

void MassSpringSystemSimulator::onClick(int x, int y){
	m_trackmouse.x = x;
	m_trackmouse.y = y;
};
void MassSpringSystemSimulator::onMouse(int x, int y){
	m_oldtrackmouse.x = x;
	m_oldtrackmouse.y = y;
	m_trackmouse.x = x;
	m_trackmouse.y = y;
};

// Specific Functions
void MassSpringSystemSimulator::setMass(float mass){};
void MassSpringSystemSimulator::setStiffness(float stiffness){};
void MassSpringSystemSimulator::setDampingFactor(float damping){};
int MassSpringSystemSimulator::addMassPoint(Vec3 position, Vec3 Velocity, bool isFixed){
	return 0;
};
void MassSpringSystemSimulator::addSpring(int masspoint1, int masspoint2, float initialLength){};
int MassSpringSystemSimulator::getNumberOfMassPoints(){
	return 0;
};
int MassSpringSystemSimulator::getNumberOfSprings(){
	return 0;
};
Vec3 MassSpringSystemSimulator::getPositionOfMassPoint(int index){
	return Vec3();
};
Vec3 MassSpringSystemSimulator::getVelocityOfMassPoint(int index){
	return Vec3();
};
void MassSpringSystemSimulator::applyExternalForce(Vec3 force){};