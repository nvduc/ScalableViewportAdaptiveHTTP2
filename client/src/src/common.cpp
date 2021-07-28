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

#include "common.h"
#include <utility>
#include <sys/time.h>
#include <time.h>
#include <string>

using namespace std;
ostream& operator<<(ostream& out, const ViewportPos& data){
  out << "(" << data.phi << "," << data.theta << ")";
  return out;
}
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
run_cfg load_run_cfg(const char* cfg){
  printf("#[load_run_cfg]:\n");
  string s;
  string delimiter = "=";
  string comment = "#";
  string key;
  string val_str;
  //
  string deli = ",";
  string token;
  size_t pos = 0;
  //
  double val;
  size_t pos_deli = 0;
  size_t pos_comm;
  ifstream infile(cfg);
  run_cfg config;
  if(infile == NULL){
   	exit(-1);
  }else{
    cout << "Reading file " << cfg << endl;
  }
  while(std::getline(infile, s)){
    // cout << s << endl;
    if((pos_deli=s.find(delimiter)) != std::string::npos){
      key = s.substr(0, pos_deli-1);
      pos_comm = s.find(comment);
      if(pos_comm != std::string::npos){
        val_str = s.substr(pos_deli + 2, pos_comm-pos_deli-2);
      }else{
        val_str = s.substr(pos_deli + 2, s.length());
      }
    }
    if(key.compare("NO_SEG")==0)  config.NO_SEG = (int) std::stod(val_str);
    if(key.compare("VP_MODE")==0)  config.VP_MODE = (int) std::stod(val_str);
    // printf("%s\n", val_str.c_str());
     if(key.compare("ERR")==0){
      printf("%sa\n", val_str.c_str());
      config.ERR_NUM = 0;
      while((pos=val_str.find(deli)) != std::string::npos){
        token = val_str.substr(0, pos);
        config.ERR.push_back((int) std::stod(token));
        // cout << std::stod(token) << endl;
        config.ERR_NUM ++;
        val_str.erase(0, pos + deli.length());
      }
    }
    // viewport
    if(key.compare("DASH_VER")==0){
      printf("%sa\n", val_str.c_str());
      config.DASH_VER_NUM = 0;
      while((pos=val_str.find(deli)) != std::string::npos){
        token = val_str.substr(0, pos);
        config.DASH_VER.push_back((int) std::stod(token));
        // cout << std::stod(token) << endl;
        config.DASH_VER_NUM ++;
        val_str.erase(0, pos + deli.length());
      }
    }
    if(key.compare("TILE_SELECT_METHOD")==0){
      printf("%sa\n", val_str.c_str());
      config.METHOD_NUM = 0;
      while((pos=val_str.find(deli)) != std::string::npos){
        token = val_str.substr(0, pos);
        config.METHOD_LIST.push_back((int) std::stod(token));
        // cout << std::stod(token) << endl;
        config.METHOD_NUM ++;
        val_str.erase(0, pos + deli.length());
      }
      printf("#[load_run_info] no_method=%d\n", config.METHOD_NUM);
    }
    if(key.compare("BWTRACE")==0){
      // printf("%sa\n", val_str.c_str());
      config.BWTRACE_NUM = 0;
      while((pos=val_str.find(deli)) != std::string::npos){
        token = val_str.substr(0, pos);
        config.BWTRACE_LIST.push_back((int) std::stod(token));
        // cout << std::stod(token) << endl;
        config.BWTRACE_NUM++;
        val_str.erase(0, pos + deli.length());
      }
    }
if(key.compare("BANDWIDTH")==0){
      // printf("%sa\n", val_str.c_str());
      config.BW_NUM = 0;
      while((pos=val_str.find(deli)) != std::string::npos){
        token = val_str.substr(0, pos);
        config.BW_LIST.push_back((int) std::stod(token));
        // cout << std::stod(token) << endl;
        config.BW_NUM++;
        val_str.erase(0, pos + deli.length());
      }
    }
    if(key.compare("INTERVAL")==0){
      // printf("%sa\n", val_str.c_str());
      config.INTER_NUM = 0;
      while((pos=val_str.find(deli)) != std::string::npos){
        token = val_str.substr(0, pos);
        config.INTER_LIST.push_back((int) std::stod(token));
        // cout << std::stod(token) << endl;
        config.INTER_NUM++;
        val_str.erase(0, pos + deli.length());
      }
    }
    if(key.compare("BUFFER")==0){
      // printf("%sa\n", val_str.c_str());
      config.BUFF_NUM = 0;
      while((pos=val_str.find(deli)) != std::string::npos){
        token = val_str.substr(0, pos);
        config.BUFF_LIST.push_back((int) std::stod(token));
        // cout << std::stod(token) << endl;
        config.BUFF_NUM++;
        val_str.erase(0, pos + deli.length());
      }
    }
    if(key.compare("HEADTRACE")==0){
      // printf("%sa\n", val_str.c_str());
      config.HEADTRACE_NUM = 0;
      while((pos=val_str.find(deli)) != std::string::npos){
        token = val_str.substr(0, pos);
        config.HEADTRACE_LIST.push_back((int) std::stod(token));
        // cout << std::stod(token) << endl;
        config.HEADTRACE_NUM++;
        val_str.erase(0, pos + deli.length());
      }
    }
    if(key.compare("VP_EST_METHOD")==0){
      // printf("%sa\n", val_str.c_str());
      config.VP_EST_METHOD_NUM = 0;
      while((pos=val_str.find(deli)) != std::string::npos){
        token = val_str.substr(0, pos);
        config.VP_EST_METHOD_LIST.push_back((int) std::stod(token));
        // cout << std::stod(token) << endl;
        config.VP_EST_METHOD_NUM++;
        val_str.erase(0, pos + deli.length());
      }
    }
    if(key.compare("PHI")==0){
      // printf("%sa\n", val_str.c_str());
      while((pos=val_str.find(deli)) != std::string::npos){
        token = val_str.substr(0, pos);
        config.PHI.push_back((int) std::stod(token));
        val_str.erase(0, pos + deli.length());
      }
    }
    if(key.compare("THETA")==0){
      // printf("%sa\n", val_str.c_str());
      while((pos=val_str.find(deli)) != std::string::npos){
        token = val_str.substr(0, pos);
        config.THETA.push_back((int) std::stod(token));
        val_str.erase(0, pos + deli.length());
      }
    }
    if(key.compare("SPEED")==0){
      // printf("%sa\n", val_str.c_str());
      while((pos=val_str.find(deli)) != std::string::npos){
        token = val_str.substr(0, pos);
        config.SPEED.push_back((int) std::stod(token));
        val_str.erase(0, pos + deli.length());
      }
    }
  }
  return config;
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
