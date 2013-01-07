__kernel void  bayer(__global float* input, __global float* output, __constant uchar * params, __global float* balance) 
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
	  output[3*ptro + r_pos] = (P.s0 + P.s2 + P.s5 + P.s7) / 4.0 * balance[0];
	  output[3*ptro + g_pos] = (P.s1 + P.s3 + P.s4 + P.s6) / 4.0 * balance[1];
	  output[3*ptro + b_pos] = input[ptr] * balance[2];
  }
  else if (pixel_offset_type == 1)
  {
	  output[3*ptro + r_pos] = (P.s1 + P.s6) / 2.0 * balance[0];
	  output[3*ptro + g_pos] = input[ptr] * balance[1];
	  output[3*ptro + b_pos] = (P.s3 + P.s4) / 2.0 * balance[2];
  }
  else if (pixel_offset_type == 2)
  {
	  output[3*ptro + r_pos] = (P.s3 + P.s4) / 2.0 * balance[0];
	  output[3*ptro + g_pos] = input[ptr] * balance[1];
	  output[3*ptro + b_pos] = (P.s1 + P.s6) / 2.0 * balance[2];
  }
  else
  {
	  output[3*ptro + r_pos] = input[ptr] * balance[0];
	  output[3*ptro + g_pos] = (P.s1 + P.s3 + P.s4 + P.s6) / 4.0 * balance[1];
	  output[3*ptro + b_pos] = (P.s0 + P.s2 + P.s5 + P.s7) / 4.0 * balance[2];
  }
  
}

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
    
  if (pixel_offset_type == 0)
  {
      red = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;
      green = (p_tt.s0 + p_ll.s0 + p_rr.s0 + p_bb.s0) / 4.0;
      blue = pixel.s0;
  }
  else if (pixel_offset_type == 1)
  {
      red = (p_tt.s0 + p_bb.s0) / 2.0;
      green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 5.0;
      blue = (p_ll.s0 + p_rr.s0) / 2.0;
  }
  else if (pixel_offset_type == 2)
  {
      red = (p_ll.s0 + p_rr.s0) / 2.0;
      green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 5.0;
      blue = (p_tt.s0 + p_bb.s0) / 2.0;
  }
  else
  {
      red = pixel.s0;
      green = (p_tt.s0 + p_bb.s0 + p_ll.s0 + p_rr.s0) / 4.0;
      blue = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;
  }
  pixel.s0 = blue * balance[2];
  pixel.s1 = green * balance[1];
  pixel.s2 = red * balance[0];
  
  write_imagef(output, (int2)(i,j), pixel);
}

/**
 * bayer_image_circle_mask
 * 
 * Interpolate BayerMatrix output to RGB image using circle mask
 * 
 *   #
 *  ###
 * #####
 *  ###
 *   #
 * 
 */
__kernel void  bayer_image_circle_mask(__read_only image2d_t input, __write_only image2d_t output, __constant uchar * params, __global float* balance) 
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
  
  float4 p_ttt = read_imagef(input, sampler, (int2)(i-2,j  ));
  float4 p_bbb = read_imagef(input, sampler, (int2)(i+2,j));
  float4 p_lll = read_imagef(input, sampler, (int2)(i  ,j-2));
  float4 p_rrr = read_imagef(input, sampler, (int2)(i  ,j+2));
  
  float4 p_tl = read_imagef(input, sampler, (int2)(i-1,j-1));
  float4 p_tr = read_imagef(input, sampler, (int2)(i-1,j+1));
  float4 p_bl = read_imagef(input, sampler, (int2)(i+1,j-1));
  float4 p_br = read_imagef(input, sampler, (int2)(i+1,j+1));
  
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
      red = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;
      green = (p_tt.s0 + p_ll.s0 + p_rr.s0 + p_bb.s0) / 4.0;
      blue = (pixel.s0 + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 5.0;
  }
  else if (pixel_offset_type == 1)
  {
      red = (p_tt.s0 + p_bb.s0) / 2.0;
      green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0  + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 9.0;
      blue = (p_ll.s0 + p_rr.s0) / 2.0;
  }
  else if (pixel_offset_type == 2)
  {
      red = (p_ll.s0 + p_rr.s0) / 2.0;
      green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0  + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 9.0;
      blue = (p_tt.s0 + p_bb.s0) / 2.0;
  }
  else
  {
      red = (pixel.s0 + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 5.0;
      green = (p_tt.s0 + p_bb.s0 + p_ll.s0 + p_rr.s0) / 4.0;
      blue = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;
  }
  pixel.s0 = blue * balance[2];
  pixel.s1 = green * balance[1];
  pixel.s2 = red * balance[0];
  
  write_imagef(output, (int2)(i,j), pixel);
}

