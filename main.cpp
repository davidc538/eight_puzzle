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

		auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_s);

		std::cout << "Time: " << time.count() << std::endl;
	}
};

void log(const std::string& log_string, const puzzle_state& state)
{
	return;
	std::cout << state.to_string_short() << "             : " << log_string << std::endl;
}

void log2(const std::string& log_string, const puzzle_state& first, const puzzle_state& second)
{
	return;
	std::cout << first.to_string_short() << "|" << second.to_string_short() << " : " << log_string << std::endl;
}

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
		log("discovering", state);

		auto it = states_previous.find(state);
		
		// if we already have a cheaper or equal cost route to this state: return
		if (it != states_previous.end())
			return;

		log2("map", state, prev);
		states_previous[state] = prev;

		if (search_queue.find(state) != search_queue.end())
		{
			log("already enqueued", state);
			return;
		}

		log("enqueue", state);
		search_queue.insert(state);
	}

	puzzle_state get_previous_for(const puzzle_state& state) const
	{
		auto it = states_previous.find(state);

		if (it != states_previous.end())
			return it->second;
		else
			return puzzle_state();

		//return (states_previous.find(state)->second);
	}

	std::vector<puzzle_state> find_solution(const puzzle_state& initial_state)
	{
		discover_adjacent(initial_state, initial_state);

		while (!is_empty())
		{
			puzzle_state start = dequeue();

			log("dequeued", start);

			auto all_possible_moves = start.all_possible_moves();

			for (const puzzle_state& move : all_possible_moves)
			{
				discover_adjacent(move, start);

				if (move.is_solved())
				{
					std::vector<puzzle_state> retVal;

					puzzle_state i = move;

					while (i != initial_state)
					{
						retVal.push_back(i);

						i = get_previous_for(i);
					}

					std::reverse(retVal.begin(), retVal.end());

					return retVal;
				}
			}
		}

		return std::vector<puzzle_state>();
	}
}; 

// STATE                       | STEPS
// {4, 3, 5, 8, 0, 2, 7, 1, 6} | 100

int main(int argc, char** argv)
{
	puzzle_state initial = puzzle_state::randomize(-1);

	std::cout << "INITIAL STATE: " << std::endl;
	std::cout << initial.to_string() << std::endl;

	int steps_taken = -1;

	special_queue _solver;

	std::vector<puzzle_state> solution;

	{
		BlockTimer b;

		solution = _solver.find_solution(initial);
	}

	for (const auto& b : solution)
	{
		std::cout << b.to_string() << std::endl;
	}

	steps_taken = solution.size();

	std::cout << "total steps taken: " << steps_taken << std::endl;
	
	std::cin.get();
}