#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <nghttp2/asio_http2_server.h>


using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;
using namespace boost::posix_time;
const int num_of_face = 25; // NUmber of faces/tiles
std::string bitrate[num_of_face];
int num[num_of_face];

int segment_duration = 1; // 1000ms
auto avail_seg = std::make_shared<int>();
int server_seg [num_of_face];
int dang_server_seg = 0;
const int MAX_SEGMENTS = 20001;
std::vector<int> tiles_in_region [num_of_face];

bool on_pushing_in_periodic_mode[num_of_face] ;
bool on_steady_stage = false;

ptime start_time, end_time;
bool check_finish_arr[num_of_face];
int terminate_seg = -1, terminate_face = -1;
bool increased = false;

int headed_seg = 0;

bool check_finish_segment(){
  for(int i = 0 ; i < num_of_face ; i++){
    // if(!check_finish_arr[i])
    //   return false;
    if(num[i] != 0) return false;
  }
  // for (int i = 1 ; i<num_of_face+1 ; i++)
  //   check_finish_arr[i] = false;
  if(!increased){
    dang_server_seg++;
    increased = true;
  }
   return true;
}
int get_time() {
  end_time = microsec_clock::local_time();
  return (end_time - start_time).total_milliseconds();
}

void print_new_seg(int cur_seg,int index) {
  std::cout << std::endl << "Respond seg # " << cur_seg <<", face= "<< index+1 << " bitrate= " << bitrate[index];
  std::cout << " at time: " << get_time() << "ms" << std::endl;
}


void push_remaining_files(const response *res, int face_index, int curr_seg) {
  if (curr_seg >= MAX_SEGMENTS) {
    res->write_head(200);
    res->end();
    return;
  } 

  if (num[face_index] == 0 || (face_index == terminate_face && curr_seg == terminate_seg)) { 
    return; 
  }
  print_new_seg(curr_seg, face_index);
  num[face_index]--;

  boost::system::error_code ec;
  auto push = res->push(ec, "GET", "/seg_"+std::to_string(curr_seg)+"_rate_"+bitrate[face_index]+"_face_"+std::to_string(face_index));

  push->on_close([res,face_index, curr_seg](uint32_t error_code) {
    if (!error_code) {                    // if CANCEL is not sent
      increased = false;
     // server_seg[region] = server_seg[region] + 1;
      std::cout << "Sent seg #" << curr_seg <<",face= "<< face_index+1 <<" bitrate= " << bitrate[face_index]
                        << " at time: " << get_time() << "ms" << std::endl;
    }
    res->write_head(200);
    res->end();
    // std::cout<<"rebuff : "<<num[0]<<std::endl;
    // headed_seg = curr_seg;
    // if ( num[face_index] == 0 || (face_index == terminate_face && curr_seg == terminate_seg)) {
    //     on_pushing_in_periodic_mode[face_index] = false;
    //     check_finish_arr[face_index] = true;
    //     res->write_head(200);
    //     res->end(); 
    // } else {
    //   // on_pushing_in_periodic_mode[face_index] = true;
    //   push_remaining_files(res,face_index, curr_seg + 1);
    // }

  });

  push->write_head(200);
  push->end(file_generator("./vr_rate/6f_2x2/"+std::to_string(segment_duration) + "s/Face" + std::to_string(face_index) + "/" + bitrate[face_index]));
  // on_pushing_in_periodic_mode[face_index] = true;
}


void push_file(
    std::shared_ptr<boost::asio::basic_deadline_timer<ptime>> timer, 
    const response *res, std::shared_ptr<bool> closed, boost::system::error_code &ec, int face_index) {
  if (ec || *closed) {
    return;
  }

  if (on_pushing_in_periodic_mode[face_index]) {
    std::cout << "[push_file] Return face_index=" <<  face_index << std::endl;
    return; 
  }

  //  boost::system::error_code ec;
  // auto push = res->push(ec, "GET", "/seg_"+std::to_string(curr_seg)+"_rate_"+bitrate[face_index]+"_face_"+std::to_string(face_index));

  // push->on_close([res,face_index, curr_seg](uint32_t error_code) {
  //   if (!error_code) {                    // if CANCEL is not sent
  //     increased = false;
  //    // server_seg[region] = server_seg[region] + 1;
  //     std::cout << "Sent seg #" << curr_seg <<",face= "<< face_index+1 <<" bitrate= " << bitrate[face_index]
  //                       << " at time: " << get_time() << "ms" << std::endl;
  //   }
  //   //std::cout<<"rebuff : "<<num[0]<<std::endl;
  //   // headed_seg = curr_seg;
  //   // if ( num[face_index] == 0 || (face_index == terminate_face && curr_seg == terminate_seg)) {
  //   //     on_pushing_in_periodic_mode[face_index] = false;
  //   //     check_finish_arr[face_index] = true;
  //   //     res->write_head(200);
  //   //     res->end(); 
  //   // } else {
  //   //   // on_pushing_in_periodic_mode[face_index] = true;
  //   //   push_remaining_files(res,face_index, curr_seg + 1);
  //   // }

  // });

  // push->write_head(200);
  // push->end(file_generator("./vr_rate/6f_2x2/"+std::to_string(segment_duration) + "s/Face" + std::to_string(face_index) + "/" + bitrate[face_index]));

  push_remaining_files(res,face_index, headed_seg);
}


