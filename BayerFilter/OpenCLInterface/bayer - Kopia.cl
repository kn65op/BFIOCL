__kernel void  bayer( __constant uchar * params, __constant uchar * LUT, __global float* input, __global float* output) 
{
  int width = get_global_size(0); 
  int height = get_global_size(1); 
  int j = get_global_id(0); //column number
  int i = get_global_id(1); //row number
  int ptr = i * width + j; 
  uchar offset_type = params[0]; 
  uchar pixel_offset_type = ( (offset_type & 0x02) + (i%2 << 1) + (offset_type + j)%2) %4;
  uchar r_pos = params[1]; 
  uchar g_pos = params[2]; 
  uchar b_pos = params[3]; 
  float8 P; 
  if (i > 0 && j > 0) P.s0 = input[(i-1)*width + j -1];
  else P.s0 = 0;
  
  if (i > 0) P.s1 = input[(i-1)*width + j];
  else P.s1 = 0;
  
  if (i > 0 && j+1 < width ) P.s2 = input[(i-1)*width + j+1];
  else P.s2 = 0;
  
  if (j > 0) P.s3 = input[(i)*width + j-1];
  else P.s3 = 0;
  
  if (j+1 < width) P.s4 = input[(i)*width + j+1];
  else P.s4 = 0;
  
  if (i+1 < height && j > 0) P.s5 = input[(i+1)*width + j-1];
  else P.s5 = 0;
  
  if (i+1 < height ) P.s6 = input[(i+1)*width + j];
  else P.s6 = 0;
  
  if (i+1 < height && j+1 < width) P.s7 = input[(i+1)*width + j+1];
  else P.s7 = 0;
  
  
  //Tu obliczenie współczynników P 
  float8 MEANS; 
  MEANS.s0 = input[ptr];
  MEANS.s1 = (P.s0 + P.s2 + P.s4 + P.s6) / 4; 
  MEANS.s2 = (P.s1 + P.s3 + P.s5 + P.s7) / 4; 
  MEANS.s3 = (P.s3 + P.s4) / 2; 
  MEANS.s4 = (P.s1 + P.s6) / 2; 
  
  output[3*ptr + r_pos] = MEANS[LUT[3*pixel_offset_type]]; 
  output[3*ptr + g_pos] = MEANS[LUT[3*pixel_offset_type+1]]; 
  output[3*ptr + b_pos] = MEANS[LUT[3*pixel_offset_type+2]]; 
  
}