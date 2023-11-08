#include "archlab.hpp"
#include "CNN/canela.hpp"
#include "math.h"
#include "pin_tags.h"
#include <functional>
#include <fstream>      // std::fstream
std::fstream trace;

#define MAX_OFFSET 8


// Some macros to reduce repetition below.  The last argument is
// 'true' so that we will get a new tag each time call DUMP_START.  In
// our case, this will let us zoom in on one iteration of the outer
// loop.
#define DUMP_START_TENSOR(TAG, T)  DUMP_START( TAG, (void *) &(T.data[0]), (void *) &(T.data[T.element_count() - 1]), true)
#define DUMP_STOP_TENSOR(TAG) DUMP_STOP( TAG)

void do_stabilize_baseline(const tensor_t<double> & images, tensor_t<double> & output, int TILE_SIZE)
{

	// The interesting part starts here.

	// We iterate over each frame and compare it to the previous
	// one (so `this_frame` starts at 1)
	//
	// Frames are identified by their index in the batch.  The
	// index is the `b` dimension of the tensor, which always
	// appears last when we access elements of the tensor.

	for (int this_frame = 1; this_frame < images.size.b; this_frame++) {
		int previous_frame = this_frame - 1;

		// We will shift around the previous frame relative to
		// the current frame and compute the "sum of absolute
		// differences" for each different shift amount.

		// Here we are shifting by up to MAX_OFFSET pixels up, down,
		// left, and right.
		for (int offset_x = 0; offset_x < MAX_OFFSET; offset_x++)  {
			for (int offset_y = 0; offset_y < MAX_OFFSET; offset_y++)  {

				// Iterate over the pixels in the two
				// images.  `pixel_x` and `pixel_y`
				// will be use for the current frame.
				for(int pixel_x = 0; pixel_x < images.size.x; pixel_x++) {
					for(int pixel_y = 0; pixel_y < images.size.y; pixel_y++) {

						int shifted_x = pixel_x + offset_x;  // pixel location in the shifted previous frame
						int shifted_y = pixel_y + offset_y;

						if (shifted_x >= images.size.x || // Bounds check.
						    shifted_y >= images.size.y)
							continue;

						// calculate and accumulate the difference between the images at this shifting amount.
						// We add MAX_OFFSET because the offsets can be negative.

						output(offset_x, offset_y, 0, this_frame) += 
							fabs(images(pixel_x, pixel_y, 0, this_frame) -
							     images(shifted_x, shifted_y, 0, previous_frame));
					}
				}
			}
		}
		// Close the tags.  This is not strictly necessary,
		// but if you don't the first tag will have all the
		// iterations.  The second tag will have all but the
		// first, etc. 
	}

}

void do_stabilize_reorder_pixelxy(const tensor_t<double> & images, tensor_t<double> & output, int TILE_SIZE)
{

	for (int this_frame = 1; this_frame < images.size.b; this_frame++) {
		int previous_frame = this_frame - 1;
		for (int offset_x = 0; offset_x < MAX_OFFSET; offset_x++)  {
			for (int offset_y = 0; offset_y < MAX_OFFSET; offset_y++)  {

				for(int pixel_y = 0; pixel_y < images.size.y; pixel_y++) {
					for(int pixel_x = 0; pixel_x < images.size.x; pixel_x++) {

						int shifted_x = pixel_x + offset_x; 
						int shifted_y = pixel_y + offset_y;

						if (shifted_x >= images.size.x || 
						    shifted_y >= images.size.y)
							continue;
						output(offset_x, offset_y, 0, this_frame) += 
							fabs(images(pixel_x, pixel_y, 0, this_frame) -
							     images(shifted_x, shifted_y, 0, previous_frame));
					}
				}
			}
		}
	}
}

//#define TILE_SIZE 2
void do_stabilize_pretile_y(const tensor_t<double> & images, tensor_t<double> & output, int TILE_SIZE)
{

	for (int this_frame = 1; this_frame < images.size.b; this_frame++) {
		int previous_frame = this_frame - 1;

		for (int offset_x = 0; offset_x < MAX_OFFSET; offset_x++)  {
			for (int offset_y = 0; offset_y < MAX_OFFSET; offset_y++)  {
				
				for(int pixel_yy = 0; pixel_yy < images.size.y; pixel_yy += TILE_SIZE) {
					for(int pixel_y = pixel_yy; pixel_y < pixel_yy + TILE_SIZE && pixel_y < images.size.y; pixel_y++) {
						for(int pixel_x = 0; pixel_x < images.size.x; pixel_x++) {
							
							int shifted_x = pixel_x + offset_x; 
							int shifted_y = pixel_y + offset_y;
							
							if (shifted_x >= images.size.x ||
							    shifted_y >= images.size.y)
								continue;
							output(offset_x, offset_y, 0, this_frame) += 
								fabs(images(pixel_x, pixel_y, 0, this_frame) -
								     images(shifted_x, shifted_y, 0, previous_frame));

						}
					}
				}
			}
		}

	}
}

