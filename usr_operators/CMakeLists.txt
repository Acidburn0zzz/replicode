set(usr_operators_SRC
    auto_focus.cpp
    usr_operators.cpp
    Callbacks/callbacks.cpp
    Operators/operators.cpp
    TestProgram/test_program.cpp
    )

add_library(usr_operators SHARED ${usr_operators_SRC})
target_link_libraries(usr_operators r_exec)
# set_target_properties(usr_operators PROPERTIES PREFIX "")
