#ifndef INTERVAL_H
#define INTERVAL_H
#include <vector>
#include <algorithm>
#include <memory>

// Template for interval holding value of type T,
// with start/end points of type K (which are usu-
// -ally fine left as size_t).
template<typename T,typename K = std::size_t>
struct Interval {
	// The value held by this interval
	T value;
	// The start and end points of this interval
	K start; K stop;
	// Constructor, const val so we don't modify original value
	Interval(K &strt, K &stp, const T &val)
		:  start(strt), stop(stop), value(val) {}
};

template<typename T, typename K = std::size_t>
class IntervalTree {
public:
	std::vector<Interval<T,K>> Intervals;
	std::unique_ptr<IntervalTree> left;
	std::unique_ptr<IntervalTree> right;
	K center;

	// Constructor
	IntervalTree<T,K> : left(nullptr), right (nullptr),
		center (0) { }
};
#endif // !INTERVAL_H
