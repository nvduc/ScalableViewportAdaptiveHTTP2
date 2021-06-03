//#define MAX_SEG 1000
//#define NO_FACE 6
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <locale>
#include "common.h"
using namespace std;
class VisibleMask{
  public:
    int*** vmask;
    int*** pixel;
    int NO_TILE; 

    /* init() */
    VisibleMask(const char* vmaskfile, int NO_TILE){
      int i,j,rows,cols;
      vector<double> v;
      this->NO_TILE = NO_TILE;
      vmask = init3dArrayInt(360, 181, NO_TILE);
      pixel = init3dArrayInt(360, 181, NO_TILE);
      /* read file */
      if(import_matrix_from_txt_file(vmaskfile, v, rows, cols) != 0){
        cout << "#[VisibleMask] Cannot open file " << endl;
        exit(-1); 
      } 
      for(i=0; i < rows; i++){
        for(j=0; j < NO_TILE; j++){
          pixel[int(v[i*cols])][int(v[i*cols + 1]) + 90][j] = v[i * cols + 2 + j];
          if(pixel[int(v[i*cols])][int(v[i*cols + 1]) + 90][j] > 0)
            vmask[int(v[i*cols])][int(v[i*cols + 1]) + 90][j] = 1;
          else
            vmask[int(v[i*cols])][int(v[i*cols + 1]) + 90][j] = 0;
        }
      }
    }

    /* get visible mask */ 
    int* get_vmask(int* vp_pos){
      int tmp_phi;
      if(vp_pos[0] < 0)
        tmp_phi = vp_pos[0] + 360;
      else
        tmp_phi = vp_pos[0];
      return this->vmask[tmp_phi][vp_pos[1]+90];
    }

    /* get visible tiles */ 
    vector<int> get_visible_tile(int* vp_pos){
      int* vmask = get_vmask(vp_pos);
      vector<int> visible_tile;
      int i;
      for(i=0; i < NO_TILE; i++)
        if(vmask[i] == 1)
          visible_tile.push_back(i+1);
      return visible_tile;
    }

    /* get visible pixel */
    int* get_pixel(int* vp_pos){
      return this->pixel[vp_pos[0]][vp_pos[1]+90];
    }

