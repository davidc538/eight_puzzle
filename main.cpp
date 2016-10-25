#include <iostream>
#include <vector>
#include <deque>
#include <random>
#include <queue>
#include <functional>
#include <string>
#include <algorithm>
#include <set>
#include <memory>
#include <chrono>

struct randomizer
{
	std::mt19937 rand_eng;

	// pass -1 for random seed
	randomizer(int seed)
	{
		if (seed != -1)
			new (&rand_eng) std::mt19937(seed);
		else
		{
			std::random_device rand_dev;

			new (&rand_eng) std::mt19937(rand_dev());
		}
	}

	int get_uniform_between(int lower, int upper)
	{
		std::uniform_int_distribution<int> ud(lower, upper);

		return ud(rand_eng);
	}
};

struct puzzle_state
{
	int places[9];

	puzzle_state()
	{
		for (int i = 0; i < 9; i++)
			places[i] = i;
	}

	puzzle_state(const puzzle_state& copy_me)
	{
		for (int i = 0; i < 9; i++)
			places[i] = copy_me.places[i];
	}

	bool is_solved() const
	{
		return (manhattan_distance() == 0);
	}

	// for comparisons in std::set
	int hash() const
	{
		int i, retVal = 0;
		
		for (i = 0; i < 9; i++)
			retVal += i * places[i];

		return retVal;
	}

	puzzle_state copy() const
	{
		puzzle_state retVal;

		for (int i = 0; i < 9; i++)
			retVal.places[i] = places[i];

		return retVal;
	}

	static puzzle_state randomize(int seed)
	{
		puzzle_state retVal;

		std::vector<int> places;

		for (int i = 0; i < 9; i++)
			places.push_back(i);

		randomizer random(seed);

		for (int i = 0; i < 9; i++)
		{
			int index = random.get_uniform_between(0, places.size() - 1);
			int temp = places[index];

			retVal.places[i] = temp;

			auto it = std::find(places.begin(), places.end(), temp);

			places.erase(it, it + 1);
		}

		return retVal;
	}

	static void get_x_y_from_place(int place, int& x, int& y)
	{
		x = place % 3;
		y = place / 3;
	}

	static int get_place_from_x_y(int x, int y)
	{
		int retVal;

		retVal = y * 3;
		retVal += x;

		return retVal;
	}

	static int manhattan_distance_between(int first, int second)
	{
		int first_x, first_y, second_x, second_y, dist_x, dist_y, retVal;

		get_x_y_from_place(first, first_x, first_y);
		get_x_y_from_place(second, second_x, second_y);

		dist_x = first_x - second_x;
		dist_y = first_y - second_y;

		dist_x = abs(dist_x);
		dist_y = abs(dist_y);

		retVal = dist_x + dist_y;

		return retVal;
	}

	int empty_tile() const
	{
		for (int i = 0; i < 9; i++)
			if (places[i] == 0)
				return i;

		return -1;
	}
	
	void swap_tiles(int first, int second)
	{
		int temp = places[first];
		places[first] = places[second];
		places[second] = temp;
	}

	void slide_left()
	{
		int x, y, space = empty_tile();

		get_x_y_from_place(space, x, y);

		if (x == 0)
			throw 0;

		int other_space = get_place_from_x_y(x - 1, y);

		swap_tiles(space, other_space);
	}

	void slide_right()
	{
		int x, y, space = empty_tile();

		get_x_y_from_place(space, x, y);

		if (x == 2)
			throw 0;

		int other_space = get_place_from_x_y(x + 1, y);

		swap_tiles(space, other_space);
	}

	void slide_up()
	{
		int x, y, space = empty_tile();

		get_x_y_from_place(space, x, y);

		if (y == 0)
			throw 0;

		int other_space = get_place_from_x_y(x, y - 1);

		swap_tiles(space, other_space);
	}

	void slide_down()
	{
		int x, y, space = empty_tile();

		get_x_y_from_place(space, x, y);

		if (y == 2)
			throw 0;

		int other_space = get_place_from_x_y(x, y + 1);

		swap_tiles(space, other_space);
	}

	std::vector<puzzle_state> all_possible_moves() const
	{
		std::vector<puzzle_state> retVal;

		int space = empty_tile();

		int x, y;

		get_x_y_from_place(space, x, y);

		if (x != 0)
		{
			puzzle_state p = this->copy();

			p.slide_left();

			retVal.push_back(p);
		}

		if (x != 2)
		{
			puzzle_state p = this->copy();

			p.slide_right();

			retVal.push_back(p);
		}

		if (y != 2)
		{
			puzzle_state p = this->copy();

			p.slide_down();

			retVal.push_back(p);
		}

		if (y != 0)
		{
			puzzle_state p = this->copy();

			p.slide_up();

			retVal.push_back(p);
		}

		return retVal;
	}

	int manhattan_distance() const
	{
		int retVal = 0, dist, i;

		for (i = 0; i < 9; i++)
		{
			dist = manhattan_distance_between(i, places[i]);

			retVal += dist;
		}

		return retVal;
	}

	int correctly_placed_tiles() const
	{
		int i, retVal = 0;

		for (i = 0; i < 9; i++)
			if (places[i] == i)
				retVal++;

		return retVal;
	}

	bool operator==(const puzzle_state& r) const
	{
		for (int i = 0; i < 9; i++)
			if (places[i] != r.places[i])
				return false;

		return true;
	}

