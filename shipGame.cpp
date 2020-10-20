#include "shipGame.h"
#include <iomanip>
namespace myShipGame {
	std::istream& operator>>(std::istream& in, cord& point) {
		in >> point.x >> point.y;
		return in;
	}
	std::ostream& operator<<(std::ostream& out, const cord& point) {
		out << '(' << point.x << ',' << ' ' << point.y << ')';
		return out;
	}
	std::ostream& operator<<(std::ostream& out, const rectangle& rect) {
		out << "( " << rect.lu << ' ' << rect.rd << " )";
		return out;
	}

	myShipGame::shipGame::shipGame(myInt size)
		: m_size{ size }
	{
		ValidateFieldSize();
		setBullets();
		initMilitaryObjects();
		initField();
	}

	void myShipGame::shipGame::startGame()
	{
		defenderStart();
		attackerStart();
		clear();
	}

	void myShipGame::shipGame::show() const
	{
		// top indexes
		std::cout << "  ";
		for (myInt i = 0; i < m_size; ++i)
			std::cout << std::setw(4) << i;
		std::cout << std::endl;

		// others
		for (myInt i = 0; i < m_size; ++i) {
			std::cout << std::setw(3) << i << "  ";
			for (char x : m_field[i])
				std::cout << x << "   ";
			std::cout << std::endl;
		}
	}

	void shipGame::rulesOfTheGame()
	{
		std::cout << "###########################################################################\n";
		std::cout << "#  To Create a game the size of the field should be given.\n";
		std::cout << "#  Size restrictions:\n";
		std::cout << "#      1. Size >= 100\n";
		std::cout << "#      2. Size should be be multiplier of 100\n";
		std::cout << "#  ---\n";
		std::cout << "#  Then military objects should be set. Total area of\n";
		std::cout << "#  the objects is approximately 15 percent of the field.\n";
		std::cout << "#  Coordinates restriction.\n";
		std::cout << "#       . the objects must be set so that\n";
		std::cout << "#         no part of it go out of the field\n";
		std::cout << "#  ---\n";
		std::cout << "#  Then bombing will be started, the attacker will have\n";
		std::cout << "#  (15 percent of total area) bullets. if the bombing is in a\n";
		std::cout << "#  cordinate of part of an object then that object will be destroyed\n";
		std::cout << "#  ---\n";
		std::cout << "#  Defender will win:\n";
		std::cout << "#     if after bombing at least one military object will not be destroyed\n";
		std::cout << "#  Attacker will win:\n";
		std::cout << "#     if all military objects will be destroyed\n";
		std::cout << "##########################################################################\n\n";
	}

	void shipGame::clear()
	{
		initField();
		m_militaryObjectsRemained.clear();
		m_militaryObjectsRemained.resize(m_maxObjectSize + 1, true);
		m_objectsRemained = m_maxObjectSize;
		setBullets();
	}

	void shipGame::congratulateAttacker()
	{
		for (myInt i = 0; i < m_size; ++i) {
			if(m_field[i][i] != m_states[filled])
				m_field[i][i] = m_states[attacked];
			if(m_field[i][m_size - i - 1] != m_states[filled])
				m_field[i][m_size - i - 1] = m_states[attacked];
		}
		std::cout << "Attacker Won: the military objects have been destroyed.\n";
		show();
	}

	void shipGame::congratulateDefender()
	{
		initField();
		for (myInt i = 1; i <= m_maxObjectSize; ++i) {
			updateField(m_militaryObjects[i].lu, i, filled);
		}
		std::cout << "Defender Won. Attacker failed to destroy all military objects.\n";
		show();
	}

	void shipGame::updateFieldForDestroyedObject(myInt index)
	{
		rectangle& rect = m_militaryObjects[index];
		cord& p1 = rect.lu, & p2 = rect.rd;
		for (myInt i = p1.y; i <= p2.y; ++i)
			for (myInt j = p1.x; j <= p2.x; ++j)
				m_field[i][j] = m_states[filled];
		m_militaryObjectsRemained[index] = false;
		--m_objectsRemained;
	}

	bool shipGame::ifDestroys(const cord& point, const rectangle& rect) const
	{
		if (rect.lu.x <= point.x && rect.rd.x >= point.x && 
			rect.lu.y <= point.y && rect.rd.y >= point.y)
			return true;
		return false;
	}

	void shipGame::processBombing(const cord& point)
	{
		bool destroys = false;
		myInt i = 1;
		for (; i <= m_maxObjectSize; ++i) {
			if (m_militaryObjectsRemained[i]) {
				if ( ( destroys = ifDestroys(point, m_militaryObjects[i]) ) ) {
					break;
				}
			}
		}
		if (destroys) {
			updateFieldForDestroyedObject(i);
		}
		else {
			if(m_field[point.y][point.x] != m_states[filled])
				m_field[point.y][point.x] = m_states[attacked];
		}
	}