    /* init 3D array */
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
    int import_matrix_from_txt_file(const char* filename_X, vector <double>& v, int& rows, int& cols){
      ifstream file_X;
      string line;
      // erase all current elements
      v.erase(v.begin(), v.end());
      cout << "#[import_matrix_from_txt_file] open text file: " << filename_X << endl;
      file_X.open(filename_X);
      if (file_X.is_open())
      {
        int i=0;
        getline(file_X, line);
        cols = ReadNumbers( line, v );
        rows = 0;
        if(cols > 0)
          rows = 1;
        // cout << "cols:" << cols << endl;
        while(getline(file_X, line)){
          cols = ReadNumbers(line, v);
          rows ++;
        }        
        file_X.close();
        return 0;
      }
      else{
        cout << "#[import_matrix_from_txt_file] Cannot open file " << filename_X << endl;
        return -1;
      }
    }
    int ReadNumbers( const string & s, vector <double> & v ){
      istringstream is( s );
      double n;
      int oriSize = v.size();
      while( is >> n ) {
        v.push_back( n );
      }
      return v.size() - oriSize;
    }
};
class Headtrace{
  public:
    int** vp_pos; /* viewport position at each frame */
    int NO_FRAME; /* Number of video frames */
    int FPS;
    Headtrace(const char* tracefile, int NO_FRAME, int FPS){
      int i,j,rows,cols;
      vector<double> v;
      /* load viewport positions */
      this->NO_FRAME = NO_FRAME;
      this->FPS = FPS;
      
      /* init vp_pos */
      vp_pos = new int*[NO_FRAME];
      for(i=0; i < NO_FRAME; i++)
        vp_pos[i] = new int[2];

      /* read file */
      if(import_matrix_from_txt_file(tracefile, v, rows, cols) != 0){
        cout << "#[Headtrace] Cannot open file " << endl;
        exit(-1); 
      }
      for(i=0; i < NO_FRAME; i++){
        for(j=0; j < cols; j++){
          vp_pos[i][j] = v[(i+1) * cols + j]; /* exclusive the first line */
        }
      }
    } 
    /* get viewport postion at a given time */
    int* get_vp_pos(double time){
      int frame_id = time / (1000.0/FPS);  
      return vp_pos[frame_id]; 
    } 
    int import_matrix_from_txt_file(const char* filename_X, vector <double>& v, int& rows, int& cols){
      ifstream file_X;
      string line;
      // erase all current elements
      v.erase(v.begin(), v.end());
      cout << "#[import_matrix_from_txt_file] open text file: " << filename_X << endl;
      file_X.open(filename_X);
      if (file_X.is_open())
      {
        int i=0;
        getline(file_X, line);
        cols = ReadNumbers( line, v );
        rows = 0;
        if(cols > 0)
          rows = 1;
        // cout << "cols:" << cols << endl;
        while(getline(file_X, line)){
          cols = ReadNumbers(line, v);
          rows ++;
        }        
        file_X.close();
        return 0;
      }
      else{
        cout << "#[import_matrix_from_txt_file] Cannot open file " << filename_X << endl;
        return -1;
      }
    }
    int ReadNumbers( const string & s, vector <double> & v ){
      istringstream is( s );
      double n;
      int oriSize = v.size();
      while( is >> n ) {
        v.push_back( n );
      }
      return v.size() - oriSize;
    }
};
class Buffer{
  public:
    int NO_SEG;
    int NO_TILE;
    int** tile_ver;
    mutex m;
    Buffer(){
    }
    void init(int NO_SEG, int NO_TILE){
      int i,j;
      this->NO_SEG = NO_SEG;
      this->NO_TILE = NO_TILE;
      this->tile_ver = new int*[NO_SEG];
      for(i=0; i < NO_SEG; i++){
       this->tile_ver[i] = new int[NO_TILE]; 
       for(j=0; j < NO_TILE; j++){
        this->tile_ver[i][j] = -1; 
       }
      }
      std::lock_guard<std::mutex> lk(m);
    }
    int* get_tile_version(int seg_id){
     int* ret = new int[NO_TILE];
     m.lock();
     std::copy(tile_ver[seg_id], tile_ver[seg_id] + NO_TILE, ret);  
     m.unlock();
     return ret;
    }
    void add_tile_version(int seg_id, bool BASE_LAYER, vector<int> tile_list, int version){
      int i;
      m.lock();
      if(BASE_LAYER){
        for(i=0; i < NO_TILE; i++)
          tile_ver[seg_id][i] = 0;
      }else{
        for(i=0; i < tile_list.size(); i++)
          tile_ver[seg_id][tile_list[i]] = version;
      }
      m.unlock();
    }
    void update_tile_version(int seg_id, int face_id, int version){
      int i;
      m.lock();
      if(face_id == 0){
        for(i=0; i < NO_TILE; i++)
          tile_ver[seg_id][i] = version;
      }else{
        tile_ver[seg_id][face_id] = version;
      }
      m.unlock();
    }
};
class Player{
  public:
    Buffer buff;
    VisibleMask* vmask;
    Headtrace* htrace; 
    //
    double start_time;
    double base_layer_buff_sec; //ms
    double played_sec; //ms
    int playing_seg_id;
    double BASE_LAYER_BUFF_SIZE; //ms
    double VIDEO_LEN; //ms
    double* seg_avg_qual;
    double* seg_playback_deadline;
    int** seg_tile_version;
    int* seg_stall_time;
    int SD; //ms
    int NO_SEG;
    int NO_TILE;
    int STATE; /* 0: INITIAL BUFFERING; 1: PLAYING; 2: RE-BUFFERING */
    int INTER = 100; /* sampling time: 100ms */
    mutex m;
    std::chrono::steady_clock::time_point connect_end_time;
    //
    FILE* log;
    Player(int BASE_LAYER_BUFF_SIZE, int VIDEO_LEN, int SD, int NO_TILE, VisibleMask* vmask, Headtrace* htrace){
      /* ms */
      this->BASE_LAYER_BUFF_SIZE = BASE_LAYER_BUFF_SIZE * 1000;
      this->VIDEO_LEN = VIDEO_LEN * 1000;
      this->SD = SD * 1000;
      this->NO_TILE = NO_TILE;
      //
      this->played_sec = 0;
      this->base_layer_buff_sec = 0;
      this->STATE = 0; 
      this->playing_seg_id = -1; /* not playing yet */
      std::lock_guard<std::mutex> lk(m);
      //
      this->NO_SEG = VIDEO_LEN/SD;
      this->seg_avg_qual = new double[NO_SEG];
      this->seg_playback_deadline = new double[NO_SEG];
      this->seg_stall_time = new int[NO_SEG];
      for(int i=0; i < NO_SEG; i++)
        this->seg_stall_time[i] = 0;
      this->seg_tile_version = new int*[NO_SEG];
      //
      this->buff.init(NO_SEG, NO_TILE);
      //
      this->vmask = vmask;
      this->htrace = htrace;
      //
      this->log = fopen("log_player.txt", "w");
    }
    /* play video */
    void start_playback(){  
      int i;
      while(played_sec < VIDEO_LEN){
        switch(STATE){
          case 0:
            if(get_cur_buff_size() == SD){
              STATE = 1;
              playing_seg_id = 0;
              seg_tile_version[playing_seg_id] = buff.get_tile_version(playing_seg_id); 
              seg_avg_qual[playing_seg_id] = calc_avg_visi_tile_qua(seg_tile_version[playing_seg_id], vmask->get_visible_tile(htrace->get_vp_pos(played_sec))); 

              /* log */
              cout << "#[Player] Started !! " << endl;
              cout << "#[Player] seg #" << playing_seg_id << " started, avg_qua=" << seg_avg_qual[playing_seg_id] << endl;
              fprintf(log, "%.2f,%d,%.2f\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - connect_end_time).count()/1000.0, playing_seg_id, seg_avg_qual[playing_seg_id]);
              fflush(log);

              /* update segment playback deadlines */
              seg_playback_deadline[playing_seg_id] = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - connect_end_time).count()/1000.0;
              for(i=playing_seg_id + 1; i < NO_SEG; i++)
                seg_playback_deadline[i] = seg_playback_deadline[i-1] + SD;

            }
            break;
          case 1:
            /* update buffer/player status */
            decre_buff_size(INTER);
            incre_play_time(INTER); 

            /* start new segment */
            if(played_sec == playing_seg_id * SD){
              /* calculate average visible tiles quality */
              if(get_cur_buff_size() != 0){
                seg_tile_version[playing_seg_id] = buff.get_tile_version(playing_seg_id); 
                seg_avg_qual[playing_seg_id] = calc_avg_visi_tile_qua(seg_tile_version[playing_seg_id], vmask->get_visible_tile(htrace->get_vp_pos(played_sec))); 
                fprintf(log, "%.2f,%d,%.2f\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - connect_end_time).count()/1000.0, playing_seg_id, seg_avg_qual[playing_seg_id]);
                fflush(log);
              }
            }

            if(get_cur_buff_size() == 0)
              STATE = 2;
            break;
          case 2:
            seg_stall_time[playing_seg_id] += INTER;
            if(get_cur_buff_size() == SD){
              STATE = 1;

              /* calculate average playback quality only once at the beginning */
              seg_tile_version[playing_seg_id] = buff.get_tile_version(playing_seg_id); 
              seg_avg_qual[playing_seg_id] = calc_avg_visi_tile_qua(seg_tile_version[playing_seg_id], vmask->get_visible_tile(htrace->get_vp_pos(played_sec))); 
              fprintf(log, "%.2f,%d,%.2f\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - connect_end_time).count()/1000.0, playing_seg_id, seg_avg_qual[playing_seg_id]);
              
              /* update playback deadlines */
              seg_playback_deadline[playing_seg_id] = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - connect_end_time).count()/1000.0;
              for(i=playing_seg_id + 1; i < NO_SEG; i++)
                seg_playback_deadline[i] = seg_playback_deadline[i-1] + SD;
            }
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(INTER));
      }
    }
    
    /* increase buffer size */
    void incre_buff_size(int amount){
      m.lock();
      this->base_layer_buff_sec += amount;
      m.unlock();
    }

    /* decrease buffer size */
    void decre_buff_size(int amount){
      m.lock();
      this->base_layer_buff_sec -= amount;
      m.unlock();
    }

    /* get buffer size */
    int get_cur_buff_size(){
      m.lock();
      int buff_size = this->base_layer_buff_sec;
      m.unlock();
      return buff_size;
    }
   
    /* get buffer size (second) */
    double get_cur_buff_size_second(){
      m.lock();
      double buff_size = this->base_layer_buff_sec/1000.0;
      m.unlock();
      return buff_size;
    }
 
    /* increase play time */
    void incre_play_time(int amount){
      m.lock();
      this->played_sec += amount;
      playing_seg_id = this->played_sec/SD;
      m.unlock();
    } 
    
    /* get play time */
    int get_cur_play_time(){
      m.lock();
      int play_time = this->played_sec;
      m.unlock();
      return play_time;
    }

};
class Metric{
  public:
    /* segment */
    double** face_req_time;
    double** face_req_rtt;
    double** face_rcv_time;
    double** face_end_time;
    double** face_dow_time;
    double** face_dow_thrp;
    double** face_dow_data;
    double** face_avg_thrp;
    double** face_ini_weight;
    double* base_buf_sec;
    int** face_br;
    int** face_late;
    double** face_size;
    bool* seg_EL_download;
    double* iter_down_time_budget;
    //
    int** face_stream_ID; // request stream id (client)
    int** face_push_stream_id; // push stream id (server)
    //
    map<int, int> stream_id_2_sid;
    map<int, int> stream_id_2_fid;
    //
    map<int, int> push_stream_id_2_sid;
    map<int, int> push_stream_id_2_fid;