void handle(const request &req, const response &res,int face_index){
  std :: cout << "Received request "<< face_index << std::endl;
  std :: cout << req.uri().path << std::endl;
  if(!increased){
    headed_seg ++;
    increased = true;
  }
  std::vector<std::string> strs;
    boost::split(strs, req.uri().path, boost::is_any_of("/"));
    if (strs.size() != 4) {
      std::cout << "[ERROR] : url is incorrect" << std::endl;
      return;
    }

    //std :: cout << "pass "<<std::endl;
   // tiles_in_region [face_index].clear();
    std::vector<std::string> temp;
    boost::split(temp, strs[2], boost::is_any_of("="));
    bitrate[face_index] = temp[1];
    std :: cout << "pass "<<std::endl;
    temp.clear();
    boost::split(temp , strs[3] , boost::is_any_of("="));
    //std :: cout << "pass "<<std::endl;
    //std :: cout << temp[0<<std::endl;
    num[face_index] = std::stoi(temp[1]);
   // std :: cout << "pass "<<std::endl;
    // for(int i=0;i<temp.size();i++){
    //     tiles_in_region[face_index].push_back(std::stoi(temp.at(i)));
    // }
    

    // compute the available time instant of the next segment
    auto avail_time = start_time + milliseconds(server_seg[face_index] * segment_duration);

    // compute the wait interval to the next available time instant. It may be negative
    auto wait_intv = avail_time - microsec_clock::local_time();

    // call the tick function
    boost::system::error_code ec;
    auto timer = std::make_shared<boost::asio::deadline_timer>(res.io_service(), wait_intv);
    auto closed = std::make_shared<bool>();

    res.on_close([timer, closed](uint32_t error_code) {
     // std::cout << "Finished rebuffering !!!" << std::endl;
      timer->cancel();
      *closed = true;
    });
    push_file(timer,&res,closed,ec,face_index);

    ///////////////////////// DUC /////////////////////////////////
    // boost::system::error_code ec;
  //   auto push = res.push(ec, "GET", "/seg_"+std::to_string(headed_seg)+"_rate_"+bitrate[face_index]+"_face_"+std::to_string(face_index));

  //   push->on_close([res,face_index, 0](uint32_t error_code) {
  //   if (!error_code) {                    // if CANCEL is not sent
  //     increased = false;
  //    // server_seg[region] = server_seg[region] + 1;
  //     std::cout << "Sent seg #" << 0 <<",face= "<< face_index+1 <<" bitrate= " << bitrate[face_index]
  //     << " at time: " << get_time() << "ms" << std::endl;
  //   }
  //   res->write_head(200);
  //   res->end();

  // });

  //   push->write_head(200);
  //   push->end(file_generator("./vr_rate/6f_2x2/"+std::to_string(segment_duration) + "s/Face" + std::to_string(face_index) + "/" + bitrate[face_index]));

}


