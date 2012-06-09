/* vim:ft=c
   This is the OpenCL Utility library. It defines a set of functions and C
   macros to make the host side of OpenCL programming less tedious

   Copyright (C) 2011 Giuseppe Bilotta

   See LEGAL for license information and other legalese
 */

#include <unistd.h>

/* Memory functions */

cl_mem
cluMalloc(
	size_t size,
	cl_mem_flags flags,
	void * host_ptr,
	cl_int *errcode_ret)
{
	cl_int error = CL_SUCCESS;
	cl_context ctx = cluGetCurrentContext(&error);
	RETURN_ON_ERROR;
	return clCreateBuffer(ctx, flags, size, host_ptr, errcode_ret);
}

cl_mem
cluMallocPitch(
	size_t *pitch,
	size_t hpitch,
	size_t height,
	cl_mem_flags flags,
	void * host_ptr,
	cl_int *errcode_ret)
{
	size_t size = 0;
	cl_uint align = 0;
	cl_int error = CL_SUCCESS;
	cl_device_id dev;
	clu_devptr devp;
	cl_context ctx = cluGetCurrentContext(&error);
	RETURN_ON_ERROR;
	/* TODO ideally, we should support multiple devices and
	 * find the largest common pitch */
	dev = cluGetCurrentDevice(&error);
	RETURN_ON_ERROR;
	devp = cluGetDeviceByID(dev, &error);
	RETURN_ON_ERROR;
	align = (devp->mem_base_addr_align)/8;
	/* TODO overflow check */
	size = (hpitch + align - 1)/align;
	size *= align;
	*pitch = size;
	size *= height;
	return clCreateBuffer(ctx, flags, size, host_ptr, errcode_ret);
}


static pthread_once_t _pagesize_init_control = PTHREAD_ONCE_INIT;
static size_t _pagesize = 0; 

static 
void 
_initPagesize() {
  _pagesize = sysconf(_SC_PAGESIZE);
}

/**
   Allocate host  memory that is page aligned  (using operating system
   facilities). This does not use OpenCL at all.
 */
void* 
cluHostMallocPageAligned(size_t size) {
  void* ptr;
  (void) pthread_once(&_pagesize_init_control, _initPagesize);

  int err = posix_memalign(&ptr, _pagesize, size);
  if( !err )
    return ptr;
  else
    return NULL;
}