void do_stabilize_tile_y_1(const tensor_t<double> & images, tensor_t<double> & output, int TILE_SIZE)
{
	for (int this_frame = 1; this_frame < images.size.b; this_frame++) {
		int previous_frame = this_frame - 1;

		for(int pixel_yy = 0; pixel_yy < images.size.y; pixel_yy +=  TILE_SIZE) {

			for (int offset_x = 0; offset_x < MAX_OFFSET; offset_x++)  {
				for (int offset_y = 0; offset_y < MAX_OFFSET; offset_y++)  {
	
					for(int pixel_y = pixel_yy; pixel_y < pixel_yy + TILE_SIZE && pixel_y < images.size.y; pixel_y++) {
						for(int pixel_x = 0; pixel_x < images.size.x; pixel_x++) {
							
							int shifted_x = pixel_x + offset_x; 
							int shifted_y = pixel_y + offset_y;
							
							if (shifted_x >= images.size.x ||
							    shifted_y >= images.size.y)
								continue;

							output(offset_x, offset_y, 0, this_frame) += 
								fabs(images(pixel_x, pixel_y, 0, this_frame) -
								     images(shifted_x, shifted_y, 0, previous_frame));
						}
					}
				}
			}
		}
	}
}



void do_stabilize_innerloop_offsets(const tensor_t<double> & images, tensor_t<double> & output, int TILE_SIZE)
{
	for (int this_frame = 1; this_frame < images.size.b; this_frame++) {
		int previous_frame = this_frame - 1;

		for(int pixel_y = 0; pixel_y < images.size.y; pixel_y++) {
			for(int pixel_x = 0; pixel_x < images.size.x; pixel_x++) {

				for (int offset_x = 0; offset_x < MAX_OFFSET; offset_x++)  {
					for (int offset_y = 0; offset_y < MAX_OFFSET; offset_y++)  {
						

						int shifted_x = pixel_x + offset_x; 
						int shifted_y = pixel_y + offset_y;

						if (shifted_x >= images.size.x ||
						    shifted_y >= images.size.y)
							continue;
						output(offset_x, offset_y, 0, this_frame) += 
							fabs(images(pixel_x, pixel_y, 0, this_frame) -
							     images(shifted_x, shifted_y, 0, previous_frame));
					}
				}
			}
		}
	}

}

void do_stabilize_tile_y_1_omp_simple(const tensor_t<double> & images, tensor_t<double> & output, int TILE_SIZE)
{
	// parallizing across results in no sharing in `output` since
	// each frame output it's result to one element of `output`.
#pragma omp parallel for
	for (int this_frame = 1; this_frame < images.size.b; this_frame++) {
		int previous_frame = this_frame - 1;
		for(int pixel_yy = 0; pixel_yy < images.size.y; pixel_yy +=  TILE_SIZE) {

			for (int offset_x = 0; offset_x < MAX_OFFSET; offset_x++)  {
				for (int offset_y = 0; offset_y < MAX_OFFSET; offset_y++)  {
	
					for(int pixel_y = pixel_yy; pixel_y < pixel_yy + TILE_SIZE && pixel_y < images.size.y; pixel_y++) {
						for(int pixel_x = 0; pixel_x < images.size.x; pixel_x++) {
							
							int shifted_x = pixel_x + offset_x; 
							int shifted_y = pixel_y + offset_y;
							
							if (shifted_x >= images.size.x ||
							    shifted_y >= images.size.y)
								continue;
							double t = fabs(images(pixel_x, pixel_y, 0, this_frame) -
									images(shifted_x, shifted_y, 0, previous_frame));
							output(offset_x, offset_y, 0, this_frame) += t;
						}
					}
				}
			}
		}
	}
}
void do_stabilize_tile_y_1_omp_critical(const tensor_t<double> & images, tensor_t<double> & output, int TILE_SIZE)
{
	for (int this_frame = 1; this_frame < images.size.b; this_frame++) {
		int previous_frame = this_frame - 1;

		// Parallelizing on pixel_yy, results in sharing in output.
#pragma omp parallel for
		for(int pixel_yy = 0; pixel_yy < images.size.y; pixel_yy +=  TILE_SIZE) {

			for (int offset_x = 0; offset_x < MAX_OFFSET; offset_x++)  {
				for (int offset_y = 0; offset_y < MAX_OFFSET; offset_y++)  {
	
					for(int pixel_y = pixel_yy; pixel_y < pixel_yy + TILE_SIZE && pixel_y < images.size.y; pixel_y++) {
						for(int pixel_x = 0; pixel_x < images.size.x; pixel_x++) {
							
							int shifted_x = pixel_x + offset_x; 
							int shifted_y = pixel_y + offset_y;
							
							if (shifted_x >= images.size.x ||
							    shifted_y >= images.size.y)
								continue;
							double t = fabs(images(pixel_x, pixel_y, 0, this_frame) -
									images(shifted_x, shifted_y, 0, previous_frame));

							// So you have to use omp critical to protect the update to output.  The overhead kills us -- massive slow down.
							#pragma omp critical
							{
								output(offset_x, offset_y, 0, this_frame) += t;
							}
						}
					}
				}
			}
		}
	}
}

