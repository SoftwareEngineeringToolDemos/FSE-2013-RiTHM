#!/bin/bash

# factors declaration
monitoring_array=( false true )
iterations_array=( 1000 1000000 10000000 )

#output file header
echo -e "Run\tMonitoring\tIterations\tTime Sec\tTime Micro\tTime Nano" >> R.txt

for iterations_index in 0 1 2
do
  for monitoring_index in 0 1
  do
	echo "./BlowfishTest/Release/BlowfishTest -m ${monitoring_array[monitoring_index]} -i ${iterations_array[iterations_index]}"

	i="0"
	while [ $i -lt 50 ]
	do
#		echo -ne "$i\t${device_array[device_index]}\t${cores_array[cores_index]}\t${history_size_array[history_size_index]}\t${buffer_size_array[buffer_size_index]}\t${num_of_properties_array[kernel_file_index]}\t${property_cycle_array[kernel_file_index]}\t${inconclusive_states_array[kernel_file_index]}\t${comp_load_array[kernel_file_index]}\t" >> R.txt	

		echo -ne "$i\t${monitoring_array[monitoring_index]}\t${iterations_array[iterations_index]}\t" >> R.txt
		nice -n -10 ./BlowfishTest/Release/BlowfishTest -m ${monitoring_array[monitoring_index]} -i ${iterations_array[iterations_index]} >> R.txt

#		nice -n -10 ./Monitor/Release/Monitor device=${device_array[device_index]} cores_number=${cores_array[cores_index]} buffer_size=${buffer_size_array[buffer_size_index]} history_size=${history_size_array[history_size_index]} nproperties=${num_of_properties_array[kernel_file_index]} debug=false skip_cycles=5 property_cycle=${property_cycle_array[kernel_file_index]} kernel_file=./Monitor/kernel${kernel_file_array[kernel_file_index]}.cl	
		i=$[$i+1]
	done
  done
done

echo "finished!"
exit 0
