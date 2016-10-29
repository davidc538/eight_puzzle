#include <iostream>
#include <vector>
#include <deque>
#include <queue>
#include <functional>
#include <string>
#include <algorithm>
#include <set>
#include <memory>
#include <chrono>

#include "functors.hpp"
#include "puzzle_state.hpp"

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

struct solver
{
	std::priority_queue<puzzle_state_search, std::deque<puzzle_state_search>, distance_hash_comparator> search_queue;

	void enqueue(const puzzle_state_search& item)
	{
		search_queue.push(item);
	}

	puzzle_state_search dequeue()
	{
		puzzle_state_search retVal = search_queue.top();

		search_queue.pop();

		return retVal;
	}

	bool search_queue_is_empty()
	{
		return search_queue.empty();
	}

	std::set<puzzle_state, hash_comparator> searched_states;

	void searched_state(const puzzle_state& state)
	{
		searched_states.insert(state);
	}

	bool searched_states_contains(const puzzle_state& state)
	{
		return (std::find(searched_states.begin(), searched_states.end(), state) != searched_states.end());
	}

	std::vector<puzzle_state> find_solution(const puzzle_state& initial_state, int& steps_taken)
	{
		size_t expanded_states_size, search_queue_size;

		enqueue(puzzle_state_search(initial_state, initial_state, 0));

		while (!search_queue_is_empty())
		{
			auto current = dequeue();

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

				if (!searched_states_contains(i.current))
				{
					enqueue(i);
					searched_state(i.current);

					expanded_states_size = searched_states.size();
					search_queue_size = search_queue.size();
				}
			}
		}

		std::cout << "ex: " << expanded_states_size << " , q: " << search_queue_size << std::endl;

		// if we get here, return an empty vector
		return std::vector<puzzle_state>();
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

		solver _solver;

		auto a = _solver.find_solution(initial, steps_taken);

		for (const auto& b : a)
		{
			std::cout << b.to_string() << std::endl;
		}

		std::cout << "total steps taken: " << steps_taken << std::endl;
	}
	std::cin.get();
}
