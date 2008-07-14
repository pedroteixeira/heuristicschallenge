/*
 * sa.hpp
 *
 *  Created on: Jul 13, 2008
 *      Author: pedro
 */

#ifndef SA_HPP_
#define SA_HPP_

#include "steiner.hpp"

class SaSteiner {
public:
	SaSteiner(std::string);
	~SaSteiner();
	void run();
private:
	Steiner* instance;
};


#endif /* SA_HPP_ */
