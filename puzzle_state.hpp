#pragma once

#include <vector>
#include <random>
#include <string>
#include <memory>
#include <math.h>

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

	puzzle_state& set(int place, int value)
	{
		places[place] = value;
		return *this;
	}

	bool is_solved() const
	{
		return (correctly_placed_tiles() == 9);
	}

	// for comparisons in std::set
	int hash() const
	{
		int i, retVal = 0, p;

		for (i = 0; i < 9; i++)
		{
			p = pow(10, i);
			retVal += p * places[i];
		}

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

	bool operator!=(const puzzle_state& r) const
	{
		return (!(*this == r));
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

	std::string to_string_short() const
	{
		int _hash = hash();

		return std::to_string(_hash);
	}
};

