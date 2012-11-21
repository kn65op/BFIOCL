__kernel void  bayer( __constant uchar * params, __global float* input, __global float* output) 
{
  int width_output = get_global_size(0); 
  int width = get_global_size(0) + 2; 

//  int height = get_global_size(1); 
  int jout = get_global_id(0); //column number
  int j = get_global_id(0) + 1; //column number
  int i = get_global_id(1) + 1; //row number
  int iout = get_global_id(1); 
  int ptr = i * width + j; 
  int ptro = iout * width_output+ jout; 
  uchar offset_type = params[0]; 
  uchar pixel_offset_type = ( (offset_type & 0x02) + (iout%2 << 1) + (offset_type + j+1)%2) %4;
  uchar r_pos = params[1]; 
  uchar g_pos = params[2]; 
  uchar b_pos = params[3]; 

  float8 P;
  
  P.s0 = input[(i-1)*width + j -1];
  P.s1 = input[(i-1)*width + j];
  P.s2 = input[(i-1)*width + j+1];
  P.s3 = input[(i)*width + j-1];
  P.s4 = input[(i)*width + j+1];
  P.s5 = input[(i+1)*width + j-1];
  P.s6 = input[(i+1)*width + j];
  P.s7 = input[(i+1)*width + j+1];

  if (pixel_offset_type == 0)
  {
	  output[3*ptro + r_pos] = (P.s0 + P.s2 + P.s5 + P.s7) / 4.0;
	  output[3*ptro + g_pos] = (P.s1 + P.s3 + P.s4 + P.s6) / 4.0;
	  output[3*ptro + b_pos] = input[ptr];
  }
  else if (pixel_offset_type == 1)
  {
	  output[3*ptro + r_pos] = (P.s1 + P.s6) / 2.0;
	  output[3*ptro + g_pos] = input[ptr];
	  output[3*ptro + b_pos] = (P.s3 + P.s4) / 2.0;
  }
  else if (pixel_offset_type == 2)
  {
	  output[3*ptro + r_pos] = (P.s3 + P.s4) / 2.0;
	  output[3*ptro + g_pos] = input[ptr];
	  output[3*ptro + b_pos] = (P.s1 + P.s6) / 2.0;
  }
  else
  {
	  output[3*ptro + r_pos] = input[ptr];
	  output[3*ptro + g_pos] = (P.s1 + P.s3 + P.s4 + P.s6) / 4.0;
	  output[3*ptro + b_pos] = (P.s0 + P.s2 + P.s5 + P.s7) / 4.0;
  }
  
}

const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_NEAREST;
  
__kernel void  bayer_image( __constant uchar * params,  __read_only image2d_t input, __write_only image2d_t output) 
{
  
  const int i = get_global_id(1); //row
  const int j = get_global_id(0); //column
  const uchar offset_type = params[0];
  const uchar pixel_offset_type = ( (offset_type & 0x02) + (i%2 << 1) + (offset_type + j)%2) %4;
  
  // pixel, p_tl, p_tt, p_tr, p_ll, p_rr, p_bl, p_bb, p_br, , 
  float green, red, blue;
  float4 pixel = read_imagef(input, sampler, (int2)(i,j));

  float4 p_tt = read_imagef(input, sampler, (int2)(i-1,j  ));
  float4 p_bb = read_imagef(input, sampler, (int2)(i+1,j));
  float4 p_ll = read_imagef(input, sampler, (int2)(i  ,j-1));
  float4 p_rr = read_imagef(input, sampler, (int2)(i  ,j+1));
  
  // 0 -> tl
  // 1 -> tt
  // 2 -> tr
  // 3 -> ll
  // 4 -> rr
  // 5 -> bl
  // 6 -> bb
  // 7 -> br
  
  if (pixel_offset_type == 0)
  {
      float4 p_tl = read_imagef(input, sampler, (int2)(i-1,j-1));
      float4 p_tr = read_imagef(input, sampler, (int2)(i-1,j+1));
 	  float4 p_bl = read_imagef(input, sampler, (int2)(i+1,j-1));
 	  float4 p_br = read_imagef(input, sampler, (int2)(i+1,j+1));
      red = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;
      green = (p_tt.s0 + p_ll.s0 + p_rr.s0 + p_bb.s0) / 4.0;
      blue = pixel.s0;
  }
  else if (pixel_offset_type == 1)
  {
      red = (p_tt.s0 + p_bb.s0) / 2.0;
      green = pixel.s0;
      blue = (p_ll.s0 + p_rr.s0) / 2.0;
  }
  else if (pixel_offset_type == 2)
  {
      red = (p_ll.s0 + p_rr.s0) / 2.0;
      green = pixel.s0;
      blue = (p_tt.s0 + p_bb.s0) / 2.0;
  }
  else
  {
      float4 p_tl = read_imagef(input, sampler, (int2)(i-1,j-1));
      float4 p_tr = read_imagef(input, sampler, (int2)(i-1,j+1));
 	  float4 p_bl = read_imagef(input, sampler, (int2)(i+1,j-1));
 	  float4 p_br = read_imagef(input, sampler, (int2)(i+1,j+1));
      red = pixel.s0;
      green = (p_tt.s0 + p_bb.s0 + p_ll.s0 + p_rr.s0) / 4.0;
      blue = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;
  }
  float4 output_pixel;
  output_pixel.s0 = blue;
  output_pixel.s1 = green;
  output_pixel.s2 = red;
//   output_pixel.s0 = 0;
//   output_pixel.s1 = 0;
//   output_pixel.s2 = 0;
  
  write_imagef(output, (int2)(i,j), output_pixel);
}