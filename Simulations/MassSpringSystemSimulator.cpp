#include "MassSpringSystemSimulator.h"

MassSpringSystemSimulator::MassSpringSystemSimulator() {
	m_iTestCase = 0;
	m_iMethodCase = EULER;
	g = 0.3;
	initNaieve();
};

void MassSpringSystemSimulator::initNaieve() {
	m_print = true;
	//usingEuler = true;

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
	for (auto it = m_relation.begin(); it != m_relation.end(); ++it) it->clear();
	m_relation[0].push_back(1);
	m_relation[1].push_back(0);

	m_ballForce.resize(m_numOfBall);
}

void MassSpringSystemSimulator::initComplex() {
	m_print = false;
	//usingEuler = false;

	m_ballPos.clear();
	m_ballVec.clear();
	m_relation.clear();
	m_ballForce.clear();

	/* init for basic situation */
	m_fMass = 1;
	m_flength = 0.2;
	m_fStiffness = 100;
	m_ballSize = 0.01f;
	m_fDamping = 2.0;
	//gravity = Vec3(0, -0.3, 0);

	m_size_n = 10;
	m_size_m = 10;
	m_startPos = Vec3(0, 0.5, -1);

	for (int i = 0; i < m_size_n; i++) {
		for (int j = 0; j < m_size_m; j++) {
			Vec3 targetPos = m_startPos + Vec3(i, 0, j) * m_flength;
			m_ballPos.push_back(targetPos);
			m_ballVec.push_back(Vec3(0, 0, 0));
		}
	}

	m_numOfBall = m_ballPos.size();

	m_relation.resize(m_numOfBall);
	for (auto it = m_relation.begin(); it != m_relation.end(); ++it) it->clear();
	for (int i = 0; i < m_size_n; i++) {
		for (int j = 0; j < m_size_m; j++) {
			for (int k = -1; k <= 1; k++) {
				for (int l = -1; l <= 1; l++) {
					if ((k == 0) && (l == 0)) continue;
					int idx_n = i + k;
					int idx_m = j + l;
					if (is_inside(idx_n, idx_m)) {
						m_relation[idx2ord(i, j)].push_back(idx2ord(idx_n, idx_m));
					}
				}
			}
		}
	}

	m_ballForce.resize(m_numOfBall);
	m_isFixed.clear();
	m_isFixed.resize(m_numOfBall, false);
	m_isFixed[idx2ord(0, 0)] = true;
	m_isFixed[idx2ord(0, m_size_m - 1)] = true;
}

const char* MassSpringSystemSimulator::getTestCasesStr() {
	return "Simple, Complex";
};

const char* MassSpringSystemSimulator::getMethodCasesStr() {
	return "Euler, LeapFrog, MidPoint";
}

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
	case 0:
		cout << "Simple Scene !\n";
		initNaieve();
		break;
	case 1:
		cout << "COMPLEX Scene !\n";
		initComplex();
		break;
	default:
		cout << "Empty Test!\n";
		initNaieve();
		break;
	}
};

void MassSpringSystemSimulator::notifyMethodChanged(int methodCase) {
	m_iMethodCase = methodCase;
	switch (m_iMethodCase)
	{
	case EULER:
		cout << "Euler Method !\n";
		break;
	case LEAPFROG:
		cout << "LeapFrog Method !\n";
		break;
	case MIDPOINT:
		cout << "MidPoint Method !\n";
		break;
	default:
		break;
	}
}

void MassSpringSystemSimulator::externalForcesCalculations(float timeElapsed){
	return;
};

