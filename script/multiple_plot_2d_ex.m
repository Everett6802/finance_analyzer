#!/usr/bin/env octave

%printf ("%s\n", program_name());
arg_list = argv();
index = 1
offset = 1
%printf ("\n");

%printf ("nargin: %d\n", nargin);

close all; %close any previous figures 

plot_config_filename = ".multiple_2d_graph.conf"
while (index <= nargin)
   if (strcmp(arg_list{index}, "-f") != 0)
      assert(index + 1 <= nargin, "The data filename argument NOT found")
      plot_config_filename = arg_list{index + 1}
      offset = 2
   else
      assert(false, sprintf("Unknown flag: %s", arg_list{index}))
   endif
   index += offset
endwhile

plot_config_filepath = sprintf("%s/%s/%s", pwd(), "result", plot_config_filename)
plot_config_fid = fopen(plot_config_filepath); % TODO: check if fid is -1 and give error 
assert(plot_config_fid != -1, sprintf("The plot 2D config file[%s] does NOT exist", plot_config_filepath))
count = 1;
while (-1 ~= (line = fgetl(plot_config_fid)))
%   printf("nargin: %s\n", line);
   [data_filename, rem] = strtok (line, ",");
   graph_title = substr(rem, 2);
%   printf("tok: %s, rem: %s, line: %s\n", tok, rem, line);

%   xlabel ("x");
%   ylabel ("sin (x)");
%   text (pi, 0.7, "arbitrary text");
%   legend ("sin (x)");
   data_filepath = sprintf("%s/%s/%s", pwd(), "result", data_filename)
   fid = fopen(data_filepath); % TODO: check if fid is -1 and give error 
   assert(fid != -1, sprintf("The data file[%s] does NOT exist", data_filepath))

   line = 0; %for storing the string value of each line of the file 
   data = zeros(2, 2); %will be faster if we know how many lines are in file 
   %as this array will not have to be resized by octave 

   line_i = 1; 
   while (-1 ~= (line = fgetl(fid))) 
      data(line_i++, :) = str2num(line); 
   end 
   fclose(fid); 

   figure(count);
   printf("graph_title: %s\n", graph_title);
   plot(data(:,1), data(:,2), "*", "markersize", 10);
   title(graph_title);
   count++;
end 

pause()
