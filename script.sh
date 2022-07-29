#!/bin/bash

rm mpi sequencial
rm -rf results/

gcc sequencial.c -o sequencial
mpicc mpi.c -o mpi