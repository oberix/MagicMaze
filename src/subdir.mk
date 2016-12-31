################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cgUtils.c \
../src/dim.c \
../src/game.c \
../src/main.c \
../src/maze.c \
../src/mazeCG.c \
../src/player.c \
../src/playerCG.c 

OBJS += \
./src/cgUtils.o \
./src/dim.o \
./src/game.o \
./src/main.o \
./src/maze.o \
./src/mazeCG.o \
./src/player.o \
./src/playerCG.o 

C_DEPS += \
./src/cgUtils.d \
./src/dim.d \
./src/game.d \
./src/main.d \
./src/maze.d \
./src/mazeCG.d \
./src/player.d \
./src/playerCG.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


