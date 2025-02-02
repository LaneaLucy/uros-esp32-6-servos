#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <micro_ros_utilities/type_utilities.h>

// #include <std_msgs/msg/int32.h>
#include <std_msgs/msg/float32_multi_array.h>

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <unistd.h>

#include "servo_driver.h"
//#include "angles.h"
#include <math.h>

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

#define OFFSET M_PI/2

rcl_subscription_t subscriber1;
// rcl_subscription_t subscriber2;
// rcl_subscription_t subscriber3;
// rcl_subscription_t subscriber4;
// rcl_subscription_t subscriber5;
// rcl_subscription_t subscriber6;
std_msgs__msg__Float32MultiArray msg1;
// std_msgs__msg__Int32 msg2;
// std_msgs__msg__Int32 msg3;
// std_msgs__msg__Int32 msg4;
// std_msgs__msg__Int32 msg5;
// std_msgs__msg__Int32 msg6;

/*!
 * \brief Convert radians to degrees
 */
static inline double to_degrees(double radians)
{
  return radians * 180.0 / M_PI;
}

void subscription_callback1(const void * msgin)
{
	const std_msgs__msg__Float32MultiArray * msg = (const std_msgs__msg__Float32MultiArray *)msgin;

	printf("received message\n");

	if (msg->data.size != 6) {
		printf("wrong array size!\n");
		return;
	}

	//printf("Received: %d, and setting servo angle's\n", msg->data.data);

	for(int i=1; i<=6; i++){
		double radians = msg->data.data[i-1];
		double normalized = radians + OFFSET; // + M_PI;
		int degrees = to_degrees(normalized);
    	printf("channel: %d, angle: %f, normalized: %f, angle: %d\n", i, radians, normalized, degrees);
		set_servo_angle(degrees, i);
		printf("angle set\n");
	}

	// set_servo_angle(msg->data, 1);
	// printf("angle set\n");
}

// void subscription_callback2(const void * msgin)
// {
// 	const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
// 	printf("Received: %d, and setting servo angle on channel 2\n", msg->data);

// 	//set_servo_angle(msg->data, 2);
// 	printf("angle set\n");
// }

// void subscription_callback3(const void * msgin)
// {
// 	const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
// 	printf("Received: %d, and setting servo angle on channel 3\n", msg->data);

// 	set_servo_angle(msg->data, 3);
// 	printf("angle set\n");
// }

// void subscription_callback4(const void * msgin)
// {
// 	const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
// 	printf("Received: %d, and setting servo angle on channel 4\n", msg->data);

// 	set_servo_angle(msg->data, 4);
// 	printf("angle set\n");
// }

// void subscription_callback5(const void * msgin)
// {
// 	const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
// 	printf("Received: %d, and setting servo angle on channel 5\n", msg->data);

// 	set_servo_angle(msg->data, 5);
// 	printf("angle set\n");
// }

// void subscription_callback6(const void * msgin)
// {
// 	const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
// 	printf("Received: %d, and setting servo angle on channel 6\n", msg->data);

// 	set_servo_angle(msg->data, 6);
// 	printf("angle set\n");
// }


void appMain(void * arg)
{
  	rcl_allocator_t allocator = rcl_get_default_allocator();
	rclc_support_t support;

	// set up servo on pin 18
	servo_driver_initialize();
	printf("servo driver initialised\n");

	// create init_options
	RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

	// create node
	rcl_node_t node;
	RCCHECK(rclc_node_init_default(&node, "servo_uros_node", "", &support));

	// create subscriber
	RCCHECK(rclc_subscription_init_default(
		&subscriber1,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray),
		"/microROS/servos"));

	// RCCHECK(rclc_subscription_init_default(
	// 	&subscriber2,
	// 	&node,
	// 	ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
	// 	"/microROS/int32_subscriber2"));

	// RCCHECK(rclc_subscription_init_default(
	// 	&subscriber3,
	// 	&node,
	// 	ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
	// 	"/microROS/int32_subscriber3"));

	// RCCHECK(rclc_subscription_init_default(
	// 	&subscriber4,
	// 	&node,
	// 	ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
	// 	"/microROS/int32_subscriber4"));

	// RCCHECK(rclc_subscription_init_default(
	// 	&subscriber5,
	// 	&node,
	// 	ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
	// 	"/microROS/int32_subscriber5"));

	// RCCHECK(rclc_subscription_init_default(
	// 	&subscriber6,
	// 	&node,
	// 	ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
	// 	"/microROS/int32_subscriber6"));

	static micro_ros_utilities_memory_conf_t conf = {0};

	conf.max_basic_type_sequence_capacity = 12;

	bool success = micro_ros_utilities_create_message_memory(
	  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray),
	  &msg1,
	  conf
	);

	// create executor
	rclc_executor_t executor;
	RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
	RCCHECK(rclc_executor_add_subscription(&executor, &subscriber1, &msg1, &subscription_callback1, ON_NEW_DATA));
	// RCCHECK(rclc_executor_add_subscription(&executor, &subscriber2, &msg2, &subscription_callback2, ON_NEW_DATA));
	// RCCHECK(rclc_executor_add_subscription(&executor, &subscriber3, &msg3, &subscription_callback3, ON_NEW_DATA));
	// RCCHECK(rclc_executor_add_subscription(&executor, &subscriber4, &msg4, &subscription_callback4, ON_NEW_DATA));
	// RCCHECK(rclc_executor_add_subscription(&executor, &subscriber5, &msg5, &subscription_callback5, ON_NEW_DATA));
	// RCCHECK(rclc_executor_add_subscription(&executor, &subscriber6, &msg6, &subscription_callback6, ON_NEW_DATA));

	printf("microROS initialised\n");

	while(1){
			rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
			usleep(100000);
	}

	printf("leaving microROS\n");
	// free resources
	RCCHECK(rcl_subscription_fini(&subscriber1, &node));
	// RCCHECK(rcl_subscription_fini(&subscriber2, &node));
	// RCCHECK(rcl_subscription_fini(&subscriber3, &node));
	// RCCHECK(rcl_subscription_fini(&subscriber4, &node));
	// RCCHECK(rcl_subscription_fini(&subscriber5, &node));
	// RCCHECK(rcl_subscription_fini(&subscriber6, &node));
	RCCHECK(rcl_node_fini(&node));
	
	vTaskDelete(NULL);
}
