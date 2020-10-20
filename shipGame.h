#include <vector>
#include <iostream>
#ifndef MY_SHIP_GAME
#define MY_SHIP_GAME

namespace myShipGame {
	using myInt = size_t;
	struct cord {
		myInt x{};
		myInt y{};
	};
	struct rectangle {
		cord lu{};
		cord rd{};
	};

	enum state { hidden = 0, filled, attacked };

	class shipGame
	{
	public:
		shipGame(myInt = 100);
		shipGame(const shipGame&) = delete;
		shipGame& operator=(const shipGame&) = delete;
		void startGame();
		void show() const;
		static void rulesOfTheGame();
	private:
		void ValidateFieldSize() const;
		void setBullets();
		void initMilitaryObjects();
		void initField();
		void updateField(const cord&, myInt, state);
		void defenderStart();
		myInt intersectsWithAnObject(myInt) const;
		bool intersects(myInt, myInt) const;
		void attackerStart();
		void updateFieldForDestroyedObject(myInt);
		bool ifDestroys(const cord&, const rectangle&) const;
		void processBombing(const cord&);
		bool destroyed() const;
		void congratulateAttacker();
		void congratulateDefender();
		void clear();
		std::vector<std::vector<char>> m_field;
		std::vector<rectangle> m_militaryObjects;
		std::vector<bool> m_militaryObjectsRemained;
		myInt m_size{};
		myInt m_bullets{};
		myInt m_maxObjectSize{};
		myInt m_objectsRemained{};
		char m_states[3] = { '.', '#', 'X' };
	};

	// exceptions
	struct shipGameException {
		virtual void what() const = 0;
	};

	struct shipFieldInvalidSize : public shipGameException {
		void what() const {
			std::cout << "The size of the field is invalid\n";
		}
	};

	struct shipGameOutOfBounds : public shipGameException {
		virtual void what() const {
			std::cout << "An attack or a military object goes out of bounds\n";
		}
	};

	struct militaryObjectOutOfBounds : public shipGameOutOfBounds {
		void what() const {
			std::cout << "military object goes out of bounds\n";
		}
	};

	struct militaryObjectIntersectsException : public shipGameException {
		void what() const {
			std::cout << "Military object shouldn`t intersect with another military object.\n";
		}
	};

}


#endif // !MY_SHIP_GAME

