#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#ifndef WIN32
#ifndef ANDROID
#include <execinfo.h>
#endif /* !ANDROID */
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/syscall.h>
#endif /* !WIN32 */
#ifdef ANDROID
#include <android/log.h>
#endif /* ANDROID */
#ifdef __APPLE__
#include <syslog.h>
#endif

#if !defined(WIN32) && !defined(__APPLE__) && !defined(ANDROID)
#include <X11/Xlib.h>
#endif

#include <utility>
#include <sys/time.h>
#include <time.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

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

ostream& operator<<(ostream& out, const ViewportPos& data){
  out << "(" << data.phi << "," << data.theta << ")";
  return out;
}
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


struct Tile
{
  int fid; /* face id */
  int tid_h; /* Tile id in horizontal axis */
  int tid_w; /* Tile id in vertical axis */
};

// tile position structure
struct Pos {
  int i;
  int j;
};


int* getAdjacentTileCMP(int NO_TILE_H, int NO_TILE_W, int tid, int* N);
int* getAdjacentTileERP2(int NO_TILE_H, int NO_TILE_W, int tid, int* N);
int* getAdjacentTileERP(int NO_TILE_H, int NO_TILE_W, int tid, int* N);
int* getAdjacentTile(int NO_FACE, int NO_TILE_H, int NO_TILE_W, int tid, int *N);
int* extVmask(int* vmask,int NO_FACE, int NO_TILE_H, int NO_TILE_W, int ext_width);
int* extVmask2(int* vmask,int NO_FACE, int NO_TILE_H, int NO_TILE_W, int ext_width);
void showVmask(int* vmask, int NO_FACE, int NO_TILE_H, int NO_TILE_W);
void showArray(int *arr, int N);
Tile* get_adj_tile_in_face(int NO_TILE_H, int NO_TILE_W, int tid_w, int tid_h, int* N);

struct Pos TOP(int i, int j, int M, int N);
struct Pos DOWN(int i, int j, int M, int N);
struct Pos LEFT(int i, int j, int M, int N);
struct Pos RIGHT(int i, int j, int M, int N);
struct Pos TOP_LEFT(int i, int j, int M, int N);
struct Pos TOP_RIGHT(int i, int j, int M, int N);
struct Pos DOWN_LEFT(int i, int j, int M, int N);
struct Pos DOWN_RIGHT(int i, int j, int M, int N);



//
int** calc_visi_vp(int FoV){
  int PHI_NUM = 360;
  int THETA_NUM = 181;
  int VP_NUM = PHI_NUM * THETA_NUM, phi, theta, phi2, theta2;
  int** visi_vp = init2dArrayInt(VP_NUM, VP_NUM);
  for(phi=-179; phi <= 180; phi++){
    for(theta = -90; theta <= 90; theta ++){
      for(phi2=-179; phi2 <= 180; phi2++){
        for(theta2 = -90; theta2 <= 90; theta2 ++){
          if( acos(sin(theta*M_PI/180) * sin(theta2*M_PI/180) + cos(theta*M_PI/180) * cos(theta2*M_PI/180) * cos(abs(phi - phi2)*M_PI/180)) / M_PI * 180 <= FoV){
            visi_vp[(phi+179)*181 + theta + 90][(phi2+179)*181 + theta2 + 90] = 1; 
          }else{
            visi_vp[(phi+179)*181 + theta + 90][(phi2+179)*181 + theta2 + 90] = 0; 
          }
        }
      }
    }
  }
  return visi_vp;
}
map<ViewportPos, vector<ViewportPos>> calc_visi_viewport(int FoV){
  int phi, theta, phi2, theta2, cnt;
  ofstream datfile;
  string fname;
  ViewportPos p,q;
  vector<ViewportPos> p_list;
  vector<ViewportPos> p_list_last;
  vector<ViewportPos> diff(100000);
  vector<ViewportPos>::iterator it;
  struct timeval t1, t2;
  cout << "[calc_visi_viewport] Started " << endl;
  // map
  map<ViewportPos, vector<ViewportPos>> b;
  gettimeofday(&t1, NULL); 
  cnt = 0;
  for(phi=-179; phi <= 180; phi++){
    for(theta = -90; theta <= 90; theta ++){
      cout << phi << "," << theta << endl;
      //fname = "data/visi_viewport/visi_vp_" + to_string(phi+179) + "_" + to_string(theta + 90) + ".txt";
      //cout << fname << endl;
      //datfile.open(fname);
      p = ViewportPos(phi, theta);
      p_list.clear();
      for(phi2=-179; phi2 <= 180; phi2++){
        for(theta2 = -90; theta2 <= 90; theta2 ++){
          //if(get_ang_dist(phi, theta, phi2, theta2) <= FoV){
          if(acos(sin(theta*M_PI/180) * sin(theta2*M_PI/180) + cos(theta*M_PI/180) * cos(theta2*M_PI/180) * cos(abs(phi - phi2)*M_PI/180)) / M_PI * 180 <= FoV){
            //datfile << phi2 << "," << theta2 << endl;
            q = ViewportPos(phi2, theta2);
            p_list.push_back(q);
          }
        }
      }
      // store only differnt values
      if(cnt % 2 == 1){
       // sort last and current vector
       cout << "Sorting array ..." << endl;
       sort(p_list_last.begin(), p_list_last.end());
       sort(p_list.begin(), p_list.end());
       it=set_difference(p_list_last.begin(), p_list_last.end(), p_list.begin(), p_list.end(), diff.begin());
       diff.resize(it - diff.begin());
       cout << "p_list:" << p_list.size() << "p_list_last:" << p_list_last.size() << "diff:" << diff.size() <<  endl;
      }else{
        b.insert(std::pair<ViewportPos,vector<ViewportPos>> (p,p_list));
      }
      p_list_last = p_list;
      cout << "p_list:" << p_list.size() << ", bsize=" << sizeof(b) << endl;
      cnt ++;
    }
  }
  gettimeofday(&t2, NULL);
  cout << "Time: " << (t2.tv_sec - t1.tv_sec)/1000.0 << "(s)" << endl;
  exit(1);
  return b;
}
//
double get_ang_dist(int phi1, int theta1, int phi2, int theta2){
  return acos(sin(theta1*M_PI/180) * sin(theta2*M_PI/180) + cos(theta1*M_PI/180) * cos(theta2*M_PI/180) * cos(abs(phi1 - phi2)*M_PI/180)) / M_PI * 180;
}
double* convert2Dto1D(double** a, int m, int n){
  double* ret = new double[m*n];
  int i,j;
  for(i=0; i < m; i++)
    for(j=0; j < n; j++)
      ret[i*n + j] = a[i][j];
  return ret;
}
double PSNR2MSE(double psnr){
  return 255.0*255.0/pow(10,psnr/10);
}
double MSE2PSNR(double mse){
  return 10 * log10(255.0*255.0/mse);
}
int maximum(vector<int> scores, int size){
  if (size == 0)
  {
    return 0;  // Undefined, really.
  }
  else
  {
    sort(scores.begin(), scores.begin()+size);
    return scores[size-1];    
  }
}
int maximum(vector<int> scores){
  size_t size = scores.size();

  if (size == 0)
  {
    return 0;  // Undefined, really.
  }
  else
  {
    sort(scores.begin(), scores.end());
    return scores[size-1];    
  }
}
double average(vector<int> scores){
  double ret = 0;
  for(int i=0; i < scores.size(); i++)
   ret += scores.at(i) * 1.0 / scores.size(); 
  return ret;
}
double stddev(vector<int> a){
  double mean = average(a);
  double std = 0;
  for(int i=0; i < a.size(); i++)
    std += 1.0/(a.size()-1)*(a.at(i) - mean)*(a.at(i) - mean); 
  std = sqrt(std);
  return std;
}
double average(vector<int> scores, int N){
  double ret = 0;
  for(int i=0; i < N; i++)
   ret += scores.at(i) * 1.0 / N; 
  return ret;
}
int median(vector<int> scores, int size){
  if (size == 0)
  {
    return 0;  // Undefined, really.
  }
  else
  {
    sort(scores.begin(), scores.begin()+size);
    if (size % 2 == 0)
    {
      return (scores[size / 2 - 1] + scores[size / 2]) / 2;
    }
    else 
    {
      return scores[size / 2];
    }
  }
}

