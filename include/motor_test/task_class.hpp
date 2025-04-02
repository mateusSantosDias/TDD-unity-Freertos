#ifndef TASK_CLASS_HPP
#define TASK_CLASS_HPP

#include <stdint.h>

typedef enum
{
    FREE,
    BUSY,
    PROCCESS_TERMINATED
} task_status;

typedef enum
{
    COMMAND_1,
    COMMAND_2
} commands;


class TaskClass
{
private:
    uint8_t task_id;
    commands command;
    task_status status;
    

public:

    commands get_command();
    void set_command(commands command_in);

    task_status get_status();
    void set_status(task_status status_in);

    uint8_t get_task_id();
    void set_task_id(uint8_t task_id_in);
};


#endif


commands TaskClass::get_command()
{
    return command;
}
void TaskClass::set_command(commands command_in)
{
    this->command = command_in;
}

task_status TaskClass::get_status()
{
    return status;
}
void TaskClass::set_status(task_status status_in)
{
    this->status = status_in;
}

uint8_t TaskClass::get_task_id()
{
    return task_id;
}
void TaskClass::set_task_id(uint8_t task_id_in)
{
    this->task_id = task_id_in;
}