int main(int argc, char *argv[]) {
  boost::system::error_code ec;
  boost::asio::ssl::context tls(boost::asio::ssl::context::sslv23);
  http2 server;
  
  for(int i=0 ; i < num_of_face  ; i++){
    on_pushing_in_periodic_mode[i]=false;
    server_seg[i]=0;
  }
  start_time = microsec_clock::local_time();
  server.num_threads(150);

  server.handle("/reset_session", [](const request &req, const response &res) {
    on_steady_stage = false;
    res.write_head(200);
    res.end();
  });

  server.handle("/face0/", [](const request &req, const response &res) {
    //headed_seg ++ ;
    handle(req,res,0);
  });

  server.handle("/face1/", [](const request &req, const response &res) {
    
    handle(req,res,1);
    //std::cout<<"dcm"<<bitrate[1]<<num[1]<<std::endl;
  });

  server.handle("/face2/", [](const request &req, const response &res) {
    
    handle(req,res,2);
  });

  server.handle("/face3/", [](const request &req, const response &res) {
    
    handle(req,res,3);
  });

  server.handle("/face4/", [](const request &req, const response &res) {
    
    handle(req,res,4);
  });

  server.handle("/face5/", [](const request &req, const response &res) {
    std :: cout << "Received request 5"<< std::endl;
    handle(req,res,5);
  });

  server.handle("/face6/", [](const request &req, const response &res) {
      std :: cout << "Received request 6"<< std::endl;
      handle(req,res,6);
      });
  server.handle("/face7/", [](const request &req, const response &res) {
      std :: cout << "Received request 7"<< std::endl;
      handle(req,res,7);
      });
  server.handle("/face8/", [](const request &req, const response &res) {
      std :: cout << "Received request 8"<< std::endl;
      handle(req,res,8);
      });
  server.handle("/face9/", [](const request &req, const response &res) {
      std :: cout << "Received request 9"<< std::endl;
      handle(req,res,9);
      });
  server.handle("/face10/", [](const request &req, const response &res) {
      std :: cout << "Received request 10"<< std::endl;
      handle(req,res,10);
      });
  server.handle("/face11/", [](const request &req, const response &res) {
      std :: cout << "Received request 11"<< std::endl;
      handle(req,res,11);
      });
  server.handle("/face12/", [](const request &req, const response &res) {
      std :: cout << "Received request 12"<< std::endl;
      handle(req,res,12);
      });
  server.handle("/face13/", [](const request &req, const response &res) {
      std :: cout << "Received request 13"<< std::endl;
      handle(req,res,13);
      });
  server.handle("/face14/", [](const request &req, const response &res) {
      std :: cout << "Received request 14"<< std::endl;
      handle(req,res,14);
      });
  server.handle("/face15/", [](const request &req, const response &res) {
      std :: cout << "Received request 15"<< std::endl;
      handle(req,res,15);
      });
  server.handle("/face16/", [](const request &req, const response &res) {
      std :: cout << "Received request 16"<< std::endl;
      handle(req,res,16);
      });

   server.handle("/face17/", [](const request &req, const response &res) {
      std :: cout << "Received request 17"<< std::endl;
      handle(req,res,17);
      });
   server.handle("/face18/", [](const request &req, const response &res) {
      std :: cout << "Received request 18"<< std::endl;
      handle(req,res,18);
      });
  server.handle("/face19/", [](const request &req, const response &res) {
      std :: cout << "Received request 19"<< std::endl;
      handle(req,res,19);
      });
  server.handle("/face20/", [](const request &req, const response &res) {
      std :: cout << "Received request 20"<< std::endl;
      handle(req,res,20);
      });
  server.handle("/face21/", [](const request &req, const response &res) {
      std :: cout << "Received request 21"<< std::endl;
      handle(req,res,21);
      });
  server.handle("/face22/", [](const request &req, const response &res) {
      std :: cout << "Received request 22"<< std::endl;
      handle(req,res,22);
      });
  server.handle("/face23/", [](const request &req, const response &res) {
      std :: cout << "Received request 23"<< std::endl;
      handle(req,res,23);
      });
  server.handle("/face24/", [](const request &req, const response &res) {
      std :: cout << "Received request 24"<< std::endl;
      handle(req,res,24);
      });
server.handle("/terminate_segment/", [](const request &req, const response &res){

    std::cout<<"Received cancel"<<std::endl;
    std::vector<std::string> temp;
    boost::split(temp, req.uri().path, boost::is_any_of("/"));
    terminate_seg = std::stoi(temp.at(2));
    terminate_face = std::stoi(temp.at(3));
    
    res.write_head(200);
    res.end();
  });

  std::string ip = "192.168.9.10";
  // std::string ip = "127.0.0.1";
  std::cout << "Listening at the address: " << ip<< std::endl;
  if (server.listen_and_serve(ec, ip, "3002")) {
    std::cerr << "error: " << ec.message() << std::endl;
  }

/*
  if (server.listen_and_serve(ec, tls, "192.168.9.76", "3002")) {
    std::cerr << "error: " << ec.message() << std::endl;
  }
*/
}