	std::string to_string() const
	{
		std::string retVal = "";

		for (int i = 0; i < 9; i++)
		{
			retVal += std::to_string(places[i]);

			if (i % 3 == 2)
				retVal += "\r\n";
		}

		return retVal;
	}
};

struct puzzle_state_search
{
	std::vector<std::shared_ptr<puzzle_state> > previous_states;
	puzzle_state current;
	int steps_taken;

	puzzle_state_search(const puzzle_state& came_from, const puzzle_state& current, int steps_taken)
	{
		// this invokes the constructor -> puzzle_state(const puzzle_state& copy_me)
		previous_states.push_back(std::make_shared<puzzle_state>(came_from));

		this->current = current.copy();
		this->steps_taken = steps_taken;
	}

	puzzle_state_search(const puzzle_state_search& came_from, const puzzle_state& current)
	{
		for (const auto& state : came_from.previous_states)
			previous_states.push_back(state);

		previous_states.push_back(std::make_shared<puzzle_state>(came_from.current));

		this->current = current.copy();
		this->steps_taken = came_from.steps_taken + 1;
	}

	std::vector<puzzle_state_search> all_possible_moves() const
	{
		std::vector<puzzle_state> p = current.all_possible_moves();

		std::vector<puzzle_state_search> r;

		for (const auto& i : p)
			r.emplace_back(*this, i);

		return r;
	}
};

struct hybrid_comparator
{
	bool operator() (const puzzle_state_search& l, const puzzle_state_search& r)
	{
		int l_cor = l.current.correctly_placed_tiles(),
			r_cor = r.current.correctly_placed_tiles(),
			l_man = l.current.manhattan_distance(),
			r_man = r.current.manhattan_distance(),
			l_steps = l.steps_taken,
			r_steps = r.steps_taken,
			l_h = (0 - l_cor + l_man + l_steps),
			r_h = (0 - r_cor + r_man + r_steps);

		return (l_h > r_h);
	}
};

struct distance_comparator
{
	bool operator() (const puzzle_state_search& l, const puzzle_state_search& r)
	{
		return ((l.current.manhattan_distance() + l.steps_taken)
			> (r.current.manhattan_distance() + r.steps_taken));
	}
};

struct distance_hash_comparator
{
	bool operator() (const puzzle_state_search& l, const puzzle_state_search& r)
	{
		int l_heur = l.current.manhattan_distance() + l.steps_taken;
		int r_heur = r.current.manhattan_distance() + r.steps_taken;

		if (l_heur != r_heur)
			return (l_heur > r_heur);
		else
			return (l.current.hash() > r.current.hash());
	}
};

struct hash_comparator
{
	bool operator() (const puzzle_state& l, const puzzle_state& r) const
	{
		return (l.hash() > r.hash());
	}
};

struct puzzle_state_set
{
	std::set<puzzle_state, hash_comparator> _states;

	void insert(const puzzle_state& state)
	{
		_states.insert(state);
	}

	bool contains(const puzzle_state& state)
	{
		return (std::find(_states.begin(), _states.end(), state) != _states.end());
	}
};

std::vector<puzzle_state> find_solution(const puzzle_state& initial_state, int& steps_taken)
{
	std::priority_queue<puzzle_state_search, std::deque<puzzle_state_search>, distance_hash_comparator> queue;
	puzzle_state_set expanded_states;

	queue.emplace(initial_state, initial_state, 0);

	while (!queue.empty())
	{
		auto current = queue.top();
		queue.pop();

		auto all = current.all_possible_moves();

		for (const auto& i : all)
		{
			if (i.current.is_solved())
			{
				steps_taken = i.steps_taken;

				std::vector<puzzle_state> retVal;

				for (const auto& b : i.previous_states)
					retVal.push_back(b->copy());

				std::reverse(retVal.begin(), retVal.end());

				retVal.push_back(i.current);

				return retVal;
			}

			if (!expanded_states.contains(i.current))
			{
				queue.push(i);
				expanded_states.insert(i.current);
			}
		}
	}

	return std::vector<puzzle_state>();
}

void test()
{
	for (int i = 0; i < 100; i++)
	{
		puzzle_state p = puzzle_state::randomize(i);

		std::cout << p.to_string() <<
			"correctly_placed_tiles: " << p.correctly_placed_tiles() << std::endl <<
			"manhattan_distance_heu: " << p.manhattan_distance() << std::endl;
	}
}

class BlockTimer
{
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

public:
	BlockTimer()
	{
		start = std::chrono::high_resolution_clock::now();
	}

	~BlockTimer()
	{
		end = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> elapsed_s = end - start;

		auto time = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_s);

		std::cout << "Time: " << time.count() << std::endl;
	}
};

// seed | steps
// -------------
// 18   | 20

int main(int argc, char** argv)
{
	{
		BlockTimer b;

		puzzle_state initial = puzzle_state::randomize(18);

		/*

		initial.places[0] = 0; initial.places[1] = 1; initial.places[2] = 2;
		initial.places[3] = 3; initial.places[4] = 4; initial.places[5] = 5;
		initial.places[6] = 6; initial.places[7] = 7; initial.places[8] = 8;

		// */

		std::cout << "INITIAL STATE: " << std::endl;
		std::cout << initial.to_string() << std::endl;

		int steps_taken = -1;

		auto a = find_solution(initial, steps_taken);

		for (const auto& b : a)
		{
			std::cout << b.to_string() << std::endl;
		}

		std::cout << "total steps taken: " << steps_taken << std::endl;
	}
	std::cin.get();
}
