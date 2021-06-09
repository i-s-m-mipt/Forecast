#pragma once

#include <functional>
#include "zsheader.hpp"

namespace aim
{
	template<class T>
	struct hash
	{
		std::size_t operator()(const T& s) const {
			return std::hash<T>{}(s);
		}
	};

	template<class T>
	inline void hash_combine(size_t& l, T r) {
		l ^= (hash<T>{}(r)+0x9e3779b9 + (l << 6) + (l >> 2));
	}

	inline void hash_combine(size_t& l, size_t r) {
		l ^= (r + 0x9e3779b9 + (l << 6) + (l >> 2));
	}

	template<>
	struct hash< Point >
	{
		std::size_t operator()(const Point& s) const {
			size_t res = hash<std::string>{}(s.name);
			hash_combine(res, s.dt);
			return res;
		}
	};

	template<class T>
	struct hash< std::vector<T> >
	{
		size_t operator()(const std::vector<T>& s) const {
			if (s.empty())
				return 0;

			size_t res = hash<T>{}(s[0]);
			for (size_t i = 1; i < s.size(); ++i) {
				hash_combine(res, s[i]);
			}
			return res;
		}
	};

	template<>
	struct hash< NitkaID >
	{
		std::size_t operator()(const NitkaID& s) const {
			size_t res = hash<unsigned int>{}(s.StartTime);
			hash_combine(res, hash< std::vector<Point> >{}(s.points));
			hash_combine(res, hash<unsigned int>{}(s.idStartTime));
			hash_combine(res, hash< std::vector<Point> >{}(s.idPoints));
			hash_combine(res, hash<unsigned int>{}(s.pgStartTime));
			hash_combine(res, hash< std::vector<Point> >{}(s.pgPoints));
			return res;
		}
	};
}
