#include <opencl.h>

/**
 * Function prints on standard output list of all OpenCL devices and platforms.
 */
void list_devices();

/**
 * Functions return std::string with human readable OpenCl's image channel format.
 * @param image cl_int which image channel format.
 * @return std::string with name of image channel format.
 */
std::string getImageChannelFormat (cl_int image);

/**
 * Functions return std::string with human readable OpenCl's image channel order.
 * @param image cl_int which image channel order.
 * @return std::string with name of image channel order.
 */
std::string getImageChannelOrder (cl_int image);

/**
 * Function prints on standard output list of all supported OpenCL image formats.
 */
void list_supported_image_formats ();