#pragma once

#include "puzzle_state.hpp"

struct puzzle_state_comparator
{
	bool operator() (const puzzle_state& l, const puzzle_state& r) const
	{
		int l_manh = l.manhattan_distance(),
			r_manh = r.manhattan_distance(),
			l_hash = l.hash(),
			r_hash = r.hash();

		if (l_manh == r_manh)
			return (l_hash < r_hash);
		else
			return (l_manh < r_manh);
	}
};

struct puzzle_state_hash_comparator
{
	bool operator() (const puzzle_state& l, const puzzle_state& r) const
	{
		return (l.hash() > r.hash());
	}
};

