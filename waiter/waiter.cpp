#include <string>
#include "stdlib.h"

#include "../includes/externs.h"
#include "../includes/PRINT.h"
#include "../includes/waiter.h"

using namespace std;

//ID is just a number used to identify this particular baker
//(used with PRINT statements)
//filename is what waiter reads in orders from
Waiter::Waiter(int id,std::string filename):id(id),myIO(filename){
//	b_WaiterIsFinished = false;  // TODO: Possibly add this
}

Waiter::~Waiter()
{
}

//gets next Order from file_IO
//if return == SUCCESS then anOrder
//contains new order
//otherwise return contains fileIO error
int Waiter::getNext(ORDER &anOrder){
	if (myIO.getNext(anOrder) == SUCCESS) {
		return SUCCESS;
	}
	return COULD_NOT_OPEN_FILE;
}

//contains a loop that will get orders from filename one at a time
//then puts them in order_in_Q then signals baker(s) using cv_order_inQ
//so they can be consumed by baker(s)
//when finished exits loop and signals baker(s) using cv_order_inQ that
//it is done using b_WaiterIsFinished
void Waiter::beWaiter() {
	ORDER myOrder = ORDER();
	while (getNext(myOrder) == SUCCESS) {
		{
			// acquire the lock
			unique_lock<mutex> lck(mutex_order_inQ);
			order_in_Q.push(myOrder);  // Possibly need a deep-copy
//			PRINT4("Waiter (", id, ") add order #", myOrder.order_number);
		}
		cv_order_inQ.notify_all();
	}
//	PRINT3("Waiter (", id, ") signing out");
	b_WaiterIsFinished = true;
	cv_order_inQ.notify_all();
}

