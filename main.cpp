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

struct randomizer
{
	std::default_random_engine rand_eng;

	// pass -1 for random seed
	randomizer(int seed)
	{
		if (seed != -1)
			new (&rand_eng) std::default_random_engine(seed);
		else
		{
			std::random_device rand_dev;

			new (&rand_eng) std::default_random_engine(rand_dev());
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
		return (manhattan_distance_heuristic() == 0);
	}

	// for comparisons in std::set
	long long hash() const
	{
		long long retVal = 0;
		
		for (int i = 0; i < 9; i++)
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

	static puzzle_state randomize()
	{
		puzzle_state retVal;

		std::vector<int> chars;

		for (int i = 0; i < 9; i++)
			chars.push_back(i);

		randomizer random(-1);

		for (int i = 0; i < 9; i++)
		{
			int index = random.get_uniform_between(0, chars.size() - 1);
			int temp = chars[index];

			retVal.places[i] = temp;

			auto it = std::find(chars.begin(), chars.end(), temp);

			chars.erase(it, it + 1);
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

	static int manhattan_distance(int first, int second)
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

	int manhattan_distance_heuristic() const
	{
		int retVal = 0;

		for (int i = 0; i < 9; i++)
		{
			int dist = manhattan_distance(i, places[i]);

			retVal += dist;
		}

		return retVal;
	}

	bool operator<(const puzzle_state& r) const
	{
		int left = manhattan_distance_heuristic();
		int right = r.manhattan_distance_heuristic();
		
		return (left < right);
	}

	bool operator>(const puzzle_state& r) const
	{
		int left = manhattan_distance_heuristic();
		int right = r.manhattan_distance_heuristic();

		return (left > right);
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

	int current_heuristic() const
	{
		return manhattan_distance_heuristic();
	}
};

void test_man_dist()
{
	int i, x, y;

	for (i = 0; i < 9; i++)
	{
		puzzle_state::get_x_y_from_place(i, x, y);
		std::cout << i << " : " << x << " , " << y << std::endl;
	}
}

void test_lots()
{
	puzzle_state p;

	std::cout << p.to_string() << " , " << p.manhattan_distance_heuristic() << std::endl;

	std::priority_queue<puzzle_state, std::vector<puzzle_state>, std::greater<puzzle_state> > q;
	//std::set<puzzle_state, std::less<puzzle_state> > q;

	for (int i = 0; i < 1000000; i++)
	{
		puzzle_state p = puzzle_state::randomize();

		//q.insert(p);
		q.push(p);
	}

	while (q.size() > 0)
	{
		auto temp = q.top();
		q.pop();
		std::cout << temp.to_string() << " : " << temp.manhattan_distance_heuristic() << std::endl;
	}

	/*
	for (const auto& temp : q)
	{
	std::cout << temp.to_string() << " : " << temp.manhattan_distance_heuristic() << std::endl;
	}
	*/
}

void test_all_possible_moves()
{
	puzzle_state p;

	std::cout << p.to_string() << std::endl;

	auto all = p.all_possible_moves();

	for (const auto& t : all)
	{
		std::cout << t.to_string() << std::endl;
	}
}

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

	bool is_solved() const
	{
		return current.is_solved();
	}


	// TODO: come back here when adding new puzzle_state_search constructor
	std::vector<puzzle_state_search> all_possible_moves() const
	{
		std::vector<puzzle_state> p = current.all_possible_moves();

		std::vector<puzzle_state_search> r;

		for (const auto& i : p)
			r.emplace_back(current, i, steps_taken + 1);

		return r;
	}

	int heuristic() const
	{
		return current.current_heuristic() + steps_taken;
	}
	// /*
	bool operator<(const puzzle_state_search& r) const
	{
		int h1 = heuristic(),
			h2 = r.heuristic();

		return (h1 < h2);
	}

	bool operator>(const puzzle_state_search& r) const
	{
		int h1 = heuristic(),
			h2 = r.heuristic();

		return (h1 > h2);
	}
	//*/
};

struct puzzle_state_search_comparator_manhattan_distance
{
	bool operator() (const puzzle_state_search& l, const puzzle_state_search& r)
	{
		return (l.current.manhattan_distance_heuristic() < r.current.manhattan_distance_heuristic());
	}
};

struct puzzle_state_comparator_hash
{
	bool operator() (const puzzle_state& l, const puzzle_state& r) const
	{
		return (l.hash() < r.hash());
	}
};

std::vector<puzzle_state> find_solution(const puzzle_state& initial_state, int& steps_taken)
{
	//std::priority_queue<puzzle_state_search, std::vector<puzzle_state_search>, puzzle_state_search_comparator_manhattan_distance > q;
	std::priority_queue<puzzle_state_search, std::vector<puzzle_state_search>, std::greater<puzzle_state_search> > queue;
	std::set<puzzle_state, puzzle_state_comparator_hash> expanded_states;

	queue.emplace(initial_state, initial_state, 0);

	while (!queue.empty())
	{
		auto t = queue.top();
		queue.pop();

		auto v = t.all_possible_moves();

		for (const auto& a : v)
		{
			if (a.is_solved())
			{
				steps_taken = a.steps_taken;

				std::vector<puzzle_state> retVal;

				for (const auto& b : a.previous_states)
					retVal.push_back(b->copy());

				std::reverse(retVal.begin(), retVal.end());

				retVal.push_back(a.current);

				return retVal;
			}

			if (std::find(expanded_states.begin(), expanded_states.end(), a.current) == expanded_states.end())
			{
				queue.push(a);
				expanded_states.insert(a.current);
			}
		}
	}

	return std::vector<puzzle_state>();
}

int main(int argc, char** argv)
{
	/*
	test_man_dist();
	test_lots();
	test_all_possible_moves();
	*/
	puzzle_state initial = puzzle_state::randomize();

	std::cout << "INITIAL STATE: " << std::endl;
	std::cout << initial.to_string() << std::endl;

	int steps_taken = -1;

	auto a = find_solution(initial, steps_taken);

	for (const auto& b : a)
	{
		std::cout << b.to_string() << std::endl;
	}

	std::cout << "total steps taken: " << steps_taken << std::endl;

	std::cin.get();
}
