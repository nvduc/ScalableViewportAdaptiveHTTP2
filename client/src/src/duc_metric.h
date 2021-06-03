
//#define MAX_SEG 1000
//#define NO_FACE 6
#include <map>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <thread>
#include <mutex>
#include <locale>
#include <string>
// #include "common.h"
#include "Tile.h"
using namespace std;

class StreamInfo{
  public:
    int sid; // segment id
    int fid; // face id
    int lid; // layer id
  StreamInfo(int sid, int fid, int lid){
    this->sid = sid;
    this->fid = fid;
    this->lid = lid;
  }

};
class VisibleMask{
  public:
    int*** vmask;
    int*** pixel;
    int NO_TILE; 

    /* init() */
    VisibleMask(const char* vmaskfile, int NO_TILE){
      cout << "[VisibleMask] Load vmask from " << vmaskfile << endl;
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

     /* get tiles next to the adjacent tiles */ 
    vector<int> get_adjacent_tile(int* vp_pos, int ext_width){
      int NO_FACE = 6;
      int NO_TILE_H = 2;
      int NO_TILE_W = 2;
      // int ext_width = 1;
      int* vmask = get_vmask(vp_pos);
      int* vmask_ext = extVmask(vmask, NO_FACE, NO_TILE_H, NO_TILE_W, ext_width);
      vector<int> visible_tile;
      int i;
      for(i=0; i < NO_TILE; i++)
        if(vmask_ext[i] == 2)
          visible_tile.push_back(i+1);
      return visible_tile;
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
      int tmp_phi;
      if(vp_pos[0] < 0)
        tmp_phi = vp_pos[0] + 360;
      else
        tmp_phi = vp_pos[0];
      return this->pixel[tmp_phi][vp_pos[1]+90];
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
        cout << "#[Headtrace] Cannot open file " << tracefile << endl;
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

    /* get viewport postion at a given time */
    int* get_vp_pos_byFrameId(int frame_id){ 
      return vp_pos[frame_id]; 
    } 

    int import_matrix_from_txt_file(const char* filename_X, vector <double>& v, int& rows, int& cols){
      ifstream file_X;
      string line;
      // erase all current elements
      v.erase(v.begin(), v.end());
      cout << "#[import_matrix_from_txt_file] open text file: " << filename_X << "a" << endl;
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
    int NO_LAYER;
    int*** tile_ver;
    mutex m;
    Buffer(){
    }
    void init(int NO_SEG, int NO_TILE, int NO_LAYER){
      int i,j,k;
      this->NO_SEG = NO_SEG;
      this->NO_TILE = NO_TILE;
      this->NO_LAYER = NO_LAYER;
      this->tile_ver = new int**[NO_SEG];
      for(i=0; i < NO_SEG; i++){
       this->tile_ver[i] = new int*[NO_TILE]; 
       for(j=0; j < NO_TILE; j++){
        this->tile_ver[i][j] = new int[NO_LAYER];
        for(k=0; k < NO_LAYER; k++){
          this->tile_ver[i][j][k] = -1;
        }
       }
      }
      std::lock_guard<std::mutex> lk(m);
    }
    int* get_tile_version(int seg_id){
     int* ret = new int[NO_TILE];
     int i,j;
     // 
     m.lock();
     // std::copy(tile_ver[seg_id], tile_ver[seg_id] + NO_TILE, ret);
     for(i=0; i < NO_TILE; i++){
        for(j=0; j < NO_LAYER; j++){
          if(tile_ver[seg_id][i][j] == -1)
            break;
          ret[i] = j;
        }
     }
     m.unlock();
     return ret;
    }
    void update_tile_version(int seg_id, int face_id, int layerId){
      int i;
      m.lock();
      if(face_id == 0){
        for(i=0; i < NO_TILE; i++)
          tile_ver[seg_id][i][0] = 1;
      }else{
        tile_ver[seg_id][face_id-1][layerId] = 1;
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
    double* seg_playback_deadline;
    int timeoffset;
    
    //////////// Quality metrics //////////////
    double* seg_avg_qual;               // Average quality of visible tiles
    double* seg_avg_qual_w;             // Weighted average quality of visible tiles
    double* seg_avg_bitrate;            // Average bitrate of visible tiles
    double* seg_avg_bitrate_w;          // Weighted average bitrate of visible tiles
    double* seg_std_bitrate;            // std bitrate of visible tiles
    int*    seg_stall_time;             // Stalling time in miliseconds
    int**  seg_tile_version;            // tiles' versions
    double* seg_playback_time;          // segment playback time
    int*    buff_size;
    double* frame_viewport_bitrate;     // viewport bitrate per frame
    int* frame_vtile_num;
    int* frame_vtile_err;
    double* frame_vp_err;               // percentage of different pixels between two viewport
    int* frame_vp_err_tile;          // number of different tiles between two viewport
    double* seg_vp_err;                 // prediction errors averaged over all frames of a segment
    double* seg_vp_err_tile;            // num. diff tiles averaged over all frames of a segment
    double* seg_smooth_vp_err;
    double* seg_smooth_vp_err_adapt; 
    int* ext_width;
    int** frame_vp_pos;
    int** seg_est_vp;



    int SD; //ms
    int NO_SEG;
    int NO_TILE;
    int STATE; /* 0: INITIAL BUFFERING; 1: PLAYING; 2: RE-BUFFERING */
    int INTER = 100; /* sampling interval: 100ms */
    int FPS = 30;
    int VP_SIZE = 960 * 960;  // viewport size in pixels
    int TILE_SIZE = 480 * 480; // tile size in pixels

    //////////////////////////
    // int tile_br[3] = {3000, 3200, 3600};
    // int tile_br[3] = {125, 325, 725};
    // int tile_br[3] = {136, 270, 510};
    // int NO_LAYER = 3;
    int* tile_br;
    int NO_LAYER;

    mutex m;
    std::chrono::steady_clock::time_point connect_end_time;
    //
    FILE* log;
    FILE* log_frame;

    Player(int BASE_LAYER_BUFF_SIZE, int VIDEO_LEN, int SD, int NO_TILE, VisibleMask* vmask, Headtrace* htrace, int ALG, int base_layer_br, int EL_VER_NUM, int* enha_layer_br){
      char fname[128];
      cout << "[Player] Initialization\n";
      /* ms */
      this->NO_LAYER = EL_VER_NUM + 1;
      tile_br = new int[this->NO_LAYER];
      tile_br[0] = base_layer_br/NO_TILE;
      for(int i=1; i < this->NO_LAYER; i++){
        tile_br[i] = tile_br[i-1] + enha_layer_br[i-1];
      }

      for(int i=0; i < NO_LAYER; i++){
        cout << tile_br[i] << ",";
      }
      cout << "\n";
      // 
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
      this->seg_avg_qual_w = new double[NO_SEG];
      this->seg_playback_deadline = new double[NO_SEG];
      this->seg_playback_time = new double[NO_SEG];
      this->seg_stall_time = new int[NO_SEG];
      this->seg_tile_version = new int*[NO_SEG];
      this->buff_size = new int[NO_SEG];
      this->seg_avg_bitrate = new double[NO_SEG];
      this->seg_avg_bitrate_w = new double[NO_SEG];
      this->seg_std_bitrate = new double[NO_SEG];
      this->frame_viewport_bitrate = new double[NO_SEG * FPS];
      this->frame_vtile_num = new int[NO_SEG * FPS];
      this->frame_vtile_err = new int[NO_SEG * FPS];
      this->frame_vp_pos = new int*[NO_SEG * FPS];
      this->seg_est_vp = new int*[NO_SEG];
      this->frame_vp_err = new double[NO_SEG * FPS];
      this->frame_vp_err_tile = new int[NO_SEG * FPS];
      this->seg_vp_err = new double[NO_SEG];
      this->seg_vp_err_tile = new double[NO_SEG];
      this->seg_smooth_vp_err = new double[NO_SEG];
      this->seg_smooth_vp_err_adapt = new double[NO_SEG];
      this->ext_width = new int[NO_SEG];

      for(int i=0; i < NO_SEG; i++){
        this->seg_stall_time[i] = 0;
        this->seg_est_vp[i] = new int[2];
        this->seg_est_vp[i][0] = 0;
        this->seg_est_vp[i][1] = 0;
        this->seg_vp_err[i] = 0;
        this->seg_smooth_vp_err[i] = 0;
        this->seg_smooth_vp_err_adapt[i] = 0;
        this->ext_width[i] = 0;
      }


      //
      this->buff.init(NO_SEG, NO_TILE, NO_LAYER);
      //
      this->vmask = vmask;
      this->htrace = htrace;
      //
      sprintf(fname, "log_player_ALG_%d.txt", ALG);
      this->log = fopen(fname, "w");
      fprintf(this->log, "time, playing_seg_id, stall_time, buff_size, seg_avg_qual, seg_avg_qual_w, seg_avg_bitrate, seg_avg_bitrate_w, seg_std_bitrate\n");
      //
      sprintf(fname, "log_player_ALG_%d_frame.txt", ALG);
      this->log_frame = fopen(fname, "w");
      fprintf(this->log_frame, "seg_id, frame_id, viewport_bitrate, pitch, yaw, est-pitch, est-yaw, diff-pixel\n");

      // exit(1);
   
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
              seg_playback_time[playing_seg_id] = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - connect_end_time).count()/1000.0;
              buff_size[playing_seg_id] = get_cur_buff_size();
          
              /* log */
              cout << "#[Player] Started !! " << endl;

              /* update segment playback deadlines */
              seg_playback_deadline[playing_seg_id] = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - connect_end_time).count()/1000.0;
              // 
              if(playing_seg_id == 0)
                timeoffset = seg_playback_deadline[playing_seg_id];
              // 
              for(i=playing_seg_id + 1; i < NO_SEG; i++)
                seg_playback_deadline[i] = seg_playback_deadline[i-1] + SD;

            }
            break;
          case 1:
            /* update buffer/player status */
            decre_buff_size(INTER);
            incre_play_time(INTER); 
            // cout << "########## PLAYER: " << played_sec << endl;

            /* start new segment */
            if(played_sec == playing_seg_id * SD){
              /* calculate average visible tiles quality */
              // cout << "######### PLAYER: " << played_sec << "," << VIDEO_LEN << "," << get_cur_buff_size() << "," << playing_seg_id << endl;
              calc_metric(seg_tile_version[playing_seg_id-1], playing_seg_id-1);
              // for(i=0; i < NO_TILE; i++)
              //   cout << (i+1) << "-" << seg_tile_version[playing_seg_id-1][i] << ",";
              cout << endl; 

              if(get_cur_buff_size() != 0){
                seg_tile_version[playing_seg_id] = buff.get_tile_version(playing_seg_id); 
                seg_playback_time[playing_seg_id] = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - connect_end_time).count()/1000.0;
                buff_size[playing_seg_id] = get_cur_buff_size();
                
                // calculate and log metrics
                // calc_metric(seg_tile_version[playing_seg_id], playing_seg_id);
                // fprintf(log,"%d,%.2f,%d,%.2f, %.2f,%.2f,%.2f,%.2f\n", playing_seg_id, seg_playback_time[playing_seg_id], seg_stall_time[playing_seg_id], buff_size[playing_seg_id]/1000.0, seg_avg_qual[playing_seg_id], seg_avg_qual_w[playing_seg_id], seg_avg_bitrate[playing_seg_id], seg_avg_bitrate_w[playing_seg_id]);
                // fflush(log);
              }
            }

            if(get_cur_buff_size() == 0){
              // cout << "########### PLAYER: Enter state 2" << endl;
              STATE = 2;
            }
            break;
          case 2:
            seg_stall_time[playing_seg_id] += INTER;
            if(get_cur_buff_size() == SD){
              STATE = 1;

              /* calculate average playback quality only once at the beginning --> should be at every frame */
              seg_tile_version[playing_seg_id] = buff.get_tile_version(playing_seg_id);
              seg_playback_time[playing_seg_id] = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - connect_end_time).count()/1000.0;
              buff_size[playing_seg_id] = get_cur_buff_size();
              
              /* update playback deadlines */
              seg_playback_deadline[playing_seg_id] = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - connect_end_time).count()/1000.0;
              for(i=playing_seg_id + 1; i < NO_SEG; i++)
                seg_playback_deadline[i] = seg_playback_deadline[i-1] + SD;
            }
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(INTER));
      }
      // write outputs to file
      cout << "################## Player finish !!:" << endl;
      for(playing_seg_id =0; playing_seg_id < NO_SEG; playing_seg_id ++){
        calc_metric(seg_tile_version[playing_seg_id], playing_seg_id);
        fprintf(log,"%d,%.2f,%d,%.2f, %.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%.2f,%.2f\n", playing_seg_id, seg_playback_time[playing_seg_id], seg_stall_time[playing_seg_id], buff_size[playing_seg_id]/1000.0, seg_avg_qual[playing_seg_id], seg_avg_qual_w[playing_seg_id], seg_avg_bitrate[playing_seg_id], seg_avg_bitrate_w[playing_seg_id], seg_std_bitrate[playing_seg_id], seg_vp_err[playing_seg_id], seg_smooth_vp_err[playing_seg_id], ext_width[playing_seg_id], seg_smooth_vp_err_adapt[playing_seg_id], seg_vp_err_tile[playing_seg_id]);
        fflush(log);
        //
        for(int frameID = playing_seg_id * FPS; frameID < (playing_seg_id+1) * FPS; frameID ++){
          fprintf(log_frame, "%d,%d,%.2f,%d,%d,%d,%d,%d,%.2f,%d\n", playing_seg_id, frameID, frame_viewport_bitrate[frameID], frame_vtile_num[frameID], frame_vp_pos[frameID][0], frame_vp_pos[frameID][1], seg_est_vp[playing_seg_id][0], seg_est_vp[playing_seg_id][1],frame_vp_err[frameID], frame_vp_err_tile[frameID]);
        }
      }
      fclose(log);
      fclose(log_frame);

      // terminate client //check
      exit(1);
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

    /* calculate average quality of visible tiles */
    double calc_avg_visi_tile_qua(int* tile_ver, int playing_seg_id){
      double ret = 0;
      int frameId, tileId;
      vector<int> visiMask;
      for(frameId = playing_seg_id*FPS; frameId < (playing_seg_id+1) * FPS; frameId ++){
        visiMask = vmask->get_visible_tile(htrace->get_vp_pos_byFrameId(frameId));
        for(tileId=0; tileId < visiMask.size(); tileId++)
          ret += (tile_ver[visiMask[tileId]] + 1);
      }
      return ret*1.0/FPS;
    }

    /* calculate per-segment metrics */
    void calc_metric(int* tile_ver, int playing_seg_id){

      cout << "############[calc_metric] seg #" << playing_seg_id << endl;
      double alpha = 0.8;
      
      seg_avg_qual[playing_seg_id] = 0;
      seg_avg_qual_w[playing_seg_id] = 0;
      seg_avg_bitrate[playing_seg_id] = 0;
      seg_avg_bitrate_w[playing_seg_id] = 0;
      seg_std_bitrate[playing_seg_id] = 0;

      int frameId, tileId, i;
      vector<int> visiMask;
      int* pixel, *vpPos;
      double seg_std_tmp, seg_avg_tmp;
      // for(tileId = 0; tileId < NO_TILE; tileId++)
      //   cout << tileId + 1 << ":" << tile_ver[tileId] << ",";
      // cout << endl; 
      // 
      seg_vp_err[playing_seg_id] = 0;
      for(frameId = playing_seg_id*FPS; frameId < (playing_seg_id+1) * FPS; frameId ++){
        vpPos = htrace->get_vp_pos_byFrameId(frameId);
        // cout << "frame: " << frameId << endl;
        visiMask = vmask->get_visible_tile(vpPos);
        // for(i=0; i < visiMask.size(); i++)
        //   cout << visiMask[i] << ",";
        // cout << endl;
        pixel = vmask->get_pixel(vpPos);
        cout << endl;
        seg_avg_tmp = 0;
        frame_viewport_bitrate[frameId] = 0;
        frame_vp_pos[frameId] = vpPos;
        frame_vtile_num[frameId] = visiMask.size();
        frame_vp_err[frameId] = calc_vp_diff(vpPos, seg_est_vp[playing_seg_id], NO_TILE) * 1.0 / VP_SIZE * 100;
        frame_vp_err_tile[frameId] = calc_vp_diff_tile(vpPos, seg_est_vp[playing_seg_id], NO_TILE);
        seg_vp_err[playing_seg_id] += frame_vp_err[frameId] / FPS;
        seg_vp_err_tile[playing_seg_id] += frame_vp_err_tile[frameId] * 1.0 / FPS;


        for(tileId=0; tileId < NO_TILE; tileId++){
          if(pixel[tileId] > 0){
            seg_avg_qual[playing_seg_id] += (tile_ver[tileId] + 1)*1.0/visiMask.size();
            seg_avg_bitrate[playing_seg_id] += tile_br[tile_ver[tileId]]*1.0/visiMask.size();
            seg_avg_tmp += tile_br[tile_ver[tileId]]*1.0/visiMask.size();
          }
          seg_avg_qual_w[playing_seg_id] += (tile_ver[tileId] + 1) * pixel[tileId];
          seg_avg_bitrate_w[playing_seg_id] += tile_br[tile_ver[tileId]] * pixel[tileId];
          frame_viewport_bitrate[frameId] += tile_br[tile_ver[tileId]] * pixel[tileId] * 1.0 / TILE_SIZE;
          // cout << tileId << "," << tile_ver[tileId] << "," << tile_br[tile_ver[tileId]] << "," << pixel[tileId] << "," << frame_viewport_bitrate[frameId] << endl;
        }

        // calculate standard deviation of visible tiles's bitrate values
        seg_std_tmp = 0;
        for(tileId=0; tileId < NO_TILE; tileId ++){
          if(pixel[tileId] > 0){
            seg_std_tmp += (tile_br[tile_ver[tileId]] - seg_avg_tmp) * (tile_br[tile_ver[tileId]] - seg_avg_tmp);
          }
        }
        seg_std_bitrate[playing_seg_id] += sqrt(seg_std_tmp/visiMask.size());
      }
      seg_std_bitrate[playing_seg_id] /= (FPS * 1.0);
      seg_avg_qual[playing_seg_id] /= (FPS * 1.0);
      seg_avg_bitrate[playing_seg_id] /= (FPS * 1.0);
      seg_avg_qual_w[playing_seg_id] /= (FPS * TILE_SIZE * 1.0);
      seg_avg_bitrate_w[playing_seg_id] /= (FPS * TILE_SIZE * 1.0);
      // cout << "#[calc_metric] " << playing_seg_id << ", " << seg_avg_qual[playing_seg_id] << ", " << seg_avg_qual_w[playing_seg_id] << endl;
      if(playing_seg_id == 0){
        seg_smooth_vp_err[playing_seg_id] = 0;
      }else{
        if(playing_seg_id < 6){
          seg_smooth_vp_err[playing_seg_id] = seg_vp_err[playing_seg_id-1];
        }else{
          seg_smooth_vp_err[playing_seg_id] = alpha * seg_smooth_vp_err[playing_seg_id-1] + (1-alpha) * seg_vp_err[playing_seg_id];
        }
      }

      // adapt diff
      if(playing_seg_id == 0){
        seg_smooth_vp_err_adapt[playing_seg_id] = 0;
      }else{
        if(playing_seg_id < 6){
          seg_smooth_vp_err_adapt[playing_seg_id] = seg_vp_err[playing_seg_id-1];
        }else{
          if(seg_vp_err[playing_seg_id-1] - seg_vp_err[playing_seg_id-2] > 30)
            alpha = 0.2;
          else
            alpha = 0.8;
          seg_smooth_vp_err_adapt[playing_seg_id] = alpha * seg_smooth_vp_err_adapt[playing_seg_id-1] + (1-alpha) * seg_vp_err[playing_seg_id];
        }
      }
    }

    double calc_vp_diff(int* realVP, int* estVP, int NO_TILE){
      int* pixelRealVP, *pixelEstVP;
      vector<int> real(NO_TILE), est(NO_TILE), vmask_real, vmask_est, shared_tile(NO_TILE), diff_tile(NO_TILE);
      vector<int>::iterator it;
      int diff_pixel_num = 0;

      if(estVP == NULL){
        return -1;
      }else{
        // get visible pixels per tile
        pixelRealVP = vmask->get_pixel(realVP);
        pixelEstVP = vmask->get_pixel(estVP);

        // determine shared and different tiles
        vmask_real = vmask->get_visible_tile(realVP);
        vmask_est = vmask->get_visible_tile(estVP);

        sort(vmask_real.begin(), vmask_real.begin() + vmask_real.size());
        sort(vmask_est.begin(), vmask_est.begin() + vmask_est.size());

        // determine shared tiles
        it = std::set_intersection(vmask_real.begin(), vmask_real.begin()+vmask_real.size(), vmask_est.begin(), vmask_est.begin() + vmask_est.size(), shared_tile.begin());
        shared_tile.resize(it - shared_tile.begin());

        // determine diff tiles
        it = std::set_difference(vmask_real.begin(), vmask_real.begin()+vmask_real.size(), vmask_est.begin(), vmask_est.begin() + vmask_est.size(), diff_tile.begin());
        diff_tile.resize(it - diff_tile.begin());

        // calculate new pixels in 1) shared tiles and 2) diff tiles
        // for shared tiles
        // for(it=shared_tile.begin(); it != shared_tile.end(); it++){
        //   diff_pixel_num += (pixelRealVP[*it-1] - pixelEstVP[*it-1]);
        // }
        // for diff tiles
        for(it=diff_tile.begin(); it != diff_tile.end(); it++){
          diff_pixel_num += pixelRealVP[*it-1];
        }
        return diff_pixel_num;
      }
    }
    int calc_vp_diff_tile(int* realVP, int* estVP, int NO_TILE){
      int* pixelRealVP, *pixelEstVP;
      vector<int> real(NO_TILE), est(NO_TILE), vmask_real, vmask_est, shared_tile(NO_TILE), diff_tile(NO_TILE);
      vector<int>::iterator it;
      int diff_pixel_num = 0;

      if(estVP == NULL){
        return -1;
      }else{
        // get visible pixels per tile
        pixelRealVP = vmask->get_pixel(realVP);
        pixelEstVP = vmask->get_pixel(estVP);

        // determine shared and different tiles
        vmask_real = vmask->get_visible_tile(realVP);
        vmask_est = vmask->get_visible_tile(estVP);

        sort(vmask_real.begin(), vmask_real.begin() + vmask_real.size());
        sort(vmask_est.begin(), vmask_est.begin() + vmask_est.size());

        // determine shared tiles
        // it = std::set_intersection(vmask_real.begin(), vmask_real.begin()+vmask_real.size(), vmask_est.begin(), vmask_est.begin() + vmask_est.size(), shared_tile.begin());
        // shared_tile.resize(it - shared_tile.begin());

        // determine diff tiles
        it = std::set_difference(vmask_real.begin(), vmask_real.begin()+vmask_real.size(), vmask_est.begin(), vmask_est.begin() + vmask_est.size(), diff_tile.begin());
        // diff_tile.resize(it - diff_tile.begin());

        // calculate new pixels in 1) shared tiles and 2) diff tiles
        // for shared tiles
        // for(it=shared_tile.begin(); it != shared_tile.end(); it++){
        //   diff_pixel_num += (pixelRealVP[*it-1] - pixelEstVP[*it-1]);
        // }
        // for diff tiles
        // for(it=diff_tile.begin(); it != diff_tile.end(); it++){
        //   diff_pixel_num += pixelRealVP[*it-1];
        // }
        return (it - diff_tile.begin());
      }
    }

};
class Metric{
  public:
    /* segment */
    double*** face_req_time;
    double*** face_req_rtt;
    double*** face_rcv_time;
    double*** face_end_time;
    double*** face_dow_time;
    double*** face_dow_thrp;
    double*** face_dow_data;
    double*** face_avg_thrp;
    double*** face_ini_weight;
    double* base_buf_sec;
    int*** face_br;
    int*** face_late;
    int*** face_cancel;
    double*** face_size;
    bool* seg_EL_download;
    double* iter_down_time_budget;
    //
    int*** face_stream_ID; // request stream id (client)
    int*** face_push_stream_id; // push stream id (server)
    //
    map<int, int> stream_id_2_sid; // map stream_id to segment id
    map<int, int> stream_id_2_fid; // map stream_id to face id
    map<int, int> stream_id_2_lid; // map stream_id to layer id
    map<int, StreamInfo*> RequestStreamInfo; // map stream_id to layer id
    //
    map<int, int> push_stream_id_2_sid;
    map<int, int> push_stream_id_2_fid;
    map<int, int> push_stream_id_2_lid;
    map<int, StreamInfo*> pushStreamInfo;

