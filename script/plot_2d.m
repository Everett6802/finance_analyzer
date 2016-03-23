#!/usr/bin/env octave

%printf ("%s\n", program_name());
arg_list = argv();
index = 1
offset = 1
%printf ("\n");

%printf ("nargin: %d\n", nargin);

data_filename = "result_data.txt"
while (index <= nargin)
   if (strcmp(arg_list{index}, "-f") != 0)
      assert(index + 1 <= nargin, "The data filename argument NOT found")
      data_filename = arg_list{index + 1}
      offset = 2
   else
      assert(false, sprintf("Unknown flag: %s", arg_list{index}))
   endif
   index += offset
endwhile

data_filepath = sprintf("%s/%s/%s", pwd(), "result", data_filename)

close all; %close any previous figures 

fid = fopen(data_filepath); % TODO: check if fid is -1 and give error 
assert(fid != -1, sprintf("The data file[%s] does NOT exist", data_filepath))

line = 0; %for storing the string value of each line of the file 
data = zeros(2,2); %will be faster if we know how many lines are in file 
%as this array will not have to be resized by octave 

line_i = 1; 
while (-1 ~= (line = fgetl(fid))) 
   data(line_i++, :) = str2num(line); 
end 

fclose(fid); 

%displays data 
%data 

%plot(data(:,1),data(:,2)) 
plot(data(:,1),data(:,2), "*", "markersize", 10)
pause()
