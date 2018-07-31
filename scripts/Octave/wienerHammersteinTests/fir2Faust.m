function[] = fir2Faust(fileName, permission, b, format)

fileID = fopen(fileName, permission);

fprintf(fileID, 'fi = library("filters.lib");\n\ntaps = (');

for t = 1:length(b)
    
    fprintf(fileID, num2str(b(t), format));
    
    if isequal(t, length(b))
        fprintf(fileID, ');\n');
    else
        fprintf(fileID, ', ');
    endif
    
endfor

fprintf(fileID, '\nprocess = fi.fir(taps);\n');

fclose(fileID);

endfunction