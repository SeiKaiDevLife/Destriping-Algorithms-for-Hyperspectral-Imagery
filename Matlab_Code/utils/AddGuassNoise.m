img = double(imread("D:\Denoise\WDC\测试图_纯条带\80PR.tif"));
[h,w,d] =size(img); 

for i = 1:d
    randD = 0.01 * rand(1);
    img(:,:,i) = imnoise(img(:,:,i),'gaussian',0,randD);
    randD = 0.05 * rand(1);
    img(:,:,i) = imnoise(img(:,:,i), 'salt & pepper', randD);
end

tagstruct.ImageLength = h; % 影像的长度
tagstruct.ImageWidth = w;  % 影像的宽度
tagstruct.Photometric = 1;
tagstruct.BitsPerSample = 64;
tagstruct.SamplesPerPixel = d;
tagstruct.RowsPerStrip = 16;
tagstruct.PlanarConfiguration = Tiff.PlanarConfiguration.Chunky;
tagstruct.Software = 'MATLAB'; 
tagstruct.SampleFormat = 3;

filepath = "D:\Denoise\WDC\测试图\80PR.tif";
Result = img;
Result = double(Result);
t = Tiff(filepath,'w');
t.setTag(tagstruct);
t.write(Result);
t.close