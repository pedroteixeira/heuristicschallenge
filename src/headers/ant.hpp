/*
 * ant.hpp
 *
 *  Created on: Jul 12, 2008
 *      Author: pedro
 */

#ifndef ANT_HPP_
#define ANT_HPP_

#include "steiner.hpp"

class AntSteiner {
public:
	AntSteiner(std::string);
	~AntSteiner();
	void run();
private:
	Steiner* instance;
};
#endif /* ANT_HPP_ */