    /* interation */
    double* iter_down_data;
    double* iter_down_data_frame;
    double* iter_start_time;
    double* iter_end_time;
    double* iter_down_time;
    double* iter_sam_thrp;
    double* iter_est_thrp;
    double *iter_smooth_bitrate;
    double* iter_base_buff;
    bool* iter_re_decide;
    bool* iter_end;
    vector<int>* iter_tile_list;
    vector<int>* iter_new_vtile;
    vector<int>* iter_old_vtile;
    vector<int>* iter_req_stream;
    vector<int>* iter_push_stream;
    vector<int>* iter_req_stream_fin;
    vector<int>* iter_push_stream_fin;
    int* iter_tile_version;
    int** iter_est_vp;
    mutex m;

    Metric(int NO_SEG, int NO_FACE, int NO_LAYER){
      int i;
      /* segment */
      this->face_stream_ID = this->init3dArrayInt(NO_SEG, NO_FACE, NO_LAYER);      
      this->face_push_stream_id = this->init3dArrayInt(NO_SEG, NO_FACE, NO_LAYER);      
      this->face_br = this->init3dArrayInt(NO_SEG, NO_FACE, NO_LAYER);
      this->face_late = this->init3dArrayInt(NO_SEG, NO_FACE, NO_LAYER);
      this->face_cancel = this->init3dArrayInt(NO_SEG, NO_FACE, NO_LAYER);
      this->face_size = this->init3dArrayDouble(NO_SEG, NO_FACE, NO_LAYER);
      this->face_req_time = this->init3dArrayDouble(NO_SEG, NO_FACE, NO_LAYER);
      this->face_req_rtt = this->init3dArrayDouble(NO_SEG, NO_FACE, NO_LAYER);
      this->face_rcv_time = this->init3dArrayDouble(NO_SEG, NO_FACE, NO_LAYER);
      this->face_end_time = this->init3dArrayDouble(NO_SEG, NO_FACE, NO_LAYER);
      this->face_dow_time = this->init3dArrayDouble(NO_SEG, NO_FACE, NO_LAYER);
      this->face_dow_thrp = this->init3dArrayDouble(NO_SEG, NO_FACE, NO_LAYER);
      this->face_dow_data = this->init3dArrayDouble(NO_SEG, NO_FACE, NO_LAYER);
      this->face_avg_thrp = this->init3dArrayDouble(NO_SEG, NO_FACE, NO_LAYER);
      this->face_ini_weight = this->init3dArrayDouble(NO_SEG, NO_FACE, NO_LAYER);
      this->base_buf_sec = new double[NO_SEG];
      this->seg_EL_download = new bool[NO_SEG];
      for(i=0; i < NO_SEG; i++){
       this->seg_EL_download[i] = false; 
      }

      /* interation */
      this->iter_sam_thrp = new double[5*NO_SEG];
      this->iter_est_thrp = new double[5*NO_SEG];
      this->iter_smooth_bitrate = new double[5 * NO_SEG];
      this->iter_down_data = new double[5*NO_SEG];
      this->iter_down_data_frame = new double[5*NO_SEG];
      this->iter_down_time = new double[5*NO_SEG];
      this->iter_start_time = new double[5*NO_SEG];
      this->iter_end_time = new double[5*NO_SEG];
      this->iter_base_buff = new double[5*NO_SEG];
      this->iter_est_vp = this->init2dArrayInt(5*NO_SEG, 5);
      this->iter_tile_list = new vector<int>[5*NO_SEG];
      this->iter_old_vtile = new vector<int>[5*NO_SEG];
      this->iter_new_vtile = new vector<int>[5*NO_SEG];
      this->iter_req_stream = new vector<int>[5*NO_SEG];
      this->iter_req_stream_fin = new vector<int>[5*NO_SEG];
      this->iter_push_stream = new vector<int>[5*NO_SEG];
      this->iter_push_stream_fin = new vector<int>[5*NO_SEG];
      this->iter_tile_version = new int[5*NO_SEG];
      this->iter_down_time_budget = new double[5*NO_SEG];
      this->iter_re_decide = new bool[5*NO_SEG];
      this->iter_end = new bool[5*NO_SEG];

      /* mutex */
      std::lock_guard<std::mutex> lk(m);
    }
    