void do_stabilize_tile_y_1_omp_critical_fast(const tensor_t<double> & images, tensor_t<double> & output, int TILE_SIZE)
{
	for (int this_frame = 1; this_frame < images.size.b; this_frame++) {
		int previous_frame = this_frame - 1;
		// same thing: need to protect 
#pragma omp parallel for 
		for(int pixel_yy = 0; pixel_yy < images.size.y; pixel_yy +=  TILE_SIZE) {
			tensor_t<double>_output(output.size);
			_output.clear();
			for (int offset_x = 0; offset_x < MAX_OFFSET; offset_x++)  {
				for (int offset_y = 0; offset_y < MAX_OFFSET; offset_y++)  {
						
					for(int pixel_y = pixel_yy; pixel_y < pixel_yy + TILE_SIZE && pixel_y < images.size.y; pixel_y++) {
						for(int pixel_x = 0; pixel_x < images.size.x; pixel_x++) {
								
							int shifted_x = pixel_x + offset_x; 
							int shifted_y = pixel_y + offset_y;
								
							if (shifted_x >= images.size.x ||
							    shifted_y >= images.size.y)
								continue;

							double t = fabs(images(pixel_x, pixel_y, 0, this_frame) -
									images(shifted_x, shifted_y, 0, previous_frame));
							// accumulate the updates locally
							_output(offset_x, offset_y, 0, this_frame) += t;
						}
					}
				}
			}
#pragma omp critical // Apply them en masse this is reasonably fast because it's small, so the serialization isn't a big deal.
			{
				for (int offset_y = 0; offset_y < MAX_OFFSET; offset_y++)  {
					for (int offset_x = 0; offset_x < MAX_OFFSET; offset_x++)  {
						output(offset_x, offset_y, 0, this_frame) += _output(offset_x, offset_y, 0, this_frame);
					}
				}
				
			}
		}
	}
}



 
void stabilize(const std::string & version, const dataset_t & test, int frames, int tile_size)
{

        // Declare a 4D tensor to hold frames video frames
	tensor_t<double> batch_data(tdsize(test.data_size.x, test.data_size.y, test.data_size.z, frames));

	int batch_index = 0;
	// Copy the frames video frames into batch_data.  This a bit
	// strange because, the dataset we want might not have enough
	// frames.  So, we copy the dataset over and over until we
	// have enough data.
	while(batch_index < frames) {
		for (auto& t : test.test_cases ) {
			for (int x = 0; x < t.data.size.x; x += 1)
				for (int y = 0; y < t.data.size.y; y += 1){
					int z= 0; // `test` might hold color
					// images so the z dimension
					// would be 3 (RGB).  We are
					// only doing gray scale, so
					// we just take layer 0 (R
					// for RGB images and gray
					// for grayscale).
					batch_data(x, y, z, batch_index) = t.data(x, y, z);
				}
			batch_index += 1;
			if (batch_index >= frames) {
				break;
			}
		}
	}

	tensor_t<double>::diff_prints_deltas = true;
 
	// Tensor to hold the outputs
	tensor_t<double> output(MAX_OFFSET,MAX_OFFSET,1,batch_data.size.b);
	
	bool verbose = true;
	
	std::map<const std::string, void(*)(const tensor_t<double> &, tensor_t<double> &, int)>
		impl_map =
		{
#define IMPL(n) {#n, do_stabilize_##n}
			IMPL(baseline),
			IMPL(reorder_pixelxy),
			IMPL(pretile_y),
			IMPL(tile_y_1),
			IMPL(innerloop_offsets), 
			IMPL(tile_y_1_omp_simple),
			IMPL(tile_y_1_omp_critical),
			IMPL(tile_y_1_omp_critical_fast)
		};

	
	START_TRACE();  // Turn in Moneta Tracing.  Nothing wil get recorded before this.
	do_stabilize_baseline(batch_data, output, 1);
	tensor_t<double> reference = output;

	for(auto &i: impl_map) {
		if (version == i.first || version == "all") {
			std::cerr << "Running " << i.first << "\n";
			{
				output.clear();
				ArchLabTimer timer;
				pristine_machine();
				theDataCollector->disable_prefetcher();
				set_cpu_clock_frequency(1900);
				timer.attr("function", i.first).go();
				DUMP_START_TENSOR("images", batch_data);
				DUMP_START_TENSOR("output", output);
				i.second(batch_data, output, tile_size);
				DUMP_STOP_TENSOR("images");
				DUMP_STOP_TENSOR("output");
			}
			
			if (output != reference) {
				if (verbose) {
					std::cout << output <<"\n";
					std::cout << diff(output, reference)<< "\n";
				}
				assert(0);
			}
			
			output.clear();
		}
	}
}

