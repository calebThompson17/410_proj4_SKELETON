#include <mutex>

#include "../includes/baker.h"
#include "../includes/box.h"
#include "../includes/externs.h"
#include "../includes/PRINT.h"

using namespace std;

//ID is just a number used to identify this particular baker
//(used with PRINT statements)
Baker::Baker(int id):id(id)
{
}

Baker::~Baker()
{
}

//bake, box and append to anOrder.boxes vector
//if order has 13 donuts there should be 2 boxes
//1 with 12 donuts, 1 with 1 donut
void Baker::bake_and_box(ORDER &anOrder) {
	int numOfDonuts = anOrder.number_donuts;
	Box myBox = Box();
	while (numOfDonuts > 0) {
		DONUT myDonut = DONUT();
		if (!myBox.addDonut(myDonut)) {
			anOrder.boxes.push_back(myBox);
			myBox.clear();
			continue;
		}
		numOfDonuts--;
		if (numOfDonuts == 0 && myBox.size() > 0)
			anOrder.boxes.push_back(myBox);
	}
}

//as long as there are orders in order_in_Q then
//for each order:
//	create box(es) filled with number of donuts in the order
//  then place finished order on order_outvector
//  if waiter is finished (b_WaiterIsFinished) then
//  finish up remaining orders in order_in_Q and exit
//
//You will use cv_order_inQ to be notified by waiter
//when either order_in_Q.size() > 0 or b_WaiterIsFinished == true
//hint: wait for something to be in order_in_Q or b_WaiterIsFinished == true
void Baker::beBaker() {
	while (true) {
		ORDER myOrder = ORDER();
		{
			// get the order
			unique_lock<mutex> lck(mutex_order_inQ);
			if (order_in_Q.empty() && !b_WaiterIsFinished)
				cv_order_inQ.wait(lck);
			if (!order_in_Q.empty()) {
				myOrder = order_in_Q.front();
				order_in_Q.pop();
				PRINT6("Baker (", id, ") got order #", myOrder.order_number, "\torder_in_Q size:", order_in_Q.size());
			}
		}
		if (myOrder.order_number != UNINITIALIZED) {
			bake_and_box(myOrder);
			{
				// deliver the order
				unique_lock<mutex> lck(mutex_order_outQ);
				order_out_Vector.push_back(myOrder);
			}
		}
		if (order_in_Q.empty() && b_WaiterIsFinished) {
			PRINT3("Baker (", id, ") signing out");
			break;
		}
	}
}
