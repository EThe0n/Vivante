__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                               CLK_ADDRESS_CLAMP_TO_EDGE |
                               CLK_FILTER_NEAREST;

kernel void sobel(__read_only image2d_t src, __write_only image2d_t dst)
{
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
    int width = get_image_width(dst);

    if (coord.x >= width) {
        return;
    }

    uint4 p00 = read_imageui(src, smplr, (int2)(x - 1, y - 1));
    uint4 p10 = read_imageui(src, smplr, (int2)(x    , y - 1));
    uint4 p20 = read_imageui(src, smplr, (int2)(x + 1, y - 1));
    
    uint4 p01 = read_imageui(src, smplr, (int2)(x - 1, y));
    uint4 p21 = read_imageui(src, smplr, (int2)(x + 1, y));
    
    uint4 p02 = read_imageui(src, smplr, (int2)(x - 1, y + 1));
    uint4 p12 = read_imageui(src, smplr, (int2)(x    , y + 1));
    uint4 p22 = read_imageui(src, smplr, (int2)(x + 1, y + 1));    

    int gx = -p00.x + p20.x + ((p21.x - p01.x) << 1) - p02.x + p22.x; 
    int gy = -p00.x - p20.x + ((p12.x - p10.x) << 1) + p02.x + p22.x; 

    uint gradient = abs(gx) + abs(gy);
    write_imageui(dst, coord, (uint4)(max(min(gradient, 255), 0), 0, 0, 255));
}