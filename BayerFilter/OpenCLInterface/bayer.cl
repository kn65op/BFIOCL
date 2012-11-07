__kernel void  bayer( __constant uchar * params, __constant uchar * LUT, __global float* input, __global float* output,
  __local float * P, __local float * MEANS) 
{
  int width_output = get_global_size(0); 
  int width = get_global_size(0) + 2; 

//  int height = get_global_size(1); 
  int j = get_global_id(0); //column number
  int i = get_global_id(1) + 1; //row number
  int iout = get_global_id(1); 
  int ptr = i * width + j; 
  int ptro = iout * width_output+ j; 
  uchar offset_type = params[0]; 
  uchar pixel_offset_type = ( (offset_type & 0x02) + (i%2 << 1) + (offset_type + j)%2) %4;
  uchar r_pos = params[1]; 
  uchar g_pos = params[2]; 
  uchar b_pos = params[3]; 
  
  P[0] = input[(i-1)*width + j -1];
  
  P[1] = input[(i-1)*width + j];
  
  P[2] = input[(i-1)*width + j+1];
  
  P[3] = input[(i)*width + j-1];
  
  P[4] = input[(i)*width + j+1];
  
  P[5] = input[(i+1)*width + j-1];
  
  P[6] = input[(i+1)*width + j];
  
  P[7] = input[(i+1)*width + j+1];
  
  //Tu obliczenie współczynników P 
  MEANS[0] = input[ptr];
  MEANS[1] = (P[0] + P[2] + P[5] + P[7]) / 4.0; 
  MEANS[2] = (P[1] + P[3] + P[4] + P[6]) / 4.0; 
  MEANS[3] = (P[3] + P[4]) / 2.0; 
  MEANS[4] = (P[1] + P[6]) / 2.0; 
  
  output[3*ptro + r_pos] = MEANS[LUT[3*pixel_offset_type]]; 
  output[3*ptro + g_pos] = MEANS[LUT[3*pixel_offset_type+1]]; 
  output[3*ptro + b_pos] = MEANS[LUT[3*pixel_offset_type+2]]; 
  
}