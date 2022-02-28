#include "pfb.h"

#define SAMPLING_RATE (1e8)
#define NFFT (2048)
#define NTAPS (8)

int main() {
  PolyphaseFilterBank P(SAMPLING_RATE, NFFT, NTAPS, None);
  return 0;
}