    /* print stream stats to output streams */
    void print(int sid, int fid, int lid){
      cout << sid << ", " << fid << ", " << lid << ", "; 
      cout << face_br[sid][fid][lid] << ", " << face_ini_weight[sid][fid][lid] << ", " << face_req_time[sid][fid][lid] << ", " << face_rcv_time[sid][fid][lid] << ", ";
      cout << face_dow_thrp[sid][fid][lid]  << endl; 
    }
    
    /* print stream stats to file */
    void print_to_file(FILE* f, int iter_id, int sid, int fid, int lid){
      fprintf(f, "%d,%d,%.2f,%.2f,%.2f,%d,%d,", face_stream_ID[sid][fid][lid], face_push_stream_id[sid][fid][lid], face_req_time[sid][fid][lid], face_end_time[sid][fid][lid] , face_dow_thrp[sid][fid][lid], face_late[sid][fid][lid], face_cancel[sid][fid][lid]); 
      fprintf(f, "%d,%d,%d,%d,", iter_id, sid, fid, lid); 
      fprintf(f, "%d,%.2f,%.2f\n", face_br[sid][fid][lid],face_size[sid][fid][lid], face_ini_weight[sid][fid][lid] );
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
    double*** init3dArrayDouble(int M, int N, int P){
      double*** ret;
      int i,j,k;
      ret = new double**[M];
      for(i=0; i < M; i++){
        ret[i] = new double*[N];
        for(j=0; j < N; j++)
          ret[i][j] = new double[P];
          // for(k=0; k < P; k++){
          //   ret[i][j][k] = 0;
          // }
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
    /*
    * This function estimates available network throughput for the next iteration
    * using the average throughput of last three segments
    */
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

    /* calculate bitrate allowed for smoother quality */
  void cal_smooth_bitrate(int iter_id, int step) {
    if (iter_est_thrp[iter_id] < iter_smooth_bitrate[iter_id - 1] - step)
      iter_smooth_bitrate[iter_id] = iter_smooth_bitrate[iter_id - 1] - step;
    else if (iter_est_thrp[iter_id] > iter_smooth_bitrate[iter_id - 1] + step)
      iter_smooth_bitrate[iter_id] = iter_smooth_bitrate[iter_id - 1] + step;
    else
      iter_smooth_bitrate[iter_id] = iter_est_thrp[iter_id];
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
    //////////// video info ////////////
    int NO_SEG;
    int SD;
    int NO_TILE;
    int NO_FACE;
    int VLEN;
    int BASE_BUFF_THRES;
    int FPS;
    int base_layer_br;
    int* enha_layer_br;
    int EL_VER_NUM;
    // const char* vmask, *htrace;
    string vmask, htrace;
    string bwtrace;

    /////////// Adaptation algorithm ///////
    int ALG;

    // Manifest(int VLEN, int SD, int B, int base_layer_br, int* enha_layer_br, int EL_VER_NUM, int NO_FACE){
    //   this->VLEN = VLEN;
    //   this->SD = SD;
    //   this->NO_SEG = VLEN/SD;
    //   this->BASE_BUFF_THRES = B;
    //   this->base_layer_br = base_layer_br;
    //   this->enha_layer_br = enha_layer_br;
    //   this->EL_VER_NUM = EL_VER_NUM;
    //   this->NO_FACE = NO_FACE;
    // }
    Manifest(const char* confFile){
      string s, deli="=", comment="#", key, val_str, token, htrace_tmp, vmask_tmp, bwtrace_tmp;
      size_t pos_deli = 0, pos_comm, pos;
      int i;
      vector<int> tmp;
      ifstream infile(confFile);
      if(infile == NULL){
       cout << "#[setParamFromFile] Cannot open file " << confFile << endl;
       exit(1);
      }else{
       cout << "#[setParamFromFile] Reading file" << endl;
      }
      while(std::getline(infile, s)){
       cout << s << endl;
       if((pos_deli=s.find(deli)) != std::string::npos){
         key = s.substr(0, pos_deli-1);
         pos_comm = s.find(comment);
         if(pos_comm != std::string::npos){
           val_str = s.substr(pos_deli + 2, pos_comm-pos_deli-2);
         }else{
           val_str = s.substr(pos_deli + 2, s.length());
         }
         key = trim(key);
         val_str = trim(val_str);
         cout << key << "," << val_str << "a" << endl;
         if(key.compare("NO_SEG") == 0){
          this->NO_SEG = std::stod(val_str);
         }
         //
         if(key.compare("SD") == 0){
          this->SD = std::stod(val_str);
         }
         //
         if(key.compare("NO_TILE") == 0){
          this->NO_TILE = std::stod(val_str);
          this->NO_FACE = NO_TILE + 1;
         }
         //
         if(key.compare("base_layer_buff") == 0){
          this->BASE_BUFF_THRES = std::stod(val_str);
         }
         //
         if(key.compare("FPS") == 0){
          this->FPS = std::stod(val_str);
         }
         //
         if(key.compare("base_layer_br") == 0){
          this->base_layer_br = std::stod(val_str);
         }
         //
         if(key.compare("ALG") == 0){
          this->ALG = std::stod(val_str);
         }
         //
         if(key.compare("htrace") == 0){
          htrace_tmp = val_str;
          // this->htrace = htrace_tmp.c_str();
          this->htrace = htrace_tmp;
         }
         //
         if(key.compare("bwtrace") == 0){
          bwtrace_tmp = val_str;
          // this->htrace = htrace_tmp.c_str();
          this->bwtrace = bwtrace_tmp;
         }
         //
         if(key.compare("vmask") == 0){
          vmask_tmp = val_str;
          // this->vmask = vmask_tmp.c_str();
          this->vmask = vmask_tmp;
         }
         //
         if(key.compare("enha_layer_br") == 0){
          // remove all current elements
          tmp.clear();
          while((pos=val_str.find(",")) != std::string::npos){
            token = val_str.substr(0, pos);
            tmp.push_back((int) std::stod(token));
            val_str.erase(0, pos + 1);
          }
          //
          this->EL_VER_NUM = tmp.size();
          enha_layer_br = new int[this->EL_VER_NUM];
          for(i=0; i < this->EL_VER_NUM; i++)
            enha_layer_br[i] = tmp[i];
         }
       }
     }
     //
     this->VLEN = this->NO_SEG * this->SD;
     // check
     cout << this->SD << "," << this->NO_SEG << "," << this->NO_TILE << "," << this->BASE_BUFF_THRES << "," << this->base_layer_br << "," << this->EL_VER_NUM << ",";
     for(int i=0; i < EL_VER_NUM; i++){
      cout << this->enha_layer_br[i] << ",";
     }
     cout << endl;
     // cout << this->htrace << endl << this->vmask << endl;
     cout << this->htrace.c_str() << endl << this->vmask.c_str() << endl;
     // exit(1); 
   }
   string trim(const string& str){
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
    {
      return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
  }
};

bool IsVectorElement(vector<int> V, int x){
  for(int i=0; i < V.size(); i++)
    if(V[i] == x)
      return true;
    return false;
}

