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

    typedef s32 task_result_t;
    typedef task_result_t (*function_t)(scheduler_t*, void*);

    const task_result_t RESULT_OK    = 0;
    const task_result_t RESULT_DONE  = 1;
    const task_result_t RESULT_ERROR = -1;

    struct scheduler_t
    {
        void clear();

        void add_task(function_t function, void* param);
        void add_guard_task(function_t function, void* param);
        void add_timeout_task(s32 timeout_ms, function_t function, void* param);
        void add_periodic_task(s32 period_ms, function_t function, void* param);
    };

    // ----------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------

    // WiFi
    task_result_t start_connection_to_wifi(scheduler_t* scheduler, void* param);
    task_result_t is_wifi_connected(scheduler_t* scheduler, void* param);

    // Remote Server
    task_result_t start_connection_to_remote(scheduler_t* scheduler, void* param);
    task_result_t is_remote_connected(scheduler_t* scheduler, void* param);

    // WiFi and Remote Server
    task_result_t is_connected(scheduler_t* scheduler, void* param);

    // Configuration Mode
    task_result_t start_configuration(scheduler_t* scheduler, void* param);
    task_result_t receive_configuration(scheduler_t* scheduler, void* param);

    // Sensor
    task_result_t start_reading_sensors(scheduler_t* scheduler, void* param);
    task_result_t read_sensor_BH1750(scheduler_t* scheduler, void* param);
    task_result_t read_sensor_BME280(scheduler_t* scheduler, void* param);
    task_result_t read_sensor_SCD41(scheduler_t* scheduler, void* param);

    // High level tasks
    task_result_t task_wifi_start(scheduler_t* scheduler, void* param);
    task_result_t task_wifi_until_connected(scheduler_t* scheduler, void* param);
    task_result_t task_remote_server_start(scheduler_t* scheduler, void* param);
    task_result_t task_remote_server_until_connected(scheduler_t* scheduler, void* param);
    task_result_t task_config_start(scheduler_t* scheduler, void* param);
    task_result_t task_config_wait_for_config(scheduler_t* scheduler, void* param);
    task_result_t task_schedule_sensor_reading(scheduler_t* scheduler, void* param);

    task_result_t task_schedule_sensor_reading(scheduler_t* scheduler, void* param)
    {
        scheduler->clear();
        scheduler->add_guard_task(is_connected, param);
        scheduler->add_periodic_task(5000, read_sensor_BH1750, param);
        scheduler->add_periodic_task(60000, read_sensor_BME280, param);
        scheduler->add_periodic_task(5000, read_sensor_SCD41, param);
    }

    task_result_t task_config_receive_new_configuration(scheduler_t* scheduler, void* param)
    {
        if (receive_configuration(scheduler, param) == RESULT_DONE)
        {
            scheduler->clear();
            task_wifi_start(scheduler, param);
        }
    }

    task_result_t task_config_wait_for_connection(scheduler_t* scheduler, void* param)
    {
        // TODO
        return RESULT_OK;
    }

    task_result_t task_config_start(scheduler_t* scheduler, void* param)
    {
        // TODO
        return RESULT_DONE;
    }

    task_result_t task_remote_server_start(scheduler_t* scheduler, void* param)
    {
        // TODO
        return RESULT_DONE;
    }

    task_result_t task_remote_server_until_connected(scheduler_t* scheduler, void* param)
    {
        // TODO
        return RESULT_OK;
    }

    task_result_t task_wifi_start(scheduler_t* scheduler, void* param)
    {
        if (start_connection_to_wifi(scheduler, param) == RESULT_ERROR)
        {
            scheduler->clear();
            scheduler->add_task(task_config_start, param);
        }
        else
        {
            scheduler->clear();
            scheduler->add_timeout_task(5000, task_wifi_until_connected, param);
        }
        return RESULT_DONE;
    }

    task_result_t task_wifi_until_connected(scheduler_t* scheduler, void* param)
    {
        // TODO
        return RESULT_OK;
    }

    task_result_t task_setup(scheduler_t* scheduler, void* param)
    {
        scheduler->add_task(task_wifi_start, param);
        return RESULT_DONE;
    }

    task_result_t is_connected(scheduler_t* scheduler, void* param)
    {
        // if WiFi and Remote are still connected return OK, otherwise return ERROR
        return RESULT_OK;
    }

}  // namespace ncore