#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 4
#define NUM_CUSTOMERS 6
#define NUM_COOKS 2
#define ORDERS_PER_CUSTOMER 2
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
void* BENSCHILLIBOWLCustomer(void* tid) {
    int customer_id = (int)(long) tid;
  
    for (int i = 0; i < ORDERS_PER_CUSTOMER; i++){
      // allocate space (memory) for an order.
      Order *order = (Order*) malloc(sizeof(Order));
      
      // select a menu item.
      MenuItem menu_item = PickRandomMenuItem();
      
      // populate the order with their menu item and their customer ID.
      order->customer_id = customer_id;
			order->menu_item = menu_item;
      order->next = NULL;
      
      // add their order to the restaurant.
      int order_number = AddOrder(bcb, order);
      printf("Customer #%d added order #%d\n", customer_id, order_number);
    }
    return NULL;
}

/**
 * Thread function that represents a cook in the restaurant. A cook should:
 *  - get an order from the restaurant.
 *  - if the order is valid, it should fulfill the order, and then
 *    free the space taken by the order.
 * The cook should take orders from the restaurants until it does not
 * receive an order.
 */
void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = (int)(long) tid;
    int orders_fulfilled = 0;
    Order *order;
    // get an order from the restaurant.
    order = GetOrder(bcb);
  
    // get orders from the restaurants until there's no order available.
    while(order != NULL){
      // free the space taken by the order.
      free(order); 
      orders_fulfilled += 1;
      order = GetOrder(bcb);
    }
    
    printf("Cook #%d fulfilled %d orders\n", cook_id, orders_fulfilled);
    return NULL;
}

/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {
    // open the restaurant
    bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);
    
    // create customers and cooks
    // Customers and Cooks are represented as Threads using the pthread library. 
    pthread_t customers[NUM_CUSTOMERS];
    int customer_id[NUM_CUSTOMERS];
  
    for(int i = 0; i < NUM_CUSTOMERS; i++){
      customer_id[i] = i+1;
      pthread_create(&(customers[i]), NULL, BENSCHILLIBOWLCustomer, &(customer_id[i]));
    }
    
    pthread_t cooks[NUM_COOKS];
    int cook_id[NUM_COOKS];
    for (int i = 0; i < NUM_COOKS; i++){
      cook_id[i] = i+1;
      pthread_create(&(cooks[i]), NULL, BENSCHILLIBOWLCook, &(cook_id[i]));
    }
    
    // wait for all customers and cooks to be done
    for(int i = 0; i< NUM_CUSTOMERS; i++){
      pthread_join(customers[i], NULL);
    }
    for (int i = 0; i< NUM_COOKS; i++){
      pthread_join(cooks[i], NULL);
    }
    
    // close the restaurant.
    CloseRestaurant(bcb);
  
    return 0;
}