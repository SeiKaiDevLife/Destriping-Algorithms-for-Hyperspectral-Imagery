img = imread("D:\PaviaC\测试图1\80R.tif");
[h,w,d] = size(img);
out = zeros(h,w,d / 2);
for i = 1:d
    if mod(i,2) == 0
        itemp = floor(i / 2);
        out(:,:,itemp) = img(:,:,i);
    end
end

tagstruct.ImageLength = h; % 影像的长度
tagstruct.ImageWidth = w;  % 影像的宽度
tagstruct.Photometric = 1;
tagstruct.BitsPerSample = 64;
tagstruct.SamplesPerPixel = d / 2;
tagstruct.RowsPerStrip = 16;
tagstruct.PlanarConfiguration = Tiff.PlanarConfiguration.Chunky;
tagstruct.Software = 'MATLAB'; 
tagstruct.SampleFormat = 3;

filepath = ['D:\PaviaC\PaviaC80R240.tif'];
    Result = out;
    Result = double(Result);
    t = Tiff(filepath,'w');
    t.setTag(tagstruct);
    t.write(Result);
    t.close