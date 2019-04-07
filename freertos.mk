FREERTOS_DIR = ./lib/amazon-freertos
FREERTOS_SRC = 	$(FREERTOS_DIR)/lib/FreeRTOS/list.c  \
				$(FREERTOS_DIR)/lib/FreeRTOS/queue.c \
				$(FREERTOS_DIR)/lib/FreeRTOS/tasks.c \
				$(FREERTOS_DIR)/lib/FreeRTOS/timers.c \
				$(FREERTOS_DIR)/lib/FreeRTOS/event_groups.c
FREERTOS_SRC += $(FREERTOS_DIR)/lib/FreeRTOS/portable/GCC/ARM_CM3/port.c
FREERTOS_SRC += $(FREERTOS_DIR)/lib/FreeRTOS/portable/MemMang/heap_2.c

FREERTOS_INC = 	-I$(FREERTOS_DIR)/lib/include/ \
				-I$(FREERTOS_DIR)/lib/include/private/ \
				-I$(FREERTOS_DIR)/lib/FreeRTOS/portable/GCC/ARM_CM3/
