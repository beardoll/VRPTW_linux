#include "TxtRecorder.h"

string path = "./outputtxt/out7_tight.txt";
ofstream TxtRecorder::outfile(path.c_str(), ofstream::trunc);