int median(vector<int> scores){
  size_t size = scores.size();

  if (size == 0)
  {
    return 0;  // Undefined, really.
  }
  else
  {
    sort(scores.begin(), scores.end());
    if (size % 2 == 0)
    {
      return (scores[size / 2 - 1] + scores[size / 2]) / 2;
    }
    else 
    {
      return scores[size / 2];
    }
  }
}
bool compare_string(string a, string b){
  if(a.compare(8,1,b,8,1) != 0){
    if(a.compare(b) <= 0)
      return true;
    return false;
  }
  if(a.size() == b.size()){
    if(a.compare(b) <= 0)
      return true;
    return false;
  }
  if(a.size() < b.size())
    return true;
  return false;
}
void norm_vp_range(int* norm_vp){
  while(*norm_vp >= 180)
    *norm_vp -= 360;
  while(*norm_vp < -180)
    *norm_vp += 360;
  while(*(norm_vp+1) >= 90)
    *(norm_vp+1) -= 90;
  while(*(norm_vp+1) <= -90)
    *(norm_vp+1) += 90;
}
double max(double a, double b){
  if(a >= b) return a;
  return b;
}
double min(double a, double b){
  if(a <= b) return a;
  return b;
}
int min(int a, int b){
  if(a <= b) return a;
  return b;
}
int max(int a, int b){
  if(a >= b) return a;
  return b;
}/**
 * Compute the time difference for two \a timeval data structure, i.e.,
 * \a tv1 - \a tv2.
 *
 * @param tv1 [in] Pointer to the first \a timeval data structure.
 * @param tv2 [in] Pointer to the second \a timeval data structure.
 * @return The difference in micro seconds.
 */
long long
tvdiff_us(struct timeval *tv1, struct timeval *tv2) {
	struct timeval delta;
	delta.tv_sec = tv1->tv_sec - tv2->tv_sec;
	delta.tv_usec = tv1->tv_usec - tv2->tv_usec;
	if(delta.tv_usec < 0) {
		delta.tv_sec--;
		delta.tv_usec += 1000000;
	}
	return 1000000LL*delta.tv_sec + delta.tv_usec;
}

/**
 * Sleep and wake up at \a ptv + \a interval (micro seconds).
 *
 * @param interval [in] The expected sleeping time (in micro seconds).
 * @param ptv [in] Pointer to the baseline time.
 * @return Currently always return 0.
 *
 * This function is useful for controlling precise sleeps.
 * We usually have to process each video frame in a fixed interval.
 * Each time interval includes the processing time and the sleeping time.
 * However, the processing time could be different in each iteration, so
 * the sleeping time has to be adapted as well.
 * To achieve the goal, we have to obtain the baseline time \a ptv
 * (using \a gettimeofday function)
 * \em before the processing task and call this function \em after
 * the processing task. In this case, the \a interval is set to the total
 * length of the interval, e.g., 41667 for 24fps video.
 *
 * This function sleeps for \a interval micro seconds if the baseline
 * time is not specified.
 */
long long
ga_usleep(long long interval, struct timeval *ptv) {
	long long delta;
	struct timeval tv;
	if(ptv != NULL) {
		gettimeofday(&tv, NULL);
		delta = tvdiff_us(&tv, ptv);
		if(delta >= interval) {
			usleep(1);
			return -1;
		}
		interval -= delta;
	}
	usleep(interval);
	return 0LL;
}
double**** init4dArrayDouble(int M, int N, int P, int Q){
  double**** ret;
  int i,j,k;
  ret = new double***[M];
  for(i=0; i < M; i++){
    ret[i] = new double**[N];
    for(j=0; j < N; j++){
      ret[i][j] = new double*[P];
      for(k=0; k < P; k++)
        ret[i][j][k] = new double[Q];
    }
  }
  return ret;
}
int**** init4dArrayInt(int M, int N, int P, int Q){
  int**** ret;
  int i,j,k;
  ret = new int***[M];
  for(i=0; i < M; i++){
    ret[i] = new int**[N];
    for(j=0; j < N; j++){
      ret[i][j] = new int*[P];
      for(k=0; k < P; k++)
        ret[i][j][k] = new int[Q];
    }
  }
  return ret;
}
int*** init3dArrayInt(int M, int N, int P){
	int*** ret;
	int i,j;
	ret = new int**[M];
	for(i=0; i < M; i++){
		ret[i] = new int*[N];
		for(j=0; j < N; j++)
			ret[i][j] = new int[P];
	}
	return ret;
}
int** init2dArrayInt(int M, int N){
	int** ret;
	int i,j;
	ret = new int*[M];
	for(i=0; i < M; i++){
		ret[i] = new int[N];
	}
	return ret;
}
double*** init3dArrayDouble(int M, int N, int P){
	double*** ret;
	int i,j;
	ret = new double**[M];
	for(i=0; i < M; i++){
		ret[i] = new double*[N];
		for(j=0; j < N; j++)
			ret[i][j] = new double[P];
	}
	return ret;
}
double** init2dArrayDouble(int M, int N){
	double** ret;
	int i,j;
	ret = new double*[M];
	for(i=0; i < M; i++){
		ret[i] = new double[N];
	}
	return ret;
}
double avg(double* a, int N){
	double avg=0;
	for(int i=0; i < N; i++)
		avg += a[i] / N;
	return avg;
}
double sum(double* a, int N){
	double sum=0;
	for(int i=0; i < N; i++)
		sum += a[i];
	return sum;
}
void get_face_tid(int No_face, int No_tile_h, int No_tile_v, int tid, int* face_id, int* tile_id){
	if(No_face == 1 || No_face == 6){
		*face_id = tid / (No_tile_h * No_tile_v);
		*tile_id = tid - (*face_id) * No_tile_h * No_tile_v;
	}
	if(No_face == 2){
		*face_id = 0;
		*tile_id = tid;
	}
}
void showArrayInt(int *arr, int N){
	int i;
	for(i=0; i < N; i++){
		printf("%d, ", arr[i]);
	}
	printf("\n");
}
void showArrayDouble(double *arr, int N){
	int i;
	for(i=0; i < N; i++){
		printf("%.2f, ", arr[i]);
	}
	printf("\n");
}
FILE* open_file(char* fileName){
	FILE* f = NULL;
	f = fopen(fileName, "w");
	if(f == NULL){
		printf("#[open_file] Cannot open file %s\n", fileName);
		exit(-1);
	}
}
void showTileVersion(int* tile_ver, int No_tile_h, int No_tile_v){
	int i,j;
	for(i=0; i < No_tile_v; i++){
		for(j=0; j < No_tile_h; j++){
			printf("%d ", tile_ver[i * No_tile_h + j]);
		}
		printf("\n");
	}
}
string showTileVersionInSeg(int** tile_ver, int NO_FRAME, int No_tile_h, int No_tile_v){
  std::stringstream ss;
  std::string out;
 int i,j,k, ii;
 bool FLAG = false;
 for(i=0; i < No_tile_v; i++){
   for(j=0; j < NO_FRAME; j++){
     for(k=0; k < No_tile_h; k++){
       FLAG = false;
       if(j>0){
          if(tile_ver[0][i*No_tile_h+k] == 0 && tile_ver[j][i*No_tile_h+k] == 1)
            FLAG = true;  
       }
       if(!FLAG)
         ss << tile_ver[j][i*No_tile_h+k] << " "; 
       else
         ss << "+" << tile_ver[j][i*No_tile_h+k] << " "; 
       //printf("%d ", tile_ver[j][i*No_tile_h+k]);
     }
     ss << "\t";
     //printf("\t");
   }
   ss << "\n";
   //printf("\n");
 }
 return ss.str();
}
void showTileInfo(double* tile_ver, int No_tile_h, int No_tile_v){
  int i,j;
  for(i=0; i < No_tile_v; i++){
    for(j=0; j < No_tile_h; j++){
      printf("%.5f ", tile_ver[i * No_tile_h + j]);
    }
    printf("\n");
  }
}
void getErrorPoint(int phi, int theta, int err, int point_id, int No_point, int* phi_est, int *theta_est){
  int tmp_phi_est;
  int tmp_theta_est;
  switch(point_id){
    case 1:
      tmp_phi_est = phi + err;
      tmp_theta_est = theta;
    break;
    case 2:
      tmp_phi_est = phi - err;
      tmp_theta_est = theta;
    break;
    case 3:
      tmp_phi_est = phi;
      tmp_theta_est = theta - err;
    break;
    case 4:
      tmp_phi_est = phi;
      tmp_theta_est = theta + err;
    break;
  }
  // 
  *phi_est = tmp_phi_est;
  if(tmp_phi_est <= -180)
    *phi_est += 360;
  if(tmp_phi_est > 180)
    *phi_est -= 360;
  // 
  *theta_est = tmp_theta_est;
  if(tmp_theta_est > 90)
    *theta_est = 90;
  if(tmp_theta_est < -90)
    *theta_est = -90;
}

