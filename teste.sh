#!/usr/bin/env bash

entries="entries"
results="results"

if [ ! -d "${entries}" ]; then
    echo "Entries directory not found"
    exit 1
# elif [ -d "${results}" ]; then
#     echo "Deleting old results"
#     rm -r ${results}
fi
if [ ! -d "${results}" ]; then
    echo "Creating main results directory"
    mkdir ${results}
fi

# ITERATIVE KNAPSACK
program="sequencial"
if [ ! -d "${results}/${program}" ]; then
    echo "Creating results directory for ${program}"
    mkdir ${results}/${program}
fi
echo "Executing tests for ${program}"
for entry in $(ls -Sr ${entries}); do
    echo "Executing for ${entry}"
    results_file=${results}/${program}/${entry}_results
    for i in {1..20}; do
        # size=$(echo ${entry} | cut -d"_" -f3)
        out=$(./${program} < ${entries}/${entry})
        res=$(echo ${out} | cut -d" " -f1)
        time=$(echo ${out} | cut -d":" -f2)

        echo -e "Exec ${i}\t| ${entry}\t|${time} \t| ${res}" >> ${results_file}
    done
done

# MPI KNAPSACK
program="mpi"
if [ ! -d "${results}/${program}" ]; then
    echo "Creating results directory for ${program}"
    mkdir ${results}/${program}
fi
echo "Executing tests for ${program}"
for entry in $(ls -Sr ${entries}); do
    for n_procs in {2,4,6}; do
        echo "Executing for ${entry} with ${n_procs} ranks"
        results_file=${results}/${program}/${entry}_${n_procs}_results
        for i in {1..20}; do
            out=$(mpirun -np ${n_procs} ./${program} < ${entries}/${entry})
            res=$(echo ${out} | cut -d" " -f1)
            time=$(echo ${out} | cut -d":" -f2)

            echo -e "Exec ${i}\t| ${entry}\t|${time} \t| ${res}" >> ${results_file}
        done
    done
done