#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include <functional>
#include <string>
#include <algorithm>

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
	
	std::vector<puzzle_state> all_possible_moves() const
	{
		std::vector<puzzle_state> retVal;

		int space = empty_tile();



		// TODO: actually implement this
		retVal.push_back(puzzle_state());

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

void test_man_dist()
{
	int i, x, y;

	for (i = 0; i < 9; i++)
	{
		puzzle_state::get_x_y_from_place(i, x, y);
		std::cout << i << " : " << x << " , " << y << std::endl;
	}
}

int main(int argc, char** argv)
{
	test_man_dist();

	puzzle_state p;

	std::cout << p.to_string() << " , " << p.manhattan_distance_heuristic() << std::endl;

	int p_t = p.manhattan_distance_heuristic();

	std::priority_queue<puzzle_state, std::vector<puzzle_state>, std::less<puzzle_state> > q;

	for (int i = 0; i < 1000000; i++)
	{
		puzzle_state p = puzzle_state::randomize();
		
		q.push(p);
	}

	while (q.size() > 0)
	{
		auto temp = q.top();
		q.pop();
		std::cout << temp.to_string() << " : " << temp.manhattan_distance_heuristic() << std::endl;
	}

	std::cin.get();
}
