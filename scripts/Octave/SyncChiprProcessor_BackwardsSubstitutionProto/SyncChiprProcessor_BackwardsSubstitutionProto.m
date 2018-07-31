clc; clear all;

% Create a random upper triangular complex matrix (coefficents)
N = 16;
W = 8192;

C = triu(rand(N)) + 1i * triu(rand(N));

% Matrix of known data
H = rand(N, W) + 1i * rand(N, W);

% Benchmark solution
G = C \ H;

% Backwards substitution solution
F = zeros(N, W) + 1i * zeros(N, W);

% Pupulate last row of F
F(N, :) = H(N, :) ./ C(N, N);

% Populate all the other rows
r = N - 1;

while (r > 0)
    
    numerator   = H(r, :) - C(r, (r + 1):N) * F((r + 1):N, :);
    denominator = C(r, r);
    
    F(r, :)     = numerator ./ denominator;
    
    r = r - 1;
    
end

mDiff = F - G;

dNorm = sqrt((mDiff(:))' * mDiff(:));

figure(1);
subplot(2, 1, 1);
imagesc(real(mDiff));
colorbar;
caxis([-max(abs(mDiff(:))) max(abs(mDiff(:)))]);
axis ij
%axis equal tight
title('Error - Real Part');

subplot(2, 1, 2);
imagesc(imag(mDiff));
colorbar;
caxis([-max(abs(mDiff(:))) max(abs(mDiff(:)))]);
axis ij
%axis equal tight
title('Error - Imaginary Part');