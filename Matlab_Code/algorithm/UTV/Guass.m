function Result = Guass(Uk,Vk,lambda)
NeiU = NeighborsU(Uk,lambda);
NeiV = NeighborsV(Vk);
Result = (NeiU + NeiV) / (2 + 2 * lambda);

% tagstruct.ImageLength = 128; % 影像的长度
% tagstruct.ImageWidth = 128;  % 影像的宽度
% tagstruct.Photometric = 1;
% tagstruct.BitsPerSample = 64;
% tagstruct.SamplesPerPixel = 1;
% tagstruct.RowsPerStrip = 16;
% tagstruct.PlanarConfiguration = Tiff.PlanarConfiguration.Chunky;
% tagstruct.Software = 'MATLAB'; 
% tagstruct.SampleFormat = 3;
% 
% filepath = ['D:\PaviaC\Test\NeiUm.tif'];
% A = NeiU;
% A = double(A);
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(A);
% t.close
% 
% filepath = ['D:\PaviaC\Test\NeiVm.tif'];
% A = NeiV;
% A = double(A);
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(A);
% t.close
% 
% Result = (NeiU + NeiV) / (2 + 2 * lambda);

end

