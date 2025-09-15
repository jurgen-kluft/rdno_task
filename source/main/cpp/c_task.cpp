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

class callback_t;

struct opcode_t
{
    u16 m_type : 3;   // 0= conditional, 1 = callback, 2 = timeout, 3 = timed, 7 = u64 value
    u16 m_index : 13; // index (0 to 8191)
};

struct instruction_t
{
    opcode_t m_opcodes[4];
};

struct conditional_t
{
    opcode_t m_type;
    opcode_t m_condition;
    opcode_t m_onTrue;
    opcode_t m_onFalse;
};

struct timeout_t
{
    opcode_t m_type;
    opcode_t m_timeout;
    opcode_t m_action;
    opcode_t m_onTimeout;
};

struct timed_t
{
    opcode_t m_type;
    opcode_t m_interval;
    opcode_t m_lastRunTimeInMillis;
    opcode_t m_onAction;
};

struct scheduler_t
{
    instruction_t* m_instructions;
    s16            m_instructionCount;
    s16            m_instructionMaxCount;
    callback_t*    m_callbacks;
    s16            m_callbackCount;
    s16            m_callbackMaxCount;
    u64*           m_values;
    s16            m_valueCount;
    s16            m_valueMaxCount;
    s16*           m_taskqueue;
    s16            m_taskqueueCount;
    s16            m_taskqueueMaxCount;
};

action_t register_action(scheduler_t* scheduler, callback_t cb)
{
    if (scheduler->m_callbackCount + 1 > scheduler->m_callbackMaxCount)
        return;

    action_t action = (action_t)scheduler->m_callbackCount;
    scheduler->m_callbacks[scheduler->m_callbackCount++] = cb;
    return action;
}

void add_timeout(scheduler_t* scheduler, value_t timeoutInMillis, action_t actionIndex, s16 onTimeoutIndex)
{
    if (scheduler->m_instructionCount + 1 > scheduler->m_instructionMaxCount)
        return;

    instruction_t& instr = scheduler->m_instructions[scheduler->m_instructionCount++];
    instr.m_opcodes[0]   = {2, 0}; // timeout
    instr.m_opcodes[1]   = {7, (s16)scheduler->m_valueCount};
    instr.m_opcodes[2]   = {1, actionIndex};
    instr.m_opcodes[3]   = {1, onTimeoutIndex};

    if (scheduler->m_valueCount + 1 > scheduler->m_valueMaxCount)
        return;

    scheduler->m_values[scheduler->m_valueCount++] = timeoutInMillis;
}

#ifdef TARGET_ESP32

#    include "Arduino.h"

namespace ncore
{
    namespace ntask
    {

    } // namespace ntask
} // namespace ncore

#else

namespace ncore
{
    namespace ntask
    {

    } // namespace ntask
} // namespace ncore

#endif
