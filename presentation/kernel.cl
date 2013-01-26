const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_NEAREST;
  
__kernel void  bayer_image(__read_only image2d_t input, __write_only image2d_t output, __constant uchar * params, __global float* balance) 
{
  
  const int i = get_global_id(0); //row
  const int j = get_global_id(1); //column
  const uchar offset_type = params[0];
  const uchar pixel_offset_type = ( (offset_type & 0x02) + (i%2 << 1) + (offset_type + j)%2) %4;
  
  // pixel, p_tl, p_tt, p_tr, p_ll, p_rr, p_bl, p_bb, p_br, , 
  float green, red, blue;
  float4 pixel = read_imagef(input, sampler, (int2)(i,j));

  float4 p_tt = read_imagef(input, sampler, (int2)(i-1,j  ));
  float4 p_bb = read_imagef(input, sampler, (int2)(i+1,j));
  float4 p_ll = read_imagef(input, sampler, (int2)(i  ,j-1));
  float4 p_rr = read_imagef(input, sampler, (int2)(i  ,j+1));
  
  float4 p_tl = read_imagef(input, sampler, (int2)(i-1,j-1));
  float4 p_tr = read_imagef(input, sampler, (int2)(i-1,j+1));
  float4 p_bl = read_imagef(input, sampler, (int2)(i+1,j-1));
  float4 p_br = read_imagef(input, sampler, (int2)(i+1,j+1));
    
  if (pixel_offset_type == 0) {
      red = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;
      green = (p_tt.s0 + p_ll.s0 + p_rr.s0 + p_bb.s0) / 4.0;
      blue = pixel.s0;
  } else if (pixel_offset_type == 1) {
      red = (p_tt.s0 + p_bb.s0) / 2.0;
      green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 5.0;
      blue = (p_ll.s0 + p_rr.s0) / 2.0;
  } else if (pixel_offset_type == 2) {
      red = (p_ll.s0 + p_rr.s0) / 2.0;
      green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 5.0;
      blue = (p_tt.s0 + p_bb.s0) / 2.0;
  } else {
      red = pixel.s0;
      green = (p_tt.s0 + p_bb.s0 + p_ll.s0 + p_rr.s0) / 4.0;
      blue = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;
  }

  pixel.s0 = blue * balance[2];
  pixel.s1 = green * balance[1];
  pixel.s2 = red * balance[0];
  
  write_imagef(output, (int2)(i,j), pixel);
}
