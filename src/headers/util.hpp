/*
 * util.hpp
 *
 *  Created on: Jul 12, 2008
 *      Author: pedro
 */

#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <boost/foreach.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>

#define foreach BOOST_FOREACH

typedef boost::multi_index::multi_index_container<int,
	boost::multi_index::indexed_by<
	boost::multi_index::random_access<>,
	boost::multi_index::hashed_unique<boost::multi_index::identity<int> >
	>
	> IntSet;

typedef IntSet::nth_index<1>::type IntHashSet;

class Util {
public:
};

#endif /* UTIL_HPP_ */
