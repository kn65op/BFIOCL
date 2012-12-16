const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;


__kernel void  intToFloatOneChannel(__read_only image2d_t input, __write_only image2d_t output) 
{
  
  const int i = get_global_id(0); //row
  const int j = get_global_id(1); //column

  uint4 pixel = read_imageui(input, sampler, (int2)(i,j));

  float4 out;
  out.s0 = pixel.s0 / 255.0;
  
  write_imagef(output, (int2)(i,j), out);
}

__kernel void  floatToUInt8ThreeChannels(__read_only image2d_t input, __write_only image2d_t output) 
{
  
  const int i = get_global_id(0); //row
  const int j = get_global_id(1); //column

  float4 pixel = read_imagef(input, sampler, (int2)(i,j));

  uint4 out = convert_uint4(pixel * 255.0);
  
  write_imageui(output, (int2)(i,j), out);
}

__kernel void  floatToUInt16ThreeChannels(__read_only image2d_t input, __write_only image2d_t output) 
{
  
  const int i = get_global_id(0); //row
  const int j = get_global_id(1); //column

  float4 pixel = read_imagef(input, sampler, (int2)(i,j));

  uint4 out = convert_uint4(pixel * 511.0);
  
  write_imageui(output, (int2)(i,j), out);
}

__kernel void  floatToUInt32ThreeChannels(__read_only image2d_t input, __write_only image2d_t output) 
{
  
  const int i = get_global_id(0); //row
  const int j = get_global_id(1); //column

  float4 pixel = read_imagef(input, sampler, (int2)(i,j));

  uint4 out = convert_uint4(pixel * 1023.0);
  
  write_imageui(output, (int2)(i,j), out);
}