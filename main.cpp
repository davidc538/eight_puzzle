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
#include <map>
#include <tuple>

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

struct special_queue
{
	std::map<puzzle_state, puzzle_state, puzzle_state_hash_comparator> states_previous;
	std::set<puzzle_state, puzzle_state_comparator> search_queue;

	bool is_empty() const
	{
		return search_queue.empty();
	}

	puzzle_state dequeue()
	{
		puzzle_state retVal = *search_queue.begin();

		search_queue.erase(retVal);

		return retVal;
	}

	void discover_adjacent(const puzzle_state& state, const puzzle_state& prev)
	{
		auto it = states_previous.find(state);
		
		// if we already have a cheaper or equal cost route to this state: return
		if (it != states_previous.end() && it->first.steps_taken <= state.steps_taken)
			return;

		// if we already have a more expensive route to this state:
		// erase it from the search queue and overwrite it
		if (it != states_previous.end() && it->first.steps_taken > state.steps_taken)
		{
			puzzle_state temp = it->first;
			search_queue.erase(temp);
		}

		states_previous[state] = prev;
		search_queue.insert(state);
	}

	puzzle_state get_previous_for(const puzzle_state& state) const
	{
		return (states_previous.find(state)->second);
	}

	std::vector<puzzle_state> find_solution(const puzzle_state& initial_state)
	{
		discover_adjacent(initial_state, initial_state);

		while (!is_empty())
		{
			puzzle_state start = dequeue();

			auto all_possible_moves = start.all_possible_moves();

			for (const puzzle_state& move : all_possible_moves)
			{
				if (move.is_solved())
				{
					std::vector<puzzle_state> retVal;

					puzzle_state i = move;

					while (i != initial_state)
					{
						retVal.push_back(i);

						i = get_previous_for(i);
					}

					return retVal;
				}

				discover_adjacent(move, start);
			}
		}

		return std::vector<puzzle_state>();
	}
};

struct solver
{
	std::priority_queue<puzzle_state_search, std::deque<puzzle_state_search>, distance_hash_comparator> search_queue;
	std::set<puzzle_state, puzzle_state_hash_comparator> found_states;

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

	void searched_state(const puzzle_state& state)
	{
		found_states.insert(state);
	}

	bool searched_states_contains(const puzzle_state& state)
	{
		return (std::find(found_states.begin(), found_states.end(), state) != found_states.end());
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

					expanded_states_size = found_states.size();
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

		special_queue _solver;

		auto a = _solver.find_solution(initial);

		for (const auto& b : a)
		{
			std::cout << b.to_string() << std::endl;
		}

		std::cout << "total steps taken: " << steps_taken << std::endl;
	}
	std::cin.get();
}