/**
 * bayer_image_cross_mask
 * 
 * Interpolate BayerMatrix output to RGB image using cross mask:
 * 
 *  ###
 * #####
 * #####
 * #####
 *  ###
 * 
 */
__kernel void  bayer_image_cross_mask(__read_only image2d_t input, __write_only image2d_t output, __constant uchar * params, __global float* balance) 
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
  
  float4 p_ttt = read_imagef(input, sampler, (int2)(i-2,j  ));
  float4 p_bbb = read_imagef(input, sampler, (int2)(i+2,j));
  float4 p_lll = read_imagef(input, sampler, (int2)(i  ,j-2));
  float4 p_rrr = read_imagef(input, sampler, (int2)(i  ,j+2));
  
  float4 p_tl = read_imagef(input, sampler, (int2)(i-1,j-1));
  float4 p_tr = read_imagef(input, sampler, (int2)(i-1,j+1));
  float4 p_bl = read_imagef(input, sampler, (int2)(i+1,j-1));
  float4 p_br = read_imagef(input, sampler, (int2)(i+1,j+1));
  
  
  float4 p_ttl = read_imagef(input, sampler, (int2)(i-2,j-1));
  float4 p_ttr = read_imagef(input, sampler, (int2)(i-2,j+1));
  
  float4 p_bbl = read_imagef(input, sampler, (int2)(i+2,j-1));
  float4 p_bbr = read_imagef(input, sampler, (int2)(i+2,j+1));
  
  float4 p_llt = read_imagef(input, sampler, (int2)(i-1,j-2));
  float4 p_llb = read_imagef(input, sampler, (int2)(i+1,j-2));

  float4 p_rrt = read_imagef(input, sampler, (int2)(i-1,j+2));
  float4 p_rrb = read_imagef(input, sampler, (int2)(i+1,j+2));
  
  if (pixel_offset_type == 0)
  {
      red = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;
      green = (p_tt.s0 + p_ll.s0 + p_rr.s0 + p_bb.s0 + p_ttl.s0 + p_ttr.s0 + p_bbl.s0 + p_bbr.s0 + p_llt.s0 + p_llb.s0 + p_rrt.s0 + p_rrb.s0) / 12.0;
      blue = (pixel.s0 + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 5.0;
  }
  else if (pixel_offset_type == 1)
  {
      red = (p_tt.s0 + p_bb.s0 + p_llt.s0 + p_llb.s0 + p_rrt.s0 + p_rrb.s0) / 6.0;
      green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0  + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 9.0;
      blue = (p_ll.s0 + p_rr.s0 + p_ttl.s0 + p_ttr.s0 + p_bbl.s0 + p_bbr.s0) / 6.0;
  }
  else if (pixel_offset_type == 2)
  {
      red = (p_ll.s0 + p_rr.s0 + p_ttl.s0 + p_ttr.s0 + p_bbl.s0 + p_bbr.s0) / 6.0;
      green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0  + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 9.0;
      blue = (p_tt.s0 + p_bb.s0 + p_llt.s0 + p_llb.s0 + p_rrt.s0 + p_rrb.s0) / 6.0;
  }
  else
  {
      red = (pixel.s0 + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 5.0;
      green = (p_tt.s0 + p_ll.s0 + p_rr.s0 + p_bb.s0 + p_ttl.s0 + p_ttr.s0 + p_bbl.s0 + p_bbr.s0 + p_llt.s0 + p_llb.s0 + p_rrt.s0 + p_rrb.s0) / 12.0;
      blue = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;
  }
  
  pixel.s0 = blue * balance[2];
  pixel.s1 = green * balance[1];
  pixel.s2 = red * balance[0];
  
  write_imagef(output, (int2)(i,j), pixel);
}