    /* interation */
    double* iter_down_data;
    double* iter_down_data_frame;
    double* iter_start_time;
    double* iter_end_time;
    double* iter_down_time;
    double* iter_sam_thrp;
    double* iter_est_thrp;
    double* iter_base_buff;
    bool* iter_re_decide;
    bool* iter_end;
    vector<int>* iter_tile_list;
    int* iter_tile_version;
    int** iter_est_vp;
    mutex m;

    Metric(int NO_SEG, int NO_FACE){
      int i;
      /* segment */
      this->face_stream_ID = this->init2dArrayInt(NO_SEG, NO_FACE);      
      this->face_push_stream_id = this->init2dArrayInt(NO_SEG, NO_FACE);      
      this->face_br = this->init2dArrayInt(NO_SEG, NO_FACE);
      this->face_late = this->init2dArrayInt(NO_SEG, NO_FACE);
      this->face_size = this->init2dArrayDouble(NO_SEG, NO_FACE);
      this->face_req_time = this->init2dArrayDouble(NO_SEG, NO_FACE);
      this->face_req_rtt = this->init2dArrayDouble(NO_SEG, NO_FACE);
      this->face_rcv_time = this->init2dArrayDouble(NO_SEG, NO_FACE);
      this->face_end_time = this->init2dArrayDouble(NO_SEG, NO_FACE);
      this->face_dow_time = this->init2dArrayDouble(NO_SEG, NO_FACE);
      this->face_dow_thrp = this->init2dArrayDouble(NO_SEG, NO_FACE);
      this->face_dow_data = this->init2dArrayDouble(NO_SEG, NO_FACE);
      this->face_avg_thrp = this->init2dArrayDouble(NO_SEG, NO_FACE);
      this->face_ini_weight = this->init2dArrayDouble(NO_SEG, NO_FACE);
      this->base_buf_sec = new double[NO_SEG];
      this->seg_EL_download = new bool[NO_SEG];
      for(i=0; i < NO_SEG; i++){
       this->seg_EL_download[i] = false; 
      }

      /* interation */
      this->iter_sam_thrp = new double[2*NO_SEG];
      this->iter_est_thrp = new double[2*NO_SEG]; 
      this->iter_down_data = new double[2*NO_SEG];
      this->iter_down_data_frame = new double[2*NO_SEG];
      this->iter_down_time = new double[2*NO_SEG];
      this->iter_start_time = new double[2*NO_SEG];
      this->iter_end_time = new double[2*NO_SEG];
      this->iter_base_buff = new double[2*NO_SEG];
      this->iter_est_vp = this->init2dArrayInt(2*NO_SEG, 2);
      this->iter_tile_list = new vector<int>[2*NO_SEG];
      this->iter_tile_version = new int[2*NO_SEG];
      this->iter_down_time_budget = new double[2*NO_SEG];
      this->iter_re_decide = new bool[2*NO_SEG];
      this->iter_end = new bool[2*NO_SEG];

      /* mutex */
      std::lock_guard<std::mutex> lk(m);
    }
    
