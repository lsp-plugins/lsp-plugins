function [dNorm, cppDet, matDet] = testCoefficientsMatrices(rePartFile, imPartFile)
    
    if ~exist('rePartFile', 'var')
        rePartFile = '/tmp/mCoeffsRe.csv';
    end
    
    if ~exist('imPartFile', 'var')
        imPartFile = '/tmp/mCoeffsIm.csv';
    end
    
    if ( ...
        ~exist(rePartFile, 'file') || ...
        ~exist(imPartFile, 'file') ...
        )
        return;
    end
    
    cppMatrix   = csvread(rePartFile) + 1i * csvread(imPartFile);
    cppDet      = prod(diag(cppMatrix));
    
    matMatrix   = transpose(powersin_ampl(size(cppMatrix, 1), 1.25892568));
    matDet      = prod(diag(matMatrix));
    
    mDiff       = matMatrix - cppMatrix;
    
    dNorm       = sqrt((mDiff(:))' * mDiff(:));
    
    figure('Name', ['Norm of the error: ' num2str(dNorm)]);
    
    colormap('jet');
    
    subplot(3, 2, 1);
    pcolor(real(mDiff));
    colorbar;
    caxis([-max(abs(mDiff(:))) max(abs(mDiff(:)))]);
    axis ij
    axis square
    title('Error - Real Part');
    
    subplot(3, 2, 2);
    pcolor(imag(mDiff));
    colorbar;
    caxis([-max(abs(mDiff(:))) max(abs(mDiff(:)))]);
    axis ij
    axis square
    title('Error - Imaginary Part');
    
    subplot(3, 2, 3);
    pcolor(real(matMatrix));
    colorbar;
    caxis([-max(abs(matMatrix(:))) max(abs(matMatrix(:)))]);
    axis ij
    axis square
    title('Coefficients Matrix Real Part - Octave');
    
    subplot(3, 2, 4);
    pcolor(imag(matMatrix));
    colorbar;
    caxis([-max(abs(matMatrix(:))) max(abs(matMatrix(:)))]);
    axis ij
    axis square
    title('Coefficients Matrix Imaginary Part - Octave');
    
    subplot(3, 2, 5);
    pcolor(real(cppMatrix));
    colorbar;
    caxis([-max(abs(cppMatrix(:))) max(abs(cppMatrix(:)))]);
    axis ij
    axis square
    title('Coefficients Matrix Real Part - C++');
    
    subplot(3, 2, 6);
    pcolor(imag(cppMatrix));
    colorbar;
    caxis([-max(abs(cppMatrix(:))) max(abs(cppMatrix(:)))]);
    axis ij
    axis square
    title('Coefficients Matrix Imaginary Part - C++');
    
    figure;
    pcolor(20.0 * log10(abs(matMatrix)));
    colorbar;
    caxis([-max(20.0 * log10(abs(cppMatrix(:)))) max(20.0 * log10(abs(cppMatrix(:))))]);
    axis ij
    axis square
    title('Matrix dB Values - Octave');
    
    figure;
    pcolor(20.0 * log10(abs(inv(matMatrix))));
    colorbar;
    caxis([-max(20.0 * log10(abs(cppMatrix(:)))) max(20.0 * log10(abs(cppMatrix(:))))]);
    axis ij
    axis square
    title('Inverse Matrix dB Values - Octave');
    
end
