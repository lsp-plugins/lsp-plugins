function C = powersin_ampl(N, A)
    % Code from http://ant-novak.com/Nonlinear_System_Identification_Waveguide.php

    % C = powersin_ampl(N,A);
    % .... N = number of harmonics
    % .... A = input amplitude
    % .... C = matrix for power-to-harmoncis conversion

    C = zeros(N, N) + 1i * zeros(N, N);
    
    for n = 1:N
        for m = 1:N
            if ( (n >= m) && (mod(n + m, 2) == 0) )
                C(n,m) = ...
                (((-1)^(2 * n + (1 - m) / 2)) / (2^(n - 1))) * ...
                nchoosek(n,(n - m) / 2);
            end
        end
    end
    
    if isequal(A, 1)
        return;
    end

    for k=1:N
        C(k, :) = C(k, :) .* A^(k - 1);
    end

end