	bool shipGame::destroyed() const
	{
		return m_objectsRemained == 0;
	}

	void shipGame::initField()
	{
		if(!m_field.size()) {
			m_field.resize(m_size);
			for (myInt i = 0; i < m_size; ++i)
				m_field[i].resize(m_size);
		}
	    updateField({ 0,0 }, m_size, hidden);
	}

	void shipGame::updateField(const cord& lu, myInt length, state st)
	{
		myInt x = lu.x, y = lu.y;
		myInt x_r = x + length;
		myInt y_d = y + length;
		for (; y < y_d; ++y) {
			for (x = lu.x; x < x_r; ++x) {
				m_field[y][x] = m_states[st];
			}
		}
	}

	void shipGame::defenderStart()
	{
		show();
		std::cout << "The objects areas that should be set\n";
		for (myInt i = 1; i <= m_maxObjectSize; ++i)
			std::cout << i << 'x' << i << std::endl;

		std::cout << "Now, give (x, y) cordinates of the objects...\n";
		myInt intersectedRectIndex;
		for (myInt i = 1; i <= m_maxObjectSize; ++i) {
			std::cout << i << 'x' << i << ": ";
			std::cin >> m_militaryObjects[i].lu;

			m_militaryObjects[i].rd.x = m_militaryObjects[i].lu.x + i - 1;
			m_militaryObjects[i].rd.y = m_militaryObjects[i].lu.y + i - 1;
			if (m_militaryObjects[i].rd.x >= m_size || m_militaryObjects[i].rd.y >= m_size
				   || m_militaryObjects[i].lu.x < 0 || m_militaryObjects[i].lu.y < 0) 
			{
				std::cout << "An object area goes out of the field: " << m_militaryObjects[i] 
					      << ", " << i << 'x' << i << std::endl;
				throw militaryObjectOutOfBounds{};
			}

			if ( (intersectedRectIndex = intersectsWithAnObject(i)) != 0) {
				std::cout << "Military object " << m_militaryObjects[i] << " intersects with another military object "
					      << m_militaryObjects[intersectedRectIndex] << std::endl;
				throw militaryObjectIntersectsException{};
			}
			updateField(m_militaryObjects[i].lu, i, filled);
			show();
			std::cout << std::endl;
		}
		for (myInt i = 1; i <= m_maxObjectSize; ++i)
			updateField(m_militaryObjects[i].lu, i, hidden);
		show();
	}

	myInt shipGame::intersectsWithAnObject(myInt ind) const
	{
		for (myInt i = 1; i != ind; ++i)
			if (intersects(ind, i))
				return i;
		return 0;
	}

	bool shipGame::intersects(myInt i1, myInt i2) const
	{
		const rectangle& r1 = m_militaryObjects[i1], &r2 = m_militaryObjects[i2];
		if ((r1.lu.x <= r2.lu.x && r1.rd.x >= r2.lu.x || r1.lu.x <= r2.rd.x && r1.rd.x >= r2.rd.x) && 
			(r1.lu.y <= r2.lu.y && r1.rd.y >= r2.lu.y || r1.lu.y <= r2.rd.y && r1.rd.y >= r2.rd.y)) 
		{
			return true;
		}
		return false;
	}

	void shipGame::attackerStart()
	{
		cord bombing;
		while (!destroyed() && m_bullets != 0) {
			std::cout << m_bullets << " available.\nbombing coordinates:\n";
		    std::cin >> bombing;
			if (bombing.x >= 0 && bombing.x < m_size &&
				bombing.y >= 0 && bombing.y < m_size)
			{
				processBombing(bombing);
			}
			show();
			--m_bullets;
			std::cout << std::endl;
		}
		if (destroyed()) {
			congratulateAttacker();
		}
		else {
			congratulateDefender();
		}
	}

	void shipGame::setBullets()
	{
		m_bullets = m_size*m_size / 20 * 3;
	}

	void myShipGame::shipGame::initMilitaryObjects()
	{
		// the sum of the areas of the objects should
		// approximately be equel to bullets count
		myInt limit = m_bullets;
		myInt sum = 0, i = 0;
		while (sum <= limit) {
			++i;
			// areas += ith area;
			sum += i * i;
		}
		m_maxObjectSize = --i;
		// the objects count is clear. So,
		// reserving place for military objects
		m_militaryObjects.resize(m_maxObjectSize+1); // + 1 to start indexing from 1 (1...k)
		m_militaryObjectsRemained.resize(m_maxObjectSize + 1, true);
		m_objectsRemained = m_maxObjectSize;
	}

	void shipGame::ValidateFieldSize() const
	{
		if (m_size < 10 || (m_size % 10) != 0) {
			std::cout << "size of the field is set to " << m_size
				      << ". Learn rules of the game\n";
			throw shipFieldInvalidSize{};
		}
	}
}
