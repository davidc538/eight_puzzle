#pragma once

#include "puzzle_state.hpp"

struct puzzle_state_comparator
{
	bool operator() (const puzzle_state& l, const puzzle_state& r) const
	{
		return ((l.manhattan_distance() + l.steps_taken)
			> (r.manhattan_distance() + r.steps_taken));
	}
};

struct hybrid_comparator
{
	bool operator() (const puzzle_state_search& l, const puzzle_state_search& r) const
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
	bool operator() (const puzzle_state_search& l, const puzzle_state_search& r) const
	{
		return ((l.current.manhattan_distance() + l.steps_taken)
			> (r.current.manhattan_distance() + r.steps_taken));
	}
};

struct distance_hash_comparator
{
	bool operator() (const puzzle_state_search& l, const puzzle_state_search& r) const
	{
		int l_heur = l.current.manhattan_distance() + l.steps_taken;
		int r_heur = r.current.manhattan_distance() + r.steps_taken;

		if (l_heur != r_heur)
			return (l_heur > r_heur);
		else
			return (l.current.hash() > r.current.hash());
	}
};

struct puzzle_state_hash_comparator
{
	bool operator() (const puzzle_state& l, const puzzle_state& r) const
	{
		return (l.hash() > r.hash());
	}
};
