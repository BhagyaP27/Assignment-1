/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 * @author Bhagya Patel
 */

#include<interrupts.hpp>

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/
    int current_time = 0;
    int context_save_time = 10; // time to save or restore context


    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        if (activity == "CPU"){
            //CPU burst execution
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", CPU burst\n";
            current_time += duration_intr;
        } else if (activity == "SYSCALL"){
            //System call - duration_intr is the vector tavle position
            int vector_position = duration_intr;
            //Handle system call interrupt using boilerplate
            auto [intr_execution, new_time] = intr_boilerplate(current_time, vector_position, context_save_time, vectors);
            execution += intr_execution;
            current_time = new_time;
            //Execute system call handler
            int syscall_duration = 5; // fixed duration for system call handler
            execution += std::to_string(current_time) + ", " + std::to_string(syscall_duration) + ", System call handler at " + vectors[vector_position] + "\n";
            current_time += syscall_duration;
            //Context restore
            execution += std::to_string(current_time) + ", " + std::to_string(context_save_time) + ", context restored\n";
            current_time += context_save_time;

            execution += std::to_string(current_time) + ", " + std::to_string(1) + ", switch to user mode\n";
            current_time += 1;
        }else if (activity == "END_IO"){
            //I/O operation completion - duration_intr is the device number
            int device_id = duration_intr;

            execution += std::to_string(current_time) + ", " + std::to_string(1) + ", I/O completion from device " + std::to_string(device_id) + "\n";

            //Handle I/O interrupt using boilerplate
            auto [intr_execution, new_time] = intr_boilerplate(current_time, device_id, context_save_time, vectors);
            execution += intr_execution;
            current_time = new_time;

            //Execute ISR for I/O completion
            int isr_duration = delays[device_id]/20; // ISR duration is 5% of device delay
            if (isr_duration < 1) isr_duration = 1; // minimum ISR duration is 1

            execution += std::to_string(current_time) + ", " + std::to_string(isr_duration) + ", ISR for device " + std::to_string(device_id) + " at " + vectors[device_id] + "\n";
            current_time += isr_duration;
            //Context restore
            execution += std::to_string(current_time) + ", " + std::to_string(context_save_time) + ", context restored\n";
            current_time += context_save_time;
            execution += std::to_string(current_time) + ", " + std::to_string(1) + ", switch to user mode\n";
            current_time += 1;
        }


        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
