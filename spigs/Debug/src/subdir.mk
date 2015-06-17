################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/PigsDAQ.cpp \
../src/PigsDAQ_test.cpp \
../src/newDAQ.cpp 

OBJS += \
./src/PigsDAQ.o \
./src/PigsDAQ_test.o \
./src/newDAQ.o 

CPP_DEPS += \
./src/PigsDAQ.d \
./src/PigsDAQ_test.d \
./src/newDAQ.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/root -I"/home/o/workspace/newDAQ/src" -I/usr/local/include/CAEN -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


