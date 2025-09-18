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

class action_t;

enum eOpcodeType
{
    OPCODE_TYPE_FUNCTION = 0,
    OPCODE_TYPE_SINGLE   = 1,
    OPCODE_TYPE_TIMEOUT  = 2,
    OPCODE_TYPE_TIMED    = 3,
};

struct opcode_t
{
    inline opcode_t(s16 type)
        : m_type(type)
        , m_index(-1)
    {
    }
    s16 m_type;   // type
    s16 m_index;  // index (0 to 8191)
};

struct scheduler_t
{
    instruction_t* m_instructions;
    s16            m_instructionCount;
    s16            m_instructionMaxCount;
    action_t*      m_callbacks;
    s16            m_callbackCount;
    s16            m_callbackMaxCount;
    u64*           m_values;
    s16            m_valueCount;
    s16            m_valueMaxCount;
    s16*           m_taskqueue;
    s16            m_taskqueueCount;
    s16            m_taskqueueMaxCount;
};

//
// Concepts
//
typedef void* action_t;

struct function_t
{
    inline function_t(bool (*function)(void*), void* param)
        : m_type(OPCODE_TYPE_FUNCTION)
        , m_function(function)
        , m_param(param)
    {
    }
    opcode_t m_type;
    bool (*m_function)(void*);
    void* m_param;
};

struct single_t
{
    inline single_t(action_t* onAction)
        : m_type(OPCODE_TYPE_SINGLE)
        , m_onAction(onAction)
    {
    }
    opcode_t  m_type;
    action_t* m_onAction;
};

struct timeout_t
{
    inline timeout_t(u64 timeoutInMillis, action_t* onAction, action_t* onFinished, action_t* onTimeout)
        : m_type(OPCODE_TYPE_TIMEOUT)
        , m_timeoutInMillis(timeoutInMillis)
        , m_onAction(onAction)
        , m_onFinished(onFinished)
        , m_onTimeout(onTimeout)
    {
    }
    opcode_t  m_type;
    u64       m_timeoutInMillis;
    action_t* m_onAction;
    action_t* m_onFinished;
    action_t* m_onTimeout;
};

struct timed_t
{
    inline timed_t(u64 intervalInMillis, action_t* onAction)
        : m_type(OPCODE_TYPE_TIMED)
        , m_onAction(onAction)
        , m_intervalInMillis(intervalInMillis)
        , m_lastRunTimeInMillis(0)
    {
    }
    opcode_t  m_type;
    action_t* m_onAction;
    u64       m_intervalInMillis;
    u64       m_lastRunTimeInMillis;
};

// ----------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------

// Task based actions
bool funcStartConnectionToWiFi(void* param) { return false; }
bool funcIsConnectedToWiFi(void* param) { return false; }
bool funcStartConnectionToRemoteServer(void* param) { return false; }
bool funcIsConnectedToRemoteServer(void* param) { return false; }
bool funcStartReceiveNewConfigurationFromAccessPoint(void* param) { return false; }
bool funcStartReadingSensors(void* param) { return false; }

bool funcReadBH1750Sensor(void* param) { return false; }
bool funcReadBME280Sensor(void* param) { return false; }
bool funcReadSCD41Sensor(void* param) { return false; }

// Actions

function_t task_functions[] = {
    function_t(funcIsConnectedToWiFi, nullptr),                            // WiFi
    function_t(funcStartConnectionToWiFi, nullptr),                        // WiFi
    function_t(funcIsConnectedToRemoteServer, nullptr),                    // Remote Server
    function_t(funcStartConnectionToRemoteServer, nullptr),                // Remote Server
    function_t(funcStartReceiveNewConfigurationFromAccessPoint, nullptr),  // Configuration
    function_t(funcStartReadingSensors, nullptr),                          // Read Sensors
    function_t(funcReadBH1750Sensor, nullptr),                             // BH1750 sensor reading
    function_t(funcReadBME280Sensor, nullptr),                             // BME280 sensor reading
    function_t(funcReadSCD41Sensor, nullptr),                              // SCD41 sensor reading
};

// Tasks that run one single time

single_t single_tasks[] = {
    single_t(&callbacks[1]),  // Start WiFi connection
    single_t(&callbacks[4]),  // Start Configuration from Access Point
};

// Tasks with a timeout

timeout_t timeout_tasks[] = {
    timeout_t(10 * 1000, &callbacks[0], &callbacks[3], &callbacks[4]),  // WiFi connection
    timeout_t(60 * 1000, &callbacks[2], &callbacks[5], &callbacks[4]),  // Remote Server connection
};

// Tasks running on a time interval

timed_t timed_tasks[] = {
    timed_t(2 * 1000, &task_functions[6]),   // BH1750 sensor reading task
    timed_t(60 * 1000, &task_functions[7]),  // BME280 sensor reading task
    timed_t(5 * 1000, &task_functions[8]),   // SCD41 sensor reading task
};

#ifdef TARGET_ESP32

#    include "Arduino.h"

namespace ncore
{
    namespace ntask
    {

    }  // namespace ntask
}  // namespace ncore

#else

namespace ncore
{
    namespace ntask
    {

    }  // namespace ntask
}  // namespace ncore

#endif
