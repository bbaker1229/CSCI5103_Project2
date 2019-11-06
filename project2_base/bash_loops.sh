#!/bin/bash

echo "Algorithm,Program,Number_of_pages,Number_of_frames,Function_result,Page_faults,Disk_reads,Disk_writes" > dataset.csv

# Create rand sort dataset
for ((x=1; x <= 100; x+=1))
do
    echo -n "rand,sort,100,$x," >> dataset.csv
    ./virtmem 100 $x rand sort | grep -oP '([\d.]+)' | xargs | sed 's/ /,/g' >> dataset.csv
done

# Create rand scan dataset
for ((x=1; x <= 100; x+=1))
do
    echo -n "rand,scan,100,$x," >> dataset.csv
    ./virtmem 100 $x rand scan | grep -oP '([\d.]+)' | xargs | sed 's/ /,/g' >> dataset.csv
done

# Create rand focus dataset
for ((x=1; x <= 100; x+=1))
do
    echo -n "rand,focus,100,$x," >> dataset.csv
    ./virtmem 100 $x rand focus | grep -oP '([\d.]+)' | xargs | sed 's/ /,/g' >> dataset.csv
done

# Create fifo sort dataset
for ((x=1; x <= 100; x+=1))
do
    echo -n "fifo,sort,100,$x," >> dataset.csv
    ./virtmem 100 $x fifo sort | grep -oP '([\d.]+)' | xargs | sed 's/ /,/g' >> dataset.csv
done

# Create fifo scan dataset
for ((x=1; x <= 100; x+=1))
do
    echo -n "fifo,scan,100,$x," >> dataset.csv
    ./virtmem 100 $x fifo scan | grep -oP '([\d.]+)' | xargs | sed 's/ /,/g' >> dataset.csv
done

# Create fifo focus dataset
for ((x=1; x <= 100; x+=1))
do
    echo -n "fifo,focus,100,$x," >> dataset.csv
    ./virtmem 100 $x fifo focus | grep -oP '([\d.]+)' | xargs | sed 's/ /,/g' >> dataset.csv
done

# # Create custom sort dataset
# for ((x=1; x <= 100; x+=1))
# do
#    echo -n "custom,sort,100,$x," >> dataset.csv
#    ./virtmem 100 $x custom sort | grep -oP '([\d.]+)' | xargs | sed 's/ /,/g' >> dataset.csv
# done

# # Create custom scan dataset
# for ((x=1; x <= 100; x+=1))
# do
#    echo -n "custom,scan,100,$x," >> dataset.csv
#    ./virtmem 100 $x custom scan | grep -oP '([\d.]+)' | xargs | sed 's/ /,/g' >> dataset.csv
# done

# # Create custom focus dataset
# for ((x=1; x <= 100; x+=1))
# do
#    echo -n "custom,scan,100,$x," >> dataset.csv
#    ./virtmem 100 $x custom focus | grep -oP '([\d.]+)' | xargs | sed 's/ /,/g' >> dataset.csv
# done
