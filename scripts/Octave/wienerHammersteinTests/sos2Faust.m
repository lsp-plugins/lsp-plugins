function[] = sos2Faust(fileName, permission, SOS, G, format)

fileID = fopen(fileName, permission);

fprintf(fileID, 'fi = library("filters.lib");\n\nprocess = ');

fprintf(fileID, ['*(_, ' num2str(G, format) ') : ']);

for s = 1:size(SOS, 1)
    
    fprintf(fileID, ['fi.tf2np(']);
    
    for c = [1 2 3 5 6]
        
        fprintf(fileID, num2str(SOS(s, c), format));
        
        if isequal(c, 6)
            if isequal(s, size(SOS, 1))
                fprintf(fileID, ');\n');
            else
                fprintf(fileID, ') : ');
            endif
        else
            fprintf(fileID, ', ');
        endif
        
    endfor
    
endfor

fclose(fileID);

endfunction