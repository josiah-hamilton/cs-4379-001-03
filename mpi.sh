#!/bin/sh
#$ -V
#$ -cwd
#$ -S /bin/bash
#$ -N MPI_Test_Job
#$ -o $JOB_NAME.o$JOB_ID
#$ -e $JOB_NAME.e$JOB_ID
#$ -q omni
#$ -pe fill 4
#$ -P quanah

module load gnu openmpi
mpirun --machinefile machinefile.$JOB_ID -np $NSLOTS ./hwk3