////////////////////////////////////// Projection.cpp /////////////////////////


int* getAdjacentTileERP(int NO_TILE_H, int NO_TILE_W, int tid, int* N){
  int tid_w = tid / NO_TILE_H; /* vertical position */
  int tid_h = tid - tid_w * NO_TILE_H; /* Horizontal position */
  int MAX = 16;
  int cnt = 0, i;
  Tile* adjTiles = new Tile[MAX];
  int* ret;
  // top
  if(tid_w - 1 >= 0){
    // (w-1, h)
    adjTiles[cnt].tid_h = tid_h;
    adjTiles[cnt++].tid_w = tid_w - 1;
    // (w-1, h-1)
    adjTiles[cnt].tid_h = ((tid_h - 1) >= 0)?(tid_h - 1):(NO_TILE_H - 1);
    adjTiles[cnt++].tid_w = tid_w - 1;
    // (w-1, h+1)
    adjTiles[cnt].tid_h = ((tid_h + 1) < NO_TILE_H)?(tid_h + 1):0;
    adjTiles[cnt++].tid_w = tid_w - 1;
  }
  // bottom
  if(tid_w + 1 < NO_TILE_W){
    // (w+1, h)
    adjTiles[cnt].tid_h = tid_h;
    adjTiles[cnt++].tid_w = tid_w + 1;
    // (w+1, h-1)
    adjTiles[cnt].tid_h = ((tid_h - 1) >= 0)?(tid_h - 1):(NO_TILE_H - 1);
    adjTiles[cnt++].tid_w = tid_w + 1;
    // (w+1, h+1)
    adjTiles[cnt].tid_h = ((tid_h + 1) < NO_TILE_H)?(tid_h + 1):0;
    adjTiles[cnt++].tid_w = tid_w + 1;
  }
  // middle
  // (w, h-1)
  adjTiles[cnt].tid_h = ((tid_h - 1) >= 0)?(tid_h - 1):(NO_TILE_H - 1);
  adjTiles[cnt++].tid_w = tid_w;
  // (w, h+1)
  adjTiles[cnt].tid_h = ((tid_h + 1) < NO_TILE_H)?(tid_h + 1):0;
  adjTiles[cnt++].tid_w = tid_w;
  // top and bottom tiles
  if(tid_w == 0 || tid_w == NO_TILE_W - 1){
    for(i=0; i < NO_TILE_H; i++){
      if(i != tid_h && i != ((tid_h - 1 >=0)?(tid_h - 1):(NO_TILE_H-1)) && i != ((tid_h + 1 < NO_TILE_H)?(tid_h+1):0)){
        // printf("i=%d\n", i);
        adjTiles[cnt].tid_h = i;
        adjTiles[cnt].tid_w = tid_w;
        cnt++;
      }
    }
  }
  *N = cnt;
  ret = new int[*N];
  /* convert to tile's id*/
  for(i=0; i < *N; i++){
    ret[i] = adjTiles[i].tid_w * NO_TILE_H + adjTiles[i].tid_h;
  }
  return ret;
}
int* getAdjacentTileERP2(int NO_TILE_H, int NO_TILE_W, int tid, int* N){
  int tid_w; /* vertical position */
  int tid_h; /* Horizontal position */
  int cnt = 0, i, K, MAX = 8;
  int* ret = new int[MAX];
  int* tmp;
  int NO_TILE_W2 = NO_TILE_W + 2;
  bool TOP_TILE = false, BOT_TILE = false;
  //
  if(tid == 0 || tid == 1){ // top or bottom tiles
    tid_w = (tid == 0)?0:(NO_TILE_W-1);
    //printf("tid_w=%d\n", tid_w);
    for(tid_h =0; tid_h < NO_TILE_H; tid_h ++){
      ret[tid_h] = 2 + (tid_w) * NO_TILE_H + tid_h;
    }
    *N = NO_TILE_H;
  }else{
    tid_w = (tid - 2 + NO_TILE_H)/NO_TILE_H;
    tid_h = (tid - 2 + NO_TILE_H)%NO_TILE_H;
    ret = getAdjacentTileERP(NO_TILE_H, NO_TILE_W + 2, tid_w*NO_TILE_H + tid_h, &K);
    //printf("tid_w=%d tid_h=%d\n", tid_w, tid_h);
    showArray(ret, K);
    //
    for(i=0; i < K; i++){
      // indices on MXN grid
      tid_w = ret[i] / NO_TILE_H;
      tid_h = ret[i] % NO_TILE_H;
      // if the tile belongs to poles tiles
      if(tid_w == 0){
        if(!TOP_TILE){
          ret[cnt++] = 0;
          TOP_TILE = true;
        }
      }else{
        if(tid_w == NO_TILE_W2 - 1){
          if(!BOT_TILE){
            ret[cnt++] = 1;
            BOT_TILE = true;
          }
        }else{
          ret[cnt++] = 2 + (tid_w - 1) * NO_TILE_H + tid_h;
        }
      }
      //printf("i=%d tid_w=%d tid_h=%d cnt=%d last=%d\n", i, tid_w, tid_h, cnt, ret[cnt-1]);
    }
    *N = cnt;
  }
  return ret;
}
int* getAdjacentTileCMP(int NO_TILE_H, int NO_TILE_W, int tid, int* N){
  int i, MAX = 8, M, cnt;
  int* ret;
  Tile tile;
  tile.fid = tid / (NO_TILE_H * NO_TILE_W);
  tile.tid_w = (tid - (tile.fid * (NO_TILE_H * NO_TILE_W)))/NO_TILE_H;
  tile.tid_h = tid - tile.fid * (NO_TILE_H * NO_TILE_W) - tile.tid_w * NO_TILE_H; 
  Tile* tiles = new Tile[MAX + 1];
  Tile* adjTile;
  if(NO_TILE_H >= 2 && NO_TILE_W >= 2){
    if(tile.fid == 0){
      /*  type 1: in-face tiles */
      if(tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        // tiles[0]  = new Tile();
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w - 1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h - 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w;
        //5
        tiles[5].fid = tile.fid;
        tiles[5].tid_h = tile.tid_h - 1;
        tiles[5].tid_w = tile.tid_w + 1;
        //6
        tiles[6].fid = tile.fid;
        tiles[6].tid_h = tile.tid_h;
        tiles[6].tid_w = tile.tid_w + 1;
        //7
        tiles[7].fid = tile.fid;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }
      /* type 2: vertex tiles */
      if(tile.tid_h == 0 && tile.tid_w == 0){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h +1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w + 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w + 1;
        //3
        tiles[3].fid = 2;
        tiles[3].tid_h = NO_TILE_H - 1;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 2;
        tiles[4].tid_h = NO_TILE_H - 1;
        tiles[4].tid_w = NO_TILE_W - 2;
        //5
        tiles[5].fid = 4;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 4;
        tiles[6].tid_h = NO_TILE_H - 1;
        tiles[6].tid_w = 1;
        *N = 7;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w == 0){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w + 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w +1;
        //3
        tiles[3].fid = 2;
        tiles[3].tid_h = NO_TILE_H - 1;
        tiles[3].tid_w = 0;
        //4
        tiles[4].fid = 2;
        tiles[4].tid_h = NO_TILE_H - 1;
        tiles[4].tid_w = 1;
        //5
        tiles[5].fid = 5;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 5;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = 1;
        *N = 7;
      }
      if(tile.tid_h == 0 && tile.tid_w == NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h +1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = 4;
        tiles[3].tid_h = NO_TILE_H - 1;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 4;
        tiles[4].tid_h = NO_TILE_H - 1;
        tiles[4].tid_w = NO_TILE_W - 2;
        //5
        tiles[5].fid = 3;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 3;
        tiles[6].tid_h = NO_TILE_H - 1;
        tiles[6].tid_w = 1;
        *N = 7;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w == NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h -1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = 3;
        tiles[3].tid_h = NO_TILE_H - 1;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 3;
        tiles[4].tid_h = NO_TILE_H - 1;
        tiles[4].tid_w = NO_TILE_W - 2;
        //5
        tiles[5].fid = 5;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = NO_TILE_W - 2;
        //6
        tiles[6].fid = 5;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = NO_TILE_W - 1;
        *N = 7;
      }
      /* type 3: other tiles */
      if(tile.tid_w == 0 && tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h-1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w +1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h;
        tiles[3].tid_w = tile.tid_w +1;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 2;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = tile.tid_h;
        //6
        tiles[6].fid = 2;
        tiles[6].tid_h = NO_TILE_H - 1;
        tiles[6].tid_w = tile.tid_h - 1;
        //7
        tiles[7].fid = 2;
        tiles[7].tid_h = NO_TILE_H - 1;
        tiles[7].tid_w = tile.tid_h + 1;
        *N = 8;
      }
      if(tile.tid_w == NO_TILE_W - 1 && tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h -1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h;
        tiles[3].tid_w = tile.tid_w -1;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h - 1;
        tiles[4].tid_w = tile.tid_w - 1;
        //5
        tiles[5].fid = 3;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = tile.tid_h;
        //6
        tiles[6].fid = 3;
        tiles[6].tid_h = NO_TILE_H - 1;
        tiles[6].tid_w = tile.tid_h - 1;
        //7
        tiles[7].fid = 3;
        tiles[7].tid_h = NO_TILE_H - 1;
        tiles[7].tid_w = tile.tid_h + 1;
        *N = 8;
      }
      if(tile.tid_h == 0 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w + 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h + 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 4;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = tile.tid_w;
        //6
        tiles[6].fid = 4;
        tiles[6].tid_h = NO_TILE_H - 1;
        tiles[6].tid_w = tile.tid_w - 1;
        //7
        tiles[7].fid = 4;
        tiles[7].tid_h = NO_TILE_H - 1;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w + 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h - 1;
        tiles[2].tid_w = tile.tid_w - 1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h - 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h - 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 5;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = tile.tid_w;
        //6
        tiles[6].fid = 5;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = tile.tid_w - 1;
        //7
        tiles[7].fid = 5;
        tiles[7].tid_h = 0;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }   
    }
    if(tile.fid == 5){
      /*  type 1: in-face tiles */
      if(tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        // tiles[0]  = new Tile();
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w - 1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h - 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w;
        //5
        tiles[5].fid = tile.fid;
        tiles[5].tid_h = tile.tid_h - 1;
        tiles[5].tid_w = tile.tid_w + 1;
        //6
        tiles[6].fid = tile.fid;
        tiles[6].tid_h = tile.tid_h;
        tiles[6].tid_w = tile.tid_w + 1;
        //7
        tiles[7].fid = tile.fid;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }
      /* type 2: vertex tiles */
      if(tile.tid_h == 0 && tile.tid_w == 0){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h + 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w + 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w + 1;
        //3
        tiles[3].fid = 2;
        tiles[3].tid_h = NO_TILE_H - 1;
        tiles[3].tid_w = 0;
        //4
        tiles[4].fid = 2;
        tiles[4].tid_h = NO_TILE_H - 2;
        tiles[4].tid_w = 0;
        //5
        tiles[5].fid = 0;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 0;
        tiles[6].tid_h = NO_TILE_H - 1;
        tiles[6].tid_w = 1;
        *N = 7;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w == 0){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w + 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w +1;
        //3
        tiles[3].fid = 2;
        tiles[3].tid_h = 0;
        tiles[3].tid_w = 0;
        //4
        tiles[4].fid = 2;
        tiles[4].tid_h = 1;
        tiles[4].tid_w = 0;
        //5
        tiles[5].fid = 1;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 1;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = 1;
        *N = 7;
      }
      if(tile.tid_h == 0 && tile.tid_w == NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h +1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = 0;
        tiles[3].tid_h = NO_TILE_H - 1;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 0;
        tiles[4].tid_h = NO_TILE_H - 1;
        tiles[4].tid_w = NO_TILE_W - 2;
        //5
        tiles[5].fid = 3;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = NO_TILE_W - 1;
        //6
        tiles[6].fid = 3;
        tiles[6].tid_h = NO_TILE_H - 2;
        tiles[6].tid_w = NO_TILE_W - 1;
        *N = 7;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w == NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h -1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = 3;
        tiles[3].tid_h = 1;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 3;
        tiles[4].tid_h = 0;
        tiles[4].tid_w = NO_TILE_W - 1;
        //5
        tiles[5].fid = 1;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = NO_TILE_W - 2;
        //6
        tiles[6].fid = 1;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = NO_TILE_W - 1;
        *N = 7;
      }
      /* type 3: other tiles */
      if(tile.tid_w == 0 && tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h-1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w +1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h;
        tiles[3].tid_w = tile.tid_w +1;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 2;
        tiles[5].tid_h = tile.tid_h;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 2;
        tiles[6].tid_h = tile.tid_h - 1;
        tiles[6].tid_w = 0;
        //7
        tiles[7].fid = 2;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = 0;
        *N = 8;
      }
      if(tile.tid_w == NO_TILE_W - 1 && tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h -1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h;
        tiles[3].tid_w = tile.tid_w -1;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h - 1;
        tiles[4].tid_w = tile.tid_w - 1;
        //5
        tiles[5].fid = 3;
        tiles[5].tid_h = tile.tid_h;
        tiles[5].tid_w = NO_TILE_W - 1;
        //6
        tiles[6].fid = 3;
        tiles[6].tid_h = tile.tid_h - 1;
        tiles[6].tid_w = NO_TILE_W - 1;
        //7
        tiles[7].fid = 3;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = NO_TILE_W - 1;
        *N = 8;
      }
      if(tile.tid_h == 0 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w + 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h + 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 0;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = tile.tid_w;
        //6
        tiles[6].fid = 0;
        tiles[6].tid_h = NO_TILE_H - 1;
        tiles[6].tid_w = tile.tid_w - 1;
        //7
        tiles[7].fid = 0;
        tiles[7].tid_h = NO_TILE_H - 1;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w + 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h - 1;
        tiles[2].tid_w = tile.tid_w - 1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h - 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h - 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 1;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = tile.tid_w;
        //6
        tiles[6].fid = 1;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = tile.tid_w - 1;
        //7
        tiles[7].fid = 1;
        tiles[7].tid_h = 0;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }   
    }
    if(tile.fid == 1){
      /*  type 1: in-face tiles */
      if(tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        // tiles[0]  = new Tile();
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w - 1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h - 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w;
        //5
        tiles[5].fid = tile.fid;
        tiles[5].tid_h = tile.tid_h - 1;
        tiles[5].tid_w = tile.tid_w + 1;
        //6
        tiles[6].fid = tile.fid;
        tiles[6].tid_h = tile.tid_h;
        tiles[6].tid_w = tile.tid_w + 1;
        //7
        tiles[7].fid = tile.fid;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }
      /* type 2: vertex tiles */
      if(tile.tid_h == 0 && tile.tid_w == 0){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h + 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w + 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w + 1;
        //3
        tiles[3].fid = 2;
        tiles[3].tid_h = 0;
        tiles[3].tid_w = 0;
        //4
        tiles[4].fid = 2;
        tiles[4].tid_h = 0;
        tiles[4].tid_w = 1;
        //5
        tiles[5].fid = 5;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 5;
        tiles[6].tid_h = NO_TILE_H - 1;
        tiles[6].tid_w = 1;
        *N = 7;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w == 0){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w + 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w +1;
        //3
        tiles[3].fid = 2;
        tiles[3].tid_h = 0;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 2;
        tiles[4].tid_h = 0;
        tiles[4].tid_w = NO_TILE_W - 2;
        //5
        tiles[5].fid = 4;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 4;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = 1;
        *N = 7;
      }
      if(tile.tid_h == 0 && tile.tid_w == NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h +1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = 5;
        tiles[3].tid_h = NO_TILE_H - 1;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 5;
        tiles[4].tid_h = NO_TILE_H - 1;
        tiles[4].tid_w = NO_TILE_W - 2;
        //5
        tiles[5].fid = 3;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = NO_TILE_W - 1;
        //6
        tiles[6].fid = 3;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = NO_TILE_W - 2;
        *N = 7;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w == NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h -1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = 3;
        tiles[3].tid_h = 0;
        tiles[3].tid_w = 0;
        //4
        tiles[4].fid = 3;
        tiles[4].tid_h = 0;
        tiles[4].tid_w = 1;
        //5
        tiles[5].fid = 4;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = NO_TILE_W - 2;
        //6
        tiles[6].fid = 4;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = NO_TILE_W - 1;
        *N = 7;
      }
      /* type 3: other tiles */
      if(tile.tid_w == 0 && tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h-1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w +1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h;
        tiles[3].tid_w = tile.tid_w +1;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 2;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = tile.tid_h;
        //6
        tiles[6].fid = 2;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = tile.tid_h - 1;
        //7
        tiles[7].fid = 2;
        tiles[7].tid_h = 0;
        tiles[7].tid_w = tile.tid_h + 1;
        *N = 8;
      }
      if(tile.tid_w == NO_TILE_W - 1 && tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h;
        tiles[3].tid_w = tile.tid_w -1;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h - 1;
        tiles[4].tid_w = tile.tid_w - 1;
        //5
        tiles[5].fid = 3;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = tile.tid_h;
        //6
        tiles[6].fid = 3;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = tile.tid_h - 1;
        //7
        tiles[7].fid = 3;
        tiles[7].tid_h = 0;
        tiles[7].tid_w = tile.tid_h + 1;
        *N = 8;
      }
      if(tile.tid_h == 0 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w + 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h + 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 5;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = tile.tid_w;
        //6
        tiles[6].fid = 5;
        tiles[6].tid_h = NO_TILE_H - 1;
        tiles[6].tid_w = tile.tid_w - 1;
        //7
        tiles[7].fid = 5;
        tiles[7].tid_h = NO_TILE_H - 1;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w + 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h - 1;
        tiles[2].tid_w = tile.tid_w - 1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h - 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h - 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 4;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = tile.tid_w;
        //6
        tiles[6].fid = 4;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = tile.tid_w - 1;
        //7
        tiles[7].fid = 4;
        tiles[7].tid_h = 0;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }   
    }
    if(tile.fid == 4){
      /*  type 1: in-face tiles */
      if(tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        // tiles[0]  = new Tile();
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w - 1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h - 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w;
        //5
        tiles[5].fid = tile.fid;
        tiles[5].tid_h = tile.tid_h - 1;
        tiles[5].tid_w = tile.tid_w + 1;
        //6
        tiles[6].fid = tile.fid;
        tiles[6].tid_h = tile.tid_h;
        tiles[6].tid_w = tile.tid_w + 1;
        //7
        tiles[7].fid = tile.fid;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }
      /* type 2: vertex tiles */
      if(tile.tid_h == 0 && tile.tid_w == 0){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h + 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w + 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w + 1;
        //3
        tiles[3].fid = 2;
        tiles[3].tid_h = 0;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 2;
        tiles[4].tid_h = 1;
        tiles[4].tid_w = NO_TILE_W - 1;
        //5
        tiles[5].fid = 1;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 1;
        tiles[6].tid_h = NO_TILE_H - 1;
        tiles[6].tid_w = 1;
        *N = 7;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w == 0){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w + 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w +1;
        //3
        tiles[3].fid = 2;
        tiles[3].tid_h = NO_TILE_H - 1;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 2;
        tiles[4].tid_h = NO_TILE_H - 2;
        tiles[4].tid_w = NO_TILE_W - 1;
        //5
        tiles[5].fid = 0;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 0;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = 1;
        *N = 7;
      }
      if(tile.tid_h == 0 && tile.tid_w == NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h +1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = 1;
        tiles[3].tid_h = NO_TILE_H - 1;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 1;
        tiles[4].tid_h = NO_TILE_H - 1;
        tiles[4].tid_w = NO_TILE_W - 2;
        //5
        tiles[5].fid = 3;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 3;
        tiles[6].tid_h = 1;
        tiles[6].tid_w = 0;
        *N = 7;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w == NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h -1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = 3;
        tiles[3].tid_h = NO_TILE_H - 1;
        tiles[3].tid_w = 0;
        //4
        tiles[4].fid = 3;
        tiles[4].tid_h = NO_TILE_H - 2;
        tiles[4].tid_w = 0;
        //5
        tiles[5].fid = 0;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = NO_TILE_W - 2;
        //6
        tiles[6].fid = 0;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = NO_TILE_W - 1;
        *N = 7;
      }
      /* type 3: other tiles */
      if(tile.tid_w == 0 && tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h-1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w +1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h;
        tiles[3].tid_w = tile.tid_w +1;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 2;
        tiles[5].tid_h = tile.tid_h;
        tiles[5].tid_w = NO_TILE_W - 1;
        //6
        tiles[6].fid = 2;
        tiles[6].tid_h = tile.tid_h - 1;
        tiles[6].tid_w = NO_TILE_W - 1;
        //7
        tiles[7].fid = 2;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = NO_TILE_W - 1;
        *N = 8;
      }
      if(tile.tid_w == NO_TILE_W - 1 && tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h;
        tiles[3].tid_w = tile.tid_w -1;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h - 1;
        tiles[4].tid_w = tile.tid_w - 1;
        //5
        tiles[5].fid = 3;
        tiles[5].tid_h = tile.tid_h;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 3;
        tiles[6].tid_h = tile.tid_h - 1;
        tiles[6].tid_w = 0;
        //7
        tiles[7].fid = 3;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = 0;
        *N = 8;
      }
      if(tile.tid_h == 0 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w + 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h + 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 1;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = tile.tid_w;
        //6
        tiles[6].fid = 1;
        tiles[6].tid_h = NO_TILE_H - 1;
        tiles[6].tid_w = tile.tid_w - 1;
        //7
        tiles[7].fid = 1;
        tiles[7].tid_h = NO_TILE_H - 1;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w + 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h - 1;
        tiles[2].tid_w = tile.tid_w - 1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h - 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h - 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 0;
        tiles[5].tid_h = 0;
        tiles[5].tid_w = tile.tid_w;
        //6
        tiles[6].fid = 0;
        tiles[6].tid_h = 0;
        tiles[6].tid_w = tile.tid_w - 1;
        //7
        tiles[7].fid = 0;
        tiles[7].tid_h = 0;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }   
    }
    if(tile.fid == 3){
      /*  type 1: in-face tiles */
      if(tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        // tiles[0]  = new Tile();
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w - 1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h - 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w;
        //5
        tiles[5].fid = tile.fid;
        tiles[5].tid_h = tile.tid_h - 1;
        tiles[5].tid_w = tile.tid_w + 1;
        //6
        tiles[6].fid = tile.fid;
        tiles[6].tid_h = tile.tid_h;
        tiles[6].tid_w = tile.tid_w + 1;
        //7
        tiles[7].fid = tile.fid;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }
      /* type 2: vertex tiles */
      if(tile.tid_h == 0 && tile.tid_w == 0){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h + 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w + 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w + 1;
        //3
        tiles[3].fid = 4;
        tiles[3].tid_h = 0;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 4;
        tiles[4].tid_h = 1;
        tiles[4].tid_w = NO_TILE_W - 1;
        //5
        tiles[5].fid = 1;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = NO_TILE_W - 1;
        //6
        tiles[6].fid = 1;
        tiles[6].tid_h = NO_TILE_H - 2;
        tiles[6].tid_w = NO_TILE_W - 1;
        *N = 7;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w == 0){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w + 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w +1;
        //3
        tiles[3].fid = 0;
        tiles[3].tid_h = 0;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 0;
        tiles[4].tid_h = 1;
        tiles[4].tid_w = NO_TILE_W - 1;
        //5
        tiles[5].fid = 4;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = NO_TILE_W - 1;
        //6
        tiles[6].fid = 4;
        tiles[6].tid_h = NO_TILE_H - 2;
        tiles[6].tid_w = NO_TILE_W - 1;
        *N = 7;
      }
      if(tile.tid_h == 0 && tile.tid_w == NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h +1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = 1;
        tiles[3].tid_h = 0;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 1;
        tiles[4].tid_h = 1;
        tiles[4].tid_w = NO_TILE_W - 1;
        //5
        tiles[5].fid = 5;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = NO_TILE_W - 1;
        //6
        tiles[6].fid = 5;
        tiles[6].tid_h = NO_TILE_H - 2;
        tiles[6].tid_w = NO_TILE_W - 1;
        *N = 7;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w == NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h -1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = 5;
        tiles[3].tid_h = 0;
        tiles[3].tid_w = NO_TILE_W - 1;
        //4
        tiles[4].fid = 5;
        tiles[4].tid_h = 1;
        tiles[4].tid_w = NO_TILE_W - 1;
        //5
        tiles[5].fid = 0;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = NO_TILE_W - 1;
        //6
        tiles[6].fid = 0;
        tiles[6].tid_h = NO_TILE_H - 2;
        tiles[6].tid_w = NO_TILE_W - 1;
        *N = 7;
      }
      /* type 3: other tiles */
      if(tile.tid_w == 0 && tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h-1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w +1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h;
        tiles[3].tid_w = tile.tid_w +1;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 4;
        tiles[5].tid_h = tile.tid_h;
        tiles[5].tid_w = NO_TILE_W - 1;
        //6
        tiles[6].fid = 4;
        tiles[6].tid_h = tile.tid_h - 1;
        tiles[6].tid_w = NO_TILE_W - 1;
        //7
        tiles[7].fid = 4;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = NO_TILE_W - 1;
        *N = 8;
      }
      if(tile.tid_w == NO_TILE_W - 1 && tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h;
        tiles[3].tid_w = tile.tid_w -1;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h - 1;
        tiles[4].tid_w = tile.tid_w - 1;
        //5
        tiles[5].fid = 5;
        tiles[5].tid_h = tile.tid_h;
        tiles[5].tid_w = NO_TILE_W - 1;
        //6
        tiles[6].fid = 5;
        tiles[6].tid_h = tile.tid_h - 1;
        tiles[6].tid_w = NO_TILE_W - 1;
        //7
        tiles[7].fid = 5;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = NO_TILE_W - 1;
        *N = 8;
      }
      if(tile.tid_h == 0 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w + 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h + 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 1;
        tiles[5].tid_h = tile.tid_w;
        tiles[5].tid_w = NO_TILE_W - 1;
        //6
        tiles[6].fid = 1;
        tiles[6].tid_h = tile.tid_w - 1;
        tiles[6].tid_w = NO_TILE_W - 1;
        //7
        tiles[7].fid = 1;
        tiles[7].tid_h = tile.tid_w + 1;
        tiles[7].tid_w = NO_TILE_W - 1;
        *N = 8;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w + 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h - 1;
        tiles[2].tid_w = tile.tid_w - 1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h - 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h - 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 0;
        tiles[5].tid_h = tile.tid_w;
        tiles[5].tid_w = NO_TILE_W - 1;
        //6
        tiles[6].fid = 0;
        tiles[6].tid_h = tile.tid_w - 1;
        tiles[6].tid_w = NO_TILE_W - 1;
        //7
        tiles[7].fid = 0;
        tiles[7].tid_h = tile.tid_w + 1;
        tiles[7].tid_w = NO_TILE_W - 1;
        *N = 8;
      }   
    }
    if(tile.fid == 2){
      /*  type 1: in-face tiles */
      if(tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        // tiles[0]  = new Tile();
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w - 1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h - 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w;
        //5
        tiles[5].fid = tile.fid;
        tiles[5].tid_h = tile.tid_h - 1;
        tiles[5].tid_w = tile.tid_w + 1;
        //6
        tiles[6].fid = tile.fid;
        tiles[6].tid_h = tile.tid_h;
        tiles[6].tid_w = tile.tid_w + 1;
        //7
        tiles[7].fid = tile.fid;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = tile.tid_w + 1;
        *N = 8;
      }
      /* type 2: vertex tiles */
      if(tile.tid_h == 0 && tile.tid_w == 0){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h + 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w + 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w + 1;
        //3
        tiles[3].fid = 1;
        tiles[3].tid_h = 0;
        tiles[3].tid_w = 0;
        //4
        tiles[4].fid = 1;
        tiles[4].tid_h = 1;
        tiles[4].tid_w = 0;
        //5
        tiles[5].fid = 5;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 5;
        tiles[6].tid_h = NO_TILE_H - 2;
        tiles[6].tid_w = 0;
        *N = 7;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w == 0){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w + 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w +1;
        //3
        tiles[3].fid = 5;
        tiles[3].tid_h = 0;
        tiles[3].tid_w = 0;
        //4
        tiles[4].fid = 5;
        tiles[4].tid_h = 1;
        tiles[4].tid_w = 0;
        //5
        tiles[5].fid = 0;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 0;
        tiles[6].tid_h = NO_TILE_H - 2;
        tiles[6].tid_w = 0;
        *N = 7;
      }
      if(tile.tid_h == 0 && tile.tid_w == NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h +1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = 4;
        tiles[3].tid_h = 0;
        tiles[3].tid_w = 0;
        //4
        tiles[4].fid = 4;
        tiles[4].tid_h = 1;
        tiles[4].tid_w = 0;
        //5
        tiles[5].fid = 1;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 1;
        tiles[6].tid_h = NO_TILE_H - 2;
        tiles[6].tid_w = 0;
        *N = 7;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w == NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w - 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h -1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = 0;
        tiles[3].tid_h = 0;
        tiles[3].tid_w = 0;
        //4
        tiles[4].fid = 0;
        tiles[4].tid_h = 1;
        tiles[4].tid_w = 0;
        //5
        tiles[5].fid = 4;
        tiles[5].tid_h = NO_TILE_H - 1;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 4;
        tiles[6].tid_h = NO_TILE_H - 2;
        tiles[6].tid_w = 0;
        *N = 7;
      }
      /* type 3: other tiles */
      if(tile.tid_w == 0 && tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h-1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h -1;
        tiles[2].tid_w = tile.tid_w +1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h;
        tiles[3].tid_w = tile.tid_w +1;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 5;
        tiles[5].tid_h = tile.tid_h;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 5;
        tiles[6].tid_h = tile.tid_h - 1;
        tiles[6].tid_w = 0;
        //7
        tiles[7].fid = 5;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = 0;
        *N = 8;
      }
      if(tile.tid_w == NO_TILE_W - 1 && tile.tid_h > 0 && tile.tid_h < NO_TILE_H - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h - 1;
        tiles[0].tid_w = tile.tid_w;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h + 1;
        tiles[1].tid_w = tile.tid_w;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h;
        tiles[3].tid_w = tile.tid_w -1;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h - 1;
        tiles[4].tid_w = tile.tid_w - 1;
        //5
        tiles[5].fid = 4;
        tiles[5].tid_h = tile.tid_h;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 4;
        tiles[6].tid_h = tile.tid_h - 1;
        tiles[6].tid_w = 0;
        //7
        tiles[7].fid = 4;
        tiles[7].tid_h = tile.tid_h + 1;
        tiles[7].tid_w = 0;
        *N = 8;
      }
      if(tile.tid_h == 0 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w + 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h + 1;
        tiles[2].tid_w = tile.tid_w -1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h + 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h + 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 1;
        tiles[5].tid_h = tile.tid_w;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 1;
        tiles[6].tid_h = tile.tid_w - 1;
        tiles[6].tid_w = 0;
        //7
        tiles[7].fid = 1;
        tiles[7].tid_h = tile.tid_w + 1;
        tiles[7].tid_w = 0;
        *N = 8;
      }
      if(tile.tid_h == NO_TILE_H - 1 && tile.tid_w > 0 && tile.tid_w < NO_TILE_W - 1){
        //0
        tiles[0].fid = tile.fid;
        tiles[0].tid_h = tile.tid_h;
        tiles[0].tid_w = tile.tid_w + 1;
        //1
        tiles[1].fid = tile.fid;
        tiles[1].tid_h = tile.tid_h;
        tiles[1].tid_w = tile.tid_w - 1;
        //2
        tiles[2].fid = tile.fid;
        tiles[2].tid_h = tile.tid_h - 1;
        tiles[2].tid_w = tile.tid_w - 1;
        //3
        tiles[3].fid = tile.fid;
        tiles[3].tid_h = tile.tid_h - 1;
        tiles[3].tid_w = tile.tid_w;
        //4
        tiles[4].fid = tile.fid;
        tiles[4].tid_h = tile.tid_h - 1;
        tiles[4].tid_w = tile.tid_w + 1;
        //5
        tiles[5].fid = 0;
        tiles[5].tid_h = tile.tid_w;
        tiles[5].tid_w = 0;
        //6
        tiles[6].fid = 0;
        tiles[6].tid_h = tile.tid_w - 1;
        tiles[6].tid_w = 0;
        //7
        tiles[7].fid = 0;
        tiles[7].tid_h = tile.tid_w + 1;
        tiles[7].tid_w = 0;
        *N = 8;
      }   
    }
  }
  if(NO_TILE_W == 1){
    /* get in-face adjcent tiles */
    adjTile = get_adj_tile_in_face(NO_TILE_H, NO_TILE_W, tile.tid_w, tile.tid_h, &M);
    for(i=0; i < M; i++){
      tiles[i] = adjTile[i];
      tiles[i].fid = tile.fid;
      // printf("%d %d %d \n", tiles[i].fid,tiles[i].tid_w, tiles[i].tid_h);
    }
    cnt = M;
    if(tile.fid == 0){
      /* boundary tiles */
      if(tile.tid_h == 0){
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
      }
      if(tile.tid_h == NO_TILE_H - 1){
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0; 
      }
      if(tile.tid_h > 0 && tile.tid_h < NO_TILE_H-1){
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
      }
      *N = cnt;
    }
    if(tile.fid == 5){
      /* boundary tiles */
      if(tile.tid_h == 0){
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = NO_TILE_H - 2;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = NO_TILE_H - 2;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 0;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
      }
      if(tile.tid_h == NO_TILE_H - 1){
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = 1;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = 1;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 1;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0; 
      }
      if(tile.tid_h > 0 && tile.tid_h < NO_TILE_H-1){
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = tile.tid_h;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = tile.tid_h - 1;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = tile.tid_h + 1;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = tile.tid_h;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = tile.tid_h - 1;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = tile.tid_h + 1;
        tiles[cnt++].tid_w = 0;
      }
      *N = cnt;
    }
    if(tile.fid == 1){
      /* boundary tiles */
      if(tile.tid_h == 0){
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
      }
      if(tile.tid_h == NO_TILE_H - 1){
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0; 
      }
      if(tile.tid_h > 0 && tile.tid_h < NO_TILE_H-1){
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
      }
      *N = cnt;
    }
    if(tile.fid == 4){
      /* boundary tiles */
      if(tile.tid_h == 0){
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = 1;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = 1;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 1;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
      }
      if(tile.tid_h == NO_TILE_H - 1){
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = NO_TILE_H - 2;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = NO_TILE_H - 1;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = NO_TILE_H - 2;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 0;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0; 
      }
      if(tile.tid_h > 0 && tile.tid_h < NO_TILE_H-1){
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = tile.tid_h;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = tile.tid_h - 1;
        tiles[cnt++].tid_w = 0;
        //2
        tiles[cnt].fid = 2;
        tiles[cnt].tid_h = tile.tid_h + 1;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = tile.tid_h;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = tile.tid_h - 1;
        tiles[cnt++].tid_w = 0;
        //3
        tiles[cnt].fid = 3;
        tiles[cnt].tid_h = tile.tid_h + 1;
        tiles[cnt++].tid_w = 0;
      }
      *N = cnt;
    }
    if(tile.fid == 2){
      if(tile.tid_h == 0){
        //4
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
        //4
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = 1;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = NO_TILE_H-1;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = NO_TILE_H-2;
        tiles[cnt++].tid_w = 0;
        //1
        for(i=0; i < NO_TILE_H; i++){
          tiles[cnt].fid = 1;
          tiles[cnt].tid_h = i;
          tiles[cnt++].tid_w = 0;
        }
      }
      if(tile.tid_h == NO_TILE_H-1){
        //4
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
        //4
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = 1;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = NO_TILE_H-1;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = NO_TILE_H-2;
        tiles[cnt++].tid_w = 0;
        //1
        for(i=0; i < NO_TILE_H; i++){
          tiles[cnt].fid = 0;
          tiles[cnt].tid_h = i;
          tiles[cnt++].tid_w = 0;
        }
      }
      if(tile.tid_h > 0 && tile.tid_h < NO_TILE_H-1){
        //4
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = tile.tid_h;
        tiles[cnt++].tid_w = 0;
        //4
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = tile.tid_h - 1;
        tiles[cnt++].tid_w = 0;
        //4
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = tile.tid_h + 1;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = tile.tid_h;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = tile.tid_h - 1;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = tile.tid_h + 1;
        tiles[cnt++].tid_w = 0;
      }
      *N = cnt;
    }
    if(tile.fid == 3){
      if(tile.tid_h == 0){
        //4
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
        //4
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = 1;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = NO_TILE_H-1;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = NO_TILE_H-2;
        tiles[cnt++].tid_w = 0;
        //1
        for(i=0; i < NO_TILE_H; i++){
          tiles[cnt].fid = 1;
          tiles[cnt].tid_h = i;
          tiles[cnt++].tid_w = 0;
        }
      }
      if(tile.tid_h == NO_TILE_H-1){
        //4
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = 0;
        tiles[cnt++].tid_w = 0;
        //4
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = 1;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = NO_TILE_H-1;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = NO_TILE_H-2;
        tiles[cnt++].tid_w = 0;
        //1
        for(i=0; i < NO_TILE_H; i++){
          tiles[cnt].fid = 0;
          tiles[cnt].tid_h = i;
          tiles[cnt++].tid_w = 0;
        }
      }
      if(tile.tid_h > 0 && tile.tid_h < NO_TILE_H-1){
        //4
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = tile.tid_h;
        tiles[cnt++].tid_w = 0;
        //4
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = tile.tid_h - 1;
        tiles[cnt++].tid_w = 0;
        //4
        tiles[cnt].fid = 4;
        tiles[cnt].tid_h = tile.tid_h + 1;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = tile.tid_h;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = tile.tid_h - 1;
        tiles[cnt++].tid_w = 0;
        //5
        tiles[cnt].fid = 5;
        tiles[cnt].tid_h = tile.tid_h + 1;
        tiles[cnt++].tid_w = 0;
      }
      *N = cnt;
    }
  }
  ret = new int[*N];
  for(i=0; i < *N; i++){
    // printf("%d %d %d \n", tiles[i].fid,tiles[i].tid_w, tiles[i].tid_h);
    ret[i] = NO_TILE_H * NO_TILE_W * tiles[i].fid + NO_TILE_H * tiles[i].tid_w + tiles[i].tid_h;  
  }
  return ret;
}
int* getAdjacentTile(int NO_FACE, int NO_TILE_H, int NO_TILE_W, int tid, int *N){
  switch(NO_FACE){
    case 1:
    return getAdjacentTileERP(NO_TILE_H, NO_TILE_W, tid, N);
    case 2:
    return getAdjacentTileERP2(NO_TILE_H, NO_TILE_W, tid, N);
    break;
    case 6:
    return getAdjacentTileCMP(NO_TILE_H, NO_TILE_W, tid, N);
    break;    
  }
}
void showArray(int *arr, int N){
  int i;
  for(i=0; i < N; i++){
    printf("%d, ", arr[i]);
  }
  printf("\n");
}
void showVmask(int* vmask, int NO_FACE, int NO_TILE_H, int NO_TILE_W){
  int i,j,k;
  for(i=0; i < NO_FACE; i++){
    for(j=0; j < NO_TILE_W; j++){
      for(k=0; k < NO_TILE_H; k++){
        printf("%d ", vmask[i*NO_TILE_H*NO_TILE_W + j * NO_TILE_H + k]);
      }
      printf("\n");
    }
    printf("\n");
  }
}
int* extVmask(int* vmask,int NO_FACE, int NO_TILE_H, int NO_TILE_W, int ext_width){
  int N = NO_TILE_W * NO_TILE_H; /* Number of tiles per face */
  int NO_TILE = N * NO_FACE;
  int* vmask_ext = new int[NO_TILE];
  int tid, tid2;
  int* adj;
  int M,i,a;
  /* init */
  for(tid = 0; tid < NO_TILE; tid ++){
    vmask_ext[tid] = vmask[tid];
  }
  //
  for(a = 1; a <= ext_width; a++){
    for(tid = 0; tid < NO_TILE; tid ++){
      /* For each visible tile */
      if(vmask_ext[tid] == a){
        adj = getAdjacentTile(NO_FACE, NO_TILE_H, NO_TILE_W, tid, &M);
        for(i=0; i < M; i++){
          if(vmask_ext[adj[i]] == 0)
            vmask_ext[adj[i]] = a + 1;
        }
      }
    }
  }
  return vmask_ext;
}
int* extVmask2(int* vmask,int NO_FACE, int NO_TILE_H, int NO_TILE_W, int ext_width){
  int No_tile = NO_FACE * NO_TILE_H * NO_TILE_W;
  int* ret = new int[No_tile];
  int M = NO_TILE_H;
  int N = NO_TILE_W;
  int i,j;
  struct Pos pos;
  if(ext_width == 0) return vmask;
  if(ext_width >= 3){
    for(i=0; i < No_tile; i++)
      ret[i] = 1;
    return ret;
  }
  // vmask after extension
  int** vmask_ext = init2dArrayInt(M, N);
  // convert 'vmask' to rectangle form
  // printf("#[extVmask2]: vmask\n");
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      vmask_ext[i][j] = vmask[i*M + j];
      // printf("%d ", vmask[i*M + j]);
    }
    // printf("\n");
  }
  //
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] == 1){
        // printf("#[extVmask2]: (i,j)=(%d,%d)\n", i,j);
        //TOP
        pos = TOP(i,j,M,N);
        // printf("#[extVmask2]: TOP(i,j)=(%d,%d)\n", pos.i,pos.j);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 2;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] == 1){
        //DOWN
        pos = DOWN(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 3;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] == 1){
        //LEFT
        pos = LEFT(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 4;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] == 1){
        //RIGHT
        pos = RIGHT(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 5;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] == 1){
        //TOP-LEFT
        pos = TOP_LEFT(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 4;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] == 1){
        //TOP-RIGHT
        pos = TOP_RIGHT(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 5;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] == 1){
        //DOWN-LEFT
        pos = DOWN_LEFT(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 4;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] == 1){
        //DOWN-RIGHT
        pos = DOWN_RIGHT(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 5;
      }
    }
  }
  // 2->1
  // printf("#[extVmask2]: vmask_ext\n");
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      // if(vmask_ext[i][j] == 2)
      //  vmask_ext[i][j] = 1;
      ret[i*N + j]= vmask_ext[i][j];
      // printf("%d ", vmask_ext[i][j]);
    }
    // printf("\n"); 
  }
  if(ext_width == 1) return ret;
  // extension width = 2
  //
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] >= 1 && vmask_ext[i][j] <= 5){
        // printf("#[extVmask2]: (i,j)=(%d,%d)\n", i,j);
        //TOP
        pos = TOP(i,j,M,N);
        // printf("#[extVmask2]: TOP(i,j)=(%d,%d)\n", pos.i,pos.j);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 6;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] >= 1 && vmask_ext[i][j] <= 5){
        //DOWN
        pos = DOWN(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 7;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] >= 1 && vmask_ext[i][j] <= 5){
        //LEFT
        pos = LEFT(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 8;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] >= 1 && vmask_ext[i][j] <= 5){
        //RIGHT
        pos = RIGHT(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 9;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] >= 1 && vmask_ext[i][j] <= 5){
        //TOP-LEFT
        pos = TOP_LEFT(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 8;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] >= 1 && vmask_ext[i][j] <= 5){
        //TOP-RIGHT
        pos = TOP_RIGHT(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 9;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] >= 1 && vmask_ext[i][j] <= 5){
        //DOWN-LEFT
        pos = DOWN_LEFT(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 8;
      }
    }
  }
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      if(vmask_ext[i][j] >= 1 && vmask_ext[i][j] <= 5){
        //DOWN-RIGHT
        pos = DOWN_RIGHT(i,j,M,N);
        if(pos.i != -1 && vmask_ext[pos.i][pos.j] == 0)
          vmask_ext[pos.i][pos.j] = 9;
      }
    }
  }
  // 2->1
  // printf("#[extVmask2]: vmask_ext_2\n");
  for(i=0; i < M; i++){
    for(j=0; j < N; j++){
      // if(vmask_ext[i][j] == 2)
      //  vmask_ext[i][j] = 1;
      ret[i*N + j]= vmask_ext[i][j];
      // printf("%d ", vmask_ext[i][j]);
    }
    // printf("\n"); 
  }
  return ret;
}
Tile* get_adj_tile_in_face(int NO_TILE_H, int NO_TILE_W, int tid_w, int tid_h, int *N){
  int MAX = 8, cnt = 0;
  Tile* adjTiles = new Tile[MAX];
  // top
  if(tid_w - 1 >= 0){
    // (w-1, h)
    adjTiles[cnt].tid_h = tid_h;
    adjTiles[cnt++].tid_w = tid_w - 1;
    // (w-1, h-1)
    if(tid_h - 1 >= 0){
      adjTiles[cnt].tid_h = (tid_h - 1);
      adjTiles[cnt++].tid_w = tid_w - 1;
    }
    // (w-1, h+1)
    if(tid_h + 1 < NO_TILE_H){
      adjTiles[cnt].tid_h = tid_h + 1;
      adjTiles[cnt++].tid_w = tid_w - 1;
    }
  }
  // bottom
  if(tid_w + 1 < NO_TILE_W){
    // (w+1, h)
    adjTiles[cnt].tid_h = tid_h;
    adjTiles[cnt++].tid_w = tid_w + 1;
    // (w+1, h-1)
    if(tid_h - 1 >= 0){
      adjTiles[cnt].tid_h = (tid_h - 1);
      adjTiles[cnt++].tid_w = tid_w + 1;
    }
      // (w+1, h+1)
    if(tid_h + 1 < NO_TILE_H){
      adjTiles[cnt].tid_h = tid_h + 1;
      adjTiles[cnt++].tid_w = tid_w + 1;
    }
  }
  // middle
  // (w, h-1)
  if(tid_h - 1 >= 0){
    adjTiles[cnt].tid_h = (tid_h - 1);
    adjTiles[cnt++].tid_w = tid_w;
  }
  // (w, h+1)
  if(tid_h + 1 < NO_TILE_H){
    adjTiles[cnt].tid_h = tid_h + 1;
    adjTiles[cnt++].tid_w = tid_w;
  }
  *N= cnt;
  return adjTiles;
}
struct Pos TOP(int i, int j, int M, int N){
  struct Pos pos;
  pos.i = -1;
  pos.j = -1;
  if(i==0) return pos;
  pos.i = i-1;  
  pos.j = j;
  // printf("#[TOP] (pos.i, pos.j) = (%d, %d)\n", pos.i, pos.j);
  return pos;
}
struct Pos DOWN(int i, int j, int M, int N){
  struct Pos pos;
  pos.i = -1;
  pos.j = -1;
  if(i==M-1) return pos;
  pos.j = j;
  pos.i = i+1;
  return pos;
}
struct Pos LEFT(int i, int j, int M, int N){
  struct Pos pos;
  pos.i = i;
  if(j==0)
    pos.j = N-1;
  else
    pos.j = j-1;
  return pos;
}
struct Pos RIGHT(int i, int j, int M, int N){
  struct Pos pos;
  pos.i = i;
  if(j==N-1)
    pos.j = 0;
  else
    pos.j = j+1;
  return pos;
}
struct Pos TOP_LEFT(int i, int j, int M, int N){
  struct Pos pos;
  pos.i = -1;
  pos.j = -1;
  if(i==0)
    return pos;
  pos.i = i-1;
  pos.j = (j==0)?(N-1):(j-1);
  return pos;
}
struct Pos TOP_RIGHT(int i, int j, int M, int N){
  struct Pos pos;
  pos.i = -1;
  pos.j = -1;
  if(i==0)
    return pos;
  pos.i = i-1;
  pos.j = (j==N-1)?0:(j+1);
  return pos;
}
struct Pos DOWN_LEFT(int i, int j, int M, int N){
  struct Pos pos;
  pos.i = -1;
  pos.j = -1;
  if(i==M-1)
    return pos;
  pos.i = i+1;
  pos.j = (j==0)?(N-1):(j-1);
  return pos;
}
struct Pos DOWN_RIGHT(int i, int j, int M, int N){
  struct Pos pos;
  pos.i = -1;
  pos.j = -1;
  if(i==M-1)
    return pos;
  pos.i = i+1;
  pos.j = (j==N-1)?0:(j+1);
  return pos;
}

