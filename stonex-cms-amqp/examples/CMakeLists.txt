
cmake_minimum_required(VERSION 3.16)
project(examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
    
add_subdirectory(send-receive)
add_subdirectory(receiver)
add_subdirectory(sender)
add_subdirectory(ThreadSender)



