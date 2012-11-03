__kernel void  invert( __global float* input, __global float* output)
{ 
  int i = get_global_id(0);
  output[i] = 1 - input[i];
}