    /* print stream stats to output streams */
    void print(int sid, int fid){
      cout << sid << ", " << fid << ", "; 
      cout << face_br[sid][fid] << ", " << face_ini_weight[sid][fid] << ", " << face_req_time[sid][fid] << ", " << face_rcv_time[sid][fid] << ", ";
      cout << face_dow_thrp[sid][fid]  << endl; 
    }
    
    /* print stream stats to file */
    void print_to_file(FILE* f, int iter_id, int sid, int fid){
      fprintf(f, "%d,%d,%d,", iter_id, sid, fid); 
      fprintf(f, "%d,%.2f,%.2f,%.2f,%.2f,",face_br[sid][fid],face_size[sid][fid], face_ini_weight[sid][fid] ,face_req_time[sid][fid], face_end_time[sid][fid]);
      fprintf(f, "%.2f,%d\n", face_dow_thrp[sid][fid], face_late[sid][fid]); 
      fflush(f);
    }
    /* init 2D array */
    int** init2dArrayInt(int M, int N){
      int** ret;
      int i,j;
      ret = new int*[M];
      for(i=0; i < M; i++){
        ret[i] = new int[N];
      }
      return ret;
    }double*** init3dArrayDouble(int M, int N, int P){
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
    void  est_iter_thrp(int iter_id){
      int i, no_samp;
      if(iter_id == 0)
        iter_est_thrp[iter_id] = 0;
      else{
        /* get number of samples */
        iter_est_thrp[iter_id] = 0;
        if(iter_id < 3)
          no_samp = iter_id;
        else
          no_samp = 3;
        
        /* average over last no_sample samples */
        for(i=iter_id-1; i >= iter_id - no_samp; i --)
          iter_est_thrp[iter_id] += iter_sam_thrp[i];
        iter_est_thrp[iter_id] /= no_samp;
      }
    }
    vector<int> predict_viewport(int EL_seg_id){
      vector<int> ret;
      ret.push_back(1);
      ret.push_back(2);
      return ret;
    }
    void mark_end_iter(int iter_id){
      m.lock();
      iter_end[iter_id] = true;
      m.unlock();
    }
    bool is_end_iter(int iter_id){
      bool ret;
      m.lock();
      ret = iter_end[iter_id];
      m.unlock();
      return ret; 
    }
};
class Manifest{
  public:
    int VLEN;
    int SD;
    int BASE_BUFF_THRES;
    int base_layer_br;
    int* enha_layer_br;
    int NO_SEG;
    int EL_VER_NUM;
    int NO_FACE;
    Manifest(int VLEN, int SD, int B, int base_layer_br, int* enha_layer_br, int EL_VER_NUM, int NO_FACE){
      this->VLEN = VLEN;
      this->SD = SD;
      this->NO_SEG = VLEN/SD;
      this->BASE_BUFF_THRES = B;
      this->base_layer_br = base_layer_br;
      this->enha_layer_br = enha_layer_br;
      this->EL_VER_NUM = EL_VER_NUM;
      this->NO_FACE = NO_FACE;
    }
};
