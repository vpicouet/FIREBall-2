#ifndef STACKING_H_
#define STACKING_H_

// definition of structure for stacking.
typedef struct stacking_structure_t{
  unsigned int first_framenumber;
  unsigned int last_framenumber;
  double ra, dec, roll;
  char filename[255];
  char outfile[255];
  char astrometry_mode;
  char astrometry_command_line[255];
  int exptime;
  int buffsize;
  int count;
  int width, height;
  float *buf;
} stacking_structure_t;


namespace stacking{
  //  void save_and_stack(stacking_structure_t *sst);
  void *stack_and_save_thread_worker(void *sst);
}; // namespace stacking


#endif // STACKING_H_
