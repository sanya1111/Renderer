################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Renderer.cpp 

OBJS += \
./src/Renderer.o 

CPP_DEPS += \
./src/Renderer.d 


# Each subdirectory must supply rules for building sources it contributes
src/Renderer.o: ../src/Renderer.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/gtkmm-3.0 -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/Renderer.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


