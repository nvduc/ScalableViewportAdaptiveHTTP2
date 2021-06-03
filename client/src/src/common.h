
#ifndef __GA_COMMON_H__
#define __GA_COMMON_H__
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <string>
#include <thread>
#include <tuple>
using namespace std;
class ViewportPos
{
  public:
    int phi;
    int theta;
    ViewportPos()
    {
      phi = 0;
      theta = 0;
    }
    ViewportPos(int phi1, int theta1)
    {
      phi = phi1;
      theta = theta1;
    }
    bool operator<(const ViewportPos& vpObj) const
    {
      /*
      if (vpObj.phi < this->phi || (vpObj.phi == this->phi && vpObj.theta < this->theta))
        return true;
      */
      return tie(phi, theta) < tie(vpObj.phi, vpObj.theta);
    }
    ~ViewportPos(){};
    friend ostream& operator<<(ostream& out, const ViewportPos& data);
};
extern map<ViewportPos, vector<ViewportPos>> visiViewportList; // containing list of viewports from which a point can be seen
map<ViewportPos, vector<ViewportPos>> calc_visi_viewport(int FoV);
extern int** visiVPList;
int** calc_visi_vp(int FoV);
//
double get_ang_dist(int phi1, int theta1, int phi2, int theta2);
long long tvdiff_us(struct timeval *tv1, struct timeval *tv2);
long long ga_usleep(long long interval, struct timeval *ptv);
int*** init3dArrayInt(int, int , int);
int** init2dArrayInt(int, int);
double*** init3dArrayDouble(int, int , int);
double** init2dArrayDouble(int, int);
double**** init4dArrayDouble(int, int , int, int);
int**** init4dArrayInt(int, int , int, int);
double avg(double* , int);
double sum(double* , int);
void get_face_tid(int No_face, int No_tile_h, int No_tile_v, int tid, int* face_id, int* tile_id);
void showArrayInt(int *arr, int N);
void showArrayDouble(double *arr, int N);
void showTileVersion(int* tile_ver, int No_tile_h, int No_tile_v);
string showTileVersionInSeg(int** tile_ver, int NO_FRAME, int No_tile_h, int No_tile_v);
void showTileInfo(double* info, int No_tile_h, int No_tile_v);
void getErrorPoint(int phi, int theta, int err, int point_id, int No_point, int* phi_est, int *theta_est);
FILE* open_file(char*);
double min(double, double);
double max(double, double);
int min(int, int);
int max(int, int);
void norm_vp_range(int*);
bool compare_string(string, string);
int median(vector<int>);
int median(vector<int>, int N);
double average(vector<int>);
double average(vector<int>, int N);
double stddev(vector<int>);
int maximum(vector<int>);
int maximum(vector<int>, int N);
double PSNR2MSE(double);
double MSE2PSNR(double);
double* convert2Dto1D(double** a, int m, int n);
// 
struct run_cfg
{
  std::vector<int> INTER_LIST;
  std::vector<int> BUFF_LIST;
  std::vector<int> BW_LIST;
  std::vector<int> METHOD_LIST;
  std::vector<int> HEADTRACE_LIST;
  std::vector<int> BWTRACE_LIST;
  std::vector<int> VP_EST_METHOD_LIST;
  std::vector<int> PHI;
  std::vector<int> THETA;
  std::vector<int> SPEED;
  std::vector<int> DASH_VER;
  std::vector<int> ERR;
  int BW_NUM;
  int METHOD_NUM;
  int INTER_NUM;
  int BUFF_NUM;
  int HEADTRACE_NUM;
  int VP_EST_METHOD_NUM;
  int NO_SEG;
  int VP_MODE;
  int DASH_VER_NUM;
  int ERR_NUM;
  int BWTRACE_NUM;
};
run_cfg load_run_cfg(const char* );
#endif



