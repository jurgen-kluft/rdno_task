#include "rdno_core/c_target.h"
#include "rdno_task/c_task.h"

// Documentation:
/*

Conditions
Actions
Time

Action Connect To WiFi
    configure
    connect to WiFi
    SCHEDULE action to check WiFi connection status

TimeOut(60 seconds)
    Action Wait for WiFi connection
        if WiFi connected
            SCHEDULE action to connect to remote TCP server
    OnTimeOut
        SCHEDULE action to receive new configuration from Access Point

Action Connect To Remote TCP Server
    configure
    connect to remote TCP server
    SCHEDULE action to check remote TCP connection status

TimeOut(60 seconds)
    Action Wait for remote TCP connection
        if connected
            SCHEDULE action to read BH1750 sensor (every 2 seconds)
            SCHEDULE action to read BME280 sensor (every 60 seconds)
            SCHEDULE action to read SCD41 sensor (every 5 seconds)
    OnTimeOut
        SCHEDULE action to receive new configuration from Access Point

Timed(every 5 seconds)
    Conditional(WiFi Connected)
        Conditional(Remote TCP Connected)
            Action Read Sensor
                read sensor
                if sensors read successfully
                    if sensor data changed
                        SCHEDULE action to send sensor data to remote TCP server
                    else
                        SCHEDULE action to read sensors (try again)
                else
                    SCHEDULE action to read sensors (try again)
            OnConditionFalse
                SCHEDULE action to connect to remote TCP server
        OnConditionFalse
            SCHEDULE action to connect to WiFi

Action Send Sensor Data To Remote TCP Server
    send sensor data to remote TCP server
    SCHEDULE action to read sensors

*/

namespace ncore
{
    //
    // Concepts
    //

    struct scheduler_t;
    struct state_t;

    typedef s32 task_result_t;
    typedef task_result_t (*function_t)(scheduler_t*, state_t*);

    const task_result_t RESULT_OK    = 0;
    const task_result_t RESULT_DONE  = 1;
    const task_result_t RESULT_ERROR = -1;

    struct scheduler_t
    {
        void clear();

        void add_task(function_t function, state_t* state);
        void add_guard_task(function_t function, state_t* state);
        void add_timeout_task(s32 timeout_ms, function_t function, state_t* state);
        void add_periodic_task(s32 period_ms, function_t function, state_t* state);
    };

    struct wifi_state_t;
    struct remote_state_t;
    struct app_state_t;

    struct state_t
    {
        wifi_state_t*   wifi;
        remote_state_t* remote;
        app_state_t*    app;
    };

    // ----------------------------------------------------------------------------------
    // User Example
    // ----------------------------------------------------------------------------------

    // WiFi
    task_result_t func_start_connection_to_wifi(scheduler_t* scheduler, state_t* state);
    task_result_t func_is_wifi_connected(scheduler_t* scheduler, state_t* state);

    // Remote Server
    task_result_t func_start_connection_to_remote(scheduler_t* scheduler, state_t* state);
    task_result_t func_is_remote_connected(scheduler_t* scheduler, state_t* state);

    // WiFi and Remote Server
    task_result_t func_is_connected(scheduler_t* scheduler, state_t* state);

    // Configuration Mode
    task_result_t func_start_configuration(scheduler_t* scheduler, state_t* state);
    task_result_t func_receive_configuration(scheduler_t* scheduler, state_t* state);

    // Sensor
    task_result_t func_start_reading_sensors(scheduler_t* scheduler, state_t* state);
    task_result_t func_read_sensor_BH1750(scheduler_t* scheduler, state_t* state);
    task_result_t func_read_sensor_BME280(scheduler_t* scheduler, state_t* state);
    task_result_t func_read_sensor_SCD41(scheduler_t* scheduler, state_t* state);

    // High level tasks
    task_result_t func_wifi_start(scheduler_t* scheduler, state_t* state);
    task_result_t func_wifi_until_connected(scheduler_t* scheduler, state_t* state);
    task_result_t func_remote_server_start(scheduler_t* scheduler, state_t* state);
    task_result_t func_remote_server_until_connected(scheduler_t* scheduler, state_t* state);
    task_result_t func_config_start(scheduler_t* scheduler, state_t* state);
    task_result_t func_config_wait_for_config(scheduler_t* scheduler, state_t* state);
    task_result_t func_schedule_sensor_reading(scheduler_t* scheduler, state_t* state);

    task_result_t func_schedule_sensor_reading(scheduler_t* scheduler, state_t* state)
    {
        scheduler->clear();
        scheduler->add_guard_task(func_is_connected, state);
        scheduler->add_periodic_task(5000, func_read_sensor_BH1750, state);
        scheduler->add_periodic_task(60000, func_read_sensor_BME280, state);
        scheduler->add_periodic_task(5000, func_read_sensor_SCD41, state);
    }

    task_result_t func_config_receive_new_configuration(scheduler_t* scheduler, state_t* state)
    {
        if (func_receive_configuration(scheduler, state) == RESULT_DONE)
        {
            scheduler->clear();
            func_wifi_start(scheduler, state);
        }
    }

    task_result_t func_config_wait_for_connection(scheduler_t* scheduler, state_t* state)
    {
        // TODO
        return RESULT_OK;
    }

    task_result_t func_config_start(scheduler_t* scheduler, state_t* state)
    {
        // TODO
        return RESULT_DONE;
    }

    task_result_t func_remote_server_start(scheduler_t* scheduler, state_t* state)
    {
        // TODO
        return RESULT_DONE;
    }

    task_result_t func_remote_server_until_connected(scheduler_t* scheduler, state_t* state)
    {
        // TODO
        return RESULT_OK;
    }

    task_result_t func_wifi_start(scheduler_t* scheduler, state_t* state)
    {
        if (func_start_connection_to_wifi(scheduler, state) == RESULT_ERROR)
        {
            scheduler->clear();
            scheduler->add_task(func_config_start, state);
        }
        else
        {
            scheduler->clear();
            scheduler->add_timeout_task(5000, func_wifi_until_connected, state);
        }
        return RESULT_DONE;
    }

    task_result_t func_wifi_until_connected(scheduler_t* scheduler, state_t* state)
    {
        // TODO
        return RESULT_OK;
    }

    task_result_t func_setup(scheduler_t* scheduler, state_t* state)
    {
        scheduler->add_task(func_wifi_start, state);
        return RESULT_DONE;
    }

    task_result_t func_is_connected(scheduler_t* scheduler, state_t* state)
    {
        // if WiFi and Remote are still connected return OK, otherwise return ERROR
        return RESULT_OK;
    }

}  // namespace ncore