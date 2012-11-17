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