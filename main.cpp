#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <chrono>
#include <map>

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

void log1(const std::string& log_string, const puzzle_state& state)
{
	std::cout << state.to_string_short() << "           : " << log_string << std::endl;
}

void log2(const std::string& log_string, const puzzle_state& first, const puzzle_state& second)
{
	std::cout << first.to_string_short() << "|" << second.to_string_short() << " : " << log_string << std::endl;
}

struct solver
{
	std::map<puzzle_state, puzzle_state, puzzle_state_hash_comparator> _states_previous;
	std::set<puzzle_state, puzzle_state_comparator> _search_queue;

	bool is_empty() const
	{
		return _search_queue.empty();
	}

	puzzle_state dequeue()
	{
		
		puzzle_state retVal = *_search_queue.begin();

		_search_queue.erase(retVal);

		return retVal;
	}

	void enqueue(const puzzle_state& state)
	{
		_search_queue.insert(state);
	}

	bool is_inqueued(const puzzle_state& state) const
	{
		return (_search_queue.find(state) != _search_queue.end());
	}

	void discover_adjacent(const puzzle_state& state, const puzzle_state& prev)
	{
		log1("discovering", state);

		auto it = _states_previous.find(state);
		
		// if we already have a cheaper or equal cost route to this state: return
		if (it != _states_previous.end())
			return;

		log2("map", state, prev);
		_states_previous[state] = prev;

		if (is_inqueued(state))
		{
			log1("already enqueued", state);
			return;
		}

		log1("enqueue", state);
		enqueue(state);
	}

	puzzle_state get_previous_for(const puzzle_state& state) const
	{
		auto it = _states_previous.find(state);

		if (it != _states_previous.end())
			return it->second;
		else
			return puzzle_state();
	}

	std::vector<puzzle_state> find_solution(const puzzle_state& initial_state)
	{
		log1("initial", initial_state);

		discover_adjacent(initial_state, initial_state);

		while (!is_empty())
		{
			puzzle_state start = dequeue();

			log1("dequeued", start);

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

					retVal.push_back(initial_state);

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
	puzzle_state initial;

/*
	initial.set(0, 3).set(1, 1).set(2, 0);
	initial.set(3, 6).set(4, 4).set(5, 2);
	initial.set(6, 7).set(7, 8).set(8, 5);

	initial.set(0, 7).set(1, 3).set(2, 4);
	initial.set(3, 2).set(4, 1).set(5, 6);
	initial.set(6, 5).set(7, 0).set(8, 8);
*/
	initial.set(0, 5).set(1, 6).set(2, 0);
	initial.set(3, 8).set(4, 1).set(5, 2);
	initial.set(6, 4).set(7, 3).set(8, 7);

	solver _solver;

	std::vector<puzzle_state> solution;

	{
		BlockTimer b;

		solution = _solver.find_solution(initial);
	}

	for (const auto& b : solution)
	{
		std::cout << b.to_string() << std::endl;
	}

	std::cout << "total steps taken: " << solution.size() << std::endl;
}

