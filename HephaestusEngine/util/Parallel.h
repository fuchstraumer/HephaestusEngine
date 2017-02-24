#pragma once
#ifndef PARALLEL_H
#define PARALLEL_H
#include "stdafx.h"
#include <ppl.h>
#include <concurrent_queue.h>
#include <concurrent_priority_queue.h>
#include <concurrent_vector.h>
#include <concurrent_unordered_map.h>

/*

	PARALLEL_H

	Defines a set of algorithms for performing parallelized operations on various containers using
	common algorithms.

	(Currently, just using it for a breadth-first-search for lighting with a concurrent_queue)

*/

using namespace concurrency;


#endif // !PARALLEL_H