void MassSpringSystemSimulator::simulateTimestep(float timeStep){
	std::vector<Vec3> copy_pos;
	std::vector<Vec3> copy_vec;
	switch (m_iTestCase)
	{
	case 0:
		if (m_iMethodCase == EULER)
		{
			calculateSpringForce(m_ballPos);
			for (int i = 0; i < m_numOfBall; i++) {
				m_ballPos[i] += m_ballVec[i] * timeStep;
				m_ballVec[i] += m_ballForce[i] / m_fMass * timeStep;
			}
		}
		else if (m_iMethodCase == MIDPOINT) {
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
		}
		break;
	case 1:
		if (m_iMethodCase == EULER) {
			calculateSpringForceWithDamping(m_ballPos, m_ballVec);
			addGravity();
			for (int i = 0; i < m_numOfBall; i++) {
				if (m_isFixed[i]) continue;
				m_ballPos[i] += m_ballVec[i] * timeStep;
				m_ballVec[i] += m_ballForce[i] / m_fMass * timeStep;
				if (m_ballPos[i].y <= -1) m_ballPos[i].y = -1;
			}
		}
		else if (m_iMethodCase == MIDPOINT) {
			for (const auto& pos : m_ballPos) copy_pos.push_back(pos);
			for (const auto& vec : m_ballVec) copy_vec.push_back(vec);
			calculateSpringForceWithDamping(m_ballPos, m_ballVec);
			addGravity();
			/* calculate midpoint position */
			for (int i = 0; i < m_numOfBall; i++) {
				if (m_isFixed[i]) continue;
				m_ballPos[i] += (m_ballVec[i] + m_ballForce[i] / m_fMass * timeStep / 2) * timeStep;
				if (m_ballPos[i].y <= -1) m_ballPos[i].y = -1;
				// m_ballVec[i] += m_ballForce[i] / m_fMass * timeStep;
			}
			/* calculate midpoint velocity */
			for (int i = 0; i < m_numOfBall; i++)
			{
				copy_pos[i] += m_ballVec[i] * timeStep / 2;
				copy_vec[i] += m_ballForce[i] / m_fMass * timeStep / 2;
			}
			calculateSpringForceWithDamping(copy_pos, copy_vec);
			addGravity();
			for (int i = 0; i < m_numOfBall; i++) {
				if (m_isFixed[i]) continue;
				m_ballVec[i] += m_ballForce[i] / m_fMass * timeStep;
			}
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
		int cur_idx_n, cur_idx_m;
		ord2idx(i, cur_idx_n, cur_idx_m);
		for (const auto& neighborIdx : m_relation[i]) {
			Vec3 dist = positions[neighborIdx] - positions[i];
			float len = 0;
			for (int j = 0; j < 3; j++) len += dist[j] * dist[j];
			len = sqrt(len);
			//cout << len << endl;
			int neighbor_idx_n, neighbor_idx_m;
			ord2idx(neighborIdx, neighbor_idx_n, neighbor_idx_m);
			float len_coe = sqrt((cur_idx_n - neighbor_idx_n) * (cur_idx_n - neighbor_idx_n) + (cur_idx_m - neighbor_idx_m) * (cur_idx_m - neighbor_idx_m));
			m_ballForce[i] += m_fStiffness * (len - m_flength * len_coe) * dist / len;
		}
	}
}

void MassSpringSystemSimulator::calculateSpringForceWithDamping(const std::vector<Vec3>& positions, const std::vector<Vec3>& velocities) {
	for (int i = 0; i < m_ballForce.size(); i++) {
		m_ballForce[i] = Vec3(0, 0, 0);
		int cur_idx_n, cur_idx_m;
		ord2idx(i, cur_idx_n, cur_idx_m);
		for (const auto& neighborIdx : m_relation[i]) {
			Vec3 dist = positions[neighborIdx] - positions[i];
			float len = 0;
			for (int j = 0; j < 3; j++) len += dist[j] * dist[j];
			len = sqrt(len);
			//cout << len << endl;
			int neighbor_idx_n, neighbor_idx_m;
			ord2idx(neighborIdx, neighbor_idx_n, neighbor_idx_m);
			float len_coe = sqrt((cur_idx_n - neighbor_idx_n) * (cur_idx_n - neighbor_idx_n) + (cur_idx_m - neighbor_idx_m) * (cur_idx_m - neighbor_idx_m));
			m_ballForce[i] += m_fStiffness * (len - m_flength * len_coe) * dist / len;
			Vec3 v01 = velocities[neighborIdx] - velocities[i];
			Vec3 e01 = dist / len;
			m_ballForce[i] += m_fDamping * dot(v01, e01) * e01;
		}
	}
}

void MassSpringSystemSimulator::addGravity() {
	gravity = Vec3(0, -g, 0);
	for (int i = 0; i < m_numOfBall; i++) m_ballForce[i] += gravity * m_fMass;
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