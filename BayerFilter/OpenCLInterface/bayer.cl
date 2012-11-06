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
  if (i > 0 && j > 0) P[0] = input[(i-1)*width + j -1];
  else P[0] = 0;
  
  if (i > 0) P[1] = input[(i-1)*width + j];
  else P[1] = 0;
  
  if (i > 0 && j+1 < width ) P[2] = input[(i-1)*width + j+1];
  else P[2] = 0;
  
  if (j > 0) P[3] = input[(i)*width + j-1];
  else P[3] = 0;
  
  if (j+1 < width) P[4] = input[(i)*width + j+1];
  else P[4] = 0;
  
  if (i+1 < height && j > 0) P[5] = input[(i+1)*width + j-1];
  else P[5] = 0;
  
  if (i+1 < height ) P[6] = input[(i+1)*width + j];
  else P[6] = 0;
  
  if (i+1 < height && j+1 < width) P[7] = input[(i+1)*width + j+1];
  else P[7] = 0;
  
  
  //Tu obliczenie współczynników P 
  float8 MEANS; 
  MEANS[0] = input[ptr];
  MEANS[1] = (P[0] + P[2] + P[4] + P[6]) / 4; 
  MEANS[2] = (P[1] + P[3] + P[5] + P[7]) / 4; 
  MEANS[3] = (P[3] + P[4]) / 2; 
  MEANS[4] = (P[1] + P[6]) / 2; 
  
  output[3*ptr + r_pos] = MEANS[LUT[3*pixel_offset_type]]; 
  output[3*ptr + g_pos] = MEANS[LUT[3*pixel_offset_type+1]]; 
  output[3*ptr + b_pos] = MEANS[LUT[3*pixel_offset_type+2]]; 
  
}