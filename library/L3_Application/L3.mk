INCLUDES +=
SYSTEM_INCLUDES +=

SOURCES += $(LIBRARY_DIR)/L3_Application/task_scheduler.cpp

TESTS += $(LIBRARY_DIR)/L3_Application/task_scheduler.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/task_scheduler_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/commandline_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/graphics_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/periodic_scheduler_test.cpp
