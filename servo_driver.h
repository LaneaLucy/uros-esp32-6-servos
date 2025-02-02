/* Common functions for protocol examples, to establish Wi-Fi or Ethernet connection.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief sets up the servo's on given pin's and initialises the mcpwm module on ESP32 for the pin's
 *
 * @return
 *     - void
 */
void servo_driver_initialize();

/**
 * @brief Use this function to change the servo
 *
 * @param degree_angle  the angle in degrees to which servo has to rotate
 * @param channel  which servo has to rotate
 *
 * @return
 *     - void
 */
void set_servo_angle(uint32_t degree_angle, uint32_t channel);

#ifdef __cplusplus
}
#endif
