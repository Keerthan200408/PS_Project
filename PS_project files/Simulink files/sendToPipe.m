function sendToPipe(power)
    % Write wind power to the wind data file
    fid1 = fopen('\\wsl.localhost\Ubuntu\tmp\wind_data_debug.txt', 'a');
    if fid1 ~= -1
        fprintf(fid1, '%.3f\n', power);
        fclose(fid1);
    else
        warning('Could not open wind data file.');
    end

end
