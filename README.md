# rdno_task

This module provides a kind of execution processor for rdno_core projects. 

It is code driven, meaning that the user push tasks in execution order to
the processor. The processor will execute the tasks in the order they were pushed.
When using it you kind of start to see that it is a state machine supporting
mechanism.

- Function, can have delay (OnTrigger)
- Guard(OnTrue, OnFalse)
- Timeout(OnTimeOut)
- Periodic(OnPeriodic)
